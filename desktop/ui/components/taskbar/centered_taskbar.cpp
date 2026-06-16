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

#include "taskbar_icon.h"

#include "core/theme_manager.h"
#include "core/token/material_scheme/cfmaterial_token_literals.h"

#include <QHBoxLayout>
#include <QLayoutItem>
#include <QLinearGradient>
#include <QPaintEvent>
#include <QPainter>

namespace cf::desktop::desktop_component {

using cf::desktop::PanelPosition;
using namespace cf::ui::core::token::literals;

namespace {
constexpr int kTaskbarHeight = 64;    ///< Bar thickness (px).
constexpr int kSideMargin = 12;       ///< Horizontal padding (px).
constexpr int kTopBottomMargin = 4;   ///< Vertical padding (px).
constexpr int kIconSpacing = 8;       ///< Gap between tiles (px).
constexpr qreal kSurfaceAlpha = 0.92; ///< Surface fill opacity.
} // namespace

CenteredTaskbar::CenteredTaskbar(QWidget* parent) : QWidget(parent) {
    setAttribute(Qt::WA_OpaquePaintEvent);
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

    // React to theme switches (ThemeManager is the canonical source).
    connect(&cf::ui::core::ThemeManager::instance(), &cf::ui::core::ThemeManager::themeChanged,
            this, [this](const cf::ui::core::ICFTheme&) { applyTheme(); });
}

void CenteredTaskbar::applyTheme() {
    try {
        auto& tm = cf::ui::core::ThemeManager::instance();
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
    // Clear existing items and their widgets.
    while (layout_->count() != 0) {
        QLayoutItem* item = layout_->takeAt(0);
        if (item->widget() != nullptr) {
            item->widget()->deleteLater();
        }
        delete item;
    }
    icons_.clear();

    // Center the tile row between two stretchers.
    layout_->addStretch();
    for (const auto& app : apps) {
        auto* icon = new TaskbarIcon(app, this);
        connect(icon, &TaskbarIcon::clicked, this, &CenteredTaskbar::appClicked);
        layout_->addWidget(icon);
        icons_.append(icon);
    }
    layout_->addStretch();
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

    // Translucent surface.
    QColor bg = background_color_;
    bg.setAlphaF(kSurfaceAlpha);
    p.fillRect(rect(), bg);

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
