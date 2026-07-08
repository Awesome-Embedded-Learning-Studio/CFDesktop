/**
 * @file    user_info_card.h
 * @brief   User-profile card (CCIMX UserInfoCard style, desktop-data variant).
 *
 * Green-gradient card showing an avatar (the user's initial), the OS user name,
 * the hostname, and the OS/kernel line. Visual mirrors CCIMX UserInfoCard; the
 * data comes from POSIX (getpwuid / gethostname / uname) instead of CCIMX's
 * DesktopUserInfo, since CFDesktop has no user-profile subsystem and a generic
 * desktop has no real phone/email to display.
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

namespace cf::desktop::desktop_component {

/**
 * @brief  User-profile card backed by local POSIX data.
 *
 * @note   Theme-less (hardcoded QSS, CCIMX style). Gathers its own data at
 *         construction; call refresh() to re-read after a locale/user change.
 * @warning None
 * @since  0.19.0
 * @ingroup home_page
 */
class UserInfoCard : public QWidget {
    Q_OBJECT

  public:
    /**
     * @brief  Constructs the card and reads the current user info.
     * @param[in]  parent  Owning Qt parent widget.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    home_page
     */
    explicit UserInfoCard(QWidget* parent = nullptr);

    /**
     * @brief  Re-reads the OS user info and updates the labels.
     * @return None
     * @throws None
     * @note   None
     * @warning None
     * @since  0.19.0
     * @ingroup home_page
     */
    void refresh();

  private:
    QLabel* avatar_label_; ///< Circular avatar showing the user's initial.
    QLabel* name_label_;   ///< OS user name.
    QLabel* host_label_;   ///< Hostname line.
    QLabel* os_label_;     ///< OS / kernel line.
};

} // namespace cf::desktop::desktop_component
