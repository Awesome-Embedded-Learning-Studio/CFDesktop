/**
 * @file    cfmaterial_fade_animation.cpp
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief   Material Design 3 Fade Animation Implementation
 * @version 0.1
 * @date    2026-02-28
 *
 * @copyright Copyright (c) 2026
 *
 * @details
 * Implements the CFMaterialFadeAnimation class for opacity-based animations
 * following Material Design 3 motion specifications.
 */

#include "cfmaterial_fade_animation.h"
#include "base/easing.h"
#include <QDebug>
#include <QEasingCurve>

namespace cf::ui::components::material {

// =============================================================================
// Constructor / Destructor
// =============================================================================

CFMaterialFadeAnimation::CFMaterialFadeAnimation(cf::ui::core::IMotionSpec* spec, QObject* parent)
    : ICFTimingAnimation(spec, parent), currentOpacity_(1.0f), targetWidget_(nullptr),
      opacityEffect_(nullptr), ownsOpacityEffect_(false), durationMs_(200), delayMs_(0),
      elapsedTime_(0) {}

CFMaterialFadeAnimation::~CFMaterialFadeAnimation() {
    // Clean up opacity effect if we own it
    if (opacityEffect_ && ownsOpacityEffect_ && targetWidget_) {
        targetWidget_->setGraphicsEffect(nullptr);
    }
    delete opacityEffect_;
}

// =============================================================================
// ICFAbstractAnimation Interface
// =============================================================================

void CFMaterialFadeAnimation::start(Direction dir) {
    if (m_state == State::Running) {
        return; // Already running
    }

    // Get duration from motion spec if available
    if (motion_spec_) {
        // Duration would be queried from the motion token name
        // For now, use default
        durationMs_ = 200; // shortEnter default
    }

    elapsedTime_ = 0;
    m_state = State::Running;

    if (dir == Direction::Forward) {
        currentOpacity_ = m_from;
    } else {
        currentOpacity_ = m_to;
    }

    emit started();

    // Apply initial opacity
    applyOpacity(currentOpacity_);
}

void CFMaterialFadeAnimation::pause() {
    if (m_state == State::Running) {
        m_state = State::Paused;
        emit paused();
    }
}

void CFMaterialFadeAnimation::stop() {
    m_state = State::Idle;
    elapsedTime_ = 0;

    // Reset to initial value
    currentOpacity_ = m_from;
    applyOpacity(currentOpacity_);

    emit stopped();
}

void CFMaterialFadeAnimation::reverse() {
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

bool CFMaterialFadeAnimation::tick(int dt) {
    if (m_state != State::Running) {
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

    // Calculate current opacity based on range
    currentOpacity_ = m_from + (m_to - m_from) * easedProgress;

    // Apply opacity
    applyOpacity(currentOpacity_);

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
// Fade-Specific Methods
// =============================================================================

void CFMaterialFadeAnimation::setTargetWidget(QWidget* widget) {
    targetWidget_ = widget;

    // Set up opacity effect for the new widget
    if (widget) {
        ensureOpacityEffect();
    }
}

void CFMaterialFadeAnimation::applyOpacity(float opacity) {
    if (!targetWidget_) {
        return;
    }

    ensureOpacityEffect();

    if (opacityEffect_) {
        opacityEffect_->setOpacity(qBound(0.0, static_cast<double>(opacity), 1.0));

        // Trigger update
        if (targetWidget_) {
            targetWidget_->update();
        }
    }
}

void CFMaterialFadeAnimation::ensureOpacityEffect() {
    if (!targetWidget_) {
        return;
    }

    // Check if widget already has an opacity effect
    opacityEffect_ = qobject_cast<QGraphicsOpacityEffect*>(targetWidget_->graphicsEffect());

    if (!opacityEffect_) {
        // Create new opacity effect
        opacityEffect_ = new QGraphicsOpacityEffect(targetWidget_);
        opacityEffect_->setOpacity(currentOpacity_);
        targetWidget_->setGraphicsEffect(opacityEffect_);
        ownsOpacityEffect_ = true;
    } else {
        ownsOpacityEffect_ = false;
    }
}

float CFMaterialFadeAnimation::calculateEasedProgress(float linearProgress) const {
    // For now, use simple linear easing
    // TODO: Integrate with MaterialMotionScheme for proper easing curves
    return linearProgress;
}

} // namespace cf::ui::components::material
