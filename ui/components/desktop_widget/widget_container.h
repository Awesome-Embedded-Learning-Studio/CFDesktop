/**
 * @file    widget_container.h
 * @brief   Registry for desktop widgets.
 *
 * WidgetContainer owns the list of WidgetBase instances shown on the shell and
 * reparents each one to a host widget (the desktop surface) on addWidget. It
 * is a QObject, not a QWidget: the widgets themselves stack as direct children
 * of the host so Qt z-order follows the host's child creation order.
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup desktop_widget
 */

#pragma once

#include <QObject>
#include <QPoint>
#include <QString>
#include <vector>

class QWidget;

namespace cf::desktop::desktop_component {

class WidgetBase;

/**
 * @brief  Tracks the set of desktop widgets mounted on the shell.
 *
 * @note   Widgets are reparented to the host supplied to addWidget.
 * @warning None
 * @since  0.19.0
 * @ingroup desktop_widget
 */
class WidgetContainer : public QObject {
    Q_OBJECT

  public:
    /**
     * @brief  Constructs the container.
     * @param[in]  parent  Owning Qt parent.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    desktop_widget
     */
    explicit WidgetContainer(QObject* parent = nullptr);

    /**
     * @brief  Mounts a widget on the host at the given position.
     * @param[in]  widget    Widget to mount (becomes child of host).
     * @param[in]  host      Widget that parents and stacks it.
     * @param[in]  position  Top-left position in host coordinates.
     * @return     None
     * @throws     None
     * @note       Resizes the widget to its defaultSize and shows it.
     * @warning    None
     * @since      0.19.0
     * @ingroup    desktop_widget
     */
    void addWidget(WidgetBase* widget, QWidget* host, const QPoint& position);

    /**
     * @brief  Removes (and deletes) the widget with the given id, if any.
     * @param[in]  id  Widget id to remove.
     * @return     None
     * @throws     None
     * @note       No-op if no widget matches.
     * @warning    None
     * @since      0.19.0
     * @ingroup    desktop_widget
     */
    void removeWidget(const QString& id);

  private:
    std::vector<WidgetBase*> widgets_; ///< Mounted widgets (owned via Qt parentship).
};

} // namespace cf::desktop::desktop_component
