/**
 * @file    gauge_widget.cpp
 * @brief   Implementation of the GaugeWidget (ported verbatim from CCIMX).
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup home_page
 */

#include "gauge_widget.h"

#include <QPaintEvent>
#include <QPainter>
#include <QPropertyAnimation>

namespace cf::desktop::desktop_component {

GaugeWidget::GaugeWidget(QWidget* parent) : QWidget(parent) {
    setMinimumSize(WIDGET_MIN_WIDTH, WIDGET_MIN_HEIGHT);
}

void GaugeWidget::update_value(const double value) {
    auto* ani = new QPropertyAnimation(this, "value");
    ani->setDuration(ANIMATION_DURATION);
    ani->setStartValue(current_value);
    ani->setEndValue(value);
    ani->setEasingCurve(QEasingCurve::InOutCubic);
    ani->start(QAbstractAnimation::DeleteWhenStopped);
}

void GaugeWidget::paintEvent(QPaintEvent* /*event*/) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const int side = qMin(width(), height());
    // Move to the widget center; the dial is drawn in the reference
    // coordinate system and scaled to fit.
    p.translate(width() / 2.0, height() / 2.0);
    p.scale(side / WIDGET_MIN_WIDTH, side / WIDGET_MIN_HEIGHT);

    drawBackground(p);
    drawArc(p);
    drawTicks(p);
    drawLabels(p);
    drawNeedle(p);
    drawCenter(p);
    drawTexts(p);
}

void GaugeWidget::drawBackground(QPainter& p) {
    const float left_top_x = -WIDGET_MIN_WIDTH / 2;
    const float left_top_y = -WIDGET_MIN_HEIGHT / 2;
    const float right_bottom_x = WIDGET_MIN_WIDTH / 2;
    const float right_bottom_y = WIDGET_MIN_HEIGHT / 2;

    QPen border_pen(BOARD_COLOR, BOARD_LEN);
    p.setPen(border_pen);
    p.setBrush(Qt::NoBrush);
    p.drawEllipse(-WIDGET_MIN_WIDTH / 2, -WIDGET_MIN_HEIGHT / 2, WIDGET_MIN_WIDTH,
                  WIDGET_MIN_HEIGHT);

    QLinearGradient bg(left_top_x, left_top_y, right_bottom_x, right_bottom_y);
    bg.setColorAt(0, FROM_BOARD_COLOR);
    bg.setColorAt(1, TO_BOARD_COLOR);
    p.setBrush(bg);
    p.setPen(Qt::NoPen);
    p.drawEllipse(left_top_x + BOARD_LEN, left_top_y + BOARD_LEN, WIDGET_MIN_WIDTH - 2 * BOARD_LEN,
                  WIDGET_MIN_HEIGHT - 2 * BOARD_LEN);

    QRadialGradient glow(0, 0, WIDGET_MIN_WIDTH / 2);
    glow.setColorAt(0, QColor(255, 255, 255, 80));
    glow.setColorAt(1, Qt::transparent);
    p.setBrush(glow);
    p.drawEllipse(left_top_x + BOARD_LEN, left_top_y + BOARD_LEN, WIDGET_MIN_WIDTH - 2 * BOARD_LEN,
                  WIDGET_MIN_HEIGHT - 2 * BOARD_LEN);
}

void GaugeWidget::drawArc(QPainter& p) {
    QConicalGradient cg(0, 0, START_ANGLE);
    cg.setColorAt(0.0, START_COLOR);
    cg.setColorAt(1.0, END_COLOR);
    QPen arc_pen(cg, COLOR_GRAD_WIDTH, Qt::SolidLine, Qt::FlatCap);
    p.setPen(arc_pen);
    const QRectF arc_rect(-COLOR_RADIUS, -COLOR_RADIUS, 2 * COLOR_RADIUS, 2 * COLOR_RADIUS);
    p.drawArc(arc_rect, START_ANGLE * 16, -TOTAL_ANGLE * 16);
}

void GaugeWidget::drawTicks(QPainter& p) {
    static constexpr int EACH_ANGLE = TOTAL_ANGLE / (TICK_CNT - 1);
    for (int i = 0; i <= TICK_CNT - 1; ++i) {
        p.save();
        const double ang = START_ANGLE + i * EACH_ANGLE;
        p.rotate(ang);
        if (i % SUB_MAIN_RATE == 0) {
            p.setPen(QPen(TICK_COLOR, MAIN_TICK_WIDTH));
            p.drawLine(0, -COLOR_RADIUS + MAIN_TICK_LENGTH, 0, -COLOR_RADIUS);
        } else {
            p.setPen(QPen(TICK_COLOR, SUB_TICK_WIDTH));
            p.drawLine(0, -COLOR_RADIUS + SUB_TICK_LENGTH, 0, -COLOR_RADIUS);
        }
        p.restore();
    }
}

void GaugeWidget::drawLabels(QPainter& p) {
    p.setFont(label_font);
    static constexpr int EACH_ANGLE = TOTAL_ANGLE / (TICK_CNT - 1);
    static constexpr short MAIN_TICKS_N = TICK_CNT / EACH_ANGLE;
    for (int i = 0; i <= MAIN_TICKS_N - 1; ++i) {
        p.save();
        const double ang = START_ANGLE + i * TOTAL_ANGLE / 10;
        p.rotate(ang);
        p.translate(0, -70);
        p.rotate(-ang);
        const int val = min_value + i * (max_value - min_value) / 10;
        p.setPen(LABEL_COLOR);
        p.drawText(QRectF(-15, -10, 30, 20), Qt::AlignCenter, QString::number(val));
        p.restore();
    }
}

void GaugeWidget::drawNeedle(QPainter& p) {
    static constexpr int start = 225;
    static constexpr int tol = 270;
    const double range = max_value - min_value;
    const double ratio = (range != 0) ? double(current_value - min_value) / range : 0.0;
    const double needle_ang = start + ratio * tol;

    // Drop shadow.
    p.save();
    p.rotate(needle_ang);
    p.translate(2, 2);
    p.setBrush(QColor(0, 0, 0, 80));
    p.setPen(Qt::NoPen);
    static const QPointF shadow[3] = {{0, -70}, {-5, 0}, {5, 0}};
    p.drawConvexPolygon(shadow, 3);
    p.restore();

    // Needle body.
    p.save();
    p.rotate(needle_ang);
    QLinearGradient ng(0, -70, 0, 0);
    ng.setColorAt(0, QColor(255, 120, 120));
    ng.setColorAt(1, QColor(180, 0, 0));
    p.setBrush(ng);
    p.setPen(Qt::darkRed);
    static const QPointF needle[3] = {{0, -70}, {-5, 0}, {5, 0}};
    p.drawConvexPolygon(needle, 3);
    p.restore();
}

void GaugeWidget::drawCenter(QPainter& p) {
    QRadialGradient cg2(0, 0, 10);
    cg2.setColorAt(0, Qt::white);
    cg2.setColorAt(1, QColor(200, 200, 200));
    p.setBrush(cg2);
    p.setPen(Qt::gray);
    p.drawEllipse(-7, -7, 14, 14);
}

void GaugeWidget::drawTexts(QPainter& p) {
    p.setPen(Qt::black);
    p.setFont(title_font);
    p.drawText(QRectF(-40, -60, 80, 20), Qt::AlignCenter, title);
    p.setFont(value_font);
    const QString txt = QString::number(current_value, 'f', 0) + " " + unit;
    p.drawText(QRectF(-60, 60, 120, 30), Qt::AlignCenter, txt);
}

} // namespace cf::desktop::desktop_component
