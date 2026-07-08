/**
 * @file    analog_clock_widget.h
 * @brief   Analog clock dial (hands + ticks) painted with QPainter.
 *
 * Migrated from CCIMXDesktop ClockWidget. Geometry is unchanged (fixed dial
 * constants); colors are re-based on ThemeManager MD3 tokens instead of the
 * original hardcoded black/white/red.
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup home_page
 */

#pragma once

#include <QColor>
#include <QTime>
#include <QWidget>

namespace cf::desktop::desktop_component {

/**
 * @brief  Analog clock with hour/minute/second hands.
 *
 * @note   Subscribes to GlobalClockSources for the 1 s tick.
 * @warning None
 * @since  0.19.0
 * @ingroup home_page
 */
class AnalogClockWidget : public QWidget {
    Q_OBJECT

  public:
    /**
     * @brief  Constructs the analog clock.
     * @param[in]  parent  Owning Qt parent widget.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    home_page
     */
    explicit AnalogClockWidget(QWidget* parent = nullptr);

  protected:
    /**
     * @brief  Paints the dial, ticks, hands, and center dot.
     * @param[in]  event  Paint event (unused).
     * @return     None
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    home_page
     */
    void paintEvent(QPaintEvent* event) override;

  private:
    /// @brief Re-reads colors from the active theme.
    void applyTheme();

    /// @brief Caches the latest time and repaints.
    /// @param[in]  time  Current time.
    void onTimeUpdate(const QTime& time);

    void drawBackground(QPainter* p); ///< Dial background + border.
    void drawTicks(QPainter* p);      ///< Hour and minute tick marks.
    void drawHands(QPainter* p);      ///< Hour, minute, second hands.
    void drawCenterDot(QPainter* p);  ///< Pivot dot.

    QTime current_time_; ///< Latest received time.

    QColor surface_color_;         ///< Dial fill center (surface).
    QColor surface_variant_color_; ///< Dial fill edge (surfaceVariant).
    QColor outline_color_;         ///< Dial border (outline).
    QColor hand_color_;            ///< Hour/minute hand + hour ticks (onSurface).
    QColor minute_tick_color_;     ///< Minute ticks (onSurfaceVariant).
    QColor second_hand_color_;     ///< Second hand (primary).
};

} // namespace cf::desktop::desktop_component
