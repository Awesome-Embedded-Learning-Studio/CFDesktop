/**
 * @file    net_status_card.h
 * @brief   Network-status gadget card (CCIMX NetCardGadget style, cfbase-backed).
 *
 * Teal-gradient card showing the current reachability (Online / Site / Local /
 * Disconnected), the transport medium, and the first non-loopback IPv4 address.
 * Data comes from cfbase getNetworkInfo() instead of CCIMX's NetAbilityScanner;
 * the card is standalone (no AppCardWidget / DesktopToast dependency).
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
 * @brief  Bottom-grid network status card backed by cfbase.
 *
 * @note   Theme-less (hardcoded QSS, CCIMX style). Polls cfbase on a coarse
 *         timer (the probe is a fast getifaddrs-style snapshot).
 * @warning None
 * @since  0.19.0
 * @ingroup home_page
 */
class NetStatusCard : public QWidget {
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
    explicit NetStatusCard(QWidget* parent = nullptr);

  private:
    /// @brief Re-queries cfbase and updates the labels.
    void refresh();

    QLabel* status_label_; ///< Big reachability word (Online / Local / ...).
    QLabel* detail_label_; ///< Transport medium + interface + IPv4.
    QTimer* timer_;        ///< Poll timer.
};

} // namespace cf::desktop::desktop_component
