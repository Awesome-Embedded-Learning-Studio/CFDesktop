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

// =============================================================================
// System Font Detection
// =============================================================================

/**
 * @brief 获取系统默认无衬线字体
 *
 * 根据平台返回合适的默认字体：
 * - Windows: Segoe UI
 * - macOS: .SF NS Text (San Francisco)
 * - Linux: Ubuntu
 *
 * @return QString 系统默认字体名称
 */
inline QString systemDefaultFont() {
#ifdef Q_OS_WIN
    return "Segoe UI";
#elif defined(Q_OS_MACOS)
    return ".SF NS Text";
#else
    return "Ubuntu";
#endif
}

/**
 * @brief 创建指定配置的字体
 *
 * @param sizeSp 字体大小（sp 单位）
 * @param weight 字重 (QFont::Weight)
 * @param italic 是否斜体
 * @return QFont 配置好的字体对象
 */
inline QFont createFont(int sizeSp, QFont::Weight weight, bool italic = false) {
    QFont font(systemDefaultFont());
    font.setStyleHint(QFont::SansSerif);
    font.setWeight(weight);
    font.setItalic(italic);
    font.setPointSizeF(sizeSp);  // 使用 pointSize (Qt 会处理 DPI)
    return font;
}

// =============================================================================
// Material Design 3 Type Scale Registration
// =============================================================================

/**
 * @brief 注册所有默认字体到注册表
 *
 * 使用 Material Design 3 规范的字体大小、字重和行高。
 *
 * @param registry 目标注册表
 */
inline void registerDefaultFonts(EmbeddedTokenRegistry& registry) {
    namespace literals = ::cf::ui::core::token::literals;

    // =========================================================================
    // Display Styles - 用于英雄内容
    // =========================================================================
    registry.register_dynamic<QFont>(literals::TYPOGRAPHY_DISPLAY_LARGE, createFont(57, QFont::Normal));
    registry.register_dynamic<QFont>(literals::TYPOGRAPHY_DISPLAY_MEDIUM, createFont(45, QFont::Normal));
    registry.register_dynamic<QFont>(literals::TYPOGRAPHY_DISPLAY_SMALL, createFont(36, QFont::Normal));

    // 行高
    registry.register_dynamic<float>(literals::LINEHEIGHT_DISPLAY_LARGE, 64.0f);
    registry.register_dynamic<float>(literals::LINEHEIGHT_DISPLAY_MEDIUM, 52.0f);
    registry.register_dynamic<float>(literals::LINEHEIGHT_DISPLAY_SMALL, 44.0f);

    // =========================================================================
    // Headline Styles - 用于应用栏重要文本
    // =========================================================================
    registry.register_dynamic<QFont>(literals::TYPOGRAPHY_HEADLINE_LARGE, createFont(32, QFont::Normal));
    registry.register_dynamic<QFont>(literals::TYPOGRAPHY_HEADLINE_MEDIUM, createFont(28, QFont::Normal));
    registry.register_dynamic<QFont>(literals::TYPOGRAPHY_HEADLINE_SMALL, createFont(24, QFont::Normal));

    // 行高
    registry.register_dynamic<float>(literals::LINEHEIGHT_HEADLINE_LARGE, 40.0f);
    registry.register_dynamic<float>(literals::LINEHEIGHT_HEADLINE_MEDIUM, 36.0f);
    registry.register_dynamic<float>(literals::LINEHEIGHT_HEADLINE_SMALL, 32.0f);

    // =========================================================================
    // Title Styles - 用于分区标题
    // =========================================================================
    registry.register_dynamic<QFont>(literals::TYPOGRAPHY_TITLE_LARGE, createFont(22, QFont::Medium));
    registry.register_dynamic<QFont>(literals::TYPOGRAPHY_TITLE_MEDIUM, createFont(16, QFont::Medium));
    registry.register_dynamic<QFont>(literals::TYPOGRAPHY_TITLE_SMALL, createFont(14, QFont::Medium));

    // 行高
    registry.register_dynamic<float>(literals::LINEHEIGHT_TITLE_LARGE, 28.0f);
    registry.register_dynamic<float>(literals::LINEHEIGHT_TITLE_MEDIUM, 24.0f);
    registry.register_dynamic<float>(literals::LINEHEIGHT_TITLE_SMALL, 20.0f);

    // =========================================================================
    // Body Styles - 用于主要内容
    // =========================================================================
    registry.register_dynamic<QFont>(literals::TYPOGRAPHY_BODY_LARGE, createFont(16, QFont::Normal));
    registry.register_dynamic<QFont>(literals::TYPOGRAPHY_BODY_MEDIUM, createFont(14, QFont::Normal));
    registry.register_dynamic<QFont>(literals::TYPOGRAPHY_BODY_SMALL, createFont(12, QFont::Normal));

    // 行高
    registry.register_dynamic<float>(literals::LINEHEIGHT_BODY_LARGE, 24.0f);
    registry.register_dynamic<float>(literals::LINEHEIGHT_BODY_MEDIUM, 20.0f);
    registry.register_dynamic<float>(literals::LINEHEIGHT_BODY_SMALL, 16.0f);

    // =========================================================================
    // Label Styles - 用于次要信息
    // =========================================================================
    registry.register_dynamic<QFont>(literals::TYPOGRAPHY_LABEL_LARGE, createFont(14, QFont::Medium));
    registry.register_dynamic<QFont>(literals::TYPOGRAPHY_LABEL_MEDIUM, createFont(12, QFont::Medium));
    registry.register_dynamic<QFont>(literals::TYPOGRAPHY_LABEL_SMALL, createFont(11, QFont::Medium));

    // 行高
    registry.register_dynamic<float>(literals::LINEHEIGHT_LABEL_LARGE, 20.0f);
    registry.register_dynamic<float>(literals::LINEHEIGHT_LABEL_MEDIUM, 16.0f);
    registry.register_dynamic<float>(literals::LINEHEIGHT_LABEL_SMALL, 16.0f);
}

} // namespace detail

// =============================================================================
// MaterialTypography Implementation
// =============================================================================

MaterialTypography::MaterialTypography() {
    font_cache_.reserve(15);
    line_height_cache_.reserve(15);

    // 注册所有默认字体
    detail::registerDefaultFonts(registry_);
}

QFont MaterialTypography::queryTargetFont(const char* name) {
    // 检查缓存
    auto it = font_cache_.find(name);
    if (it != font_cache_.end()) {
        return it->second;
    }

    // 从注册表获取
    auto result = registry_.get_dynamic<QFont>(name);
    if (result && *result) {
        font_cache_[name] = **result;
        return **result;
    }

    // 回退到默认字体
    QFont fallback(detail::systemDefaultFont());
    fallback.setPointSizeF(14);
    return fallback;
}

float MaterialTypography::getLineHeight(const char* styleName) const {
    // 将 md.typography.xxx 转换为 md.lineHeight.xxx
    std::string key = styleName;
    const std::string prefix = "md.typography.";
    const std::string lineHeightPrefix = "md.lineHeight.";

    size_t pos = key.find(prefix);
    if (pos == 0) {
        key.replace(0, prefix.length(), lineHeightPrefix);
    }

    // 检查缓存
    auto it = line_height_cache_.find(key);
    if (it != line_height_cache_.end()) {
        return it->second;
    }

    // 从注册表获取
    auto result = registry_.get_dynamic_const<float>(key.c_str());
    if (result && *result) {
        line_height_cache_[key] = **result;
        return **result;
    }

    return 0.0f;
}

} // namespace cf::ui::core
