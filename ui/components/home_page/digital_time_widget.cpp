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

#include <QFont>
#include <QFontMetrics>
#include <QPaintEvent>
#include <QPainter>

namespace cf::desktop::desktop_component {

DigitalTimeWidget::DigitalTimeWidget(QWidget* parent) : QWidget(parent) {
    setAttribute(Qt::WA_TranslucentBackground);
    stored_time_ = QTime::currentTime();
    connect(&GlobalClockSources::instance(), &GlobalClockSources::timeUpdate, this,
            &DigitalTimeWidget::onTimeUpdate);
}

void DigitalTimeWidget::onTimeUpdate(const QTime& time) {
    stored_time_ = time;
    update();
}

void DigitalTimeWidget::paintEvent(QPaintEvent* /*event*/) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const QTime time = stored_time_;
    const QString time_text = time.toString("hh:mm:ss");

    const QDate date = QDate::currentDate();
    const QString date_text = date.toString("yyyy MMM dd ddd");

    QFont time_font("Helvetica Neue", 40, QFont::Bold);
    painter.setFont(time_font);
    const int time_text_height = QFontMetrics(time_font).height();

    QRect time_rect = rect();
    time_rect.setHeight(time_text_height);
    time_rect.moveTop(rect().center().y() - time_text_height); // vertically centered

    painter.setPen(Qt::white);
    painter.drawText(time_rect, Qt::AlignCenter, time_text);

    QFont date_font("Helvetica Neue", 15, QFont::Light);
    painter.setFont(date_font);
    const int date_text_height = QFontMetrics(date_font).height();

    QRect date_rect = rect();
    date_rect.setHeight(date_text_height);
    date_rect.moveTop(time_rect.bottom() + 10);

    painter.drawText(date_rect, Qt::AlignCenter, date_text);
}

} // namespace cf::desktop::desktop_component
