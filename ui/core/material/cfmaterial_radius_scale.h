/**
 * @file cfmaterial_radius_scale.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Material Design 3 Radius Scale with EmbeddedTokenRegistry
 * @version 0.1
 * @date 2026-02-26
 *
 * @copyright Copyright (c) 2026
 *
 * @details
 * Implements the complete Material Design 3 corner radius system with 7 scales.
 * Radii are stored in an embedded registry for independent radius scale instances.
 */

#pragma once

#include <string>
#include <unordered_map>

#include "base/hash/constexpr_fnv1a.hpp"
#include "token.hpp"
#include "radius_scale.h"
#include "token/radius_scale/cfmaterial_radius_scale_literals.h"
#include "../export.h"

namespace cf::ui::core {

// =============================================================================
// Radius Scale Token Type Aliases - Material Radius Scale System
// =============================================================================
namespace tokens {
using namespace cf::ui::core::token::literals;

using CornerNoneToken = StaticToken<float, cf::hash::fnv1a64(CORNER_NONE)>;
using CornerExtraSmallToken = StaticToken<float, cf::hash::fnv1a64(CORNER_EXTRA_SMALL)>;
using CornerSmallToken = StaticToken<float, cf::hash::fnv1a64(CORNER_SMALL)>;
using CornerMediumToken = StaticToken<float, cf::hash::fnv1a64(CORNER_MEDIUM)>;
using CornerLargeToken = StaticToken<float, cf::hash::fnv1a64(CORNER_LARGE)>;
using CornerExtraLargeToken = StaticToken<float, cf::hash::fnv1a64(CORNER_EXTRA_LARGE)>;
using CornerExtraExtraLargeToken = StaticToken<float, cf::hash::fnv1a64(CORNER_EXTRA_EXTRA_LARGE)>;

} // namespace tokens

// =============================================================================
// Material Radius Scale - 实现 IRadiusScale 接口
// =============================================================================

/**
 * @brief Material Design 3 Radius Scale with EmbeddedTokenRegistry
 *
 * 实现完整的 Material Design 3 Corner Radius 系统，包含 7 档圆角尺寸。
 * 半径存储在嵌入式注册表中，支持独立的 RadiusScale 实例。
 *
 * ### Radius Scale 规格
 *
 * | Token | Name | Value (dp) | Usage |
 * |-------|------|------------|-------|
 * | CORNER_NONE | cornerNone | 0dp | No corner radius |
 * | CORNER_EXTRA_SMALL | cornerExtraSmall | 4dp | Chips, small cards |
 * | CORNER_SMALL | cornerSmall | 8dp | Text fields, checkboxes |
 * | CORNER_MEDIUM | cornerMedium | 12dp | Cards |
 * | CORNER_LARGE | cornerLarge | 16dp | Alert dialogs |
 * | CORNER_EXTRA_LARGE | cornerExtraLarge | 28dp | FAB, modals |
 * | CORNER_EXTRA_EXTRA_LARGE | cornerExtraExtraLarge | 32dp | Drawers |
 *
 * @since 0.1
 * @ingroup ui_core
 */
class CF_UI_EXPORT MaterialRadiusScale : public IRadiusScale {
  public:
    MaterialRadiusScale();
    ~MaterialRadiusScale() override = default;

    // Non-copyable, movable
    MaterialRadiusScale(const MaterialRadiusScale&) = delete;
    MaterialRadiusScale& operator=(const MaterialRadiusScale&) = delete;
    MaterialRadiusScale(MaterialRadiusScale&&) noexcept = default;
    MaterialRadiusScale& operator=(MaterialRadiusScale&&) noexcept = default;

    /**
     * @brief 实现接口：通过名称查询圆角半径（dp）
     *
     * @param name 圆角样式名称（如 "md.shape.cornerSmall"）
     * @return float 圆角半径值（dp），未找到返回 0
     *
     * @since 0.1
     */
    float queryRadiusScale(const char* name) override;

    /**
     * @brief 访问嵌入式注册表
     *
     * @return Reference to the EmbeddedTokenRegistry.
     *
     * @since 0.1
     */
    EmbeddedTokenRegistry& registry() { return registry_; }
    const EmbeddedTokenRegistry& registry() const { return registry_; }

  private:
    /**
     * @brief 注册默认圆角半径值
     *
     * 按照 Material Design 3 规范注册所有圆角半径。
     *
     * @since 0.1
     */
    void registerDefaultCorners();

    EmbeddedTokenRegistry registry_;
    mutable std::unordered_map<std::string, float> radius_cache_;
};

} // namespace cf::ui::core
