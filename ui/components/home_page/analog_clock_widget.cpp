/**
 * @file    analog_clock_widget.cpp
 * @brief   Implementation of the AnalogClockWidget dial.
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup home_page
 */

#include "analog_clock_widget.h"

#include "global_clock_sources.h"

#include "core/theme_manager.h"
#include "core/token/material_scheme/cfmaterial_token_literals.h"

#include <QPaintEvent>
#include <QPainter>
#include <QRadialGradient>

namespace cf::desktop::desktop_component {

using namespace qw::core::token::literals;

namespace {
// Fixed dial geometry (unchanged from CCIMX; the painter is scaled so the dial
// fits the widget).
inline constexpr int kDialSize = 200;
inline constexpr int kOuterCircleRadius = 95;
inline constexpr int kThickBorderWidth = 8;

inline constexpr int kHourTickLength = 10;
inline constexpr int kMinuteTickLength = 5;
inline constexpr int kHourTickWidth = 2;
inline constexpr int kMinuteTickWidth = 1;

inline constexpr int kCenterDotRadius = 4;
inline constexpr int kHourHandLength = 50;
inline constexpr int kMinuteHandLength = 70;
inline constexpr int kSecondHandLength = 80;
inline constexpr int kHandWidth = 4;

inline constexpr double kHourRotationPerHour = 30.0;
inline constexpr double kMinuteRotationPerMinute = 6.0;
inline constexpr double kSecondRotationPerSecond = 6.0;
} // namespace

AnalogClockWidget::AnalogClockWidget(QWidget* parent) : QWidget(parent) {
    setAttribute(Qt::WA_TranslucentBackground);
    current_time_ = QTime::currentTime();
    applyTheme();
    connect(&qw::core::ThemeManager::instance(), &qw::core::ThemeManager::themeChanged, this,
            [this]() { applyTheme(); });
    connect(&GlobalClockSources::instance(), &GlobalClockSources::timeUpdate, this,
            &AnalogClockWidget::onTimeUpdate);
}

void AnalogClockWidget::onTimeUpdate(const QTime& time) {
    current_time_ = time;
    update();
}

void AnalogClockWidget::applyTheme() {
    try {
        auto& tm = qw::core::ThemeManager::instance();
        auto& theme = tm.theme(tm.currentThemeName());
        auto& cs = theme.color_scheme();
        surface_color_ = cs.queryColor(SURFACE);
        surface_variant_color_ = cs.queryColor(SURFACE_VARIANT);
        outline_color_ = cs.queryColor(OUTLINE);
        hand_color_ = cs.queryColor(ON_SURFACE);
        minute_tick_color_ = cs.queryColor(ON_SURFACE_VARIANT);
        second_hand_color_ = cs.queryColor(PRIMARY);
    } catch (...) {
        surface_color_ = QColor(0xFA, 0xF9, 0xF8);
        surface_variant_color_ = QColor(0xE7, 0xE0, 0xE9);
        outline_color_ = QColor(0x79, 0x75, 0x7A);
        hand_color_ = QColor(0x1C, 0x1B, 0x1F);
        minute_tick_color_ = QColor(0x49, 0x45, 0x4E);
        second_hand_color_ = QColor(0x67, 0x50, 0xA4);
    }
    update();
}

void AnalogClockWidget::paintEvent(QPaintEvent* /*event*/) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    const int side = qMin(width(), height());
    p.translate(width() / 2.0, height() / 2.0);
    p.scale(side / static_cast<double>(kDialSize), side / static_cast<double>(kDialSize));

    drawBackground(&p);
    drawTicks(&p);
    drawHands(&p);
    drawCenterDot(&p);
}

void AnalogClockWidget::drawBackground(QPainter* p) {
    p->save();
    p->setPen(QPen(outline_color_, kThickBorderWidth));
    p->setBrush(Qt::NoBrush);
    p->drawEllipse(QPoint(0, 0), kOuterCircleRadius - kThickBorderWidth / 2,
                   kOuterCircleRadius - kThickBorderWidth / 2);

    QRadialGradient gradient(0, 0, kOuterCircleRadius);
    gradient.setColorAt(0.0, surface_color_);
    gradient.setColorAt(1.0, surface_variant_color_);
    p->setBrush(gradient);
    p->setPen(Qt::NoPen);
    p->drawEllipse(QPoint(0, 0), kOuterCircleRadius, kOuterCircleRadius);

    p->setPen(QPen(outline_color_, 2));
    p->setBrush(Qt::NoBrush);
    p->drawEllipse(QPoint(0, 0), kOuterCircleRadius, kOuterCircleRadius);
    p->restore();
}

void AnalogClockWidget::drawTicks(QPainter* p) {
    p->save();
    p->setPen(QPen(hand_color_, kHourTickWidth));
    for (int i = 0; i < 12; ++i) {
        p->drawLine(0, -(kOuterCircleRadius - kHourTickLength), 0, -kOuterCircleRadius);
        p->rotate(kHourRotationPerHour);
    }
    p->setPen(QPen(minute_tick_color_, kMinuteTickWidth));
    for (int i = 0; i < 60; ++i) {
        if (i % 5 != 0) {
            p->drawLine(0, -(kOuterCircleRadius - kMinuteTickLength), 0, -kOuterCircleRadius);
        }
        p->rotate(kMinuteRotationPerMinute);
    }
    p->restore();
}

void AnalogClockWidget::drawHands(QPainter* p) {
    // Hour hand.
    p->save();
    p->setPen(Qt::NoPen);
    p->setBrush(hand_color_);
    p->rotate(kHourRotationPerHour * (current_time_.hour() % 12) + current_time_.minute() / 2.0);
    static const QPoint hour_hand[4] = {
        QPoint(0, kHandWidth),
        QPoint(-4, 0),
        QPoint(0, -kHourHandLength),
        QPoint(4, 0),
    };
    p->drawConvexPolygon(hour_hand, 4);
    p->restore();

    // Minute hand.
    p->save();
    p->setPen(Qt::NoPen);
    p->setBrush(hand_color_);
    p->rotate(kMinuteRotationPerMinute * current_time_.minute() + current_time_.second() / 10.0);
    static const QPoint minute_hand[4] = {
        QPoint(0, kHandWidth - 1),
        QPoint(-3, 0),
        QPoint(0, -kMinuteHandLength),
        QPoint(3, 0),
    };
    p->drawConvexPolygon(minute_hand, 4);
    p->restore();

    // Second hand.
    p->save();
    p->setPen(Qt::NoPen);
    p->setBrush(second_hand_color_);
    p->rotate(kSecondRotationPerSecond * current_time_.second());
    static const QPoint second_hand[4] = {
        QPoint(0, kHandWidth / 2),
        QPoint(-2, 0),
        QPoint(0, -kSecondHandLength),
        QPoint(2, 0),
    };
    p->drawConvexPolygon(second_hand, 4);
    p->restore();
}

void AnalogClockWidget::drawCenterDot(QPainter* p) {
    p->save();
    p->setPen(Qt::NoPen);
    p->setBrush(hand_color_);
    p->drawEllipse(QPoint(0, 0), kCenterDotRadius, kCenterDotRadius);
    p->restore();
}

} // namespace cf::desktop::desktop_component
