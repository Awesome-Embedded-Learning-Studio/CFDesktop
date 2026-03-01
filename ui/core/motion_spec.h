/**
 * @file motion_spec.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Motion specification interface for Material Design animations
 * @version 0.1
 * @date 2026-02-26
 *
 * @copyright Copyright (c) 2026
 *
 * @details
 * Defines the interface for motion/animation specifications following
 * Material Design 3 motion principles. This interface provides a way to
 * query animation duration, easing, and delay values by token name.
 */
#pragma once
#include "export.h"
#include <cstdint>

namespace cf::ui::core {

/**
 * @brief Motion specification interface.
 *
 * Defines the contract for querying motion/animation specifications.
 * Implementations should provide access to Material Design 3 motion
 * presets with proper duration, easing, and delay values.
 *
 * @since 0.1
 * @ingroup ui_core
 */
struct CF_UI_EXPORT IMotionSpec {
    virtual ~IMotionSpec() = default;

    /**
     * @brief Query motion duration by token name.
     *
     * @param[in] name Motion token name (e.g., "md.motion.shortEnter").
     * @return Duration in milliseconds.
     *
     * @since 0.1
     */
    virtual int queryDuration(const char* name) = 0;

    /**
     * @brief Query motion easing type by token name.
     *
     * @param[in] name Motion token name.
     * @return Easing type enum value (as int for cross-language compatibility).
     *
     * @since 0.1
     */
    virtual int queryEasing(const char* name) = 0;

    /**
     * @brief Query motion delay by token name.
     *
     * @param[in] name Motion token name.
     * @return Delay in milliseconds (default 0).
     *
     * @since 0.1
     */
    virtual int queryDelay(const char* name) = 0;
};

} // namespace cf::ui::core
