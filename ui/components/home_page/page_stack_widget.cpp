/**
 * @file    page_stack_widget.cpp
 * @brief   Implementation of the horizontal-swipe PageStackWidget.
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup home_page
 */

#include "page_stack_widget.h"

#include <QMouseEvent>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QWidget>

namespace cf::desktop::desktop_component {

PageStackWidget::PageStackWidget(QWidget* parent)
    : QStackedWidget(parent), current_animation_(new QPropertyAnimation(this)),
      next_animation_(new QPropertyAnimation(this)), group_(new QParallelAnimationGroup(this)) {
    current_animation_->setPropertyName("pos");
    next_animation_->setPropertyName("pos");
    group_->addAnimation(current_animation_);
    group_->addAnimation(next_animation_);
}

void PageStackWidget::setAnimationDuration(int duration_ms) {
    animation_duration_ = duration_ms;
}

int PageStackWidget::animationDuration() const {
    return animation_duration_;
}

void PageStackWidget::slideToPage(int new_index) {
    // Moving to a higher index flips the next page in from the right (to_left).
    slideToPage(new_index, currentIndex() < new_index);
}

void PageStackWidget::toNextPage() {
    if (currentIndex() < count() - 1) {
        slideToPage(currentIndex() + 1, true);
    }
}

void PageStackWidget::toPrevPage() {
    if (currentIndex() > 0) {
        slideToPage(currentIndex() - 1, false);
    }
}

void PageStackWidget::mousePressEvent(QMouseEvent* event) {
    start_pos_ = event->pos();
    dragging_ = true;
}

void PageStackWidget::mouseMoveEvent(QMouseEvent* event) {
    if (count() == 0 || !dragging_) {
        return;
    }
    const int dx = event->pos().x() - start_pos_.x();
    if (qAbs(dx) < 5) {
        return;
    }
    QWidget* current_widget = currentWidget();
    const int index = currentIndex();
    QWidget* other_wid = nullptr;

    if (dx > 0 && index > 0) {
        other_wid = widget(index - 1);
        other_wid->move(-width() + dx, 0); // right drag reveals the previous (left) page
    } else if (dx < 0 && index < count() - 1) {
        other_wid = widget(index + 1);
        other_wid->move(width() + dx, 0); // left drag reveals the next (right) page
    }

    if (other_wid != nullptr) {
        other_wid->resize(current_widget->size());
        other_wid->show();
        other_wid->raise();
    }
    current_widget->move(dx, 0);
}

void PageStackWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (count() == 0 || !dragging_) {
        return;
    }
    dragging_ = false;

    const int dx = event->pos().x() - start_pos_.x();
    const int threshold = width() / 4; // 1/4 width commit threshold
    const int idx = currentIndex();

    if (dx > threshold) {
        if (idx > 0) {
            slideToPage(idx - 1, false); // right drag -> previous page
        } else {
            bounceBack(dx, idx);
        }
    } else if (dx < -threshold) {
        if (idx < count() - 1) {
            slideToPage(idx + 1, true); // left drag -> next page
        } else {
            bounceBack(dx, idx);
        }
    } else {
        bounceBack(dx, idx);
    }
}

void PageStackWidget::bounceBack(int dx, int idx) {
    QWidget* curr = currentWidget();

    current_animation_->stop();
    current_animation_->setTargetObject(curr);
    current_animation_->setPropertyName("pos");
    current_animation_->setDuration(animation_duration_);
    current_animation_->setStartValue(curr->pos());
    current_animation_->setEndValue(QPoint(0, 0));

    next_animation_->stop();
    next_animation_->setTargetObject(nullptr);
    hide_after_animation_.clear();

    if (dx > 0 && idx > 0) {
        QWidget* prev = widget(idx - 1);
        next_animation_->setTargetObject(prev);
        next_animation_->setPropertyName("pos");
        next_animation_->setDuration(animation_duration_);
        next_animation_->setStartValue(prev->pos());
        next_animation_->setEndValue(QPoint(-width(), 0));
        hide_after_animation_ = prev;
    } else if (dx < 0 && idx < count() - 1) {
        QWidget* next = widget(idx + 1);
        next_animation_->setTargetObject(next);
        next_animation_->setPropertyName("pos");
        next_animation_->setDuration(animation_duration_);
        next_animation_->setStartValue(next->pos());
        next_animation_->setEndValue(QPoint(width(), 0));
        hide_after_animation_ = next;
    }
    if (next_animation_->targetObject() != nullptr) {
        group_->start();
    } else {
        current_animation_->start();
    }
}

void PageStackWidget::slideToPage(int new_index, bool to_left) {
    if (new_index < 0 || new_index >= count()) {
        return;
    }
    if (new_index == currentIndex()) {
        return;
    }

    QWidget* curr = currentWidget();
    QWidget* next = widget(new_index);
    const int w = width();

    next->move(to_left ? w : -w, 0);
    next->show();
    next->raise();

    next_animation_->stop();
    next_animation_->setTargetObject(next);
    next_animation_->setDuration(animation_duration_);
    next_animation_->setStartValue(next->pos());
    next_animation_->setEndValue(QPoint(0, 0));
    next_animation_->setPropertyName("pos");

    current_animation_->stop();
    current_animation_->setTargetObject(curr);
    current_animation_->setPropertyName("pos");
    current_animation_->setDuration(animation_duration_);
    current_animation_->setStartValue(curr->pos());
    current_animation_->setEndValue(QPoint(to_left ? -w : w, 0));

    hide_after_animation_ = curr;

    disconnect(group_, nullptr, nullptr, nullptr);
    connect(group_, &QParallelAnimationGroup::finished, this, [this, new_index]() {
        if (hide_after_animation_) {
            hide_after_animation_->hide();
            hide_after_animation_->move(0, 0);
            hide_after_animation_.clear();
        }
        setCurrentIndex(new_index);
    });

    group_->start();
}

} // namespace cf::desktop::desktop_component
