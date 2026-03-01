/**
 * @file    ui/widget/material/base/elevation_controller.h
 * @brief   Material Design elevation controller for shadow rendering.
 *
 * Manages elevation levels and shadow rendering for Material Design widgets.
 * Provides animated transitions between elevation levels and tonal overlay
 * colors for dark theme support.
 *
 * @author  N/A
 * @date    N/A
 * @version N/A
 * @since   N/A
 * @ingroup ui_widget_material_base
 */
#pragma once
#include "color.h"
#include "components/material/cfmaterial_animation_factory.h"
#include "core/material/cfmaterial_motion.h"
#include <QObject>

class QPainter;
class QPainterPath;
class QApplication;

namespace cf::ui::widget::material::base {
using CFColor = cf::ui::base::CFColor;

/**
 * @brief  Material Design elevation controller.
 *
 * @details Manages elevation levels and shadow rendering for Material Design
 *          widgets. Provides animated transitions between elevation levels
 *          and tonal overlay colors for dark theme support.
 *
 * @since  N/A
 * @ingroup ui_widget_material_base
 */
class CF_UI_EXPORT MdElevationController : public QObject {
    Q_OBJECT
  public:
    /**
     * @brief  Constructor with animation factory.
     *
     * @param[in]     factory WeakPtr to the animation factory.
     * @param[in]     parent QObject parent.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_base
     */
    explicit MdElevationController(
        cf::WeakPtr<components::material::CFMaterialAnimationFactory> factory,
        QObject* parent = nullptr);

    /**
     * @brief  Sets the base elevation level.
     *
     * @param[in]     level Elevation level (0-5).
     *
     * @throws        None
     * @note          Material Design defines 6 standard levels (0-5).
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_base
     */
    void setElevation(int level);

    /**
     * @brief  Gets the current elevation level.
     *
     * @return        Current elevation level (0-5).
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_base
     */
    int elevation() const;

    /**
     * @brief  Sets the light source angle.
     *
     * @param[in]     degrees Angle in degrees (positive = right, negative = left).
     *
     * @throws        None
     * @note          Material Design default is approximately 15 degrees
     *                (light from top-left).
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_base
     */
    void setLightSourceAngle(float degrees);

    /**
     * @brief  Gets the light source angle.
     *
     * @return        Light source angle in degrees.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_base
     */
    float lightSourceAngle() const { return m_lightSourceAngle; }

    /**
     * @brief  Animates to a new elevation level.
     *
     * @param[in]     level Target elevation level.
     * @param[in]     spec Motion specification for the animation.
     *
     * @throws        None
     * @note          Used for dynamic elevation changes like FAB press.
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_base
     */
    void animateTo(int level, const core::MotionSpec& spec);

    /**
     * @brief  Paints the shadow for a given shape.
     *
     * @param[in]     painter QPainter to render with.
     * @param[in]     shape Shape path to render shadow for.
     *
     * @throws        None
     * @note          Call in paintEvent before drawing the background.
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_base
     */
    void paintShadow(QPainter* painter, const QPainterPath& shape);

    /**
     * @brief  Gets the tonal overlay color for dark theme.
     *
     * @param[in]     surface Base surface color.
     * @param[in]     primary Primary color for overlay calculation.
     *
     * @return        Tonal overlay color.
     *
     * @throws        None
     * @note          Used in dark theme to indicate elevation level.
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_base
     */
    CFColor tonalOverlay(CFColor surface, CFColor primary) const;

    /**
     * @brief  Sets the pressed state.
     *
     * @param[in]     pressed true to set pressed state, false otherwise.
     *
     * @throws        None
     * @note          Pressed state increases elevation visually.
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_base
     */
    void setPressed(bool pressed);

    /**
     * @brief  Gets the pressed state.
     *
     * @return        true if pressed, false otherwise.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_base
     */
    bool isPressed() const { return m_isPressed; }

    /**
     * @brief  Gets the press offset for the current elevation.
     *
     * @return        Press offset in pixels.
     *
     * @throws        None
     * @note          Based on the current elevation level.
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_base
     */
    float pressOffset() const;

  private:
    float m_currentLevel = 0.0f;
    int m_targetLevel = 0;
    float m_lightSourceAngle = 15.0f;
    bool m_isPressed = false;
    cf::WeakPtr<components::material::CFMaterialAnimationFactory> m_animator;

    struct ShadowParams {
        float blurRadius;
        float offsetX;
        float offsetY;
        float opacity;
    };
    ShadowParams paramsForLevel(float level) const;
};
} // namespace cf::ui::widget::material::base
