/**
 * @file    home_card_manager.h
 * @brief   Registry that installs cards into a CardStackWidget.
 *
 * Migrated from CCIMXDesktop HomeCardManager. A thin wrapper around
 * QStackedWidget add/remove so HomePage code does not touch the stack directly.
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup home_page
 */

#pragma once

#include <QObject>

class QStackedWidget;
class QWidget;

namespace cf::desktop::desktop_component {

/**
 * @brief  Manages the card list shown in a CardStackWidget.
 *
 * @note   UI-thread only (no mutex; QStackedWidget is single-threaded).
 * @warning None
 * @since  0.19.0
 * @ingroup home_page
 */
class HomeCardManager : public QObject {
    Q_OBJECT

  public:
    /**
     * @brief  Constructs the manager bound to a stacked widget.
     * @param[in]  stack   The CardStackWidget (as QStackedWidget) to manage.
     * @param[in]  parent  Owning Qt parent.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    home_page
     */
    HomeCardManager(QStackedWidget* stack, QObject* parent = nullptr);

    /**
     * @brief  Appends a card to the stack.
     * @param[in]  card  Card widget to install.
     * @return     None
     * @throws     None
     * @note       Reparents the card to the stack.
     * @warning    None
     * @since      0.19.0
     * @ingroup    home_page
     */
    void installCard(QWidget* card);

    /**
     * @brief  Removes a card from the stack.
     * @param[in]  card  Card widget to remove.
     * @return     None
     * @throws     None
     * @note       Does not delete the card.
     * @warning    None
     * @since      0.19.0
     * @ingroup    home_page
     */
    void removeCard(QWidget* card);

    /**
     * @brief  Returns the number of installed cards.
     * @return     Card count.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    home_page
     */
    int cardCount() const;

  private:
    QStackedWidget* stack_; ///< The managed stacked widget.
};

} // namespace cf::desktop::desktop_component
