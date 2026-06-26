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
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QScreen>

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
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    setAutoFillBackground(false);
    setupUi();
    applyTheme();

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

    setFixedSize(w, h);
    move(x, y);
    show();
    raise();
    activateWindow();
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
    grid_ = new QGridLayout(this);
    grid_->setSpacing(kGridSpacing);
    grid_->setContentsMargins(kMargin, kMargin, kMargin, kMargin);
    grid_->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
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

    const int n = apps_.size();
    const int cols = std::max(1, std::min(kMaxColumns, n));
    int row = 0;
    int col = 0;
    for (const auto& app : apps_) {
        auto* tile = new LauncherTile(app, this);
        connect(tile, &LauncherTile::clicked, this, [this](const QString& app_id) {
            emit appLaunched(app_id);
            hideLauncher();
        });
        grid_->addWidget(tile, row, col);
        tiles_.append(tile);
        ++col;
        if (col >= cols) {
            col = 0;
            ++row;
        }
    }
}

} // namespace cf::desktop::desktop_component
