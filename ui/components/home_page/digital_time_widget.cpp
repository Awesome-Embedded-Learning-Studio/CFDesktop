/**
 * @file    digital_time_widget.cpp
 * @brief   Implementation of the DigitalTimeWidget.
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup home_page
 */

#include "digital_time_widget.h"

#include "global_clock_sources.h"

#include "core/theme_manager.h"
#include "core/token/material_scheme/cfmaterial_token_literals.h"
#include "core/token/typography/cfmaterial_typography_token_literals.h"

#include <QDate>
#include <QPaintEvent>
#include <QPainter>
#include <QTime>

namespace cf::desktop::desktop_component {

using namespace qw::core::token::literals;

DigitalTimeWidget::DigitalTimeWidget(QWidget* parent) : QWidget(parent) {
    setAttribute(Qt::WA_TranslucentBackground);
    onTimeUpdate(QTime::currentTime());
    applyTheme();
    connect(&qw::core::ThemeManager::instance(), &qw::core::ThemeManager::themeChanged, this,
            [this]() { applyTheme(); });
    connect(&GlobalClockSources::instance(), &GlobalClockSources::timeUpdate, this,
            &DigitalTimeWidget::onTimeUpdate);
}

void DigitalTimeWidget::onTimeUpdate(const QTime& time) {
    time_text_ = time.toString("HH:mm:ss");
    date_text_ = QDate::currentDate().toString("yyyy-MM-dd dddd");
    update();
}

void DigitalTimeWidget::applyTheme() {
    try {
        auto& tm = qw::core::ThemeManager::instance();
        auto& theme = tm.theme(tm.currentThemeName());
        auto& cs = theme.color_scheme();
        time_color_ = cs.queryColor(ON_SURFACE);
        date_color_ = cs.queryColor(ON_SURFACE_VARIANT);
        time_font_ = theme.font_type().queryTargetFont(TYPOGRAPHY_DISPLAY_MEDIUM);
        date_font_ = theme.font_type().queryTargetFont(TYPOGRAPHY_BODY_SMALL);
    } catch (...) {
        time_color_ = QColor(0x1C, 0x1B, 0x1F);
        date_color_ = QColor(0x49, 0x45, 0x4E);
        time_font_.setPointSize(28);
        time_font_.setBold(true);
        date_font_.setPointSize(11);
    }
    update();
}

void DigitalTimeWidget::paintEvent(QPaintEvent* /*event*/) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    QRect time_rect = rect();
    time_rect.setBottom(rect().center().y());
    QRect date_rect = rect();
    date_rect.setTop(rect().center().y());

    p.setPen(time_color_);
    p.setFont(time_font_);
    p.drawText(time_rect, Qt::AlignCenter, time_text_);

    p.setPen(date_color_);
    p.setFont(date_font_);
    p.drawText(date_rect, Qt::AlignCenter, date_text_);
}

} // namespace cf::desktop::desktop_component
