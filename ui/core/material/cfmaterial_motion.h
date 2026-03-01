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

#include "../motion_spec.h"
#include "../token.hpp"
#include "../token/motion/cfmaterial_motion_token_literals.h"
#include "base/easing.h"
#include "base/hash/constexpr_fnv1a.hpp"
#include "export.h"

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
     * @brief  Convert easing type to QEasingCurve.
     *
     * Convenience method to convert the easing type to a Qt easing curve.
     *
     * @return         QEasingCurve corresponding to the easing type.
     *
     * @throws         None
     * @note           None
     * @warning        None
     * @since          0.1
     * @ingroup        ui_core
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
     * @brief  Short enter motion preset.
     *
     * Duration: 200ms, Easing: EmphasizedDecelerate
     * Used for: Small elements entering the screen
     *
     * @return         MotionSpec with duration 200ms and EmphasizedDecelerate easing.
     *
     * @throws         None
     * @note           None
     * @warning        None
     * @since          0.1
     * @ingroup        ui_core
     */
    static MotionSpec shortEnter() {
        return {200, cf::ui::base::Easing::Type::EmphasizedDecelerate, 0};
    }

    /**
     * @brief  Short exit motion preset.
     *
     * Duration: 150ms, Easing: EmphasizedAccelerate
     * Used for: Small elements exiting the screen
     *
     * @return         MotionSpec with duration 150ms and EmphasizedAccelerate easing.
     *
     * @throws         None
     * @note           None
     * @warning        None
     * @since          0.1
     * @ingroup        ui_core
     */
    static MotionSpec shortExit() {
        return {150, cf::ui::base::Easing::Type::EmphasizedAccelerate, 0};
    }

    /**
     * @brief  Medium enter motion preset.
     *
     * Duration: 300ms, Easing: EmphasizedDecelerate
     * Used for: Medium-sized elements entering the screen
     *
     * @return         MotionSpec with duration 300ms and EmphasizedDecelerate easing.
     *
     * @throws         None
     * @note           None
     * @warning        None
     * @since          0.1
     * @ingroup        ui_core
     */
    static MotionSpec mediumEnter() {
        return {300, cf::ui::base::Easing::Type::EmphasizedDecelerate, 0};
    }

    /**
     * @brief  Medium exit motion preset.
     *
     * Duration: 250ms, Easing: EmphasizedAccelerate
     * Used for: Medium-sized elements exiting the screen
     *
     * @return         MotionSpec with duration 250ms and EmphasizedAccelerate easing.
     *
     * @throws         None
     * @note           None
     * @warning        None
     * @since          0.1
     * @ingroup        ui_core
     */
    static MotionSpec mediumExit() {
        return {250, cf::ui::base::Easing::Type::EmphasizedAccelerate, 0};
    }

    /**
     * @brief  Long enter motion preset.
     *
     * Duration: 450ms, Easing: Emphasized
     * Used for: Large elements entering the screen
     *
     * @return         MotionSpec with duration 450ms and Emphasized easing.
     *
     * @throws         None
     * @note           None
     * @warning        None
     * @since          0.1
     * @ingroup        ui_core
     */
    static MotionSpec longEnter() { return {450, cf::ui::base::Easing::Type::Emphasized, 0}; }

    /**
     * @brief  Long exit motion preset.
     *
     * Duration: 400ms, Easing: Emphasized
     * Used for: Large elements exiting the screen
     *
     * @return         MotionSpec with duration 400ms and Emphasized easing.
     *
     * @throws         None
     * @note           None
     * @warning        None
     * @since          0.1
     * @ingroup        ui_core
     */
    static MotionSpec longExit() { return {400, cf::ui::base::Easing::Type::Emphasized, 0}; }

    /**
     * @brief  State change motion preset.
     *
     * Duration: 200ms, Easing: Standard
     * Used for: State layer animations
     *
     * @return         MotionSpec with duration 200ms and Standard easing.
     *
     * @throws         None
     * @note           None
     * @warning        None
     * @since          0.1
     * @ingroup        ui_core
     */
    static MotionSpec stateChange() { return {200, cf::ui::base::Easing::Type::Standard, 0}; }

    /**
     * @brief  Ripple expand motion preset.
     *
     * Duration: 400ms, Easing: Standard
     * Used for: Ripple effect expansion
     *
     * @return         MotionSpec with duration 400ms and Standard easing.
     *
     * @throws         None
     * @note           None
     * @warning        None
     * @since          0.1
     * @ingroup        ui_core
     */
    static MotionSpec rippleExpand() { return {400, cf::ui::base::Easing::Type::Standard, 0}; }

    /**
     * @brief  Ripple fade motion preset.
     *
     * Duration: 150ms, Easing: Linear
     * Used for: Ripple effect fade out
     *
     * @return         MotionSpec with duration 150ms and Linear easing.
     *
     * @throws         None
     * @note           None
     * @warning        None
     * @since          0.1
     * @ingroup        ui_core
     */
    static MotionSpec rippleFade() { return {150, cf::ui::base::Easing::Type::Linear, 0}; }
};

// =============================================================================
// Material Motion Scheme
// =============================================================================

/**
 * @brief  Material Design 3 Motion Scheme with EmbeddedTokenRegistry.
 *
 * @details Implements the complete Material Design 3 motion system with
 * duration and easing specifications. Motion values are stored in
 * an embedded registry for independent scheme instances.
 *
 * Factory functions are available in the cf::ui::core::material namespace.
 *
 * @note           None
 * @warning        None
 * @throws         None
 * @since          0.1
 * @ingroup        ui_core
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
     *
     * @return Duration in milliseconds.
     *
     * @throws    None.
     *
     * @note      Returns 0 if preset not found.
     *
     * @warning   None.
     *
     * @since     0.1
     * @ingroup   ui_core
     */
    int queryDuration(const char* name) override;

    /**
     * @brief Query a motion easing type by name.
     *
     * @param[in] name Motion preset name.
     *
     * @return Easing type as int (cf::ui::base::Easing::Type).
     *
     * @throws    None.
     *
     * @note      Returns 0 (Linear) if preset not found.
     *
     * @warning   None.
     *
     * @since     0.1
     * @ingroup   ui_core
     */
    int queryEasing(const char* name) override;

    /**
     * @brief Query a motion delay by name.
     *
     * @param[in] name Motion preset name.
     *
     * @return Delay in milliseconds (always returns 0 for standard presets).
     *
     * @throws    None.
     *
     * @note      Returns 0 if preset not found.
     *
     * @warning   None.
     *
     * @since     0.1
     * @ingroup   ui_core
     */
    int queryDelay(const char* name) override;

    /**
     * @brief Get a complete MotionSpec by name.
     *
     * @param[in] name Motion preset name (e.g., "shortEnter").
     *
     * @return MotionSpec structure with duration, easing, delay.
     *
     * @throws    None.
     *
     * @note      Returns default MotionSpec (0ms, Linear, 0delay) if not found.
     *
     * @warning   None.
     *
     * @since     0.1
     * @ingroup   ui_core
     */
    MotionSpec getMotionSpec(const char* name);

    /**
     * @brief Access the embedded token registry.
     *
     * @return Reference to the EmbeddedTokenRegistry.
     *
     * @throws    None.
     *
     * @note      Provides direct access to internal token storage.
     *
     * @warning   Modifying tokens directly may affect color scheme behavior.
     *
     * @since     0.1
     * @ingroup   ui_core
     */
    EmbeddedTokenRegistry& registry() { return registry_; }

    /**
     * @brief Access the embedded token registry (const overload).
     *
     * @return Const reference to the EmbeddedTokenRegistry.
     *
     * @throws    None.
     *
     * @note      Read-only access to internal token storage.
     *
     * @warning   None.
     *
     * @since     0.1
     * @ingroup   ui_core
     */
    const EmbeddedTokenRegistry& registry() const { return registry_; }

    /**
     * @brief Motion presets group structure.
     *
     * Contains all predefined motion specifications.
     *
     * @since 0.1
     */
    struct MotionPresetsGroup {
        MotionSpec shortEnter;   ///< Short enter motion preset.
        MotionSpec shortExit;    ///< Short exit motion preset.
        MotionSpec mediumEnter;  ///< Medium enter motion preset.
        MotionSpec mediumExit;   ///< Medium exit motion preset.
        MotionSpec longEnter;    ///< Long enter motion preset.
        MotionSpec longExit;     ///< Long exit motion preset.
        MotionSpec stateChange;  ///< State change motion preset.
        MotionSpec rippleExpand; ///< Ripple expand motion preset.
        MotionSpec rippleFade;   ///< Ripple fade motion preset.
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
