/**
 * @file    modern_calendar_widget.cpp
 * @brief   Implementation of ModernCalendarWidget (ported from CCIMX).
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup home_page
 */

#include "modern_calendar_widget.h"

#include <QDate>
#include <QPainter>
#include <QToolButton>

namespace cf::desktop::desktop_component {

ModernCalendarWidget::ModernCalendarWidget(QWidget* parent) : QCalendarWidget(parent) {
    setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    setNavigationBarVisible(true);
    setFirstDayOfWeek(Qt::Sunday);
    styleNavigationBar();
    setStyleSheet(globalModeQss());
}

void ModernCalendarWidget::setDarkMode(bool dark) {
    dark_mode = dark;
    setStyleSheet(globalModeQss(dark));
    update();
}

void ModernCalendarWidget::setColorForDate(const QDate& date, const QColor& color) {
    date_colors_[date] = color;
    updateCell(date);
}

void ModernCalendarWidget::popColorForDate(const QDate& date) {
    date_colors_.remove(date);
    updateCell(date);
}

void ModernCalendarWidget::styleNavigationBar() {
    auto* nav_bar = this->findChild<QWidget*>("qt_calendar_navigationbar");
    if (nav_bar) {
        nav_bar->setStyleSheet("QWidget { background-color: transparent; border: none; }");
    }

    auto* month_btn = this->findChild<QToolButton*>("qt_calendar_monthbutton");
    auto* year_btn = this->findChild<QToolButton*>("qt_calendar_yearbutton");
    if (month_btn && year_btn) {
        const QString button_style = "QToolButton { color: #3F51B5; font: bold 16px;"
                                     " background: transparent; border: none; }";
        month_btn->setStyleSheet(button_style);
        year_btn->setStyleSheet(button_style);
    }
}

void ModernCalendarWidget::paintCell(QPainter* painter, const QRect& rect, const QDate date) const {
    painter->save();

    const bool is_today = date == QDate::currentDate();
    const bool is_selected = date == selectedDate();
    const QColor event_color = date_colors_.value(date, Qt::transparent);
    const int radius = std::min(rect.width(), rect.height());
    const QPointF center = rect.center();

    const QColor base_bg_color = dark_mode ? QColor(30, 30, 30) : QColor(240, 240, 240);
    const QColor today_outline_color(0x3F51B5);
    const QColor selected_fg_color = Qt::white;
    const QColor default_fg_color = dark_mode ? QColor(220, 220, 220) : Qt::black;

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setBrush(base_bg_color);
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(rect.adjusted(4, 4, -4, -4), 8, 8);

    // State indicator priority: selected > event > today.
    if (is_selected) {
        painter->setBrush(today_outline_color);
        painter->drawEllipse(center, radius / 2.3, radius / 2.3);
    } else if (event_color != Qt::transparent) {
        painter->setBrush(event_color);
        painter->drawEllipse(center, radius / 2.8, radius / 2.8);
    } else if (is_today) {
        painter->setBrush(Qt::NoBrush);
        painter->setPen(QPen(today_outline_color, 1.5));
        painter->drawEllipse(center, radius / 2.5, radius / 2.5);
    }

    QFont font = painter->font();
    if (is_selected) {
        font.setBold(true);
    }
    painter->setFont(font);
    painter->setPen(is_selected ? selected_fg_color : default_fg_color);

    if (is_today && !is_selected && event_color == Qt::transparent) {
        painter->drawText(rect.adjusted(0, 4, 0, rect.height() / 3), Qt::AlignCenter, "Today");
    } else {
        painter->drawText(rect.adjusted(0, 4, 0, 0), Qt::AlignCenter, QString::number(date.day()));
    }

    painter->restore();
}

QString ModernCalendarWidget::globalModeQss(bool is_dark_mode) {
    if (is_dark_mode) {
        return QStringLiteral(R"(
QCalendarWidget QWidget { alternate-background-color: transparent; }
QCalendarWidget QAbstractItemView {
    background-color: #121212; color: #eeeeee; border: 1px solid #333;
    selection-background-color: #3F51B5; selection-color: white; font: 14px 'Segoe UI';
}
QCalendarWidget QToolButton:hover { background-color: #1E1E1E; border-radius: 4px; }
QCalendarWidget QToolButton::menu-indicator { image: none; }
QCalendarWidget QMenu { background-color: #1E1E1E; border: 1px solid #333; color: #eee; font: 14px 'Segoe UI'; }
QCalendarWidget QMenu::item:selected { background-color: #3F51B5; color: white; }
        )");
    }
    return QStringLiteral(R"(
QCalendarWidget QWidget { alternate-background-color: transparent; }
QCalendarWidget QAbstractItemView {
    selection-background-color: transparent; outline: none; gridline-color: transparent;
    font-size: 15px;
}
QCalendarWidget QToolButton:hover { background-color: #E3F2FD; border-radius: 4px; }
QCalendarWidget QAbstractItemView {
    background-color: white; color: #333; font: 14px 'Segoe UI'; border: 1px solid #C5CAE9;
    selection-background-color: #3F51B5; selection-color: white; outline: none; padding: 4px;
}
QCalendarWidget QToolButton::menu-indicator { image: none; }
QCalendarWidget QMenu { background-color: white; border: 1px solid #C5CAE9; padding: 4px; font: 14px 'Segoe UI'; color: #333; }
QCalendarWidget QMenu::item:selected { background-color: #3F51B5; color: white; }
    )");
}

} // namespace cf::desktop::desktop_component
