/**
 * @file    ui/core/radius_scale.h
 * @brief   Defines the IRadiusScale interface for querying corner radius values.
 *
 * Provides an abstract interface for querying radius scale values
 * by name, typically used for Material Design 3 corner specifications.
 *
 * @author  Charliechen114514
 * @date    2026-02-26
 * @version N/A
 * @since   N/A
 * @ingroup ui_core
 */

#pragma once
#include "export.h"

namespace cf::ui::core {

/**
 * @brief  Abstract interface for querying corner radius scales by name.
 *
 * Defines the contract for radius scale providers that return float
 * radius values based on style names (e.g., "cornerSmall", "cornerLarge").
 *
 * @ingroup ui_core
 *
 * @code
 * IRadiusScale* radiusScale = getRadiusScale();
 * float radius = radiusScale->queryRadiusScale("cornerSmall");
 * @endcode
 */
struct CF_UI_EXPORT IRadiusScale {
    /// @brief Virtual destructor.
    virtual ~IRadiusScale() = default;

    /**
     * @brief  Query a corner radius value by name.
     *
     * @param[in] name Radius scale name (e.g., "cornerSmall", "cornerLarge").
     * @return         Radius value in density-independent pixels (dp).
     *                 Returns 0 if the name is not found.
     *
     * @throws         None
     * @note           None
     * @warning        None
     * @since          N/A
     * @ingroup        ui_core
     */
    virtual float queryRadiusScale(const char* name) = 0;
};

} // namespace cf::ui::core
