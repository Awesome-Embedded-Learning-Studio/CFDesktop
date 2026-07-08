/**
 * @file    widget_container.cpp
 * @brief   Implementation of the desktop widget registry.
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup desktop_widget
 */

#include "widget_container.h"

#include "widget_base.h"

#include <QWidget>

#include <algorithm>

namespace cf::desktop::desktop_component {

WidgetContainer::WidgetContainer(QObject* parent) : QObject(parent) {}

void WidgetContainer::addWidget(WidgetBase* widget, QWidget* host, const QPoint& position) {
    if (widget == nullptr || host == nullptr) {
        return;
    }
    widget->setParent(host);
    widget->resize(widget->defaultSize());
    widget->move(position);
    widget->setEditable(true);
    widget->show();
    widgets_.push_back(widget);
}

void WidgetContainer::removeWidget(const QString& id) {
    widgets_.erase(std::remove_if(widgets_.begin(), widgets_.end(),
                                  [&](WidgetBase* w) {
                                      if (w != nullptr && w->widgetId() == id) {
                                          delete w;
                                          return true;
                                      }
                                      return false;
                                  }),
                   widgets_.end());
}

} // namespace cf::desktop::desktop_component
