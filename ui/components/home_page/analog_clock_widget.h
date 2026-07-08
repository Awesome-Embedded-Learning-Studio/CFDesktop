/**
 * @file    analog_clock_widget.h
 * @brief   Analog clock dial (hands + ticks + numerals) painted with QPainter.
 *
 * Ported verbatim from CCIMXDesktop ClockWidget: hardcoded geometry + classic
 * palette (white radial gradient dial, 8px black border, black hands, red
 * second hand, 12/3/6/9 numerals). No MD3 theming (Phase G re-themes later).
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup home_page
 */

#pragma once

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
     * @brief  Paints the dial, numerals, ticks, hands, and center dot.
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
    /// @brief Caches the latest time and repaints.
    /// @param[in]  time  Current time.
    void onTimeUpdate(const QTime& time);

    void drawBackground(QPainter* p); ///< Dial gradient + thick border.
    void drawNumbers(QPainter* p);    ///< 12 / 3 / 6 / 9 numerals.
    void drawTicks(QPainter* p);      ///< Hour and minute tick marks.
    void drawHands(QPainter* p);      ///< Hour, minute, second hands.
    void drawCenterDot(QPainter* p);  ///< Pivot dot.

    QTime current_time_; ///< Latest received time.
};

} // namespace cf::desktop::desktop_component
