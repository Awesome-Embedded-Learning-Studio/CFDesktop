/**
 * @file    ui/widget/material/base/painter_layer.h
 * @brief   Painter layer for Material Design widget rendering.
 *
 * Provides a layer abstraction for painting operations with color
 * and opacity control. Used for layered rendering in Material Design widgets.
 *
 * @author  N/A
 * @date    N/A
 * @version N/A
 * @since   N/A
 * @ingroup ui_widget_material_base
 */
#pragma once
#include "color.h"
#include "export.h"
#include <QObject>
class QPainter;
class QPainterPath;

namespace cf::ui::widget::material::base {

/**
 * @brief  Painter layer for Material Design widget rendering.
 *
 * @details Provides a layer abstraction for painting operations with color
 *          and opacity control. Used for layered rendering in Material Design
 *          widgets.
 *
 * @since  N/A
 * @ingroup ui_widget_material_base
 */
class CF_UI_EXPORT PainterLayer : public QObject {
    Q_OBJECT
  public:
    /**
     * @brief  Constructor with parent.
     *
     * @param[in]     parent QObject parent.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_base
     */
    explicit PainterLayer(QObject* parent);

    /**
     * @brief  Sets the layer color.
     *
     * @param[in]     color Color to use for this layer.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_base
     */
    void setColor(const cf::ui::base::CFColor& color) { cached_color_ = color; }

    /**
     * @brief  Sets the layer opacity.
     *
     * @param[in]     opacity Opacity value (0.0 = transparent, 1.0 = opaque).
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_base
     */
    void setOpacity(float opacity) { opacity_ = opacity; }

    /**
     * @brief  Paints the layer.
     *
     * @param[in]     painter QPainter to render with.
     * @param[in]     clipPath Clipping path for the layer.
     *
     * @throws        None
     * @note          Call in paintEvent.
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_base
     */
    void paint(QPainter* painter, const QPainterPath& clipPath);

  protected:
    cf::ui::base::CFColor cached_color_;
    float opacity_;
};

} // namespace cf::ui::widget::material::base
