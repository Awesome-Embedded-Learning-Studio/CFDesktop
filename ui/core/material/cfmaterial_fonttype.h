/**
 * @file cfmaterial_fonttype.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Material Design 3 Typography
 * @version 0.1
 * @date 2026-02-26
 *
 * @copyright Copyright (c) 2026
 *
 * @details
 * Implements the complete Material Design 3 Type Scale system with 15 styles.
 *
 * Font selection:
 * - Windows: Segoe UI (Chinese fallback to Microsoft YaHei UI)
 * - macOS: .SF NS Text (Chinese fallback to PingFang SC)
 * - Linux: Ubuntu (Chinese fallback to Noto Sans CJK SC)
 */

#pragma once

#include <QFont>
#include <string>
#include <unordered_map>

#include "../export.h"
#include "font_type.h"

namespace cf::ui::core {

/**
 * @brief  Material Design 3 Typography.
 *
 * @details Implements the complete Material Design 3 Type Scale system with 15 styles.
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
 * QFont font = typography.queryTargetFont("md.typography.bodyLarge");
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
     * @brief  Register a font by name.
     *
     * @param[in] name Font style name (e.g., "md.typography.displayLarge").
     * @param[in] font QFont value to register.
     *
     * @since     0.1
     * @ingroup   ui_core
     */
    void setFont(const std::string& name, const QFont& font);

    /**
     * @brief  Register a line height by name.
     *
     * @param[in] name      Line height token name (e.g., "md.lineHeight.displayLarge").
     * @param[in] lineHeight Line height value in sp.
     *
     * @since     0.1
     * @ingroup   ui_core
     */
    void setLineHeight(const std::string& name, float lineHeight);

  private:
    std::unordered_map<std::string, QFont> fonts_;
    std::unordered_map<std::string, float> line_heights_;
};

} // namespace cf::ui::core
