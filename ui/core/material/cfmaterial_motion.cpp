/**
 * @file cfmaterial_motion.cpp
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Material Design 3 Motion Implementation
 * @version 0.1
 * @date 2026-02-26
 *
 * @copyright Copyright (c) 2026
 *
 * @details
 * Implements MaterialMotionScheme with token registration and query methods.
 * All motion presets follow Material Design 3 motion specifications.
 */
#include "cfmaterial_motion.h"

namespace cf::ui::core {

MaterialMotionScheme::MaterialMotionScheme() {
    // Register all motion tokens
    namespace literals = cf::ui::core::token::literals;
    auto& r = registry_;

    // Durations (Material Design 3 Motion specifications)
    r.register_dynamic<int>(literals::MOTION_SHORT_ENTER_DURATION, 200);
    r.register_dynamic<int>(literals::MOTION_SHORT_EXIT_DURATION, 150);
    r.register_dynamic<int>(literals::MOTION_MEDIUM_ENTER_DURATION, 300);
    r.register_dynamic<int>(literals::MOTION_MEDIUM_EXIT_DURATION, 250);
    r.register_dynamic<int>(literals::MOTION_LONG_ENTER_DURATION, 450);
    r.register_dynamic<int>(literals::MOTION_LONG_EXIT_DURATION, 400);
    r.register_dynamic<int>(literals::MOTION_STATE_CHANGE_DURATION, 200);
    r.register_dynamic<int>(literals::MOTION_RIPPLE_EXPAND_DURATION, 400);
    r.register_dynamic<int>(literals::MOTION_RIPPLE_FADE_DURATION, 150);

    // Easing types
    using EType = cf::ui::base::Easing::Type;
    r.register_dynamic<int>(literals::MOTION_SHORT_ENTER_EASING,
                            static_cast<int>(EType::EmphasizedDecelerate));
    r.register_dynamic<int>(literals::MOTION_SHORT_EXIT_EASING,
                            static_cast<int>(EType::EmphasizedAccelerate));
    r.register_dynamic<int>(literals::MOTION_MEDIUM_ENTER_EASING,
                            static_cast<int>(EType::EmphasizedDecelerate));
    r.register_dynamic<int>(literals::MOTION_MEDIUM_EXIT_EASING,
                            static_cast<int>(EType::EmphasizedAccelerate));
    r.register_dynamic<int>(literals::MOTION_LONG_ENTER_EASING,
                            static_cast<int>(EType::Emphasized));
    r.register_dynamic<int>(literals::MOTION_LONG_EXIT_EASING, static_cast<int>(EType::Emphasized));
    r.register_dynamic<int>(literals::MOTION_STATE_CHANGE_EASING,
                            static_cast<int>(EType::Standard));
    r.register_dynamic<int>(literals::MOTION_RIPPLE_EXPAND_EASING,
                            static_cast<int>(EType::Standard));
    r.register_dynamic<int>(literals::MOTION_RIPPLE_FADE_EASING, static_cast<int>(EType::Linear));
}

int MaterialMotionScheme::queryDuration(const char* name) {
    // Build the full duration token name
    std::string fullName = std::string("md.motion.") + name + ".duration";
    auto result = registry_.get_dynamic<int>(fullName.c_str());
    return result ? **result : 200; // Default 200ms
}

int MaterialMotionScheme::queryEasing(const char* name) {
    // Build the full easing token name
    std::string fullName = std::string("md.motion.") + name + ".easing";
    auto result = registry_.get_dynamic<int>(fullName.c_str());
    return result ? **result : static_cast<int>(cf::ui::base::Easing::Type::Standard);
}

int MaterialMotionScheme::queryDelay(const char* name) {
    // Motion specs default to 0 delay
    (void)name; // Suppress unused parameter warning
    return 0;
}

MotionSpec MaterialMotionScheme::getMotionSpec(const char* name) {
    // Check cache first
    auto it = spec_cache_.find(name);
    if (it != spec_cache_.end()) {
        return it->second;
    }

    // Build new spec
    MotionSpec spec;
    spec.durationMs = queryDuration(name);
    spec.easing = static_cast<cf::ui::base::Easing::Type>(queryEasing(name));
    spec.delayMs = queryDelay(name);

    // Cache for future queries
    spec_cache_[name] = spec;
    return spec;
}

} // namespace cf::ui::core
