/**
 * @file    ui/widget/material/base/state_machine.h
 * @brief   Material Design state machine for widget interaction states.
 *
 * Manages Material Design widget states including hover, pressed, focused,
 * disabled, and checked states with animated state layer transitions.
 *
 * @author  N/A
 * @date    N/A
 * @version N/A
 * @since   N/A
 * @ingroup ui_widget_material_base
 */
#pragma once
#include "base/include/base/weak_ptr/weak_ptr.h"
#include "components/material/cfmaterial_animation_factory.h"
#include "export.h"
#include <QObject>

namespace cf::ui::widget::material::base {

/**
 * @brief  Material Design state machine.
 *
 * @details Manages Material Design widget states including hover, pressed,
 *          focused, disabled, and checked states with animated state layer
 *          transitions.
 *
 * @since  N/A
 * @ingroup ui_widget_material_base
 */
class CF_UI_EXPORT StateMachine : public QObject {
    Q_OBJECT
  public:
    /**
     * @brief  Widget interaction states.
     *
     * @since  N/A
     * @ingroup ui_widget_material_base
     */
    enum class State {
        StateNormal = 0x00,   ///< Normal state.
        StateHovered = 0x01,  ///< Mouse hover state.
        StatePressed = 0x02,  ///< Mouse pressed state.
        StateFocused = 0x04,  ///< Keyboard focused state.
        StateDisabled = 0x08, ///< Disabled state.
        StateChecked = 0x10,  ///< Checked state.
        StateDragged = 0x20   ///< Dragged state.
    };
    Q_DECLARE_FLAGS(States, State)

    /**
     * @brief  Constructor with animation factory.
     *
     * @param[in]     factory WeakPtr to the animation factory.
     * @param[in]     parent QObject parent.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_base
     */
    explicit StateMachine(cf::WeakPtr<components::material::CFMaterialAnimationFactory> factory,
                          QObject* parent);

    /**
     * @brief  Handles hover enter event.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_base
     */
    void onHoverEnter();

    /**
     * @brief  Handles hover leave event.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_base
     */
    void onHoverLeave();

    /**
     * @brief  Handles press event.
     *
     * @param[in]     pos Press position coordinates.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_base
     */
    void onPress(const QPoint& pos);

    /**
     * @brief  Handles release event.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_base
     */
    void onRelease();

    /**
     * @brief  Handles focus in event.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_base
     */
    void onFocusIn();

    /**
     * @brief  Handles focus out event.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_base
     */
    void onFocusOut();

    /**
     * @brief  Handles enable event.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_base
     */
    void onEnable();

    /**
     * @brief  Handles disable event.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_base
     */
    void onDisable();

    /**
     * @brief  Handles checked state change event.
     *
     * @param[in]     checked true if widget is checked, false otherwise.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_base
     */
    void onCheckedChanged(bool checked);

    /**
     * @brief  Destructor.
     *
     * @details Cancels any running animation before destruction.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_base
     */
    ~StateMachine();

    /**
     * @brief  Gets the current states.
     *
     * @return        Current widget states.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_base
     */
    States currentState() const;

    /**
     * @brief  Checks if a specific state is active.
     *
     * @param[in]     s State to check.
     *
     * @return        true if the state is active, false otherwise.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_base
     */
    bool hasState(State s) const;

    /**
     * @brief  Gets the state layer opacity.
     *
     * @return        Current state layer opacity (0.0 to 1.0).
     *
     * @throws        None
     * @note          Used for Material Design state overlay rendering.
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_base
     */
    float stateLayerOpacity() const;

  signals:
    /**
     * @brief  Signal emitted when state changes.
     *
     * @param[in]     newState New widget states.
     * @param[in]     oldState Previous widget states.
     *
     * @since         N/A
     * @ingroup       ui_widget_material_base
     */
    void stateChanged(States newState, States oldState);

    /**
     * @brief  Signal emitted when state layer opacity changes.
     *
     * @param[in]     opacity New opacity value.
     *
     * @since         N/A
     * @ingroup       ui_widget_material_base
     */
    void stateLayerOpacityChanged(float opacity);

  private:
    /**
     * @brief  Animates the state layer opacity to a target value.
     *
     * @details Creates and starts an opacity animation from the current
     *          opacity value to the target value. Cancels any running
     *          animation before starting a new one.
     *
     * @param[in] to   Target opacity value (0.0 to 1.0).
     *
     * @throws        None
     * @note          Uses the animation factory to create a fade animation.
     * @warning       The animation reference is stored and can be cancelled.
     * @since         N/A
     * @ingroup       ui_widget_material_base
     */
    void animateOpacityTo(float to);

    /**
     * @brief  Cancels the currently running opacity animation.
     *
     * @throws        None
     * @note          Disconnects all signals and stops the animation.
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_base
     */
    void cancelCurrentAnimation();

    /**
     * @brief  Transitions to a new state if different from current.
     *
     * @details Compares new state with current state. If different,
     *          updates the state, emits stateChanged, and animates
     *          opacity to the target value for the new state.
     *
     * @param[in]     newState The target state to transition to.
     *
     * @throws        None
     * @note          No-op if newState equals current state.
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_base
     */
    void transitionTo(States newState);

    /**
     * @brief  Slot called when the current animation finishes.
     *
     * @details Clears the animation reference to allow new animations.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_base
     */
    void onAnimationFinished();

    /**
     * @brief  Calculates the target opacity for a given widget state.
     *
     * @param[in] s Widget state combination.
     *
     * @return        Target opacity value (0.0 to 1.0).
     *
     * @throws        None
     * @note          Higher priority states take precedence.
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_base
     */
    float targetOpacityForState(States s) const;

    States m_state = State::StateNormal;
    float m_opacity = 0.0f;
    cf::WeakPtr<components::material::CFMaterialAnimationFactory> m_animator;

    /// Reference to the currently running opacity animation
    cf::WeakPtr<components::ICFAbstractAnimation> m_currentAnimation;
};
} // namespace cf::ui::widget::material::base
