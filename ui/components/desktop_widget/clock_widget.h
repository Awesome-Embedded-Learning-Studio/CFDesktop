/**
 * @file    clock_widget.h
 * @brief   Material-styled desktop clock widget (time + date, 1s refresh).
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup desktop_widget
 */

#pragma once

#include <QColor>
#include <QFont>
#include <QString>

#include "widget_base.h"

class QTimer;

namespace cf::desktop::desktop_component {

/**
 * @brief  A draggable desktop clock showing HH:mm and the date.
 *
 * Paints a rounded Material surface (surfaceContainer) with the time in
 * displayLarge and the date in bodyMedium, following the active theme. A
 * coarse 1 s timer refreshes the text and triggers a repaint.
 *
 * @note   Drag-to-move is inherited from WidgetBase.
 * @warning None
 * @since  0.19.0
 * @ingroup desktop_widget
 */
class ClockWidget : public WidgetBase {
    Q_OBJECT

  public:
    /**
     * @brief  Constructs the clock widget.
     * @param[in]  parent  Owning Qt parent widget.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    desktop_widget
     */
    explicit ClockWidget(QWidget* parent = nullptr);

    /**
     * @brief  Returns the widget id "clock".
     * @return     "clock".
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    desktop_widget
     */
    QString widgetId() const override { return "clock"; }

    /**
     * @brief  Returns the display name "Clock".
     * @return     "Clock".
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    desktop_widget
     */
    QString widgetName() const override { return "Clock"; }

    /**
     * @brief  Returns the preferred clock size (320 x 160).
     * @return     Default size.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    desktop_widget
     */
    QSize defaultSize() const override { return {320, 160}; }

  protected:
    /**
     * @brief  Paints the rounded surface, time, and date.
     * @param[in]  event  Paint event (unused).
     * @return     None
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    desktop_widget
     */
    void paintEvent(QPaintEvent* event) override;

  private:
    /// @brief Re-reads colors and fonts from the active theme.
    void applyTheme();

    /// @brief Recomputes the cached time/date strings from the system clock.
    void refreshTime();

    QTimer* timer_{nullptr}; ///< 1 s refresh timer.
    QColor surface_color_;   ///< Card background (surfaceContainer).
    QColor time_color_;      ///< Time text color (onSurface).
    QColor date_color_;      ///< Date text color (onSurfaceVariant).
    QFont time_font_;        ///< Time font (displayLarge).
    QFont date_font_;        ///< Date font (bodyMedium).
    QString cached_time_;    ///< Formatted "HH:mm".
    QString cached_date_;    ///< Formatted "yyyy-MM-dd dddd".
};

} // namespace cf::desktop::desktop_component
