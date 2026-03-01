/**
 * @file    cfmaterial_slide_animation.cpp
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief   Material Design 3 Slide Animation Implementation
 * @version 0.1
 * @date    2026-02-28
 *
 * @copyright Copyright (c) 2026
 *
 * @details
 * Implements the CFMaterialSlideAnimation class for position-based animations
 * following Material Design 3 motion specifications.
 */

#include "cfmaterial_slide_animation.h"
#include "base/easing.h"
#include <QDebug>
#include <QEasingCurve>

namespace cf::ui::components::material {

// =============================================================================
// Constructor / Destructor
// =============================================================================

CFMaterialSlideAnimation::CFMaterialSlideAnimation(cf::ui::core::IMotionSpec* spec,
                                                   SlideDirection direction, QObject* parent)
    : ICFTimingAnimation(spec, parent), currentOffset_(0.0f), targetWidget_(nullptr),
      direction_(direction), distance_(100.0f), durationMs_(300), delayMs_(0), elapsedTime_(0) {}

CFMaterialSlideAnimation::~CFMaterialSlideAnimation() {
    // Restore original position if widget still exists
    if (targetWidget_ && !originalPosition_.isNull()) {
        targetWidget_->move(originalPosition_);
    }
}

// =============================================================================
// ICFAbstractAnimation Interface
// =============================================================================

void CFMaterialSlideAnimation::start(Direction dir) {
    if (m_state == State::Running) {
        return; // Already running
    }

    if (!targetWidget_) {
        qWarning() << "CFMaterialSlideAnimation::start: No target widget set";
        return;
    }

    // Store original position
    originalPosition_ = targetWidget_->pos();

    // Get duration from motion spec if available
    if (motion_spec_) {
        // Duration would be queried from the motion token name
        // For now, use default based on direction
        durationMs_ = 300; // mediumEnter default
    }

    elapsedTime_ = 0;
    m_state = State::Running;

    // Set initial offset
    if (dir == Direction::Forward) {
        currentOffset_ = m_from;
    } else {
        currentOffset_ = m_to;
    }

    emit started();

    // Apply initial offset
    applyOffset(currentOffset_);
}

void CFMaterialSlideAnimation::pause() {
    if (m_state == State::Running) {
        m_state = State::Paused;
        emit paused();
    }
}

void CFMaterialSlideAnimation::stop() {
    m_state = State::Idle;
    elapsedTime_ = 0;

    // Reset to original position
    currentOffset_ = m_from;
    if (targetWidget_ && !originalPosition_.isNull()) {
        targetWidget_->move(originalPosition_);
    }

    emit stopped();
}

void CFMaterialSlideAnimation::reverse() {
    if (m_state == State::Running) {
        // Calculate remaining progress and reverse from there
        float progress = static_cast<float>(elapsedTime_) / durationMs_;
        progress = 1.0f - progress; // Reverse progress

        elapsedTime_ = static_cast<int>(progress * durationMs_);
    } else {
        // Start in reverse direction
        start(Direction::Backward);
    }

    emit reversed();
}

bool CFMaterialSlideAnimation::tick(int dt) {
    if (m_state != State::Running || !targetWidget_) {
        return false;
    }

    elapsedTime_ += dt;

    // Check if we're still in delay period
    if (elapsedTime_ < delayMs_) {
        return true; // Still waiting
    }

    // Calculate actual animation time (subtract delay)
    int animTime = elapsedTime_ - delayMs_;

    // Calculate progress
    float progress = static_cast<float>(animTime) / durationMs_;

    // Clamp progress
    if (progress > 1.0f) {
        progress = 1.0f;
    }

    // Apply easing
    float easedProgress = calculateEasedProgress(progress);

    // Calculate current offset based on range
    currentOffset_ = m_from + (m_to - m_from) * easedProgress;

    // Apply offset
    applyOffset(currentOffset_);

    // Emit progress signal
    emit progressChanged(easedProgress);

    // Check if animation is complete
    if (progress >= 1.0f) {
        m_state = State::Finished;
        emit finished();
        return false;
    }

    return true;
}

// =============================================================================
// Slide-Specific Methods
// =============================================================================

void CFMaterialSlideAnimation::setTargetWidget(QWidget* widget) {
    targetWidget_ = widget;
}

void CFMaterialSlideAnimation::applyOffset(float offset) {
    if (!targetWidget_ || originalPosition_.isNull()) {
        return;
    }

    // Calculate offset point
    QPoint offsetPoint = calculateOffsetPoint(offset);

    // Apply new position
    targetWidget_->move(originalPosition_ + offsetPoint);
}

QPoint CFMaterialSlideAnimation::calculateOffsetPoint(float offset) const {
    switch (direction_) {
        case SlideDirection::Up:
            return QPoint(0, static_cast<int>(-offset)); // Move up (negative Y)

        case SlideDirection::Down:
            return QPoint(0, static_cast<int>(offset)); // Move down (positive Y)

        case SlideDirection::Left:
            return QPoint(static_cast<int>(-offset), 0); // Move left (negative X)

        case SlideDirection::Right:
            return QPoint(static_cast<int>(offset), 0); // Move right (positive X)

        default:
            return QPoint();
    }
}

float CFMaterialSlideAnimation::calculateEasedProgress(float linearProgress) const {
    // For now, use simple linear easing
    // TODO: Integrate with MaterialMotionScheme for proper easing curves
    return linearProgress;
}

} // namespace cf::ui::components::material
