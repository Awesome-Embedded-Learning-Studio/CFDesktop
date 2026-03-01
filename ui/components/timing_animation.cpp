/**
 * @file    timing_animation.cpp
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief   Timing-based Animation Interface Implementation
 * @version 0.1
 * @date    2026-02-28
 *
 * @copyright Copyright (c) 2026
 *
 * @details
 * Implementation file for ICFTimingAnimation interface.
 * This file is required for Qt's MOC (Meta-Object Compiler) to generate
 * the meta-object code for the Q_OBJECT interface.
 */

#include "timing_animation.h"

namespace cf::ui::components {

ICFTimingAnimation::ICFTimingAnimation(cf::ui::core::IMotionSpec* spec, QObject* parent)
    : ICFAbstractAnimation(parent), motion_spec_(spec) {}

} // namespace cf::ui::components
