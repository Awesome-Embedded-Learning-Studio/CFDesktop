/**
 * @file    animation.h
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief   Abstract Animation Interface
 * @version 0.1
 * @date    2026-02-28
 *
 * @copyright Copyright (c) 2026
 *
 * @details
 * Defines the base animation interface for all animation types.
 * Provides common animation states, direction control, and lifecycle methods.
 *
 * @ingroup ui_components
 */
#pragma once

#include "base/weak_ptr/weak_ptr.h"
#include "export.h"
#include <QObject>
#include <QTimer>

namespace cf::ui::components {

/**
 * @brief  Abstract animation base class.
 *
 * @details Provides the common interface for all animations including
 *          state management, direction control, and lifecycle methods.
 *
 * @since  0.1
 * @ingroup ui_components
 */
class CF_UI_EXPORT ICFAbstractAnimation : public QObject {
    Q_OBJECT
  public:
    friend class ICFAnimationManagerFactory;

    explicit ICFAbstractAnimation(QObject* parent = nullptr);

    /**
     * @brief  Animation states.
     *
     * @since  0.1
     * @ingroup ui_components
     */
    enum class State { Idle, Running, Paused, Finished };
    Q_ENUM(State)

    /**
     * @brief  Animation playback direction.
     *
     * @since  0.1
     * @ingroup ui_components
     */
    enum class Direction { Forward, Backward };
    Q_ENUM(Direction)

    /**
     * @brief  Starts the animation in the specified direction.
     *
     * @param[in] dir Direction to play the animation (default: Forward).
     *
     * @throws     None
     * @note       If already running, this may restart the animation.
     * @warning    None
     * @since      0.1
     * @ingroup    ui_components
     */
    virtual void start(Direction dir = Direction::Forward) = 0;

    /**
     * @brief  Pauses the animation.
     *
     * @throws     None
     * @note       Does nothing if the animation is not running.
     * @warning    None
     * @since      0.1
     * @ingroup    ui_components
     */
    virtual void pause() = 0;

    /**
     * @brief  Stops the animation and resets to initial state.
     *
     * @throws     None
     * @note       Emits the stopped signal.
     * @warning    None
     * @since      0.1
     * @ingroup    ui_components
     */
    virtual void stop() = 0;

    /**
     * @brief  Reverses the animation direction.
     *
     * @details Stops the current session and plays in opposite direction.
     *
     * @throws     None
     * @note       Emits the reversed signal.
     * @warning    None
     * @since      0.1
     * @ingroup    ui_components
     */
    virtual void reverse() = 0;

    /**
     * @brief  Updates the animation state.
     *
     * @details Called every frame by the animator. Subclasses implement
     *          specific interpolation logic.
     *
     * @param[in] dt Time interval since the last call (milliseconds).
     *
     * @return        true if animation continues, false if finished.
     *
     * @throws        None
     * @note          Implementations should update m_progress and apply
     *                the interpolated value to the target.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_components
     */
    virtual bool tick(int dt) = 0;

    /**
     * @brief  Gets a weak pointer to this animation.
     *
     * @details Each concrete animation class must implement this using
     *          its WeakPtrFactory.
     *
     * @return WeakPtr to this animation.
     *
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.1
     * @ingroup    ui_components
     */
    virtual cf::WeakPtr<ICFAbstractAnimation> GetWeakPtr() = 0;

    /**
     * @brief  Gets the enabled state of the animation.
     *
     * @return true if animation is enabled, false otherwise.
     *
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.1
     * @ingroup    ui_components
     */
    bool getEnabled() const { return enabled; }

  signals:
    /**
     * @brief  Signal emitted when animation starts.
     *
     * @since  0.1
     * @ingroup ui_components
     */
    void started();

    /**
     * @brief  Signal emitted when animation is paused.
     *
     * @since  0.1
     * @ingroup ui_components
     */
    void paused();

    /**
     * @brief  Signal emitted when animation stops.
     *
     * @since  0.1
     * @ingroup ui_components
     */
    void stopped();

    /**
     * @brief  Signal emitted when animation reverses direction.
     *
     * @since  0.1
     * @ingroup ui_components
     */
    void reversed();

    /**
     * @brief  Signal emitted when animation finishes.
     *
     * @since  0.1
     * @ingroup ui_components
     */
    void finished();

    /**
     * @brief  Signal emitted when animation progress changes.
     *
     * @param[in] progress Current progress value (0.0 to 1.0).
     *
     * @since  0.1
     * @ingroup ui_components
     */
    void progressChanged(float progress);

  protected:
    QTimer* driven_internal_timer{nullptr};
    float m_progress = 0.0f;
    State m_state = State::Idle;

    /**
     * @brief  Sets the enabled state of the animation.
     *
     * @param[in] enabled true to enable, false to disable.
     *
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.1
     * @ingroup    ui_components
     */
    void setEnabled(bool enabled) { this->enabled = enabled; }

  private:
    bool enabled;
};

} // namespace cf::ui::components
