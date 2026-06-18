/**
 * @file    ui/widget/material/widget/groupbox/groupbox.h
 * @brief   Material Design 3 GroupBox widget.
 *
 * Implements Material Design 3 group box with rounded corners, optional
 * elevation shadows, and theme-aware colors. Provides a container for
 * grouping related widgets with a title.
 *
 * @author  CFDesktop Team
 * @date    2026-03-01
 * @version 0.1
 * @since   0.1
 * @ingroup ui_widget_material_widget
 */
#pragma once

#include <QMarginsF>

#include "aex/weak_ptr/weak_ptr.h"
#include "base/color.h"
#include "cfmaterial_animation_factory.h"
#include "export.h"
#include <QGroupBox>
#include <QWidget>

namespace cf::ui::widget::material {

// Forward declarations
namespace base {
class MdElevationController;
} // namespace base

using CFColor = cf::ui::base::CFColor;

/**
 * @brief  Material Design 3 GroupBox widget.
 *
 * @details Implements Material Design 3 group box with rounded corners,
 *          optional elevation shadows, and theme-aware colors. Provides
 *          a container for grouping related widgets with a title.
 *
 * @since  0.1
 * @ingroup ui_widget_material_widget
 */
class CF_UI_EXPORT GroupBox : public QGroupBox {
    Q_OBJECT
    Q_PROPERTY(int elevation READ elevation WRITE setElevation)
    Q_PROPERTY(float cornerRadius READ cornerRadius WRITE setCornerRadius)
    Q_PROPERTY(bool hasBorder READ hasBorder WRITE setHasBorder)

  public:
    /**
     * @brief  Constructor.
     *
     * @param[in]     parent QObject parent.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    explicit GroupBox(QWidget* parent = nullptr);

    /**
     * @brief  Constructor with title.
     *
     * @param[in]     title Group box title.
     * @param[in]     parent QObject parent.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    explicit GroupBox(const QString& title, QWidget* parent = nullptr);

    /**
     * @brief  Destructor.
     *
     * @throws        None
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    ~GroupBox() override;

    /**
     * @brief  Gets the elevation level.
     *
     * @return        Elevation level (0-5).
     *
     * @throws        None
     * @note          Material Design defines 6 standard levels.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    int elevation() const;

    /**
     * @brief  Sets the elevation level.
     *
     * @param[in]     level Elevation level (0-5).
     *
     * @throws        None
     * @note          Affects shadow rendering.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void setElevation(int level);

    /**
     * @brief  Gets the effective corner radius.
     *
     * @return        Corner radius in pixels. Returns 0 for default (auto-calculate).
     *
     * @throws        None
     * @note          Default is based on Material Design shape scale (ShapeSmall).
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    float cornerRadius() const;

    /**
     * @brief  Sets the corner radius.
     *
     * @param[in]     radius Corner radius in pixels.
     *
     * @throws        None
     * @note          Value is clamped to valid range.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void setCornerRadius(float radius);

    /**
     * @brief  Gets whether the group box has a border.
     *
     * @return        true if border is enabled, false otherwise.
     *
     * @throws        None
     * @note          Default is true.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    bool hasBorder() const;

    /**
     * @brief  Sets whether the group box has a border.
     *
     * @param[in]     hasBorder true to enable border, false to disable.
     *
     * @throws        None
     * @note          When disabled, only shadow is drawn (if elevation > 0).
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void setHasBorder(bool hasBorder);

    /**
     * @brief  Gets the recommended size.
     *
     * @return        Recommended size for the group box.
     *
     * @throws        None
     * @note          Based on content and margins.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    QSize sizeHint() const override;

    /**
     * @brief  Gets the minimum recommended size.
     *
     * @return        Minimum recommended size.
     *
     * @throws        None
     * @note          Ensures minimum content size.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    QSize minimumSizeHint() const override;

  protected:
    /**
     * @brief  Paints the group box.
     *
     * @param[in]     event Paint event.
     *
     * @throws        None
     * @note          Implements Material Design paint pipeline.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void paintEvent(QPaintEvent* event) override;

    /**
     * @brief  Handles child event.
     *
     * @param[in]     event Child event.
     *
     * @throws        None
     * @note          Used to detect layout changes.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void childEvent(QChildEvent* event) override;

    /**
     * @brief  Handles widget change event.
     *
     * @param[in]     event Change event.
     *
     * @throws        None
     * @note          Updates title tracking when title changes.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void changeEvent(QEvent* event) override;

    /**
     * @brief  Handles resize event.
     *
     * @param[in]     event Resize event.
     *
     * @throws        None
     * @note          Updates content margins on resize.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_material_widget
     */
    void resizeEvent(QResizeEvent* event) override;

  private:
    // Layout helpers
    void updateContentMargins();

    // Drawing helpers
    QMarginsF shadowMargin() const;
    void drawShadow(QPainter& p, const QRectF& contentRect, const QPainterPath& shape);
    void drawBackground(QPainter& p, const QPainterPath& shape);
    void drawBorder(QPainter& p, const QRectF& contentRect, const QPainterPath& shape);
    void drawTitle(QPainter& p, const QRectF& contentRect);

    // Color access methods
    CFColor surfaceColor() const;
    CFColor outlineColor() const;
    CFColor titleColor() const;
    QFont titleFont() const;

    // Calculate title area for background masking
    QRectF titleArea() const;

    // Behavior components
    aex::WeakPtr<components::material::CFMaterialAnimationFactory> m_animationFactory;
    base::MdElevationController* m_elevation;

    // Properties
    float m_cornerRadius;
    bool m_hasBorder;
    bool m_hasTitle;
};

} // namespace cf::ui::widget::material
