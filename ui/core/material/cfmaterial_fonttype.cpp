/**
 * @file cfmaterial_fonttype.cpp
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Material Design 3 Typography Implementation
 * @version 0.1
 * @date 2026-02-26
 *
 * @copyright Copyright (c) 2026
 *
 */

#include "cfmaterial_fonttype.h"

namespace cf::ui::core {

namespace detail {

inline QString systemDefaultFont() {
#ifdef Q_OS_WIN
    return "Segoe UI";
#elif defined(Q_OS_MACOS)
    return ".SF NS Text";
#else
    return "Ubuntu";
#endif
}

inline QFont createFont(int sizeSp, QFont::Weight weight, bool italic = false) {
    QFont font(systemDefaultFont());
    font.setStyleHint(QFont::SansSerif);
    font.setWeight(weight);
    font.setItalic(italic);
    font.setPointSizeF(sizeSp);
    return font;
}

} // namespace detail

MaterialTypography::MaterialTypography() {
    fonts_.reserve(15);
    line_heights_.reserve(15);

    // Display Styles
    fonts_["md.typography.displayLarge"] = detail::createFont(57, QFont::Normal);
    fonts_["md.typography.displayMedium"] = detail::createFont(45, QFont::Normal);
    fonts_["md.typography.displaySmall"] = detail::createFont(36, QFont::Normal);
    line_heights_["md.lineHeight.displayLarge"] = 64.0f;
    line_heights_["md.lineHeight.displayMedium"] = 52.0f;
    line_heights_["md.lineHeight.displaySmall"] = 44.0f;

    // Headline Styles
    fonts_["md.typography.headlineLarge"] = detail::createFont(32, QFont::Normal);
    fonts_["md.typography.headlineMedium"] = detail::createFont(28, QFont::Normal);
    fonts_["md.typography.headlineSmall"] = detail::createFont(24, QFont::Normal);
    line_heights_["md.lineHeight.headlineLarge"] = 40.0f;
    line_heights_["md.lineHeight.headlineMedium"] = 36.0f;
    line_heights_["md.lineHeight.headlineSmall"] = 32.0f;

    // Title Styles
    fonts_["md.typography.titleLarge"] = detail::createFont(22, QFont::Medium);
    fonts_["md.typography.titleMedium"] = detail::createFont(16, QFont::Medium);
    fonts_["md.typography.titleSmall"] = detail::createFont(14, QFont::Medium);
    line_heights_["md.lineHeight.titleLarge"] = 28.0f;
    line_heights_["md.lineHeight.titleMedium"] = 24.0f;
    line_heights_["md.lineHeight.titleSmall"] = 20.0f;

    // Body Styles
    fonts_["md.typography.bodyLarge"] = detail::createFont(16, QFont::Normal);
    fonts_["md.typography.bodyMedium"] = detail::createFont(14, QFont::Normal);
    fonts_["md.typography.bodySmall"] = detail::createFont(12, QFont::Normal);
    line_heights_["md.lineHeight.bodyLarge"] = 24.0f;
    line_heights_["md.lineHeight.bodyMedium"] = 20.0f;
    line_heights_["md.lineHeight.bodySmall"] = 16.0f;

    // Label Styles
    fonts_["md.typography.labelLarge"] = detail::createFont(14, QFont::Medium);
    fonts_["md.typography.labelMedium"] = detail::createFont(12, QFont::Medium);
    fonts_["md.typography.labelSmall"] = detail::createFont(11, QFont::Medium);
    line_heights_["md.lineHeight.labelLarge"] = 20.0f;
    line_heights_["md.lineHeight.labelMedium"] = 16.0f;
    line_heights_["md.lineHeight.labelSmall"] = 16.0f;
}

QFont MaterialTypography::queryTargetFont(const char* name) {
    auto it = fonts_.find(name);
    if (it != fonts_.end()) {
        return it->second;
    }
    QFont fallback(detail::systemDefaultFont());
    fallback.setPointSizeF(14);
    return fallback;
}

float MaterialTypography::getLineHeight(const char* styleName) const {
    std::string key = styleName;
    const std::string prefix = "md.typography.";
    const std::string lineHeightPrefix = "md.lineHeight.";

    size_t pos = key.find(prefix);
    if (pos == 0) {
        key.replace(0, prefix.length(), lineHeightPrefix);
    }

    auto it = line_heights_.find(key);
    return it != line_heights_.end() ? it->second : 0.0f;
}

void MaterialTypography::setFont(const std::string& name, const QFont& font) {
    fonts_[name] = font;
}

void MaterialTypography::setLineHeight(const std::string& name, float lineHeight) {
    line_heights_[name] = lineHeight;
}

} // namespace cf::ui::core
