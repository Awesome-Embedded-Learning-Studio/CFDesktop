/**
 * @file    ui/core/theme.h
 * @brief   Interface for CF UI theme components.
 *
 * Defines the ICFTheme interface which provides access to color schemes,
 * motion specifications, radius scales, and font types for UI theming.
 *
 * @author  N/A
 * @date    N/A
 * @version N/A
 * @since   N/A
 * @ingroup none
 */

#pragma once

#include "color_scheme.h"
#include "export.h"
#include "font_type.h"
#include "motion_spec.h"
#include "radius_scale.h"
#include <memory>

namespace cf::ui::core {

/**
 * @brief  Interface for CF UI theme components.
 *
 * ICFTheme provides access to the core theme components including color
 * schemes, motion specifications, radius scales, and font types. Instances
 * should only be created by ThemeFactory and accessed by reference.
 *
 * @ingroup none
 *
 * @note   Theme instances should be accessed by reference. Only ThemeFactory
 *         should create instances.
 *
 * @warning Direct construction is not supported; use ThemeFactory instead.
 *
 * @code
 * const ICFTheme& theme = ThemeManager::instance().theme("default");
 * const auto& colors = theme.color_scheme();
 * @endcode
 */
struct CF_UI_EXPORT ICFTheme {
  public:
    /// @brief ThemeFactory has exclusive creation access.
    friend class ThemeFactory;

    /// @brief Virtual destructor.
    virtual ~ICFTheme() = default;

    /**
     * @brief  Gets the color scheme component.
     *
     * @return Reference to the color scheme.
     * @throws None
     * @note   None
     * @warning None
     * @since  N/A
     * @ingroup none
     */
    ICFColorScheme& color_scheme() const { return *color_scheme_; }

    /**
     * @brief  Gets the motion specification component.
     *
     * @return Reference to the motion specification.
     * @throws None
     * @note   None
     * @warning None
     * @since  N/A
     * @ingroup none
     */
    IMotionSpec& motion_spec() const { return *motion_spec_; }

    /**
     * @brief  Gets the radius scale component.
     *
     * @return Reference to the radius scale.
     * @throws None
     * @note   None
     * @warning None
     * @since  N/A
     * @ingroup none
     */
    IRadiusScale& radius_scale() const { return *radius_scale_; }

    /**
     * @brief  Gets the font type component.
     *
     * @return Reference to the font type.
     * @throws None
     * @note   None
     * @warning None
     * @since  N/A
     * @ingroup none
     */
    IFontType& font_type() const { return *font_type_; }

  protected:
    /// @brief Default constructor: protected for factory-only creation.
    ICFTheme() = default;

    /// @brief Color scheme component. Ownership: owner.
    std::unique_ptr<ICFColorScheme> color_scheme_;

    /// @brief Motion specification component. Ownership: owner.
    std::unique_ptr<IMotionSpec> motion_spec_;

    /// @brief Radius scale component. Ownership: owner.
    std::unique_ptr<IRadiusScale> radius_scale_;

    /// @brief Font type component. Ownership: owner.
    std::unique_ptr<IFontType> font_type_;
};

} // namespace cf::ui::core
