/**
 * @file app_launcher.cpp
 * @brief Start-menu style application launcher popup implementation.
 *
 * Renders a frameless rounded Material surface holding a grid of LauncherTile
 * entries. popup() sizes and centers it above the taskbar; tile clicks forward
 * appLaunched() and dismiss the popup; ESC and outside clicks (Qt::Popup) also
 * dismiss it. All rendering is QPainter-native.
 *
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date 2026-06-26
 * @version 0.1
 * @since 0.20
 * @ingroup components
 */

#include "app_launcher.h"

#include "launcher_tile.h"

#include "core/theme_manager.h"
#include "core/token/material_scheme/cfmaterial_token_literals.h"

#include <QGridLayout>
#include <QGuiApplication>
#include <QKeyEvent>
#include <QLineEdit>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QScreen>
#include <QVBoxLayout>

#include <algorithm>

namespace cf::desktop::desktop_component {

using namespace qw::core::token::literals;

namespace {
constexpr int kMaxColumns = 5;       ///< Maximum tiles per grid row.
constexpr int kGridSpacing = 12;     ///< Gap between tiles (px).
constexpr int kMargin = 24;          ///< Popup inner margin (px).
constexpr qreal kCornerRadius = 16;  ///< Popup corner radius (px).
constexpr qreal kWidthRatio = 0.5;   ///< Popup width as a fraction of available.
constexpr qreal kHeightRatio = 0.55; ///< Popup height as a fraction of available.
constexpr int kMinWidth = 480;       ///< Minimum popup width (px).
constexpr int kMaxWidth = 720;       ///< Maximum popup width (px).
constexpr int kMinHeight = 360;      ///< Minimum popup height (px).
constexpr int kMaxHeight = 540;      ///< Maximum popup height (px).
} // namespace

AppLauncher::AppLauncher(QWidget* parent) : QWidget(parent) {
    // Frameless CHILD widget -- deliberately NOT Qt::Popup. On windowless
    // targets (linuxfb, no compositor / window manager) a Qt::Popup is a
    // separate top-level window: show() activates it briefly, then the platform
    // deactivates it (~0.5s later) and Qt::Popup auto-closes on deactivation, so
    // the start menu flashed and vanished. As a child of the desktop it renders
    // inside the single desktop window -- no activation fight, no auto-close.
    // Dismissal is via the start-button toggle, ESC, and tile clicks.
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    setAutoFillBackground(false);
    setupUi();
    applyTheme();
    // Stay hidden until popup() is called. As a child of the desktop (not a
    // top-level Qt::Popup), Qt would otherwise auto-show this widget when the
    // desktop becomes visible -- rendering the tiles at the default (0,0)
    // geometry on boot, before the user ever clicks Start.
    hide();

    // Follow live theme switches (ThemeManager is the canonical source).
    connect(&qw::core::ThemeManager::instance(), &qw::core::ThemeManager::themeChanged, this,
            [this](const qw::core::ICFTheme&) { applyTheme(); });
}

AppLauncher::~AppLauncher() = default;

void AppLauncher::setApps(const QList<AppEntry>& apps) {
    apps_ = apps;
    rebuildGrid();
}

void AppLauncher::popup(const QRect& available) {
    QRect avail = available;
    if (!avail.isValid() || avail.width() <= 0 || avail.height() <= 0) {
        if (const auto* screen = QGuiApplication::primaryScreen()) {
            avail = screen->availableGeometry();
        }
    }

    const int w = std::clamp(static_cast<int>(avail.width() * kWidthRatio), kMinWidth, kMaxWidth);
    const int h =
        std::clamp(static_cast<int>(avail.height() * kHeightRatio), kMinHeight, kMaxHeight);
    const int x = avail.center().x() - w / 2;
    const int y = avail.bottom() - h; // Bottom-aligned: sits just above the taskbar.

    // Apply geometry atomically and force the grid to lay tiles out within it.
    setGeometry(x, y, w, h);
    if (grid_ != nullptr) {
        grid_->activate();
    }
    show();
    raise();
}

void AppLauncher::hideLauncher() {
    hide();
}

bool AppLauncher::isShowing() const noexcept {
    return isVisible();
}

// -- Painting --------------------------------------------------------------
void AppLauncher::paintEvent(QPaintEvent* /*event*/) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    // Rounded Material surface; the area outside the path stays transparent.
    QPainterPath surface;
    surface.addRoundedRect(QRectF(rect()), kCornerRadius, kCornerRadius);
    p.fillPath(surface, surface_color_);
}

void AppLauncher::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape) {
        hideLauncher();
        return;
    }
    QWidget::keyPressEvent(event);
}

// -- Internal --------------------------------------------------------------
void AppLauncher::setupUi() {
    auto* outer = new QVBoxLayout(this);
    outer->setContentsMargins(kMargin, kMargin, kMargin, kMargin);
    outer->setSpacing(kGridSpacing);

    search_edit_ = new QLineEdit(this);
    search_edit_->setPlaceholderText(QStringLiteral("Search apps..."));
    outer->addWidget(search_edit_);

    auto* grid_container = new QWidget(this);
    grid_ = new QGridLayout(grid_container);
    grid_->setSpacing(kGridSpacing);
    grid_->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    outer->addWidget(grid_container);

    // Live filter: any change to the search box rebuilds the grid with only
    // tiles whose display_name contains the (case-insensitive) query.
    connect(search_edit_, &QLineEdit::textChanged, this, [this](const QString&) { rebuildGrid(); });
}

void AppLauncher::applyTheme() {
    try {
        auto& tm = qw::core::ThemeManager::instance();
        const auto& theme = tm.theme(tm.currentThemeName());
        auto& cs = theme.color_scheme();
        surface_color_ = cs.queryColor(SURFACE);
        outline_color_ = cs.queryColor(OUTLINE_VARIANT);
    } catch (...) {
        // Fallback palette when no theme is registered yet.
        surface_color_ = QColor(0xF7, 0xF5, 0xF3);
        outline_color_ = QColor(0xCA, 0xC4, 0xD0);
    }
    update();
}

void AppLauncher::rebuildGrid() {
    qDeleteAll(tiles_);
    tiles_.clear();

    const QString filter =
        search_edit_ != nullptr ? search_edit_->text().trimmed().toLower() : QString();
    int row = 0;
    int col = 0;
    for (const auto& app : apps_) {
        if (!filter.isEmpty() && !app.display_name.toLower().contains(filter)) {
            continue; // Filtered out by the search box.
        }
        auto* tile = new LauncherTile(app, this);
        connect(tile, &LauncherTile::clicked, this, [this](const QString& app_id) {
            emit appLaunched(app_id);
            hideLauncher();
        });
        grid_->addWidget(tile, row, col);
        tiles_.append(tile);
        ++col;
        if (col >= kMaxColumns) {
            col = 0;
            ++row;
        }
    }
}

} // namespace cf::desktop::desktop_component
