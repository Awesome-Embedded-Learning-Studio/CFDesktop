/**
 * @file cfmaterial_fonttype.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Material Design 3 Typography with EmbeddedTokenRegistry
 * @version 0.1
 * @date 2026-02-26
 *
 * @copyright Copyright (c) 2026
 *
 * @details
 * Implements the complete Material Design 3 Type Scale system with 15 styles.
 * Fonts are stored in an embedded registry for independent typography instances.
 *
 * Font selection:
 * - Windows: Segoe UI (Chinese fallback to Microsoft YaHei UI)
 * - macOS: .SF NS Text (Chinese fallback to PingFang SC)
 * - Linux: Ubuntu (Chinese fallback to Noto Sans CJK SC)
 */

#pragma once

#include <QByteArray>
#include <QFont>
#include <string>
#include <unordered_map>

#include "../export.h"
#include "base/hash/constexpr_fnv1a.hpp"
#include "font_type.h"
#include "token.hpp"
#include "token/typography/cfmaterial_typography_token_literals.h"

namespace cf::ui::core {

// =============================================================================
// Typography Token Type Aliases - Material Typography System
// =============================================================================
namespace tokens {
using namespace cf::ui::core::token::literals;

// Display tokens
using DisplayLargeToken = StaticToken<QFont, cf::hash::fnv1a64(TYPOGRAPHY_DISPLAY_LARGE)>;
using DisplayMediumToken = StaticToken<QFont, cf::hash::fnv1a64(TYPOGRAPHY_DISPLAY_MEDIUM)>;
using DisplaySmallToken = StaticToken<QFont, cf::hash::fnv1a64(TYPOGRAPHY_DISPLAY_SMALL)>;

// Headline tokens
using HeadlineLargeToken = StaticToken<QFont, cf::hash::fnv1a64(TYPOGRAPHY_HEADLINE_LARGE)>;
using HeadlineMediumToken = StaticToken<QFont, cf::hash::fnv1a64(TYPOGRAPHY_HEADLINE_MEDIUM)>;
using HeadlineSmallToken = StaticToken<QFont, cf::hash::fnv1a64(TYPOGRAPHY_HEADLINE_SMALL)>;

// Title tokens
using TitleLargeToken = StaticToken<QFont, cf::hash::fnv1a64(TYPOGRAPHY_TITLE_LARGE)>;
using TitleMediumToken = StaticToken<QFont, cf::hash::fnv1a64(TYPOGRAPHY_TITLE_MEDIUM)>;
using TitleSmallToken = StaticToken<QFont, cf::hash::fnv1a64(TYPOGRAPHY_TITLE_SMALL)>;

// Body tokens
using BodyLargeToken = StaticToken<QFont, cf::hash::fnv1a64(TYPOGRAPHY_BODY_LARGE)>;
using BodyMediumToken = StaticToken<QFont, cf::hash::fnv1a64(TYPOGRAPHY_BODY_MEDIUM)>;
using BodySmallToken = StaticToken<QFont, cf::hash::fnv1a64(TYPOGRAPHY_BODY_SMALL)>;

// Label tokens
using LabelLargeToken = StaticToken<QFont, cf::hash::fnv1a64(TYPOGRAPHY_LABEL_LARGE)>;
using LabelMediumToken = StaticToken<QFont, cf::hash::fnv1a64(TYPOGRAPHY_LABEL_MEDIUM)>;
using LabelSmallToken = StaticToken<QFont, cf::hash::fnv1a64(TYPOGRAPHY_LABEL_SMALL)>;

} // namespace tokens

// =============================================================================
// Line Height Token Type Aliases
// =============================================================================
namespace lineHeightTokens {
using namespace cf::ui::core::token::literals;

using DisplayLargeLineHeight = StaticToken<float, cf::hash::fnv1a64(LINEHEIGHT_DISPLAY_LARGE)>;
using DisplayMediumLineHeight = StaticToken<float, cf::hash::fnv1a64(LINEHEIGHT_DISPLAY_MEDIUM)>;
using DisplaySmallLineHeight = StaticToken<float, cf::hash::fnv1a64(LINEHEIGHT_DISPLAY_SMALL)>;

using HeadlineLargeLineHeight = StaticToken<float, cf::hash::fnv1a64(LINEHEIGHT_HEADLINE_LARGE)>;
using HeadlineMediumLineHeight = StaticToken<float, cf::hash::fnv1a64(LINEHEIGHT_HEADLINE_MEDIUM)>;
using HeadlineSmallLineHeight = StaticToken<float, cf::hash::fnv1a64(LINEHEIGHT_HEADLINE_SMALL)>;

using TitleLargeLineHeight = StaticToken<float, cf::hash::fnv1a64(LINEHEIGHT_TITLE_LARGE)>;
using TitleMediumLineHeight = StaticToken<float, cf::hash::fnv1a64(LINEHEIGHT_TITLE_MEDIUM)>;
using TitleSmallLineHeight = StaticToken<float, cf::hash::fnv1a64(LINEHEIGHT_TITLE_SMALL)>;

using BodyLargeLineHeight = StaticToken<float, cf::hash::fnv1a64(LINEHEIGHT_BODY_LARGE)>;
using BodyMediumLineHeight = StaticToken<float, cf::hash::fnv1a64(LINEHEIGHT_BODY_MEDIUM)>;
using BodySmallLineHeight = StaticToken<float, cf::hash::fnv1a64(LINEHEIGHT_BODY_SMALL)>;

using LabelLargeLineHeight = StaticToken<float, cf::hash::fnv1a64(LINEHEIGHT_LABEL_LARGE)>;
using LabelMediumLineHeight = StaticToken<float, cf::hash::fnv1a64(LINEHEIGHT_LABEL_MEDIUM)>;
using LabelSmallLineHeight = StaticToken<float, cf::hash::fnv1a64(LINEHEIGHT_LABEL_SMALL)>;

} // namespace lineHeightTokens

// =============================================================================
// Typography Group Structs - Material Typography System
// =============================================================================

/**
 * @brief Display font styles group.
 *
 * Display styles are used for hero content with large emphasis.
 *
 * @since 0.1
 * @ingroup ui_core
 */
struct DisplayFonts {
    tokens::DisplayLargeToken large;   ///< 57sp, Regular, 64sp line-height
    tokens::DisplayMediumToken medium; ///< 45sp, Regular, 52sp line-height
    tokens::DisplaySmallToken small;   ///< 36sp, Regular, 44sp line-height
};

/**
 * @brief Headline font styles group.
 *
 * Headline styles are used for app bar important text.
 *
 * @since 0.1
 * @ingroup ui_core
 */
struct HeadlineFonts {
    tokens::HeadlineLargeToken large;   ///< 32sp, Regular, 40sp line-height
    tokens::HeadlineMediumToken medium; ///< 28sp, Regular, 36sp line-height
    tokens::HeadlineSmallToken small;   ///< 24sp, Regular, 32sp line-height
};

/**
 * @brief Title font styles group.
 *
 * Title styles are used for section headings.
 *
 * @since 0.1
 * @ingroup ui_core
 */
struct TitleFonts {
    tokens::TitleLargeToken large;   ///< 22sp, Medium, 28sp line-height
    tokens::TitleMediumToken medium; ///< 16sp, Medium, 24sp line-height
    tokens::TitleSmallToken small;   ///< 14sp, Medium, 20sp line-height
};

/**
 * @brief Body font styles group.
 *
 * Body styles are used for main content.
 *
 * @since 0.1
 * @ingroup ui_core
 */
struct BodyFonts {
    tokens::BodyLargeToken large;   ///< 16sp, Regular, 24sp line-height
    tokens::BodyMediumToken medium; ///< 14sp, Regular, 20sp line-height
    tokens::BodySmallToken small;   ///< 12sp, Regular, 16sp line-height
};

/**
 * @brief Label font styles group.
 *
 * Label styles are used for secondary information.
 *
 * @since 0.1
 * @ingroup ui_core
 */
struct LabelFonts {
    tokens::LabelLargeToken large;   ///< 14sp, Medium, 20sp line-height
    tokens::LabelMediumToken medium; ///< 12sp, Medium, 16sp line-height
    tokens::LabelSmallToken small;   ///< 11sp, Medium, 16sp line-height
};

// =============================================================================
// Material Typography - 实现 IFontType 接口
// =============================================================================

/**
 * @brief  Material Design 3 Typography with EmbeddedTokenRegistry.
 *
 * @details Implements the complete Material Design 3 Type Scale system with 15 styles.
 * Fonts are stored in an embedded registry for independent typography instances.
 *
 * ### Type Scale Specifications
 *
 * | Category | Name | Size | Weight | Line Height |
 * |----------|------|------|--------|-------------|
 * | Display | displayLarge | 57sp | 400 | 64sp |
 * | Display | displayMedium | 45sp | 400 | 52sp |
 * | Display | displaySmall | 36sp | 400 | 44sp |
 * | Headline | headlineLarge | 32sp | 400 | 40sp |
 * | Headline | headlineMedium | 28sp | 400 | 36sp |
 * | Headline | headlineSmall | 24sp | 400 | 32sp |
 * | Title | titleLarge | 22sp | 500 | 28sp |
 * | Title | titleMedium | 16sp | 500 | 24sp |
 * | Title | titleSmall | 14sp | 500 | 20sp |
 * | Body | bodyLarge | 16sp | 400 | 24sp |
 * | Body | bodyMedium | 14sp | 400 | 20sp |
 * | Body | bodySmall | 12sp | 400 | 16sp |
 * | Label | labelLarge | 14sp | 500 | 20sp |
 * | Label | labelMedium | 12sp | 500 | 16sp |
 * | Label | labelSmall | 11sp | 500 | 16sp |
 *
 * @note           Font selection: Windows uses Segoe UI, macOS uses .SF NS Text,
 *                 Linux uses Ubuntu. Chinese fallbacks are platform-dependent.
 * @warning        None
 * @throws         None
 * @since          0.1
 * @ingroup        ui_core
 *
 * @code
 * MaterialTypography typography;
 * QFont font = typography.queryTargetFont("bodyLarge");
 * @endcode
 */
class CF_UI_EXPORT MaterialTypography : public IFontType {
  public:
    MaterialTypography();
    ~MaterialTypography() override = default;

    // Non-copyable, movable
    MaterialTypography(const MaterialTypography&) = delete;
    MaterialTypography& operator=(const MaterialTypography&) = delete;
    MaterialTypography(MaterialTypography&&) noexcept = default;
    MaterialTypography& operator=(MaterialTypography&&) noexcept = default;

    /**
     * @brief  Implement interface: query font by name.
     *
     * @param[in] name Font style name (e.g., "md.typography.displayLarge").
     * @return         QFont object (copy).
     *
     * @throws         None
     * @note           None
     * @warning        None
     * @since          0.1
     * @ingroup        ui_core
     */
    QFont queryTargetFont(const char* name) override;

    /**
     * @brief  Get line height for a specific font style.
     *
     * @param[in] styleName Font style name.
     * @return              Line height value in sp, returns 0 if not found.
     *
     * @throws              None
     * @note                None
     * @warning             None
     * @since               0.1
     * @ingroup             ui_core
     */
    float getLineHeight(const char* styleName) const;

    /**
     * @brief  Access the embedded token registry.
     *
     * Provides direct access to the internal token registry for
     * custom token manipulation.
     *
     * @return         Reference to the EmbeddedTokenRegistry.
     *
     * @throws         None
     * @note           None
     * @warning        None
     * @since          0.1
     * @ingroup        ui_core
     */
    EmbeddedTokenRegistry& registry() { return registry_; }

    /**
     * @brief  Access the embedded token registry (const overload).
     *
     * Provides direct read-only access to the internal token registry.
     *
     * @return         Const reference to the EmbeddedTokenRegistry.
     *
     * @throws         None
     * @note           None
     * @warning        None
     * @since          0.1
     * @ingroup        ui_core
     */
    const EmbeddedTokenRegistry& registry() const { return registry_; }

    // Font group accessors - 返回包含 Token 类型的结构体
    /**
     * @brief 获取 Display 字体组
     *
     * @return DisplayFonts 结构体
     *
     * @since 0.1
     */
    [[nodiscard]] DisplayFonts display() const { return {}; }

    /**
     * @brief 获取 Headline 字体组
     *
     * @return HeadlineFonts 结构体
     *
     * @since 0.1
     */
    [[nodiscard]] HeadlineFonts headline() const { return {}; }

    /**
     * @brief 获取 Title 字体组
     *
     * @return TitleFonts 结构体
     *
     * @since 0.1
     */
    [[nodiscard]] TitleFonts title() const { return {}; }

    /**
     * @brief 获取 Body 字体组
     *
     * @return BodyFonts 结构体
     *
     * @since 0.1
     */
    [[nodiscard]] BodyFonts body() const { return {}; }

    /**
     * @brief 获取 Label 字体组
     *
     * @return LabelFonts 结构体
     *
     * @since 0.1
     */
    [[nodiscard]] LabelFonts label() const { return {}; }

  private:
    EmbeddedTokenRegistry registry_;
    mutable std::unordered_map<std::string, QFont> font_cache_;
    mutable std::unordered_map<std::string, float> line_height_cache_;
};

} // namespace cf::ui::core
