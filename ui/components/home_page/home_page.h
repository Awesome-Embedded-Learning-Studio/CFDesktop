/**
 * @file    home_page.h
 * @brief   Desktop home screen: clock column + swipeable card stack.
 *
 * Migrated from CCIMXDesktop HomePage. Layout is rewritten as hand-coded
 * QLayout (CFDesktop uses no .ui files): left column holds the analog clock
 * (5/7) above the digital time (2/7); right column holds the CardStackWidget
 * (3/4) above a placeholder app-grid area (1/4, Step B).
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup home_page
 */

#pragma once

#include <QWidget>
#include <memory>

namespace cf::desktop::desktop_component {

class AnalogClockWidget;
class DigitalTimeWidget;
class CardStackWidget;
class HomeCardManager;

/**
 * @brief  The desktop home screen widget.
 *
 * @note   Owns the clock column and the card stack; does not subscribe to
 *         GlobalClockSources itself (each clock widget does).
 * @warning None
 * @since  0.19.0
 * @ingroup home_page
 */
class HomePage : public QWidget {
    Q_OBJECT

  public:
    /**
     * @brief  Constructs the home page and installs the default cards.
     * @param[in]  parent  Owning Qt parent widget.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    home_page
     */
    explicit HomePage(QWidget* parent = nullptr);

    /**
     * @brief  Returns the card manager (for Step B card registration).
     * @return     Pointer to the HomeCardManager.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    home_page
     */
    HomeCardManager* cardManager() const;

  private:
    AnalogClockWidget* analog_clock_;               ///< Left-top analog dial.
    DigitalTimeWidget* digital_time_;               ///< Left-bottom digital time.
    CardStackWidget* card_stack_;                   ///< Right-top swipeable stack.
    std::unique_ptr<HomeCardManager> card_manager_; ///< Stack registry.
};

} // namespace cf::desktop::desktop_component
