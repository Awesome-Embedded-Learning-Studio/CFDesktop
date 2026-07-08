/**
 * @file    clock_widget.cpp
 * @brief   Implementation of the Material desktop clock widget.
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup desktop_widget
 */

#include "clock_widget.h"

#include "core/theme_manager.h"
#include "core/token/material_scheme/cfmaterial_token_literals.h"
#include "core/token/typography/cfmaterial_typography_token_literals.h"

#include <QDateTime>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QRectF>
#include <QTimer>

namespace cf::desktop::desktop_component {

using namespace qw::core::token::literals;

namespace {
/// @brief Corner radius of the clock card, in pixels.
inline constexpr double kCornerRadius = 24.0;
} // namespace

ClockWidget::ClockWidget(QWidget* parent) : WidgetBase(parent) {
    // Translucent background so the area outside the rounded rect is clear and
    // the wallpaper shows through the corners.
    setAttribute(Qt::WA_TranslucentBackground);

    refreshTime();
    applyTheme();

    // Follow live theme switches (ThemeManager is the canonical source).
    connect(&qw::core::ThemeManager::instance(), &qw::core::ThemeManager::themeChanged, this,
            [this]() { applyTheme(); });

    // Coarse 1 s timer: idletime cost is one repaint of a 320x160 card per
    // second, never a busy loop.
    timer_ = new QTimer(this);
    timer_->setTimerType(Qt::CoarseTimer);
    connect(timer_, &QTimer::timeout, this, [this]() {
        refreshTime();
        update();
    });
    timer_->start(1000);
}

void ClockWidget::refreshTime() {
    const QDateTime now = QDateTime::currentDateTime();
    cached_time_ = now.toString("HH:mm");
    cached_date_ = now.toString("yyyy-MM-dd dddd");
}

void ClockWidget::applyTheme() {
    try {
        auto& tm = qw::core::ThemeManager::instance();
        auto& theme = tm.theme(tm.currentThemeName());
        auto& cs = theme.color_scheme();
        surface_color_ = cs.queryColor(SURFACE);
        time_color_ = cs.queryColor(ON_SURFACE);
        date_color_ = cs.queryColor(ON_SURFACE_VARIANT);
        time_font_ = theme.font_type().queryTargetFont(TYPOGRAPHY_DISPLAY_LARGE);
        date_font_ = theme.font_type().queryTargetFont(TYPOGRAPHY_BODY_MEDIUM);
    } catch (...) {
        // No theme registered yet (e.g. very early in boot); neutral fallbacks.
        surface_color_ = QColor(0xF7, 0xF5, 0xF3);
        time_color_ = QColor(0x1C, 0x1B, 0x1F);
        date_color_ = QColor(0x49, 0x45, 0x4E);
        time_font_.setPointSize(40);
        time_font_.setBold(true);
        date_font_.setPointSize(12);
    }
    update();
}

void ClockWidget::paintEvent(QPaintEvent* /*event*/) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    QPainterPath surface;
    surface.addRoundedRect(QRectF(rect()), kCornerRadius, kCornerRadius);
    p.fillPath(surface, surface_color_);

    // Time on the upper half, date on the lower half.
    QRect time_rect = rect();
    time_rect.setBottom(rect().center().y() + 4);
    QRect date_rect = rect();
    date_rect.setTop(rect().center().y() + 4);

    p.setPen(time_color_);
    p.setFont(time_font_);
    p.drawText(time_rect, Qt::AlignCenter, cached_time_);

    p.setPen(date_color_);
    p.setFont(date_font_);
    p.drawText(date_rect, Qt::AlignCenter, cached_date_);
}

} // namespace cf::desktop::desktop_component
