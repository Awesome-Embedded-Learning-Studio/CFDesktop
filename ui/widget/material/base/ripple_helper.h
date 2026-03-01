/**
 * @file    ui/widget/material/base/ripple_helper.h
 * @brief   Material Design ripple effect helper.
 *
 * Manages ripple effects for Material Design widgets. Provides
 * animated ripple propagation on press/release interactions with
 * bounded and unbounded modes.
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
#include <QObject>
#include <QPointF>
class QPainterPath;

namespace cf::ui::widget::material::base {

/**
 * @brief  Material Design ripple data structure.
 *
 * @since  N/A
 * @ingroup ui_widget_material_base
 */
struct MdRipple {
    QPointF center;
    float radius;
    float opacity;
    bool releasing;
    float maxRadius;
};

/**
 * @brief  Material Design ripple effect helper.
 *
 * @details Manages ripple effects for Material Design widgets. Provides
 *          animated ripple propagation on press/release interactions with
 *          bounded and unbounded modes.
 *
 * @since  N/A
 * @ingroup ui_widget_material_base
 */
class CF_UI_EXPORT RippleHelper : public QObject {
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
    explicit RippleHelper(cf::WeakPtr<components::material::CFMaterialAnimationFactory> factory,
                          QObject* parent);

    /**
     * @brief  Ripple rendering mode.
     *
     * @since  N/A
     * @ingroup ui_widget_material_base
     */
    enum class Mode {
        Bounded,   ///< Clipped by widget bounds.
        Unbounded  ///< Not clipped by widget bounds.
    };
    Q_ENUM(Mode)

    /**
     * @brief  Sets the ripple rendering mode.
     *
     * @param[in]     mode Rendering mode to use.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_base
     */
    void setMode(Mode mode);

    /**
     * @brief  Sets the ripple color.
     *
     * @param[in]     color Ripple color (typically the state color).
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_base
     */
    void setColor(const cf::ui::base::CFColor& color);

    /**
     * @brief  Handles press event.
     *
     * @param[in]     pos Press position coordinates.
     * @param[in]     widgetRect Widget bounding rectangle.
     *
     * @throws        None
     * @note          Creates a new ripple at the press position.
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_base
     */
    void onPress(const QPoint& pos, const QRectF& widgetRect);

    /**
     * @brief  Handles release event.
     *
     * @throws        None
     * @note          Starts ripple fade-out animation.
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_base
     */
    void onRelease();

    /**
     * @brief  Handles cancel event.
     *
     * @throws        None
     * @note          Cancels unreleased ripples (e.g., when mouse leaves widget).
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_base
     */
    void onCancel();

    /**
     * @brief  Paints the ripples.
     *
     * @param[in]     painter QPainter to render with.
     * @param[in]     clipPath Clipping path for bounded mode.
     *
     * @throws        None
     * @note          Call in paintEvent.
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_base
     */
    void paint(QPainter* painter, const QPainterPath& clipPath);

    /**
     * @brief  Checks if there are active ripples.
     *
     * @return        true if any ripples are active, false otherwise.
     *
     * @throws        None
     * @note          Used to determine if repaint is needed.
     * @warning       None
     * @since         N/A
     * @ingroup       ui_widget_material_base
     */
    bool hasActiveRipple() const;

  signals:
    /**
     * @brief  Signal emitted when repaint is needed.
     *
     * @since  N/A
     * @ingroup ui_widget_material_base
     */
    void repaintNeeded();

  private:
    QList<MdRipple> m_ripples;
    Mode m_mode = Mode::Bounded;
    cf::ui::base::CFColor m_color;
    cf::WeakPtr<components::material::CFMaterialAnimationFactory> m_animator;

    float maxRadius(const QRectF& rect, const QPointF& center) const;
};

} // namespace cf::ui::widget::material::base
