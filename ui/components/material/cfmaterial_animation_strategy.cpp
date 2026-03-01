/**
 * @file    cfmaterial_animation_strategy.cpp
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief   Animation Strategy Implementation
 * @version 0.1
 * @date    2026-02-28
 *
 * @copyright Copyright (c) 2026
 *
 * @details
 * Implements the AnimationStrategy interface methods.
 */

#include "cfmaterial_animation_strategy.h"

namespace cf::ui::components::material {

// =============================================================================
// AnimationStrategy
// =============================================================================

bool AnimationStrategy::shouldEnable(QWidget* widget) const {
    (void)widget; // Suppress unused parameter warning
    return globalEnabled_;
}

void AnimationStrategy::setGlobalEnabled(bool enabled) {
    globalEnabled_ = enabled;
}

// =============================================================================
// DefaultAnimationStrategy
// =============================================================================

AnimationDescriptor DefaultAnimationStrategy::adjust(const AnimationDescriptor& descriptor,
                                                     QWidget* widget) {

    // Return descriptor unchanged - this is a no-op strategy
    (void)widget; // Suppress unused parameter warning
    return descriptor;
}

} // namespace cf::ui::components::material
