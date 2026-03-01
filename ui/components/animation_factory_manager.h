/**
 * @file    animation_factory_manager.h
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief   Animation Factory Manager Interface
 * @version 0.1
 * @date    2026-02-28
 *
 * @copyright Copyright (c) 2026
 *
 * @details
 * Defines the interface for animation factory managers that create and
 * manage animations. This interface provides token-based animation lookup
 * and supports both string-based and typed animation registration.
 *
 * The manager maintains ownership of created animations and provides
 * WeakPtr access to users for safe, non-owning references.
 *
 * @ingroup ui_components
 */
#pragma once

#include "animation.h"
#include "base/weak_ptr/weak_ptr.h"
#include <QObject>
#include <functional>

namespace cf::ui::components {

/**
 * @brief  Animation creator function type.
 *
 * @details Function signature for creating animations dynamically.
 *          Used with registerAnimationCreator for type-safe animation
 *          registration.
 *
 * @since  0.1
 * @ingroup ui_components
 */
using AnimationCreator = std::function<ICFAbstractAnimation*(QObject*)>;

/**
 * @brief  Animation Factory Manager Interface.
 *
 * @details Manages creation and lifecycle of animations with support for:
 *          - Token-based animation lookup (e.g., "md.animation.fadeIn")
 *          - Type-safe animation registration
 *          - Global and per-animation enable/disable
 *          - WeakPtr ownership model for safe access
 *
 *          Animations are owned by the manager and accessed via WeakPtr.
 *          This ensures proper lifecycle management and prevents dangling
 *           pointers when the manager is destroyed.
 *
 * @note    Implementations should be thread-safe for concurrent reads.
 * @warning Destroying the manager invalidates all WeakPtr references.
 * @since   0.1
 * @ingroup ui_components
 *
 * @code
 * // Example usage with MaterialAnimationFactory
 * MaterialAnimationFactory factory(theme);
 * auto anim = factory.getAnimation("md.animation.fadeIn");
 * if (anim) {
 *     anim->start();
 * }
 * @endcode
 */
class CF_UI_EXPORT ICFAnimationManagerFactory : public QObject {
    Q_OBJECT

  public:
    /**
     * @brief  Constructor with parent.
     *
     * @param[in] parent QObject parent.
     *
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.1
     * @ingroup    ui_components
     */
    explicit ICFAnimationManagerFactory(QObject* parent);

    /**
     * @brief  Virtual destructor.
     *
     * @since 0.1
     */
    ~ICFAnimationManagerFactory() override = default;

    // =========================================================================
    // Enumeration Types
    // =========================================================================

    /**
     * @brief  Registration result enumeration.
     *
     * @details Possible results from animation registration operations.
     *
     * @since  0.1
     * @ingroup ui_components
     */
    enum class RegisteredResult {
        OK,            ///< Registration successful
        DUP_NAME,      ///< Animation with this name already exists
        UNSUPPORT_TYPE ///< Animation type is not supported
    };
    Q_ENUM(RegisteredResult)

    virtual cf::WeakPtr<ICFAnimationManagerFactory> GetWeakPtr() = 0;
    // =========================================================================
    // Animation Registration
    // =========================================================================

    /**
     * @brief  Register an animation type by name.
     *
     * @details Associates a name with an animation type for later
     *          retrieval. The type string is used to determine
     *          which animation class to instantiate.
     *
     * @param[in] name Unique name for the animation (e.g., "fadeIn").
     * @param[in] type Animation type identifier (e.g., "fade", "slide").
     *
     * @return        Registration result indicating success or failure reason.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_components
     *
     * @code
     * manager->registerOneAnimation("buttonPress", "fade");
     * @endcode
     */
    virtual RegisteredResult registerOneAnimation(const QString& name, const QString& type) = 0;

    /**
     * @brief  Register an animation with a creator function.
     *
     * @details Associates a name with a function that creates animation
     *          instances. This provides type-safe registration for
     *          custom animation types.
     *
     * @param[in] name Unique name for the animation.
     * @param[in] creator Function that creates the animation instance.
     *
     * @return        Registration result indicating success or failure reason.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_components
     *
     * @code
     * manager->registerAnimationCreator("customFade",
     *     [](QObject* parent) {
     *         return new CFMaterialFadeAnimation(motionSpec, parent);
     *     });
     * @endcode
     */
    virtual RegisteredResult registerAnimationCreator(const QString& name,
                                                      AnimationCreator creator) = 0;

    // =========================================================================
    // Animation Retrieval
    // =========================================================================

    /**
     * @brief  Get or create an animation by name.
     *
     * @details Retrieves an existing animation or creates a new one
     *          based on the registered name/token.
     *
     *          Supports both custom registered names and Material Design
     *          tokens (e.g., "md.animation.fadeIn").
     *
     * @param[in] name Animation name or token.
     *
     * @return        WeakPtr to the animation, or invalid WeakPtr if not found.
     *
     * @throws        None
     * @note          The returned WeakPtr may become invalid if the manager
     *                is destroyed. Always check validity before use.
     * @warning       The manager owns the animation; do not delete it manually.
     * @since         0.1
     * @ingroup       ui_components
     *
     * @code
     * auto anim = manager->getAnimation("md.animation.fadeIn");
     * if (anim) {
     *     anim->start();
     * }
     * @endcode
     */
    virtual cf::WeakPtr<ICFAbstractAnimation> getAnimation(const char* name) = 0;
    // =========================================================================
    // Global Settings
    // =========================================================================

    /**
     * @brief  Set the target FPS for animations.
     *
     * @details Sets the desired frame rate for animation updates.
     *          This affects the tick interval for all animations.
     *
     * @param[in] fps Target frames per second (e.g., 60.0f).
     *
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.1
     * @ingroup    ui_components
     *
     * @code
     * manager->setTargetFps(60.0f);  // 60 FPS
     * @endcode
     */
    void setTargetFps(const float fps);

    /**
     * @brief  Set enabled state for a specific animation.
     *
     * @details Controls whether a specific animation is allowed to run.
     *          When disabled, getAnimation() returns invalid WeakPtr.
     *
     * @param[in] which Animation name.
     * @param[in] enabled true to enable, false to disable.
     *
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.1
     * @ingroup    ui_components
     *
     * @code
     * manager->setTargetEnabled("md.animation.fadeIn", false);
     * @endcode
     */
    void setTargetEnabled(const QString& which, const bool enabled);

    /**
     * @brief  Check if a specific animation is enabled.
     *
     * @param[in] which Animation name.
     *
     * @return        true if enabled, false otherwise.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_components
     */
    bool targetEnabled(const QString& which);

    /**
     * @brief  Set enabled state for all animations.
     *
     * @details When disabled, getAnimation() returns invalid WeakPtr
     *          for all animations. Existing running animations continue
     *          until completion.
     *
     * @param[in] enabled true to enable all, false to disable all.
     *
     * @throws        None
     * @note          Existing running animations continue until completion.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_components
     *
     * @code
     * // Disable all animations during heavy processing
     * manager->setEnabledAll(false);
     * // ... do heavy work ...
     * manager->setEnabledAll(true);
     * @endcode
     */
    virtual void setEnabledAll(bool enabled) = 0;

    /**
     * @brief  Check if all animations are enabled.
     *
     * @return        true if all animations are enabled, false otherwise.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_components
     */
    virtual bool isAllEnabled() = 0;

  signals:
    /**
     * @brief  Signal emitted when an animation is registered.
     *
     * @param[in] name The name of the registered animation.
     *
     * @since      0.1
     * @ingroup    ui_components
     */
    void animationRegistered(const QString& name);

    /**
     * @brief  Signal emitted when an animation's enabled state changes.
     *
     * @param[in] name The name of the animation.
     * @param[in] enabled The new enabled state.
     *
     * @since      0.1
     * @ingroup    ui_components
     */
    void animationEnabledChanged(const QString& name, bool enabled);
};

} // namespace cf::ui::components
