/**
 * @file cfmaterial_motion.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Material Design 3 Motion Scheme with EmbeddedTokenRegistry
 * @version 0.1
 * @date 2026-02-26
 *
 * @copyright Copyright (c) 2026
 *
 * @details
 * Implements the complete Material Design 3 motion system with duration,
 * easing, and delay specifications. Motion specs are stored in an embedded
 * registry for independent scheme instances.
 */
#pragma once
#include <QEasingCurve>
#include <string>
#include <unordered_map>

#include "../../export.h"
#include "base/easing.h"
#include "base/hash/constexpr_fnv1a.hpp"
#include "motion_spec.h"
#include "token.hpp"
#include "token/motion/cfmaterial_motion_token_literals.h"

namespace cf::ui::core {

// =============================================================================
// Motion Token Type Aliases - Material Motion System
// =============================================================================
namespace tokens {
using namespace cf::ui::core::token::literals;

// Duration tokens
using ShortEnterDurationToken = StaticToken<int, cf::hash::fnv1a64(MOTION_SHORT_ENTER_DURATION)>;
using ShortExitDurationToken = StaticToken<int, cf::hash::fnv1a64(MOTION_SHORT_EXIT_DURATION)>;
using MediumEnterDurationToken = StaticToken<int, cf::hash::fnv1a64(MOTION_MEDIUM_ENTER_DURATION)>;
using MediumExitDurationToken = StaticToken<int, cf::hash::fnv1a64(MOTION_MEDIUM_EXIT_DURATION)>;
using LongEnterDurationToken = StaticToken<int, cf::hash::fnv1a64(MOTION_LONG_ENTER_DURATION)>;
using LongExitDurationToken = StaticToken<int, cf::hash::fnv1a64(MOTION_LONG_EXIT_DURATION)>;
using StateChangeDurationToken = StaticToken<int, cf::hash::fnv1a64(MOTION_STATE_CHANGE_DURATION)>;
using RippleExpandDurationToken =
    StaticToken<int, cf::hash::fnv1a64(MOTION_RIPPLE_EXPAND_DURATION)>;
using RippleFadeDurationToken = StaticToken<int, cf::hash::fnv1a64(MOTION_RIPPLE_FADE_DURATION)>;

// Easing tokens (stored as int enum values)
using ShortEnterEasingToken = StaticToken<int, cf::hash::fnv1a64(MOTION_SHORT_ENTER_EASING)>;
using ShortExitEasingToken = StaticToken<int, cf::hash::fnv1a64(MOTION_SHORT_EXIT_EASING)>;
using MediumEnterEasingToken = StaticToken<int, cf::hash::fnv1a64(MOTION_MEDIUM_ENTER_EASING)>;
using MediumExitEasingToken = StaticToken<int, cf::hash::fnv1a64(MOTION_MEDIUM_EXIT_EASING)>;
using LongEnterEasingToken = StaticToken<int, cf::hash::fnv1a64(MOTION_LONG_ENTER_EASING)>;
using LongExitEasingToken = StaticToken<int, cf::hash::fnv1a64(MOTION_LONG_EXIT_EASING)>;
using StateChangeEasingToken = StaticToken<int, cf::hash::fnv1a64(MOTION_STATE_CHANGE_EASING)>;
using RippleExpandEasingToken = StaticToken<int, cf::hash::fnv1a64(MOTION_RIPPLE_EXPAND_EASING)>;
using RippleFadeEasingToken = StaticToken<int, cf::hash::fnv1a64(MOTION_RIPPLE_FADE_EASING)>;

} // namespace tokens

// =============================================================================
// Motion Data Structure
// =============================================================================

/**
 * @brief Motion specification structure.
 *
 * Describes a complete animation specification with duration,
 * easing curve, and optional delay. This is a widget-independent,
 * reusable description of animation behavior.
 *
 * @since 0.1
 * @ingroup ui_core
 */
struct MotionSpec {
    int durationMs;                    ///< Duration in milliseconds
    cf::ui::base::Easing::Type easing; ///< Easing type
    int delayMs = 0;                   ///< Delay in milliseconds

    /**
     * @brief Convert to QEasingCurve.
     *
     * Convenience method to convert the easing type to a Qt easing curve.
     *
     * @return QEasingCurve corresponding to the easing type.
     *
     * @since 0.1
     */
    QEasingCurve toEasingCurve() const { return cf::ui::base::Easing::fromEasingType(easing); }

    /**
     * @brief Equality comparison operator.
     *
     * @since 0.1
     */
    bool operator==(const MotionSpec& other) const {
        return durationMs == other.durationMs && easing == other.easing && delayMs == other.delayMs;
    }

    /**
     * @brief Inequality comparison operator.
     *
     * @since 0.1
     */
    bool operator!=(const MotionSpec& other) const { return !(*this == other); }
};

// =============================================================================
// Motion Presets
// =============================================================================

/**
 * @brief Static motion preset functions.
 *
 * Provides convenient access to predefined motion specifications
 * following Material Design 3 motion principles.
 *
 * @since 0.1
 * @ingroup ui_core
 */
struct MotionPresets {
    /**
     * @brief Short enter motion preset.
     *
     * Duration: 200ms, Easing: EmphasizedDecelerate
     * Used for: Small elements entering the screen
     *
     * @since 0.1
     */
    static MotionSpec shortEnter() {
        return {200, cf::ui::base::Easing::Type::EmphasizedDecelerate, 0};
    }

    /**
     * @brief Short exit motion preset.
     *
     * Duration: 150ms, Easing: EmphasizedAccelerate
     * Used for: Small elements exiting the screen
     *
     * @since 0.1
     */
    static MotionSpec shortExit() {
        return {150, cf::ui::base::Easing::Type::EmphasizedAccelerate, 0};
    }

    /**
     * @brief Medium enter motion preset.
     *
     * Duration: 300ms, Easing: EmphasizedDecelerate
     * Used for: Medium-sized elements entering the screen
     *
     * @since 0.1
     */
    static MotionSpec mediumEnter() {
        return {300, cf::ui::base::Easing::Type::EmphasizedDecelerate, 0};
    }

    /**
     * @brief Medium exit motion preset.
     *
     * Duration: 250ms, Easing: EmphasizedAccelerate
     * Used for: Medium-sized elements exiting the screen
     *
     * @since 0.1
     */
    static MotionSpec mediumExit() {
        return {250, cf::ui::base::Easing::Type::EmphasizedAccelerate, 0};
    }

    /**
     * @brief Long enter motion preset.
     *
     * Duration: 450ms, Easing: Emphasized
     * Used for: Large elements entering the screen
     *
     * @since 0.1
     */
    static MotionSpec longEnter() { return {450, cf::ui::base::Easing::Type::Emphasized, 0}; }

    /**
     * @brief Long exit motion preset.
     *
     * Duration: 400ms, Easing: Emphasized
     * Used for: Large elements exiting the screen
     *
     * @since 0.1
     */
    static MotionSpec longExit() { return {400, cf::ui::base::Easing::Type::Emphasized, 0}; }

    /**
     * @brief State change motion preset.
     *
     * Duration: 200ms, Easing: Standard
     * Used for: State layer animations
     *
     * @since 0.1
     */
    static MotionSpec stateChange() { return {200, cf::ui::base::Easing::Type::Standard, 0}; }

    /**
     * @brief Ripple expand motion preset.
     *
     * Duration: 400ms, Easing: Standard
     * Used for: Ripple effect expansion
     *
     * @since 0.1
     */
    static MotionSpec rippleExpand() { return {400, cf::ui::base::Easing::Type::Standard, 0}; }

    /**
     * @brief Ripple fade motion preset.
     *
     * Duration: 150ms, Easing: Linear
     * Used for: Ripple effect fade out
     *
     * @since 0.1
     */
    static MotionSpec rippleFade() { return {150, cf::ui::base::Easing::Type::Linear, 0}; }
};

// =============================================================================
// Material Motion Scheme
// =============================================================================

/**
 * @brief Material Design 3 Motion Scheme with EmbeddedTokenRegistry.
 *
 * Implements the complete Material Design 3 motion system with
 * duration and easing specifications. Motion values are stored in
 * an embedded registry for independent scheme instances.
 *
 * Factory functions are available in the cf::ui::core::material namespace.
 *
 * @since 0.1
 * @ingroup ui_core
 *
 * @code
 * #include "material_factory.hpp"
 *
 * // Create the default motion scheme
 * auto motionScheme = cf::ui::core::material::motion();
 *
 * // Query motion by name
 * int duration = motionScheme.queryDuration("shortEnter");  // 200
 * @endcode
 */
class CF_UI_EXPORT MaterialMotionScheme : public IMotionSpec {
  public:
    MaterialMotionScheme();
    ~MaterialMotionScheme() override = default;

    // Non-copyable, movable
    MaterialMotionScheme(const MaterialMotionScheme&) = delete;
    MaterialMotionScheme& operator=(const MaterialMotionScheme&) = delete;
    MaterialMotionScheme(MaterialMotionScheme&&) noexcept = default;
    MaterialMotionScheme& operator=(MaterialMotionScheme&&) noexcept = default;

    /**
     * @brief Query a motion duration by name.
     *
     * @param[in] name Motion preset name (e.g., "shortEnter").
     * @return Duration in milliseconds.
     *
     * @since 0.1
     */
    int queryDuration(const char* name) override;

    /**
     * @brief Query a motion easing type by name.
     *
     * @param[in] name Motion preset name.
     * @return Easing type as int (cf::ui::base::Easing::Type).
     *
     * @since 0.1
     */
    int queryEasing(const char* name) override;

    /**
     * @brief Query a motion delay by name.
     *
     * @param[in] name Motion preset name.
     * @return Delay in milliseconds (always returns 0 for standard presets).
     *
     * @since 0.1
     */
    int queryDelay(const char* name) override;

    /**
     * @brief Get a complete MotionSpec by name.
     *
     * @param[in] name Motion preset name (e.g., "shortEnter").
     * @return MotionSpec structure with duration, easing, delay.
     *
     * @since 0.1
     */
    MotionSpec getMotionSpec(const char* name);

    /**
     * @brief Access the embedded token registry.
     *
     * @return Reference to the EmbeddedTokenRegistry.
     *
     * @since 0.1
     */
    EmbeddedTokenRegistry& registry() { return registry_; }
    const EmbeddedTokenRegistry& registry() const { return registry_; }

    /**
     * @brief Motion presets group structure.
     *
     * Contains all predefined motion specifications.
     *
     * @since 0.1
     */
    struct MotionPresetsGroup {
        MotionSpec shortEnter;
        MotionSpec shortExit;
        MotionSpec mediumEnter;
        MotionSpec mediumExit;
        MotionSpec longEnter;
        MotionSpec longExit;
        MotionSpec stateChange;
        MotionSpec rippleExpand;
        MotionSpec rippleFade;
    };

    /**
     * @brief Get all motion presets as a group.
     *
     * @return MotionPresetsGroup containing all predefined motion specs.
     *
     * @since 0.1
     */
    [[nodiscard]] MotionPresetsGroup presets() const {
        return MotionPresetsGroup{MotionPresets::shortEnter(),  MotionPresets::shortExit(),
                                  MotionPresets::mediumEnter(), MotionPresets::mediumExit(),
                                  MotionPresets::longEnter(),   MotionPresets::longExit(),
                                  MotionPresets::stateChange(), MotionPresets::rippleExpand(),
                                  MotionPresets::rippleFade()};
    }

  private:
    EmbeddedTokenRegistry registry_;
    mutable std::unordered_map<std::string, MotionSpec> spec_cache_;
};

} // namespace cf::ui::core
