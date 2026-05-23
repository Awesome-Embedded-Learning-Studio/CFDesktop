/**
 * @file cfmaterial_radius_scale.cpp
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Material Design 3 Radius Scale Implementation
 * @version 0.1
 * @date 2026-02-26
 *
 * @copyright Copyright (c) 2026
 *
 */

#include "cfmaterial_radius_scale.h"

namespace cf::ui::core {

MaterialRadiusScale::MaterialRadiusScale() {
    radii_["md.shape.cornerNone"] = 0.0f;
    radii_["md.shape.cornerExtraSmall"] = 4.0f;
    radii_["md.shape.cornerSmall"] = 8.0f;
    radii_["md.shape.cornerMedium"] = 12.0f;
    radii_["md.shape.cornerLarge"] = 16.0f;
    radii_["md.shape.cornerExtraLarge"] = 28.0f;
    radii_["md.shape.cornerExtraExtraLarge"] = 32.0f;
}

float MaterialRadiusScale::queryRadiusScale(const char* name) {
    auto it = radii_.find(name);
    return it != radii_.end() ? it->second : 0.0f;
}

void MaterialRadiusScale::setRadius(const std::string& name, float radius) {
    radii_[name] = radius;
}

} // namespace cf::ui::core
