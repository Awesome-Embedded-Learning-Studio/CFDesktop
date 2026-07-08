/**
 * @file    local_temp_card.h
 * @brief   Local-temperature gadget card (CCIMX LocalWeatherCard desktop analog).
 *
 * Warm-gradient card showing a real local temperature read from the Linux
 * thermal subsystem (/sys/class/thermal/thermal_zone*). CCIMX's LocalWeatherCard
 * reads an ICM20608 hardware IMU (embedded-only) and fakes random values on
 * x86; CFDesktop has neither sensor, so this card reads the genuine kernel
 * thermal zone instead, and reports "N/A" honestly where no zone exists (e.g.
 * WSL2, which does not expose thermal zones).
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup home_page
 */

#pragma once

#include <QWidget>

class QLabel;
class QTimer;

namespace cf::desktop::desktop_component {

/**
 * @brief  Bottom-grid local-temperature card backed by the Linux thermal subsystem.
 *
 * @note   Theme-less (hardcoded QSS, CCIMX style). Polls the thermal zone on a
 *         coarse timer.
 * @warning None
 * @since  0.19.0
 * @ingroup home_page
 */
class LocalTempCard : public QWidget {
    Q_OBJECT

  public:
    /**
     * @brief  Constructs the card.
     * @param[in]  parent  Owning Qt parent widget.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    home_page
     */
    explicit LocalTempCard(QWidget* parent = nullptr);

  private:
    /// @brief Re-reads the thermal zone and updates the labels.
    void refresh();

    QLabel* value_label_;  ///< Temperature (e.g. "52.3 C") or "N/A".
    QLabel* detail_label_; ///< Thermal-zone label (e.g. "x86_pkg_temp").
    QTimer* timer_;        ///< Poll timer.
};

} // namespace cf::desktop::desktop_component
