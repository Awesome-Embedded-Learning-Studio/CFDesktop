/**
 * @file    digital_time_widget.h
 * @brief   Digital HH:mm:ss + date display driven by GlobalClockSources.
 *
 * Ported verbatim from CCIMXDesktop DigitalTimeWidget: white Helvetica Neue
 * (40pt Bold time / 15pt Light date), no MD3 theming (Phase G re-themes later).
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
 * @brief  Digital time (HH:mm:ss) with a date line beneath.
 *
 * @note   Subscribes to GlobalClockSources for the 1 s tick.
 * @warning None
 * @since  0.19.0
 * @ingroup home_page
 */
class DigitalTimeWidget : public QWidget {
    Q_OBJECT

  public:
    /**
     * @brief  Constructs the digital time widget.
     * @param[in]  parent  Owning Qt parent widget.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    home_page
     */
    explicit DigitalTimeWidget(QWidget* parent = nullptr);

  protected:
    /**
     * @brief  Paints the time and date text.
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
    /// @brief Updates the cached time and repaints on each tick.
    /// @param[in]  time  Current time.
    void onTimeUpdate(const QTime& time);

    QTime stored_time_; ///< Latest received time.
};

} // namespace cf::desktop::desktop_component
