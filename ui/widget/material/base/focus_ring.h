/**
 * @file    ui/widget/material/base/focus_ring.h
 * @brief   Material Design focus indicator for keyboard navigation.
 *
 * Manages the focus ring indicator for Material Design widgets.
 * Provides animated entrance/exit when widgets gain or lose focus.
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

namespace cf::ui::widget::material::base {

/**
 * @brief  Material Design focus indicator.
 *
 * @details Manages the focus ring indicator for keyboard navigation.
 *          Provides animated entrance/exit when widgets gain or lose focus.
 *
 * @since  N/A
 * @ingroup ui_widget_material_base
 */
class CF_UI_EXPORT MdFocusIndicator : public QObject {
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
    explicit MdFocusIndicator(cf::WeakPtr<components::material::CFMaterialAnimationFactory> factory,
                              QObject* parent = nullptr);

    /**
     * @brief  Handles focus in event.
     *
     * @throws        None
     * @note          Starts the focus entrance animation.
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_base
     */
    void onFocusIn();

    /**
     * @brief  Handles focus out event.
     *
     * @throws        None
     * @note          Starts the focus exit animation.
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_base
     */
    void onFocusOut();

    /**
     * @brief  Paints the focus indicator.
     *
     * @param[in]     painter QPainter to render with.
     * @param[in]     shape Shape path to render focus ring for.
     * @param[in]     indicatorColor Color for the focus indicator.
     *
     * @throws        None
     * @note          Call in paintEvent at the top layer.
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_base
     */
    void paint(QPainter* painter, const QPainterPath& shape,
               const cf::ui::base::CFColor& indicatorColor);

  private:
    float m_progress = 0.0f;
    cf::WeakPtr<components::material::CFMaterialAnimationFactory> m_animator;
};
} // namespace cf::ui::widget::material::base
