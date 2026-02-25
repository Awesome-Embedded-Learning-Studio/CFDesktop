/**
 * @file cfmaterial_scheme.cpp
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Material Design 3 Color Scheme implementation
 * @version 0.1
 * @date 2026-02-25
 *
 * @copyright Copyright (c) 2026
 *
 */

#include "cfmaterial_scheme.h"

namespace cf::ui::core {

MaterialColorScheme::MaterialColorScheme() {
    color_cache_.reserve(32);
}

QColor& MaterialColorScheme::queryExpectedColor(const char* name) {
    auto it = color_cache_.find(name);
    if (it != color_cache_.end()) {
        return it->second;
    }

    auto result = registry_.get_dynamic<CFColor>(name);
    if (result && *result) {
        auto [iter, inserted] = color_cache_.emplace(name, (*result)->native_color());
        return iter->second;
    }

    // Fallback color
    static QColor fallback(Qt::black);
    return fallback;
}

QColor MaterialColorScheme::queryColor(const char* name) const {
    auto it = color_cache_.find(name);
    if (it != color_cache_.end()) {
        return it->second;
    }

    auto result = registry_.get_dynamic_const<CFColor>(name);
    if (result && *result) {
        return (*result)->native_color();
    }

    return QColor(Qt::black);
}

} // namespace cf::ui::core