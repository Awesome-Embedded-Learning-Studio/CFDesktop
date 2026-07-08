/**
 * @file    date_card.h
 * @brief   Blue-gradient date card (CCIMX DateShowCard style).
 *
 * Ported from CCIMXDesktop DateShowCard: blue gradient background (#4A90E2 ->
 * #1E3C72) + drop shadow + white QLabels (title / day / full text). No MD3
 * theming (Phase G re-themes later).
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
 * @brief  Date display card (title + day number + full date), blue gradient.
 *
 * @note   Refreshes the date once at construction.
 * @warning None
 * @since  0.19.0
 * @ingroup home_page
 */
class DateCard : public QWidget {
    Q_OBJECT

  public:
    /**
     * @brief  Constructs the date card.
     * @param[in]  parent  Owning Qt parent widget.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    home_page
     */
    explicit DateCard(QWidget* parent = nullptr);

  private:
    /// @brief Recomputes day / full-date strings from the system date.
    void refresh();

    QLabel* title_label_;     ///< "DATE" heading.
    QLabel* day_label_;       ///< Two-digit day number.
    QLabel* full_text_label_; ///< "yyyy MMM dd ddd".
};

} // namespace cf::desktop::desktop_component
