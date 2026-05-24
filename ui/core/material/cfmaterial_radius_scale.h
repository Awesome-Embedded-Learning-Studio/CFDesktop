/**
 * @file cfmaterial_radius_scale.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Material Design 3 Radius Scale
 * @version 0.1
 * @date 2026-02-26
 *
 * @copyright Copyright (c) 2026
 *
 * @details
 * Implements the complete Material Design 3 corner radius system with 7 scales.
 */

#pragma once

#include <string>
#include <unordered_map>

#include "../export.h"
#include "radius_scale.h"

namespace cf::ui::core {

/**
 * @brief  Material Design 3 Radius Scale.
 *
 * @details Implements the complete Material Design 3 Corner Radius system with 7 scales.
 *
 * ### Radius Scale Specifications
 *
 * | Token | Name | Value (dp) | Usage |
 * |-------|------|------------|-------|
 * | cornerNone | cornerNone | 0dp | No corner radius |
 * | cornerExtraSmall | cornerExtraSmall | 4dp | Chips, small cards |
 * | cornerSmall | cornerSmall | 8dp | Text fields, checkboxes |
 * | cornerMedium | cornerMedium | 12dp | Cards |
 * | cornerLarge | cornerLarge | 16dp | Alert dialogs |
 * | cornerExtraLarge | cornerExtraLarge | 28dp | FAB, modals |
 * | cornerExtraExtraLarge | cornerExtraExtraLarge | 32dp | Drawers |
 *
 * @note           None
 * @warning        None
 * @throws         None
 * @since          0.1
 * @ingroup        ui_core
 *
 * @code
 * MaterialRadiusScale radiusScale;
 * float radius = radiusScale.queryRadiusScale("md.shape.cornerSmall");
 * @endcode
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
     * @brief  Implement interface: query corner radius by name.
     *
     * @param[in] name Radius style name (e.g., "md.shape.cornerSmall").
     * @return         Radius value in dp, returns 0 if not found.
     *
     * @throws         None
     * @note           None
     * @warning        None
     * @since          0.1
     * @ingroup        ui_core
     */
    float queryRadiusScale(const char* name) override;

    /**
     * @brief  Register a radius by name.
     *
     * @param[in] name   Radius token name (e.g., "md.shape.cornerSmall").
     * @param[in] radius Radius value in dp.
     *
     * @since     0.1
     * @ingroup   ui_core
     */
    void setRadius(const std::string& name, float radius);

  private:
    std::unordered_map<std::string, float> radii_;
};

} // namespace cf::ui::core
