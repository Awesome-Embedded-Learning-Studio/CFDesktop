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
 * Implements MaterialMotionScheme with motion preset registration and query methods.
 * All motion presets follow Material Design 3 motion specifications.
 */
#include "cfmaterial_motion.h"

namespace cf::ui::core {

MaterialMotionScheme::MaterialMotionScheme() {
    // Durations (Material Design 3 Motion specifications)
    durations_["md.motion.shortEnter.duration"] = 200;
    durations_["md.motion.shortExit.duration"] = 150;
    durations_["md.motion.mediumEnter.duration"] = 300;
    durations_["md.motion.mediumExit.duration"] = 250;
    durations_["md.motion.longEnter.duration"] = 450;
    durations_["md.motion.longExit.duration"] = 400;
    durations_["md.motion.stateChange.duration"] = 200;
    durations_["md.motion.rippleExpand.duration"] = 400;
    durations_["md.motion.rippleFade.duration"] = 150;

    // Easing types
    using EType = cf::ui::base::Easing::Type;
    easings_["md.motion.shortEnter.easing"] = static_cast<int>(EType::EmphasizedDecelerate);
    easings_["md.motion.shortExit.easing"] = static_cast<int>(EType::EmphasizedAccelerate);
    easings_["md.motion.mediumEnter.easing"] = static_cast<int>(EType::EmphasizedDecelerate);
    easings_["md.motion.mediumExit.easing"] = static_cast<int>(EType::EmphasizedAccelerate);
    easings_["md.motion.longEnter.easing"] = static_cast<int>(EType::Emphasized);
    easings_["md.motion.longExit.easing"] = static_cast<int>(EType::Emphasized);
    easings_["md.motion.stateChange.easing"] = static_cast<int>(EType::Standard);
    easings_["md.motion.rippleExpand.easing"] = static_cast<int>(EType::Standard);
    easings_["md.motion.rippleFade.easing"] = static_cast<int>(EType::Linear);
}

int MaterialMotionScheme::queryDuration(const char* name) {
    std::string fullName = std::string("md.motion.") + name + ".duration";
    auto it = durations_.find(fullName);
    return it != durations_.end() ? it->second : 200;
}

int MaterialMotionScheme::queryEasing(const char* name) {
    std::string fullName = std::string("md.motion.") + name + ".easing";
    auto it = easings_.find(fullName);
    return it != easings_.end() ? it->second
                                : static_cast<int>(cf::ui::base::Easing::Type::Standard);
}

int MaterialMotionScheme::queryDelay(const char* name) {
    (void)name;
    return 0;
}

MotionSpec MaterialMotionScheme::getMotionSpec(const char* name) {
    auto it = spec_cache_.find(name);
    if (it != spec_cache_.end()) {
        return it->second;
    }

    MotionSpec spec;
    spec.durationMs = queryDuration(name);
    spec.easing = static_cast<cf::ui::base::Easing::Type>(queryEasing(name));
    spec.delayMs = queryDelay(name);

    spec_cache_[name] = spec;
    return spec;
}

} // namespace cf::ui::core
