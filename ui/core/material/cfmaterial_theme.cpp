/**
 * @file cfmaterial_theme.cpp
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Material Design 3 Theme Implementation
 * @version 0.1
 * @date 2026-02-27
 *
 * @copyright Copyright (c) 2026
 *
 */

#include "cfmaterial_theme.h"

namespace cf::ui::core {

MaterialTheme::MaterialTheme(std::unique_ptr<MaterialColorScheme> color_scheme,
                             std::unique_ptr<MaterialTypography> font_type,
                             std::unique_ptr<MaterialRadiusScale> radius_scale,
                             std::unique_ptr<MaterialMotionScheme> motion_spec) {
    // Initialize base class protected members
    color_scheme_ = std::move(color_scheme);
    font_type_ = std::move(font_type);
    radius_scale_ = std::move(radius_scale);
    motion_spec_ = std::move(motion_spec);
}

} // namespace cf::ui::core
