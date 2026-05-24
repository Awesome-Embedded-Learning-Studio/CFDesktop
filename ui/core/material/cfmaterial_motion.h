/**
 * @file cfmaterial_motion.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Material Design 3 Motion Scheme
 * @version 0.1
 * @date 2026-02-26
 *
 * @copyright Copyright (c) 2026
 *
 * @details
 * Implements the complete Material Design 3 motion system with duration,
 * easing, and delay specifications.
 */
#pragma once
#include <QEasingCurve>
#include <string>
#include <unordered_map>

#include "../motion_spec.h"
#include "base/easing.h"
#include "export.h"

namespace cf::ui::core {

// =============================================================================
// Motion Data Structure
// =============================================================================

/**
 * @brief Motion specification structure.
 *
 * Describes a complete animation specification with duration,
 * easing curve, and optional delay.
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
     *
     * @return         MotionSpec with duration 200ms and EmphasizedDecelerate easing.
     *
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
     *
     * @return         MotionSpec with duration 150ms and EmphasizedAccelerate easing.
     *
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
     *
     * @return         MotionSpec with duration 300ms and EmphasizedDecelerate easing.
     *
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
     *
     * @return         MotionSpec with duration 250ms and EmphasizedAccelerate easing.
     *
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
     *
     * @return         MotionSpec with duration 450ms and Emphasized easing.
     *
     * @since          0.1
     * @ingroup        ui_core
     */
    static MotionSpec longEnter() { return {450, cf::ui::base::Easing::Type::Emphasized, 0}; }

    /**
     * @brief  Long exit motion preset.
     *
     * Duration: 400ms, Easing: Emphasized
     *
     * @return         MotionSpec with duration 400ms and Emphasized easing.
     *
     * @since          0.1
     * @ingroup        ui_core
     */
    static MotionSpec longExit() { return {400, cf::ui::base::Easing::Type::Emphasized, 0}; }

    /**
     * @brief  State change motion preset.
     *
     * Duration: 200ms, Easing: Standard
     *
     * @return         MotionSpec with duration 200ms and Standard easing.
     *
     * @since          0.1
     * @ingroup        ui_core
     */
    static MotionSpec stateChange() { return {200, cf::ui::base::Easing::Type::Standard, 0}; }

    /**
     * @brief  Ripple expand motion preset.
     *
     * Duration: 400ms, Easing: Standard
     *
     * @return         MotionSpec with duration 400ms and Standard easing.
     *
     * @since          0.1
     * @ingroup        ui_core
     */
    static MotionSpec rippleExpand() { return {400, cf::ui::base::Easing::Type::Standard, 0}; }

    /**
     * @brief  Ripple fade motion preset.
     *
     * Duration: 150ms, Easing: Linear
     *
     * @return         MotionSpec with duration 150ms and Linear easing.
     *
     * @since          0.1
     * @ingroup        ui_core
     */
    static MotionSpec rippleFade() { return {150, cf::ui::base::Easing::Type::Linear, 0}; }
};

// =============================================================================
// Material Motion Scheme
// =============================================================================

/**
 * @brief  Material Design 3 Motion Scheme.
 *
 * @details Implements the complete Material Design 3 motion system with
 * duration and easing specifications.
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
 * auto motionScheme = cf::ui::core::material::motion();
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
    std::unordered_map<std::string, int> durations_;
    std::unordered_map<std::string, int> easings_;
    mutable std::unordered_map<std::string, MotionSpec> spec_cache_;
};

} // namespace cf::ui::core
