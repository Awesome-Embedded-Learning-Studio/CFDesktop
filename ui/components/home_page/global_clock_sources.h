/**
 * @file    global_clock_sources.h
 * @brief   Process-wide clock tick source emitting timeUpdate each second.
 *
 * Migrated from CCIMXDesktop GlobalClockSources. A single coarse 1 s QTimer
 * drives every clock-displaying widget (analog clock, digital time) so they
 * tick in sync instead of each running its own timer.
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup home_page
 */

#pragma once

#include <QObject>

class QTime;

namespace cf::desktop::desktop_component {

/**
 * @brief  Singleton clock source emitting a 1 s tick.
 *
 * @note   Coarse timer; idletime cost is one tick per second, never a loop.
 * @warning None
 * @since  0.19.0
 * @ingroup home_page
 */
class GlobalClockSources : public QObject {
    Q_OBJECT

  public:
    /**
     * @brief  Returns the process-wide singleton.
     * @return     Reference to the singleton instance.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    home_page
     */
    static GlobalClockSources& instance();

  signals:
    /**
     * @brief  Emitted every second with the current wall-clock time.
     * @param[in]  time  Current time.
     * @return     None
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    home_page
     */
    void timeUpdate(const QTime& time);

  private:
    /**
     * @brief  Constructs the clock source and starts the timer.
     * @throws     None
     * @note       Private; access via instance().
     * @warning    None
     * @since      0.19.0
     * @ingroup    home_page
     */
    GlobalClockSources();
};

} // namespace cf::desktop::desktop_component
