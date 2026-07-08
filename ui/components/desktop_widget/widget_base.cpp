/**
 * @file    widget_base.cpp
 * @brief   Implementation of the draggable WidgetBase.
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup desktop_widget
 */

#include "widget_base.h"

#include <QMouseEvent>

namespace cf::desktop::desktop_component {

namespace {
/// @brief Manhattan distance the cursor must travel before a press becomes a
///        drag, so a static click does not nudge the widget.
inline constexpr int kDragThresholdPx = 5;
} // namespace

WidgetBase::WidgetBase(QWidget* parent) : QWidget(parent) {}

void WidgetBase::setEditable(bool editable) {
    editable_ = editable;
}

bool WidgetBase::isEditable() const {
    return editable_;
}

void WidgetBase::mousePressEvent(QMouseEvent* event) {
    if (editable_ && event->button() == Qt::LeftButton) {
        drag_anchor_ = event->pos();
        dragging_ = false;
    }
    QWidget::mousePressEvent(event);
}

void WidgetBase::mouseMoveEvent(QMouseEvent* event) {
    if (editable_ && (event->buttons() & Qt::LeftButton) != 0) {
        const QPoint delta = event->pos() - drag_anchor_;
        if (!dragging_ && delta.manhattanLength() >= kDragThresholdPx) {
            dragging_ = true;
        }
        if (dragging_) {
            move(pos() + delta);
            emit widgetMoved(pos());
        }
    }
    QWidget::mouseMoveEvent(event);
}

void WidgetBase::mouseReleaseEvent(QMouseEvent* event) {
    dragging_ = false;
    QWidget::mouseReleaseEvent(event);
}

} // namespace cf::desktop::desktop_component
