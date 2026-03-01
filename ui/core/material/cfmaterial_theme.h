/**
 * @file cfmaterial_theme.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Material Design 3 Theme - Complete Theme Implementation
 * @version 0.1
 * @date 2026-02-27
 *
 * @copyright Copyright (c) 2026
 *
 * @details
 * Implements ICFTheme interface for Material Design 3, combining:
 * - MaterialColorScheme for colors
 * - MaterialTypography for fonts
 * - MaterialRadiusScale for corner radii
 * - MaterialMotionScheme for animations
 */

#pragma once

#include "../export.h"
#include "../theme.h"
#include "cfmaterial_fonttype.h"
#include "cfmaterial_motion.h"
#include "cfmaterial_radius_scale.h"
#include "cfmaterial_scheme.h"

namespace cf::ui::core {

// Forward declaration for friend access
class MaterialFactory;

/**
 * @brief  Material Design 3 Theme Implementation.
 *
 * @details Complete theme implementation combining all Material Design 3
 * components: color scheme, typography, radius scale, and motion specs.
 *
 * Only MaterialFactory can create instances (friend class).
 *
 * @note           None.
 * @warning        None.
 * @throws         None.
 * @since          0.1
 * @ingroup        ui_core
 *
 * @code
 * // Created via MaterialFactory
 * MaterialFactory factory;
 * auto theme = factory.fromName("theme.material.light");
 *
 * // Access theme components
 * auto& colors = theme->color_scheme();
 * auto& fonts = theme->font_type();
 * auto& radius = theme->radius_scale();
 * auto& motion = theme->motion_spec();
 * @endcode
 */
class CF_UI_EXPORT MaterialTheme : public ICFTheme {
    friend class MaterialFactory;

  public:
    ~MaterialTheme() override = default;

    // Non-copyable, non-movable (due to unique_ptr members in base class)
    MaterialTheme(const MaterialTheme&) = delete;
    MaterialTheme& operator=(const MaterialTheme&) = delete;
    MaterialTheme(MaterialTheme&&) noexcept = delete;
    MaterialTheme& operator=(MaterialTheme&&) noexcept = delete;

  protected:
    /**
     * @brief  Construct MaterialTheme with all components.
     *
     * @param color_scheme Color scheme (light or dark).
     * @param font_type Typography scale.
     * @param radius_scale Corner radius scale.
     * @param motion_spec Motion/animation specifications.
     *
     * @since 0.1
     */
    MaterialTheme(std::unique_ptr<MaterialColorScheme> color_scheme,
                  std::unique_ptr<MaterialTypography> font_type,
                  std::unique_ptr<MaterialRadiusScale> radius_scale,
                  std::unique_ptr<MaterialMotionScheme> motion_spec);
};

} // namespace cf::ui::core
