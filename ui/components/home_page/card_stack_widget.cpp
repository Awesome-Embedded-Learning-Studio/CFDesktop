/**
 * @file    card_stack_widget.cpp
 * @brief   Implementation of the swipeable CardStackWidget.
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup home_page
 */

#include "card_stack_widget.h"

#include <QGraphicsOpacityEffect>
#include <QMouseEvent>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QWidget>

namespace cf::desktop::desktop_component {

QGraphicsOpacityEffect* CardStackWidget::ensureOpacity(QWidget* w) {
    auto* eff = qobject_cast<QGraphicsOpacityEffect*>(w->graphicsEffect());
    if (eff == nullptr) {
        eff = new QGraphicsOpacityEffect(w);
        w->setGraphicsEffect(eff);
    }
    return eff;
}

CardStackWidget::CardStackWidget(QWidget* parent)
    : QStackedWidget(parent), current_animation_(new QPropertyAnimation(this)),
      next_animation_(new QPropertyAnimation(this)), group_(new QParallelAnimationGroup(this)) {
    current_animation_->setPropertyName("pos");
    next_animation_->setPropertyName("pos");
    group_->addAnimation(current_animation_);
    group_->addAnimation(next_animation_);
}

void CardStackWidget::setCardTransitionMode(CardTransitionMode mode) {
    transition_mode_ = mode;
}

CardStackWidget::CardTransitionMode CardStackWidget::cardTransitionMode() const {
    return transition_mode_;
}

void CardStackWidget::setAnimationDuration(int duration_ms) {
    animation_duration_ = duration_ms;
}

int CardStackWidget::animationDuration() const {
    return animation_duration_;
}

void CardStackWidget::slideTo(int new_index) {
    slideTo(new_index, currentIndex() > new_index);
}

void CardStackWidget::mousePressEvent(QMouseEvent* event) {
    start_pos_ = event->pos();
    dragging_ = true;
}

void CardStackWidget::mouseMoveEvent(QMouseEvent* event) {
    if (count() == 0 || !dragging_) {
        return;
    }
    const int dy = event->pos().y() - start_pos_.y();
    if (qAbs(dy) < 5) {
        return;
    }
    QWidget* current_widget = currentWidget();
    const int index = currentIndex();
    QWidget* other_wid = nullptr;

    if (dy > 0 && index > 0) {
        other_wid = widget(index - 1);
        other_wid->move(0, -height() + dy); // slide down reveals the previous card
    } else if (dy < 0 && index < count() - 1) {
        other_wid = widget(index + 1);
        other_wid->move(0, height() + dy); // slide up reveals the next card
    }

    if (other_wid != nullptr) {
        other_wid->resize(current_widget->size());
        other_wid->show();
        other_wid->raise();
    }
    current_widget->move(0, dy);
}

void CardStackWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (count() == 0 || !dragging_) {
        return;
    }
    dragging_ = false;

    const int dy = event->pos().y() - start_pos_.y();
    const int threshold = height() / 4; // 1/4 height commit threshold
    const int idx = currentIndex();

    if (dy > threshold) {
        if (idx > 0) {
            slideTo(idx - 1, false);
        } else {
            bounceBack(dy, idx);
        }
    } else if (dy < -threshold) {
        if (idx < count() - 1) {
            slideTo(idx + 1, true);
        } else {
            bounceBack(dy, idx);
        }
    } else {
        bounceBack(dy, idx);
    }
}

void CardStackWidget::bounceBack(int dy, int idx) {
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

    if (dy > 0 && idx > 0) {
        QWidget* prev = widget(idx - 1);
        next_animation_->setTargetObject(prev);
        next_animation_->setPropertyName("pos");
        next_animation_->setDuration(animation_duration_);
        next_animation_->setStartValue(prev->pos());
        next_animation_->setEndValue(QPoint(0, -height()));
        hide_after_animation_ = prev;
    } else if (dy < 0 && idx < count() - 1) {
        QWidget* next = widget(idx + 1);
        next_animation_->setTargetObject(next);
        next_animation_->setPropertyName("pos");
        next_animation_->setDuration(animation_duration_);
        next_animation_->setStartValue(next->pos());
        next_animation_->setEndValue(QPoint(0, height()));
        hide_after_animation_ = next;
    }
    if (next_animation_->targetObject() != nullptr) {
        group_->start();
    } else {
        current_animation_->start();
    }
}

void CardStackWidget::slideTo(int new_index, bool slide_up) {
    if (new_index < 0 || new_index >= count()) {
        return;
    }
    if (new_index == currentIndex()) {
        return;
    }

    QWidget* curr = currentWidget();
    QWidget* next = widget(new_index);
    const int h = height();

    next->move(0, slide_up ? h : -h);
    next->show();
    next->raise();

    next_animation_->stop();
    next_animation_->setTargetObject(next);
    next_animation_->setDuration(animation_duration_);
    next_animation_->setStartValue(next->pos());
    next_animation_->setEndValue(QPoint(0, 0));
    next_animation_->setPropertyName("pos");

    if (transition_mode_ == CardTransitionMode::Fade) {
        auto* eff = ensureOpacity(curr);
        eff->setOpacity(1.0);

        current_animation_->stop();
        current_animation_->setTargetObject(eff);
        current_animation_->setPropertyName("opacity");
        current_animation_->setDuration(animation_duration_);
        current_animation_->setStartValue(1.0);
        current_animation_->setEndValue(0.0);
    } else {
        current_animation_->stop();
        current_animation_->setTargetObject(curr);
        current_animation_->setPropertyName("pos");
        current_animation_->setDuration(animation_duration_);
        current_animation_->setStartValue(curr->pos());
        current_animation_->setEndValue(QPoint(0, slide_up ? -h : h));
    }

    hide_after_animation_ = curr;

    disconnect(group_, nullptr, nullptr, nullptr);
    connect(group_, &QParallelAnimationGroup::finished, this, [this, new_index]() {
        if (hide_after_animation_) {
            hide_after_animation_->hide();
            hide_after_animation_->move(0, 0);
            if (auto* eff = qobject_cast<QGraphicsOpacityEffect*>(
                    hide_after_animation_->graphicsEffect())) {
                eff->setOpacity(1.0);
            }
            hide_after_animation_.clear();
        }
        setCurrentIndex(new_index);
    });

    group_->start();
}

} // namespace cf::desktop::desktop_component
