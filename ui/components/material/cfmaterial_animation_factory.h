/**
 * @file    cfmaterial_animation_factory.h
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief   Material Design 3 Animation Factory
 * @version 0.1
 * @date    2026-02-28
 *
 * @copyright Copyright (c) 2026
 *
 * @details
 * MaterialAnimationFactory creates and manages animations following
 * Material Design 3 motion specifications. It uses a strategy pattern
 * to allow widget-specific customization while maintaining a simple
 * token-based API for users.
 *
 * Key features:
 * - Token-based animation retrieval (e.g., "md.animation.fadeIn")
 * - Automatic mapping to MotionSpec for timing and easing
 * - Strategy pattern for widget-specific behavior
 * - WeakPtr ownership model (factory owns, users hold weak references)
 * - Global enable/disable for performance and accessibility
 *
 * The factory maintains exclusive ownership of all created animations
 * via unique_ptr, while users receive WeakPtr references. This ensures
 * proper lifecycle management and prevents dangling pointers.
 *
 * @ingroup ui_components_material
 */
#pragma once

#include "../animation.h"
#include "animation_factory_manager.h"
#include "base/weak_ptr/weak_ptr.h"
#include "base/weak_ptr/weak_ptr_factory.h"
#include "cfmaterial_animation_strategy.h"
#include "core/theme.h"
#include "export.h"
#include <QObject>
#include <memory>
#include <string>
#include <unordered_map>

namespace cf::ui::components::material {

// Forward declarations
class CFMaterialFadeAnimation;
class CFMaterialSlideAnimation;
class CFMaterialScaleAnimation;

/**
 * @brief  Material Design 3 Animation Factory.
 *
 * @details Creates and manages animations following Material Design 3
 *          motion specifications. The factory maintains exclusive ownership
 *          of created animations (via unique_ptr) and provides WeakPtr
 *          access to users.
 *
 *          Animation lifecycle:
 *          1. User calls getAnimation("md.animation.fadeIn")
 *          2. Factory resolves token to AnimationDescriptor via token mapping
 *          3. Strategy adjusts descriptor (if set)
 *          4. Factory creates animation instance
 *          5. Factory stores animation (owns it)
 *          6. Factory returns WeakPtr to user
 *
 *          Token resolution:
 *          - "md.animation.fadeIn" → Fade animation, shortEnter timing
 *          - "md.animation.slideUp" → Slide animation, mediumEnter timing
 *          - "md.animation.scaleUp" → Scale animation, shortEnter timing
 *
 * @note    Thread-safe for concurrent reads.
 * @warning Animations are owned by the factory; WeakPtr may become
 *          invalid if the factory is destroyed.
 * @throws  None (all errors return invalid WeakPtr)
 * @since   0.1
 * @ingroup ui_components_material
 *
 * @code
 * // Initialization (usually done once per application)
 * using namespace cf::ui::components::material;
 * using namespace cf::ui::core;
 *
 * MaterialFactory themeFactory;
 * auto theme = themeFactory.fromName("theme.material.light");
 * auto factory = std::make_unique<CFMaterialAnimationFactory>(*theme);
 * factory->setStrategy(std::make_unique<ButtonAnimationStrategy>());
 *
 * // Basic usage - get animation by token
 * auto fadeIn = factory->getAnimation("md.animation.fadeIn");
 * if (fadeIn) {
 *     fadeIn->setTargetWidget(myWidget);
 *     fadeIn->start();
 * }
 *
 * // Create with custom descriptor
 * AnimationDescriptor desc{"fade", "md.motion.longEnter", "opacity", 0.0f, 1.0f};
 * auto customFade = factory->createAnimation(desc, myWidget);
 *
 * // Enable/disable all animations
 * factory->setEnabledAll(false);  // Disable animations for performance
 * @endcode
 */
class CF_UI_EXPORT CFMaterialAnimationFactory : public ICFAnimationManagerFactory {
    Q_OBJECT

  public:
    /**
     * @brief  Constructor with theme reference.
     *
     * @details The theme reference must remain valid for the lifetime
     *          of the factory. It is used to query MotionSpec values
     *          (duration, easing) for animations.
     *
     * @param[in]     theme Reference to the Material Design theme.
     * @param[in]     strategy Optional strategy for widget-specific behavior.
     *                        If nullptr, DefaultAnimationStrategy is used.
     * @param[in]     parent QObject parent.
     *
     * @throws        None
     * @note          If strategy is nullptr, DefaultAnimationStrategy is used.
     * @warning       The theme must outlive this factory.
     * @since         0.1
     * @ingroup       ui_components_material
     *
     * @code
     * CFMaterialAnimationFactory factory(*theme);
     * @endcode
     */
    explicit CFMaterialAnimationFactory(const core::ICFTheme& theme,
                                        std::unique_ptr<AnimationStrategy> strategy = nullptr,
                                        QObject* parent = nullptr);

    /**
     * @brief  Destructor.
     *
     * @details All owned animations are destroyed. Any WeakPtr
     *          returned by this factory becomes invalid.
     *
     * @since 0.1
     */
    ~CFMaterialAnimationFactory() override;

    // Non-copyable, non-movable
    CFMaterialAnimationFactory(const CFMaterialAnimationFactory&) = delete;
    CFMaterialAnimationFactory& operator=(const CFMaterialAnimationFactory&) = delete;
    CFMaterialAnimationFactory(CFMaterialAnimationFactory&&) = delete;
    CFMaterialAnimationFactory& operator=(CFMaterialAnimationFactory&&) = delete;

    cf::WeakPtr<ICFAnimationManagerFactory> GetWeakPtr() override {
        return weak_factory_.GetWeakPtr();
    }

    // Implement pure virtual functions from ICFAnimationManagerFactory
    /**
     * @brief  Register an animation type by name.
     *
     * @param[in] name Unique name for the animation.
     * @param[in] type Animation type identifier.
     *
     * @return        Registration result indicating success or failure reason.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_components_material
     */
    ICFAnimationManagerFactory::RegisteredResult registerOneAnimation(const QString& name,
                                                                      const QString& type) override;

    /**
     * @brief  Register an animation with a creator function.
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
     * @ingroup       ui_components_material
     */
    ICFAnimationManagerFactory::RegisteredResult
    registerAnimationCreator(const QString& name, AnimationCreator creator) override;

    /**
     * @brief  Get or create an animation by token name.
     *
     * @details This is the primary user API for retrieving animations.
     *          Tokens are resolved through the token mapping system:
     *          - "md.animation.fadeIn" → Fade animation, shortEnter timing
     *          - "md.animation.slideUp" → Slide animation, mediumEnter timing
     *          - "md.animation.scaleUp" → Scale animation, shortEnter timing
     *
     *          If an animation with the given token already exists,
     *          the existing animation's WeakPtr is returned.
     *          Otherwise, a new animation is created and stored.
     *
     * @param  animationToken Token name (e.g., "md.animation.fadeIn").
     *
     * @return WeakPtr to the animation, or invalid WeakPtr if:
     *         - Token is not found in mapping
     *         - Animation type is not supported
     *         - Global enabled is false
     *         - Strategy disables animation
     *
     * @throws     None
     * @note       If the animation doesn't exist, a new animation is created.
     * @warning    The returned WeakPtr may become invalid if the factory
     *             is destroyed. Always check validity before use.
     * @since      0.1
     * @ingroup    ui_components_material
     *
     * @code
     * auto anim = factory->getAnimation("md.animation.fadeIn");
     * if (anim) {
     *     anim->setTargetWidget(myWidget);
     *     anim->start();
     * }
     * @endcode
     */
    cf::WeakPtr<ICFAbstractAnimation> getAnimation(const char* animationToken) override;

    /**
     * @brief  Create an animation from a descriptor.
     *
     * @details Creates a new animation instance based on the descriptor.
     *          The strategy (if set) is applied before creation.
     *
     *          Unlike getAnimation(), this method always creates a new
     *          animation instance, even if one with the same configuration
     *          already exists.
     *
     * @param  descriptor Animation configuration descriptor.
     * @param  targetWidget Optional target widget for the animation.
     *                      The animation applies to this widget.
     *
     * @return WeakPtr to the created animation, or invalid WeakPtr if:
     *         - Animation type is not supported
     *         - Global enabled is false
     *         - Strategy disables animation
     *
     * @throws     None
     * @note       The factory takes ownership of the created animation.
     * @warning    None
     * @since      0.1
     * @ingroup    ui_components_material
     *
     * @code
     * AnimationDescriptor desc{"fade", "md.motion.longEnter", "opacity", 0.0f, 1.0f};
     * auto anim = factory->createAnimation(desc, myWidget);
     * if (anim) anim->start();
     * @endcode
     */
    cf::WeakPtr<ICFAbstractAnimation> createAnimation(const AnimationDescriptor& descriptor,
                                                      QWidget* targetWidget = nullptr);

    /**
     * @brief  Set the animation strategy for this factory.
     *
     * @details The strategy is applied to all animations created after
     *          this call. Existing animations are not affected.
     *
     *          Strategies allow widget-specific customization:
     *          - Buttons: shorter animations
     *          - Dialogs: longer animations
     *          - Lists: staggered animations
     *
     * @param[in]     strategy Unique pointer to the strategy (takes ownership).
     *                        Pass nullptr to use DefaultAnimationStrategy.
     *
     * @throws        None
     * @note          Pass nullptr to use the default strategy.
     * @warning       The factory takes ownership of the strategy.
     * @since         0.1
     * @ingroup       ui_components_material
     *
     * @code
     * factory->setStrategy(std::make_unique<ButtonAnimationStrategy>());
     * @endcode
     */
    void setStrategy(std::unique_ptr<AnimationStrategy> strategy);

    /**
     * @brief  Get the current animation strategy.
     *
     * @return Pointer to the current strategy (may be nullptr).
     *
     * @since 0.1
     */
    AnimationStrategy* strategy() const { return strategy_.get(); }

    /**
     * @brief  Set the global enabled state for all animations.
     *
     * @details When disabled, getAnimation() returns invalid WeakPtr
     *          and createAnimation() returns invalid WeakPtr.
     *
     *          This is useful for:
     *          - Performance optimization during heavy processing
     *          - Accessibility (respect "reduce motion" settings)
     *          - User preference (animation toggle)
     *
     * @param[in]     enabled true to enable animations, false to disable.
     *
     * @throws        None
     * @note          This affects all animation creation methods.
     * @warning       Existing animations continue to run; only new creations
     *                are affected.
     * @since         0.1
     * @ingroup       ui_components_material
     *
     * @code
     * // Disable animations during heavy processing
     * factory->setEnabledAll(false);
     * // ... do heavy work ...
     * factory->setEnabledAll(true);
     * @endcode
     */
    void setEnabledAll(bool enabled) override;

    /**
     * @brief  Check if animations are globally enabled.
     *
     * @return true if animations are enabled, false otherwise.
     *
     * @since 0.1
     */
    bool isAllEnabled() override { return globalEnabled_; }

    /**
     * @brief  Get the associated theme.
     *
     * @return Reference to the theme.
     *
     * @warning The theme must outlive this factory.
     * @since 0.1
     */
    const core::ICFTheme& theme() const { return theme_; }

    /**
     * @brief  Get the number of animations managed by this factory.
     *
     * @return Number of owned animations.
     *
     * @since 0.1
     */
    size_t animationCount() const { return animations_.size(); }

  signals:
    /**
     * @brief  Signal emitted when an animation is created.
     *
     * @param[in]     token The animation token that was created.
     *
     * @since         0.1
     * @ingroup       ui_components_material
     */
    void animationCreated(const QString& token);

    /**
     * @brief  Signal emitted when global enabled state changes.
     *
     * @param[in]     enabled The new enabled state.
     *
     * @since         0.1
     * @ingroup       ui_components_material
     */
    void animationEnabledChanged(bool enabled);

  private:
    /// Reference to the theme for MotionSpec queries
    const core::ICFTheme& theme_;

    /// Animation strategy for widget-specific behavior
    std::unique_ptr<AnimationStrategy> strategy_;

    /// Global enabled state
    bool globalEnabled_ = true;

    /// Owned animations (unique ownership)
    /// Key: token name, Value: owned animation instance
    std::unordered_map<std::string, std::unique_ptr<ICFAbstractAnimation>> animations_;

    /**
     * @brief  Resolve a token to an AnimationDescriptor.
     *
     * @details Looks up the token in the mapping table and returns
     *          the corresponding AnimationDescriptor.
     *
     * @param  token Animation token name.
     *
     * @return AnimationDescriptor, or empty descriptor with nullptr values
     *          if token is not found.
     */
    AnimationDescriptor resolveToken(const char* token);

    /**
     * @brief  Create a fade animation.
     *
     * @param  desc Animation descriptor.
     * @param  widget Target widget.
     *
     * @return Unique pointer to the created animation, or nullptr on failure.
     */
    std::unique_ptr<ICFAbstractAnimation> createFadeAnimation(const AnimationDescriptor& desc,
                                                              QWidget* widget);

    /**
     * @brief  Create a slide animation.
     *
     * @param  desc Animation descriptor.
     * @param  widget Target widget.
     *
     * @return Unique pointer to the created animation, or nullptr on failure.
     */
    std::unique_ptr<ICFAbstractAnimation> createSlideAnimation(const AnimationDescriptor& desc,
                                                               QWidget* widget);

    /**
     * @brief  Create a scale animation.
     *
     * @param  desc Animation descriptor.
     * @param  widget Target widget.
     *
     * @return Unique pointer to the created animation, or nullptr on failure.
     */
    std::unique_ptr<ICFAbstractAnimation> createScaleAnimation(const AnimationDescriptor& desc,
                                                               QWidget* widget);

    /**
     * @brief  Apply the strategy to a descriptor.
     *
     * @details Calls strategy->adjust() if a strategy is set.
     *
     * @param  descriptor The descriptor to adjust.
     * @param  widget The target widget.
     *
     * @return Adjusted descriptor.
     */
    AnimationDescriptor applyStrategy(const AnimationDescriptor& descriptor, QWidget* widget);

    /**
     * @brief  Check if animation should be enabled.
     *
     * @details Checks global enabled state and strategy.
     *
     * @param  widget The target widget.
     *
     * @return true if animation should be enabled.
     */
    bool shouldEnableAnimation(QWidget* widget) const;

    cf::WeakPtrFactory<CFMaterialAnimationFactory> weak_factory_{this};
};

} // namespace cf::ui::components::material
