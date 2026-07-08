/**
 * @file    date_card.cpp
 * @brief   Implementation of the DateCard.
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup home_page
 */

#include "date_card.h"

#include "core/theme_manager.h"
#include "core/token/material_scheme/cfmaterial_token_literals.h"
#include "core/token/typography/cfmaterial_typography_token_literals.h"

#include <QDate>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QRectF>

namespace cf::desktop::desktop_component {

using namespace qw::core::token::literals;

namespace {
/// @brief Corner radius of the date card, in pixels.
inline constexpr double kCornerRadius = 20.0;
} // namespace

DateCard::DateCard(QWidget* parent) : QWidget(parent) {
    setAttribute(Qt::WA_TranslucentBackground);
    refresh();
    applyTheme();
    connect(&qw::core::ThemeManager::instance(), &qw::core::ThemeManager::themeChanged, this,
            [this]() { applyTheme(); });
}

void DateCard::refresh() {
    const QDate today = QDate::currentDate();
    day_text_ = today.toString("dd");
    weekday_text_ = today.toString("dddd");
}

void DateCard::applyTheme() {
    try {
        auto& tm = qw::core::ThemeManager::instance();
        auto& theme = tm.theme(tm.currentThemeName());
        auto& cs = theme.color_scheme();
        surface_color_ = cs.queryColor(SURFACE);
        day_color_ = cs.queryColor(ON_SURFACE);
        weekday_color_ = cs.queryColor(ON_SURFACE_VARIANT);
        day_font_ = theme.font_type().queryTargetFont(TYPOGRAPHY_DISPLAY_LARGE);
        weekday_font_ = theme.font_type().queryTargetFont(TYPOGRAPHY_BODY_MEDIUM);
    } catch (...) {
        surface_color_ = QColor(0xF7, 0xF5, 0xF3);
        day_color_ = QColor(0x1C, 0x1B, 0x1F);
        weekday_color_ = QColor(0x49, 0x45, 0x4E);
        day_font_.setPointSize(48);
        day_font_.setBold(true);
        weekday_font_.setPointSize(14);
    }
    update();
}

void DateCard::paintEvent(QPaintEvent* /*event*/) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    QPainterPath surface;
    surface.addRoundedRect(QRectF(rect()), kCornerRadius, kCornerRadius);
    p.fillPath(surface, surface_color_);

    QRect day_rect = rect();
    day_rect.setBottom(rect().center().y() + 6);
    QRect weekday_rect = rect();
    weekday_rect.setTop(rect().center().y() + 6);

    p.setPen(day_color_);
    p.setFont(day_font_);
    p.drawText(day_rect, Qt::AlignCenter, day_text_);

    p.setPen(weekday_color_);
    p.setFont(weekday_font_);
    p.drawText(weekday_rect, Qt::AlignCenter, weekday_text_);
}

} // namespace cf::desktop::desktop_component
