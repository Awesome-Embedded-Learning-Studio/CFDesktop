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

#include <QPaintEvent>
#include <QPainter>
#include <QPointF>
#include <QRadialGradient>

namespace cf::desktop::desktop_component {

namespace {
// Fixed dial geometry, ported verbatim from CCIMXDesktop. The painter is
// scaled so the dial fits the widget.
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
inline constexpr int kHandWidth = 7;

inline constexpr int kNumberDistanceFromCenter = 70;
inline constexpr int kNumberFontSize = 14;

inline constexpr double kHourRotationPerHour = 30.0;
inline constexpr double kMinuteRotationPerMinute = 6.0;
inline constexpr double kSecondRotationPerSecond = 6.0;
} // namespace

AnalogClockWidget::AnalogClockWidget(QWidget* parent) : QWidget(parent) {
    setAttribute(Qt::WA_TranslucentBackground);
    current_time_ = QTime::currentTime();
    connect(&GlobalClockSources::instance(), &GlobalClockSources::timeUpdate, this,
            &AnalogClockWidget::onTimeUpdate);
}

void AnalogClockWidget::onTimeUpdate(const QTime& time) {
    current_time_ = time;
    update();
}

void AnalogClockWidget::paintEvent(QPaintEvent* /*event*/) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    const int side = qMin(width(), height());
    p.translate(width() / 2.0, height() / 2.0);
    p.scale(side / static_cast<double>(kDialSize), side / static_cast<double>(kDialSize));

    drawBackground(&p);
    drawNumbers(&p);
    drawTicks(&p);
    drawHands(&p);
    drawCenterDot(&p);
}

void AnalogClockWidget::drawBackground(QPainter* p) {
    p->save();
    p->setPen(Qt::NoPen);

    // 8px black thick outer border.
    p->setPen(QPen(QColor(0, 0, 0), kThickBorderWidth));
    p->setBrush(Qt::NoBrush);
    p->drawEllipse(QPoint(0, 0), kOuterCircleRadius - kThickBorderWidth / 2,
                   kOuterCircleRadius - kThickBorderWidth / 2);

    // White radial gradient dial face.
    QRadialGradient gradient(0, 0, kOuterCircleRadius);
    gradient.setColorAt(0.0, QColor(255, 255, 255));
    gradient.setColorAt(1.0, QColor(230, 230, 230));
    p->setBrush(gradient);
    p->setPen(Qt::NoPen);
    p->drawEllipse(QPoint(0, 0), kOuterCircleRadius, kOuterCircleRadius);

    // Thin gray rim.
    p->setPen(QPen(QColor(200, 200, 200), 2));
    p->setBrush(Qt::NoBrush);
    p->drawEllipse(QPoint(0, 0), kOuterCircleRadius, kOuterCircleRadius);
    p->restore();
}

void AnalogClockWidget::drawNumbers(QPainter* p) {
    p->save();
    QFont font = p->font();
    font.setPointSize(kNumberFontSize);
    p->setFont(font);
    p->setPen(Qt::black);

    struct NumberInfo {
        int number;
        double angle_degree;
    };
    static constexpr NumberInfo numbers[] = {
        {12, 0.0},
        {3, 90.0},
        {6, 180.0},
        {9, 270.0},
    };
    for (const auto& num : numbers) {
        const double radians = qDegreesToRadians(num.angle_degree);
        const double x = kNumberDistanceFromCenter * qSin(radians);
        const double y = -kNumberDistanceFromCenter * qCos(radians);
        const QRectF rect(x - 10, y - 10, 20, 20);
        p->drawText(rect, Qt::AlignCenter, QString::number(num.number));
    }
    p->restore();
}

void AnalogClockWidget::drawTicks(QPainter* p) {
    p->save();
    p->setPen(QPen(Qt::black, kHourTickWidth));
    for (int i = 0; i < 12; ++i) {
        p->drawLine(0, -(kOuterCircleRadius - kHourTickLength), 0, -kOuterCircleRadius);
        p->rotate(kHourRotationPerHour);
    }
    p->setPen(QPen(Qt::gray, kMinuteTickWidth));
    for (int i = 0; i < 60; ++i) {
        if (i % 5 != 0) {
            p->drawLine(0, -(kOuterCircleRadius - kMinuteTickLength), 0, -kOuterCircleRadius);
        }
        p->rotate(kMinuteRotationPerMinute);
    }
    p->restore();
}

void AnalogClockWidget::drawHands(QPainter* p) {
    // Hour hand (black).
    p->save();
    p->setPen(Qt::NoPen);
    p->setBrush(Qt::black);
    p->rotate(kHourRotationPerHour * (current_time_.hour() % 12) + current_time_.minute() / 2.0);
    static const QPoint hour_hand[4] = {
        QPoint(0, kHandWidth),
        QPoint(-4, 0),
        QPoint(0, -kHourHandLength),
        QPoint(4, 0),
    };
    p->drawConvexPolygon(hour_hand, 4);
    p->restore();

    // Minute hand (black).
    p->save();
    p->setPen(Qt::NoPen);
    p->setBrush(Qt::black);
    p->rotate(kMinuteRotationPerMinute * current_time_.minute() + current_time_.second() / 10.0);
    static const QPoint minute_hand[4] = {
        QPoint(0, kHandWidth),
        QPoint(-3, 0),
        QPoint(0, -kMinuteHandLength),
        QPoint(3, 0),
    };
    p->drawConvexPolygon(minute_hand, 4);
    p->restore();

    // Second hand (red).
    p->save();
    p->setPen(Qt::NoPen);
    p->setBrush(Qt::red);
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
    p->setBrush(Qt::black);
    p->drawEllipse(QPoint(0, 0), kCenterDotRadius, kCenterDotRadius);
    p->restore();
}

} // namespace cf::desktop::desktop_component
