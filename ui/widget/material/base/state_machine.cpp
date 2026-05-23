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
        return 0.08f;
    }

    return 0.0f;
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
            disconnect(anim, &components::ICFAbstractAnimation::progressChanged, this, nullptr);
            disconnect(anim, &components::ICFAbstractAnimation::finished, this, nullptr);
            anim->stop();
        }
        m_currentAnimation = nullptr;
    }
    m_opacity = targetOpacityForState(m_state);
    emit stateLayerOpacityChanged(m_opacity);
}

/**
 * @brief Slot called when the current animation finishes.
 *
 * Clears the animation reference and ensures m_opacity is set to the
 * correct target value for the current state.
 */
void StateMachine::onAnimationFinished() {
    m_currentAnimation = nullptr;
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
    auto* factory = m_animator.Get();
    if (!factory || !factory->isAllEnabled()) {
        m_opacity = to;
        emit stateLayerOpacityChanged(m_opacity);
        return;
    }

    cancelCurrentAnimation();

    float from = m_opacity;

    AnimationDescriptor desc("fade", "md.motion.shortEnter", "opacity", from, to);

    auto anim = factory->createAnimation(desc, nullptr, this);
    if (!anim) {
        m_opacity = to;
        emit stateLayerOpacityChanged(m_opacity);
        return;
    }

    m_currentAnimation = anim;
    auto* rawAnim = anim.Get();

    connect(rawAnim, &components::ICFAbstractAnimation::progressChanged, this,
            [this, from, to](float progress) {
                m_opacity = from + (to - from) * progress;
                emit stateLayerOpacityChanged(m_opacity);
            });

    connect(rawAnim, &components::ICFAbstractAnimation::finished, this,
            &StateMachine::onAnimationFinished, Qt::UniqueConnection);

    rawAnim->start(components::ICFAbstractAnimation::Direction::Forward);
}

// ============================================================================
// State Transition
// ============================================================================

void StateMachine::transitionTo(States newState) {
    if (m_state == newState) {
        return;
    }
    States oldState = m_state;
    m_state = newState;
    emit stateChanged(m_state, oldState);
    animateOpacityTo(targetOpacityForState(m_state));
}

// ============================================================================
// Event Handlers
// ============================================================================

void StateMachine::onHoverEnter() {
    if (m_state & State::StateDisabled)
        return;
    transitionTo(m_state | State::StateHovered);
}

void StateMachine::onHoverLeave() {
    transitionTo(m_state & ~static_cast<States>(State::StateHovered));
}

void StateMachine::onPress(const QPoint& pos) {
    Q_UNUSED(pos)
    if (m_state & State::StateDisabled)
        return;
    transitionTo(m_state | State::StatePressed);
}

void StateMachine::onRelease() {
    transitionTo(m_state & ~static_cast<States>(State::StatePressed));
}

void StateMachine::onFocusIn() {
    if (m_state & State::StateDisabled)
        return;
    transitionTo(m_state | State::StateFocused);
}

void StateMachine::onFocusOut() {
    transitionTo(m_state & ~static_cast<States>(State::StateFocused));
}

void StateMachine::onEnable() {
    transitionTo(m_state & ~static_cast<States>(State::StateDisabled));
}

void StateMachine::onDisable() {
    transitionTo(m_state | State::StateDisabled);
}

void StateMachine::onCheckedChanged(bool checked) {
    if (m_state & State::StateDisabled)
        return;
    if (checked) {
        transitionTo(m_state | State::StateChecked);
    } else {
        transitionTo(m_state & ~static_cast<States>(State::StateChecked));
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
