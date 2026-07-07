/**
 * @file centered_taskbar.cpp
 * @brief Centered taskbar panel implementation.
 *
 * Implements the bottom-edge panel: a centered row of TaskbarIcon tiles laid
 * out with stretchers on both sides, painted on a translucent Material
 * surface with a horizontally-faded top hairline (mirroring the status bar
 * seam). Clicks on tiles are forwarded as appClicked(app_id).
 *
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date 2026-06-16
 * @version 0.1
 * @since 0.19
 * @ingroup components
 */

#include "centered_taskbar.h"

#include "start_button.h"
#include "taskbar_icon.h"

#include "core/theme_manager.h"
#include "core/token/material_scheme/cfmaterial_token_literals.h"

#include <QHBoxLayout>
#include <QLayoutItem>
#include <QLinearGradient>
#include <QPaintEvent>
#include <QPainter>

// Q_INIT_RESOURCE must run at global scope: the rcc-generated registration
// function lives in the global namespace, but the macro's extern declaration is
// emitted in the surrounding scope, so calling it from inside
// cf::desktop::desktop_component would look up a namespaced symbol that does
// not exist and fail to link. Without this the taskbar_icons.qrc object is
// dropped from the static archive at link time and ":/cfdesktop/taskbar/*.png"
// lookups silently return null (icons render blank).
static void registerTaskbarIconsResource() {
    Q_INIT_RESOURCE(taskbar_icons);
}

namespace cf::desktop::desktop_component {

using cf::desktop::PanelPosition;
using namespace qw::core::token::literals;

namespace {
constexpr int kTaskbarHeight = 64;    ///< Bar thickness (px).
constexpr int kSideMargin = 12;       ///< Horizontal padding (px).
constexpr int kTopBottomMargin = 4;   ///< Vertical padding (px).
constexpr int kIconSpacing = 8;       ///< Gap between tiles (px).
constexpr int kStartButtonGap = 16;   ///< Gap after the start button (px).
constexpr qreal kSurfaceAlpha = 0.82; ///< Fixed surface transparency over the wallpaper.
} // namespace

CenteredTaskbar::CenteredTaskbar(QWidget* parent) : QWidget(parent) {
    registerTaskbarIconsResource();
    // No WA_OpaquePaintEvent: the bar paints a fixed-alpha surface so the
    // wallpaper composites through it. Declaring it opaque would suppress that.
    setAutoFillBackground(false);
    setFixedHeight(kTaskbarHeight);
    setupUi();
    applyTheme();
}

CenteredTaskbar::~CenteredTaskbar() = default;

void CenteredTaskbar::setupUi() {
    layout_ = new QHBoxLayout(this);
    layout_->setContentsMargins(kSideMargin, kTopBottomMargin, kSideMargin, kTopBottomMargin);
    layout_->setSpacing(kIconSpacing);

    // Leading start affordance: requests the application launcher popup.
    start_button_ = new StartButton(this);
    connect(start_button_, &StartButton::clicked, this, &CenteredTaskbar::launcherRequested);
    layout_->addWidget(start_button_);
    layout_->addSpacing(kStartButtonGap);

    // The centered icon row lives in its own sub-layout so setApps() can rebuild
    // it without disturbing the start button or the centering stretchers.
    icon_layout_ = new QHBoxLayout();
    icon_layout_->setSpacing(kIconSpacing);
    layout_->addStretch();
    layout_->addLayout(icon_layout_);
    layout_->addStretch();

    // React to theme switches (ThemeManager is the canonical source).
    connect(&qw::core::ThemeManager::instance(), &qw::core::ThemeManager::themeChanged, this,
            [this](const qw::core::ICFTheme&) { applyTheme(); });
}

void CenteredTaskbar::applyTheme() {
    try {
        auto& tm = qw::core::ThemeManager::instance();
        const auto& theme = tm.theme(tm.currentThemeName());
        auto& cs = theme.color_scheme();
        background_color_ = cs.queryColor(SURFACE);
        divider_color_ = cs.queryColor(OUTLINE_VARIANT);
    } catch (...) {
        // Fallback palette when no theme is registered yet.
        background_color_ = QColor(0xF7, 0xF5, 0xF3);
        divider_color_ = QColor(0xCA, 0xC4, 0xD0);
    }
    update();
}

// -- IPanel ----------------------------------------------------------------
PanelPosition CenteredTaskbar::position() const {
    return PanelPosition::Bottom;
}

int CenteredTaskbar::priority() const {
    return 100;
}

int CenteredTaskbar::preferredSize() const {
    return kTaskbarHeight;
}

QWidget* CenteredTaskbar::widget() const {
    return const_cast<CenteredTaskbar*>(this);
}

// -- Taskbar API -----------------------------------------------------------
void CenteredTaskbar::setApps(const QList<AppEntry>& apps) {
    // Clear only the dynamic icon row (preserve the start button + stretchers).
    while (icon_layout_->count() != 0) {
        QLayoutItem* item = icon_layout_->takeAt(0);
        if (item->widget() != nullptr) {
            item->widget()->deleteLater();
        }
        delete item;
    }
    icons_.clear();

    for (const auto& app : apps) {
        auto* icon = new TaskbarIcon(app, this);
        connect(icon, &TaskbarIcon::clicked, this, &CenteredTaskbar::appClicked);
        icon_layout_->addWidget(icon);
        icons_.append(icon);
    }
}

void CenteredTaskbar::updateRunningState(const QString& app_id, bool running) {
    for (auto* icon : icons_) {
        if (icon != nullptr && icon->appId() == app_id) {
            icon->setRunning(running);
        }
    }
}

// -- Painting --------------------------------------------------------------
void CenteredTaskbar::paintEvent(QPaintEvent* /*event*/) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    // Fixed-transparency surface: the wallpaper composites through it without a
    // per-wallpaper blur, so there is no cached blur to invalidate on rotation
    // (no stale frame, no transition snap) and zero blur cost on every repaint.
    QColor surface = background_color_;
    surface.setAlphaF(kSurfaceAlpha);
    p.fillRect(rect(), surface);

    // Soft top elevation shadow: the bar reads as floating above the shell.
    QLinearGradient shadow(0, 0, 0, height() * 0.5);
    shadow.setColorAt(0.0, QColor(0, 0, 0, 38));
    shadow.setColorAt(1.0, QColor(0, 0, 0, 0));
    p.fillRect(QRectF(0, 0, width(), height() * 0.5), shadow);

    // Horizontally-faded top hairline, mirroring the status bar seam.
    QColor lineMid = divider_color_;
    lineMid.setAlphaF(0.45);
    QColor lineEdge = divider_color_;
    lineEdge.setAlphaF(0.0);
    QLinearGradient hairline(0, 0, width(), 0);
    hairline.setColorAt(0.0, lineEdge);
    hairline.setColorAt(0.08, lineMid);
    hairline.setColorAt(0.92, lineMid);
    hairline.setColorAt(1.0, lineEdge);
    p.setPen(QPen(hairline, 1));
    p.drawLine(QPointF(0, 0.5), QPointF(width(), 0.5));
}

} // namespace cf::desktop::desktop_component
