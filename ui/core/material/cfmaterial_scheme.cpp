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
    colors_.reserve(32);
}

QColor& MaterialColorScheme::queryExpectedColor(const char* name) {
    auto it = colors_.find(name);
    if (it != colors_.end()) {
        return it->second;
    }
    static QColor fallback(Qt::black);
    return fallback;
}

QColor MaterialColorScheme::queryColor(const char* name) const {
    auto it = colors_.find(name);
    if (it != colors_.end()) {
        return it->second;
    }
    return QColor(Qt::black);
}

void MaterialColorScheme::setColor(const std::string& name, const QColor& color) {
    colors_[name] = color;
}

bool MaterialColorScheme::hasColor(const std::string& name) const {
    return colors_.find(name) != colors_.end();
}

} // namespace cf::ui::core
