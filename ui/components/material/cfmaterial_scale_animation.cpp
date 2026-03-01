/**
 * @file    cfmaterial_scale_animation.cpp
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief   Material Design 3 Scale Animation Implementation
 * @version 0.1
 * @date    2026-02-28
 *
 * @copyright Copyright (c) 2026
 *
 * @details
 * Implements the CFMaterialScaleAnimation class for size-based animations
 * following Material Design 3 motion specifications.
 */

#include "cfmaterial_scale_animation.h"
#include <QDebug>
#include <QEasingCurve>

namespace cf::ui::components::material {

// =============================================================================
// Constructor / Destructor
// =============================================================================

CFMaterialScaleAnimation::CFMaterialScaleAnimation(cf::ui::core::IMotionSpec* spec, QObject* parent)
    : ICFTimingAnimation(spec, parent), currentScale_(1.0f), targetWidget_(nullptr),
      scaleFromCenter_(true), durationMs_(200), delayMs_(0), elapsedTime_(0) {}

CFMaterialScaleAnimation::~CFMaterialScaleAnimation() {
    // Restore original geometry if widget still exists
    if (targetWidget_ && !originalGeometry_.isEmpty()) {
        targetWidget_->setGeometry(originalGeometry_);
    }
}

// =============================================================================
// ICFAbstractAnimation Interface
// =============================================================================

void CFMaterialScaleAnimation::start(Direction dir) {
    if (m_state == State::Running) {
        return; // Already running
    }

    if (!targetWidget_) {
        qWarning() << "CFMaterialScaleAnimation::start: No target widget set";
        return;
    }

    // Store original geometry
    originalGeometry_ = targetWidget_->geometry();

    // Get duration from motion spec if available
    if (motion_spec_) {
        // Duration would be queried from the motion token name
        // For now, use default
        durationMs_ = 200; // shortEnter default
    }

    elapsedTime_ = 0;
    m_state = State::Running;

    // Set initial scale
    if (dir == Direction::Forward) {
        currentScale_ = m_from;
    } else {
        currentScale_ = m_to;
    }

    emit started();

    // Apply initial scale
    applyScale(currentScale_);
}

void CFMaterialScaleAnimation::pause() {
    if (m_state == State::Running) {
        m_state = State::Paused;
        emit paused();
    }
}

void CFMaterialScaleAnimation::stop() {
    m_state = State::Idle;
    elapsedTime_ = 0;

    // Reset to original geometry
    currentScale_ = m_from;
    if (targetWidget_ && !originalGeometry_.isEmpty()) {
        targetWidget_->setGeometry(originalGeometry_);
    }

    emit stopped();
}

void CFMaterialScaleAnimation::reverse() {
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

bool CFMaterialScaleAnimation::tick(int dt) {
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

    // Calculate current scale based on range
    currentScale_ = m_from + (m_to - m_from) * easedProgress;

    // Apply scale
    applyScale(currentScale_);

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
// Scale-Specific Methods
// =============================================================================

void CFMaterialScaleAnimation::setTargetWidget(QWidget* widget) {
    targetWidget_ = widget;
}

void CFMaterialScaleAnimation::applyScale(float scale) {
    if (!targetWidget_ || originalGeometry_.isEmpty()) {
        return;
    }

    // Calculate scaled geometry
    QRect scaledGeometry = calculateScaledGeometry(scale);

    // Apply new geometry
    targetWidget_->setGeometry(scaledGeometry);
}

QRect CFMaterialScaleAnimation::calculateScaledGeometry(float scale) const {
    if (originalGeometry_.isEmpty()) {
        return QRect();
    }

    // Calculate new size
    int newWidth = static_cast<int>(originalGeometry_.width() * scale);
    int newHeight = static_cast<int>(originalGeometry_.height() * scale);

    // Ensure minimum size
    newWidth = qMax(1, newWidth);
    newHeight = qMax(1, newHeight);

    if (scaleFromCenter_) {
        // Calculate position to maintain center point
        int widthDiff = newWidth - originalGeometry_.width();
        int heightDiff = newHeight - originalGeometry_.height();

        int newX = originalGeometry_.x() - widthDiff / 2;
        int newY = originalGeometry_.y() - heightDiff / 2;

        return QRect(newX, newY, newWidth, newHeight);
    } else {
        // Scale from top-left corner
        return QRect(originalGeometry_.x(), originalGeometry_.y(), newWidth, newHeight);
    }
}

float CFMaterialScaleAnimation::calculateEasedProgress(float linearProgress) const {
    // For now, use simple linear easing
    // TODO: Integrate with MaterialMotionScheme for proper easing curves
    return linearProgress;
}

} // namespace cf::ui::components::material
