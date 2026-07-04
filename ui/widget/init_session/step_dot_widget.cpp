/**
 * @file    step_dot_widget.cpp
 * @brief   Custom widget for drawing step indicator dots.
 *
 * @author  CFDesktop Team
 * @date    2025-03-25
 * @version 0.1
 * @since   0.1
 * @ingroup desktop_ui_widget_init_session
 */

#include "step_dot_widget.h"

namespace cf::desktop::init_session {

StepDotWidget::StepDotWidget(int index, const QString& name, QWidget* parent)
    : QWidget(parent), m_index(index), m_name(name), m_state(Inactive) {
    setFixedSize(10, 10); // STEP_DOT_SIZE
}

int StepDotWidget::stepIndex() const {
    return m_index;
}

QString StepDotWidget::stepName() const {
    return m_name;
}

StepDotWidget::State StepDotWidget::state() const {
    return m_state;
}

void StepDotWidget::setState(State s) {
    if (m_state != s) {
        m_state = s;
        update();
    }
}

void StepDotWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRectF rect = this->rect().adjusted(1, 1, -1, -1);

    // Choose color based on state
    QColor color;
    switch (m_state) {
        case Active:
            color = QColor(0xFF6751A4); // STEP_ACTIVE_COLOR
            break;
        case Completed:
            color = QColor(0xFF6751A4); // STEP_COMPLETED_COLOR
            break;
        default:
            color = QColor(0xFFE0E0E0); // STEP_INACTIVE_COLOR
            break;
    }

    // Draw the dot
    QPainterPath path;
    path.addEllipse(rect);
    painter.fillPath(path, color);

    // Draw checkmark for completed state
    if (m_state == Completed) {
        painter.setPen(QPen(Qt::white, 1.5));
        QPainterPath checkmark;
        checkmark.moveTo(rect.width() * 0.3, rect.height() * 0.5);
        checkmark.lineTo(rect.width() * 0.45, rect.height() * 0.65);
        checkmark.lineTo(rect.width() * 0.7, rect.height() * 0.35);
        painter.drawPath(checkmark);
    }
}

} // namespace cf::desktop::init_session
