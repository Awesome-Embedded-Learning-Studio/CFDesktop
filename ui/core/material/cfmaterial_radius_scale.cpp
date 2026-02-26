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
    registerDefaultCorners();
}

void MaterialRadiusScale::registerDefaultCorners() {
    namespace literals = cf::ui::core::token::literals;
    auto& r = registry_;

    // 注册默认值（Material Design 3 规范）
    r.register_dynamic<float>(literals::CORNER_NONE, 0.0f);
    r.register_dynamic<float>(literals::CORNER_EXTRA_SMALL, 4.0f);
    r.register_dynamic<float>(literals::CORNER_SMALL, 8.0f);
    r.register_dynamic<float>(literals::CORNER_MEDIUM, 12.0f);
    r.register_dynamic<float>(literals::CORNER_LARGE, 16.0f);
    r.register_dynamic<float>(literals::CORNER_EXTRA_LARGE, 28.0f);
    r.register_dynamic<float>(literals::CORNER_EXTRA_EXTRA_LARGE, 32.0f);
}

float MaterialRadiusScale::queryRadiusScale(const char* name) {
    // 先查缓存
    auto it = radius_cache_.find(name);
    if (it != radius_cache_.end()) {
        return it->second;
    }

    // 从注册表获取
    auto result = registry_.get_dynamic<float>(name);
    if (result && *result) {
        auto [iter, inserted] = radius_cache_.emplace(name, **result);
        return iter->second;
    }

    // 默认回退值
    return 0.0f;
}

} // namespace cf::ui::core
