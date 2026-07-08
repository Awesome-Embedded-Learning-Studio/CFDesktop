/**
 * @file    widget_base.h
 * @brief   Draggable base class for free-positionable desktop widgets.
 *
 * WidgetBase defines the common contract (id / name / default size) and a
 * press-drag-move interaction for desktop widgets that float on the shell
 * (clock, future system widgets). Concrete widgets implement paintEvent and
 * the metadata accessors; dragging is handled here.
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup desktop_widget
 */

#pragma once

#include <QPoint>
#include <QSize>
#include <QString>
#include <QWidget>

namespace cf::desktop::desktop_component {

/**
 * @brief  Base class for free-positionable desktop widgets.
 *
 * Subclass to add a paintEvent and supply widgetId/widgetName/defaultSize.
 * Dragging moves the widget under the cursor and emits widgetMoved.
 *
 * @note   Dragging only active when editable is true (default).
 * @warning None
 * @since  0.19.0
 * @ingroup desktop_widget
 */
class WidgetBase : public QWidget {
    Q_OBJECT

  public:
    /**
     * @brief  Constructs the widget base.
     * @param[in]  parent  Owning Qt parent widget.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    desktop_widget
     */
    explicit WidgetBase(QWidget* parent = nullptr);

    /**
     * @brief  Destroys the widget base.
     * @throws None
     * @note   None
     * @warning None
     * @since  0.19.0
     * @ingroup desktop_widget
     */
    ~WidgetBase() override = default;

    /**
     * @brief  Returns the stable identifier of this widget.
     * @return     Widget id (e.g. "clock").
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    desktop_widget
     */
    virtual QString widgetId() const = 0;

    /**
     * @brief  Returns the human-readable name of this widget.
     * @return     Display name.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    desktop_widget
     */
    virtual QString widgetName() const = 0;

    /**
     * @brief  Returns the preferred size for this widget.
     * @return     Default size in pixels.
     * @throws     None
     * @note       Used by WidgetContainer to resize on addWidget.
     * @warning    None
     * @since      0.19.0
     * @ingroup    desktop_widget
     */
    virtual QSize defaultSize() const = 0;

    /**
     * @brief  Enables or disables drag-to-move.
     * @param[in]  editable  True to allow dragging.
     * @return     None
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    desktop_widget
     */
    void setEditable(bool editable);

    /**
     * @brief  Returns whether dragging is currently allowed.
     * @return     True if editable.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    desktop_widget
     */
    bool isEditable() const;

  signals:
    /**
     * @brief  Emitted while the widget is dragged to a new position.
     * @param[in]  pos  New top-left position (parent coordinates).
     * @return     None
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    desktop_widget
     */
    void widgetMoved(const QPoint& pos);

  protected:
    /**
     * @brief  Records the press anchor when the left button goes down.
     * @param[in]  event  Mouse press event.
     * @return     None
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    desktop_widget
     */
    void mousePressEvent(QMouseEvent* event) override;

    /**
     * @brief  Moves the widget by the cursor delta once a drag threshold passes.
     * @param[in]  event  Mouse move event.
     * @return     None
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    desktop_widget
     */
    void mouseMoveEvent(QMouseEvent* event) override;

    /**
     * @brief  Ends an in-progress drag.
     * @param[in]  event  Mouse release event.
     * @return     None
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    desktop_widget
     */
    void mouseReleaseEvent(QMouseEvent* event) override;

  private:
    bool editable_{true};  ///< Whether drag-to-move is enabled.
    QPoint drag_anchor_{}; ///< Press position (local coords) at drag start.
    bool dragging_{false}; ///< Whether a drag has actually begun.
};

} // namespace cf::desktop::desktop_component
