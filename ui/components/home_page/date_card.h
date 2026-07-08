/**
 * @file    date_card.h
 * @brief   Placeholder card showing the day-of-month and weekday.
 *
 * A simple rounded Material card installed into CardStackWidget to exercise
 * the swipe transition. Step B replaces this with richer cards
 * (memory / calendar / weather).
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup home_page
 */

#pragma once

#include <QColor>
#include <QFont>
#include <QString>
#include <QWidget>

namespace cf::desktop::desktop_component {

/**
 * @brief  Date display card (day number + weekday).
 *
 * @note   Repaints on theme switches; refreshes the date once at construction.
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

  protected:
    /**
     * @brief  Paints the rounded surface, day number, and weekday.
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
    /// @brief Re-reads colors and fonts from the active theme.
    void applyTheme();

    /// @brief Recomputes day/weekday strings from the system date.
    void refresh();

    QColor surface_color_; ///< Card background (surface).
    QColor day_color_;     ///< Day-number color (onSurface).
    QColor weekday_color_; ///< Weekday color (onSurfaceVariant).
    QFont day_font_;       ///< Day-number font (displayLarge).
    QFont weekday_font_;   ///< Weekday font (bodyMedium).
    QString day_text_;     ///< Formatted "dd".
    QString weekday_text_; ///< Formatted weekday ("dddd").
};

} // namespace cf::desktop::desktop_component
