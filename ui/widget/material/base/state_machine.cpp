/**
 * @file state_machine.cpp
 * @brief Material Design State Machine Implementation
 *
 * Manages visual state transitions for Material widgets, driving
 * StateLayer opacity animations following Material Design 3 specifications.
 *
 * State opacity values (Material Design 3):
 * - Normal:   0.00
 * - Hovered:  0.08
 * - Pressed:  0.12
 * - Focused:  0.12
 * - Dragged:  0.16
 * - Disabled: 0.00
 *
 * @author Material Design Framework Team
 * @date 2026-02-28
 * @version 0.1
 * @since 0.1
 * @ingroup ui_widget_material
 */

#include "state_machine.h"
#include "components/animation.h"
#include "components/material/cfmaterial_animation_factory.h"
#include "components/material/cfmaterial_animation_strategy.h"

namespace cf::ui::widget::material::base {

using namespace cf::ui::components::material;
using namespace cf::ui::components;

/**
 * @brief Constructor - initializes state machine with animation factory.
 *
 * @param factory WeakPtr to animation factory for creating transitions.
 * @param parent QObject parent for memory management.
 */
StateMachine::StateMachine(cf::WeakPtr<components::material::CFMaterialAnimationFactory> factory,
                           QObject* parent)
    : QObject(parent), m_state(State::StateNormal), m_opacity(0.0f) {
    // WeakPtr is stored but not locked here to avoid circular dependency
    // Store the WeakPtr in a member variable for later use
    m_animator = factory;
}

/**
 * @brief Destructor - cancels any running animation.
 */
StateMachine::~StateMachine() {
    cancelCurrentAnimation();
}

/**
 * @brief Calculates target opacity for a given state.
 *
 * Follows Material Design 3 state layer opacity specifications.
 *
 * @param s The state to calculate opacity for.
 * @return Target opacity value [0.0, 1.0].
 */
float StateMachine::targetOpacityForState(States s) const {
    // Priority order: Disabled > Pressed > Dragged > Focused > Hovered > Normal
    // When multiple states are active, use the highest priority

    if (s & State::StateDisabled) {
        return 0.0f;
    }
    if (s & State::StatePressed) {
        return 0.12f;
    }
    if (s & State::StateDragged) {
        return 0.16f;
    }
    if (s & State::StateFocused) {
        return 0.12f;
    }
    if (s & State::StateHovered) {
        return 0.08f;
    }
    if (s & State::StateChecked) {
        return 0.08f; // Checked state uses same as hovered
    }

    return 0.0f; // StateNormal
}

/**
 * @brief Cancels the currently running opacity animation.
 *
 * Disconnects all signals and stops the animation to prevent multiple
 * animations from competing to update m_opacity.
 *
 * IMPORTANT: After stopping, reset m_opacity to the correct target value
 * for the current state. This is necessary because the progress signal
 * carries 0-1 progress, not the actual opacity value.
 */
void StateMachine::cancelCurrentAnimation() {
    if (m_currentAnimation) {
        auto* anim = m_currentAnimation.Get();
        if (anim) {
            // Disconnect all signals from this animation to this object FIRST
            // This prevents any late-arriving progressChanged signals after we've stopped
            disconnect(anim, &components::ICFAbstractAnimation::progressChanged, this, nullptr);
            disconnect(anim, &components::ICFAbstractAnimation::finished, this, nullptr);
            // Stop the animation
            anim->stop();
        }
        m_currentAnimation = nullptr;
    }
    // Reset m_opacity to the correct target value for the current state
    // This ensures we don't carry over stale progress values from cancelled animations
    m_opacity = targetOpacityForState(m_state);
    emit stateLayerOpacityChanged(m_opacity);
}

/**
 * @brief Slot called when the current animation finishes.
 *
 * Clears the animation reference and ensures m_opacity is set to the
 * correct target value for the current state. This prevents stale
 * intermediate values from being used when a new animation starts.
 */
void StateMachine::onAnimationFinished() {
    m_currentAnimation = nullptr;
    // Ensure m_opacity is exactly at the target value for the current state
    // This guards against any rounding errors or incomplete animations
    float targetOpacity = targetOpacityForState(m_state);
    if (m_opacity != targetOpacity) {
        m_opacity = targetOpacity;
        emit stateLayerOpacityChanged(m_opacity);
    }
}

/**
 * @brief Starts opacity transition animation.
 *
 * Creates a fade animation using the animation factory.
 * Cancels any currently running animation before starting a new one.
 *
 * @param to Target opacity value.
 */
void StateMachine::animateOpacityTo(float to) {
    // Performance mode check: if animations are disabled, set directly
    auto* factory = m_animator.Get();
    if (!factory || !factory->isAllEnabled()) {
        m_opacity = to;
        emit stateLayerOpacityChanged(m_opacity);
        return;
    }

    // CRITICAL FIX: Cancel any currently running animation before starting a new one.
    // This prevents multiple animations from competing to update m_opacity,
    // which causes visual glitches like flickering or incorrect opacity values
    // during rapid hover enter/leave events.
    cancelCurrentAnimation();

    // Start animation from current opacity value
    float from = m_opacity;

    // IMPORTANT: Use createAnimation instead of getAnimation to avoid sharing
    // the same animation instance across multiple StateMachines.
    // getAnimation returns a cached animation per token, which causes multiple
    // StateMachines to connect their lambdas to the same animation object,
    // resulting in cross-talk when any animation progresses.
    // createAnimation creates a separate animation instance per call.
    // See ElevationController::animatePressOffsetTo for the same pattern.
    AnimationDescriptor desc(
        "fade",                 // Animation type
        "md.motion.shortEnter", // Motion spec (short duration for hover states)
        "opacity",              // Property (we'll override with setRange)
        from,                   // Start value
        to                      // End value
    );

    auto anim = factory->createAnimation(desc, nullptr, this);
    if (!anim) {
        // Fallback: direct set if animation creation fails
        m_opacity = to;
        emit stateLayerOpacityChanged(m_opacity);
        return;
    }

    // Save animation reference for cancellation
    m_currentAnimation = anim;

    // Get raw pointer
    auto* rawAnim = anim.Get();

    // Connect progress signal
    // Note: Qt::UniqueConnection cannot be used with lambdas, but cancelCurrentAnimation()
    // disconnects all signals before starting a new animation, preventing duplicates
    //
    // CRITICAL: progressChanged emits 0-1 normalized progress, NOT actual opacity values.
    // We must interpolate between from and to to get the actual opacity.
    connect(rawAnim, &components::ICFAbstractAnimation::progressChanged, this,
            [this, from, to](float progress) {
                // progress is 0-1, interpolate to get actual opacity value
                m_opacity = from + (to - from) * progress;
                emit stateLayerOpacityChanged(m_opacity);
            });

    // Connect finished signal to clear the animation reference
    connect(rawAnim, &components::ICFAbstractAnimation::finished, this,
            &StateMachine::onAnimationFinished, Qt::UniqueConnection);

    // Start animation
    rawAnim->start(components::ICFAbstractAnimation::Direction::Forward);
}

// ============================================================================
// Event Handlers
// ============================================================================

void StateMachine::onHoverEnter() {
    if (m_state & State::StateDisabled)
        return;

    States oldState = m_state;
    m_state |= State::StateHovered;

    if (oldState != m_state) {
        emit stateChanged(m_state, oldState);
        animateOpacityTo(targetOpacityForState(m_state));
    }
}

void StateMachine::onHoverLeave() {
    States oldState = m_state;
    m_state &= ~static_cast<States>(State::StateHovered);

    if (oldState != m_state) {
        emit stateChanged(m_state, oldState);
        animateOpacityTo(targetOpacityForState(m_state));
    }
}

void StateMachine::onPress(const QPoint& pos) {
    Q_UNUSED(pos)
    if (m_state & State::StateDisabled)
        return;

    States oldState = m_state;
    m_state |= State::StatePressed;

    if (oldState != m_state) {
        emit stateChanged(m_state, oldState);
        animateOpacityTo(targetOpacityForState(m_state));
    }
}

void StateMachine::onRelease() {
    States oldState = m_state;
    m_state &= ~static_cast<States>(State::StatePressed);

    if (oldState != m_state) {
        emit stateChanged(m_state, oldState);
        animateOpacityTo(targetOpacityForState(m_state));
    }
}

void StateMachine::onFocusIn() {
    if (m_state & State::StateDisabled)
        return;

    States oldState = m_state;
    m_state |= State::StateFocused;

    if (oldState != m_state) {
        emit stateChanged(m_state, oldState);
        animateOpacityTo(targetOpacityForState(m_state));
    }
}

void StateMachine::onFocusOut() {
    States oldState = m_state;
    m_state &= ~static_cast<States>(State::StateFocused);

    if (oldState != m_state) {
        emit stateChanged(m_state, oldState);
        animateOpacityTo(targetOpacityForState(m_state));
    }
}

void StateMachine::onEnable() {
    States oldState = m_state;
    m_state &= ~static_cast<States>(State::StateDisabled);

    if (oldState != m_state) {
        emit stateChanged(m_state, oldState);
        animateOpacityTo(targetOpacityForState(m_state));
    }
}

void StateMachine::onDisable() {
    States oldState = m_state;
    m_state |= State::StateDisabled;

    if (oldState != m_state) {
        emit stateChanged(m_state, oldState);
        animateOpacityTo(targetOpacityForState(m_state));
    }
}

void StateMachine::onCheckedChanged(bool checked) {
    if (m_state & State::StateDisabled)
        return;

    States oldState = m_state;

    if (checked) {
        m_state |= State::StateChecked;
    } else {
        m_state &= ~static_cast<States>(State::StateChecked);
    }

    if (oldState != m_state) {
        emit stateChanged(m_state, oldState);
        animateOpacityTo(targetOpacityForState(m_state));
    }
}

// ============================================================================
// State Queries
// ============================================================================

StateMachine::States StateMachine::currentState() const {
    return m_state;
}

bool StateMachine::hasState(State s) const {
    return (m_state & s) != States();
}

float StateMachine::stateLayerOpacity() const {
    return m_opacity;
}

} // namespace cf::ui::widget::material::base
