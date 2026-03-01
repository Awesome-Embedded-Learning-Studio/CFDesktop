/**
 * @file    cfmaterial_animation_strategy.h
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief   Animation Strategy Interface for Material Design 3
 * @version 0.1
 * @date    2026-02-28
 *
 * @copyright Copyright (c) 2026
 *
 * @details
 * Defines the strategy interface for animation customization based on
 * widget types. This allows different widget types to have specialized
 * animation behaviors while maintaining a consistent factory interface.
 *
 * The Animation Strategy pattern enables:
 * - Widget-specific animation parameter adjustments
 * - Runtime animation type overrides
 * - Conditional animation enable/disable logic
 * - Reusable customization logic across different widget types
 *
 * @ingroup ui_components_material
 */
#pragma once

#include "export.h"
#include <QWidget>

namespace cf::ui::components::material {

/**
 * @brief  Animation descriptor structure.
 *
 * @details Contains all information needed to create an animation:
 *          - Animation type (fade, slide, scale, rotate, spring)
 *          - Motion specification token for timing/easing
 *          - Target property to animate
 *          - Value range
 *          - Optional delay
 *
 * This structure is passed to AnimationStrategy for potential
 * modification before animation creation.
 *
 * @since  0.1
 * @ingroup ui_components_material
 *
 * @code
 * // Create a fade-in animation descriptor
 * AnimationDescriptor desc;
 * desc.animationType = "fade";
 * desc.motionToken = "md.motion.shortEnter";
 * desc.property = "opacity";
 * desc.fromValue = 0.0f;
 * desc.toValue = 1.0f;
 * desc.delayMs = 0;
 * @endcode
 */
struct CF_UI_EXPORT AnimationDescriptor {
    /// Animation type: "fade", "slide", "scale", "rotate", "spring"
    const char* animationType;

    /// Motion spec token: e.g., "md.motion.shortEnter"
    const char* motionToken;

    /// Property to animate: "opacity", "positionX", "positionY", "scale"
    const char* property;

    /// Start value for the animation
    float fromValue;

    /// End value for the animation
    float toValue;

    /// Delay before animation starts (milliseconds)
    int delayMs = 0;

    /**
     * @brief  Default constructor.
     *
     * @since 0.1
     */
    AnimationDescriptor() = default;

    /**
     * @brief  Construct with all fields.
     *
     * @param[in]     type Animation type.
     * @param[in]     motion Motion spec token.
     * @param[in]     prop Property name.
     * @param[in]     from Start value.
     * @param[in]     to End value.
     * @param[in]     delay Delay in milliseconds.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_components_material
     *
     * @code
     * AnimationDescriptor desc("fade", "md.motion.longEnter", "opacity", 0.0f, 1.0f);
     * @endcode
     */
    AnimationDescriptor(const char* type, const char* motion, const char* prop, float from,
                        float to, int delay = 0)
        : animationType(type), motionToken(motion), property(prop), fromValue(from), toValue(to),
          delayMs(delay) {}
};

/**
 * @brief  Abstract strategy interface for animation customization.
 *
 * @details The Animation Strategy pattern allows different widget types
 *          to customize animation behavior without modifying the factory.
 *          Each strategy can:
 *          - Adjust animation parameters (duration, easing, values)
 *          - Override animation types for specific widgets
 *          - Enable/disable animations based on conditions
 *
 *          Strategies are applied during animation creation, before
 *          the actual animation object is instantiated. This allows
 *          for runtime customization based on widget state, system
 *          settings, or other contextual factors.
 *
 * @note    Strategies should be stateless and reusable across widgets.
 *          Do not store widget pointers in strategies; use only the
 *          provided widget parameter during the adjust() call.
 * @warning Do not store the widget pointer passed to adjust(); it may
 *          become invalid after the call returns.
 * @throws  None (all methods are noexcept safe)
 * @since   0.1
 * @ingroup ui_components_material
 *
 * @code
 * // Custom strategy for buttons
 * class ButtonStrategy : public AnimationStrategy {
 * public:
 *     AnimationDescriptor adjust(const AnimationDescriptor& desc,
 *                               QWidget* widget) override {
 *         // Buttons use shorter animations
 *         AnimationDescriptor adjusted = desc;
 *         if (strcmp(desc.motionToken, "md.motion.mediumEnter") == 0) {
 *             adjusted.motionToken = "md.motion.shortEnter";
 *         }
 *         return adjusted;
 *     }
 * };
 *
 * // Usage
 * factory->setStrategy(std::make_unique<ButtonStrategy>());
 * @endcode
 */
class CF_UI_EXPORT AnimationStrategy {
  public:
    /**
     * @brief  Virtual destructor.
     *
     * @since 0.1
     */
    virtual ~AnimationStrategy() = default;

    /**
     * @brief  Adjust animation descriptor for a specific widget.
     *
     * @details Called by the factory before creating an animation.
     *          Subclasses can modify any field in the descriptor to
     *          customize animation behavior.
     *
     *          Common adjustments include:
     *          - Changing motion token (duration/easing)
     *          - Substituting animation type (slide → fade)
     *          - Modifying value ranges
     *          - Adding delays
     *
     * @param[in]     descriptor Original animation descriptor.
     * @param[in]     widget Target widget (may be nullptr).
     *
     * @return        Adjusted animation descriptor.
     *
     * @throws     None
     * @note       The default implementation returns the descriptor unchanged.
     * @warning    Do not store the widget pointer; it may be invalid after
     *             this call returns.
     * @since      0.1
     * @ingroup    ui_components_material
     *
     * @code
     * AnimationDescriptor adjust(const AnimationDescriptor& desc,
     *                           QWidget* widget) override {
     *     AnimationDescriptor result = desc;
     *     // Reduce animation duration for small widgets
     *     if (widget && widget->width() < 100) {
     *         result.motionToken = "md.motion.shortEnter";
     *     }
     *     return result;
     * }
     * @endcode
     */
    virtual AnimationDescriptor adjust(const AnimationDescriptor& descriptor, QWidget* widget) = 0;

    /**
     * @brief  Check if animations should be enabled for a widget.
     *
     * @details Allows strategies to conditionally disable animations
     *          based on widget state or system settings. This can be
     *          used for accessibility, performance, or user preference.
     *
     *          Common reasons to disable:
     *          - System accessibility setting (reduce motion)
     *          - Low-performance mode
     *          - Widget-specific conditions
     *
     * @param[in]     widget Target widget (may be nullptr).
     *
     * @return        true if animation should be enabled, false otherwise.
     *
     * @throws     None
     * @note       The default implementation returns the global enabled state.
     * @warning    None
     * @since      0.1
     * @ingroup    ui_components_material
     *
     * @code
     * bool shouldEnable(QWidget* widget) const override {
     *     // Disable animations in low-performance mode
     *     if (isLowPerformanceMode()) return false;
     *     return globalEnabled_;
     * }
     * @endcode
     */
    virtual bool shouldEnable(QWidget* widget) const;

    /**
     * @brief  Set the global enabled state for this strategy.
     *
     * @details This affects all subsequent shouldEnable() calls.
     *          Individual widget checks can still override this by
     *          returning false in their implementation.
     *
     * @param[in]     enabled true to enable animations, false to disable.
     *
     * @throws        None
     * @note       This affects all subsequent shouldEnable() calls.
     * @warning    None
     * @since      0.1
     * @ingroup    ui_components_material
     *
     * @code
     * // Disable animations during heavy processing
     * strategy->setGlobalEnabled(false);
     * // ... do heavy work ...
     * strategy->setGlobalEnabled(true);
     * @endcode
     */
    void setGlobalEnabled(bool enabled);

    /**
     * @brief  Get the global enabled state.
     *
     * @return true if globally enabled, false otherwise.
     *
     * @since 0.1
     */
    bool globalEnabled() const { return globalEnabled_; }

  protected:
    /// Global enabled state for animations using this strategy.
    bool globalEnabled_ = true;
};

/**
 * @brief  Default animation strategy implementation.
 *
 * @details Provides a no-op adjustment that returns the descriptor
 *          unchanged. Used when no specific strategy is needed.
 *
 *          This is the default strategy used by MaterialAnimationFactory
 *          when no custom strategy is provided via setStrategy().
 *
 * @since  0.1
 * @ingroup ui_components_material
 */
class CF_UI_EXPORT DefaultAnimationStrategy : public AnimationStrategy {
  public:
    /**
     * @brief  Returns the descriptor unchanged.
     *
     * @param[in]     descriptor The animation descriptor.
     * @param[in]     widget The target widget (unused).
     *
     * @return        The original descriptor, unmodified.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_components_material
     */
    AnimationDescriptor adjust(const AnimationDescriptor& descriptor, QWidget* widget) override;
};

} // namespace cf::ui::components::material
