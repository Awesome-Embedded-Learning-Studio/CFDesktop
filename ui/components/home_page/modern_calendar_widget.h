/**
 * @file    modern_calendar_widget.h
 * @brief   Styled QCalendarWidget with colored date markers (ported from CCIMX).
 *
 * A QCalendarWidget subclass that paints each cell itself (rounded background,
 * selection / today / event dots) and exposes a date->color marker map. Visual
 * constants mirror CCIMX ModernCalendarWidget; nav-bar arrow icons fall back to
 * Qt defaults (CCIMX used bundled PNGs that are not present here).
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup home_page
 */

#pragma once

#include <QCalendarWidget>
#include <QColor>
#include <QDate>
#include <QHash>

class QString;

namespace cf::desktop::desktop_component {

/**
 * @brief  QCalendarWidget with custom cell painting and date markers.
 *
 * @note   Theme-less (hardcoded QSS, CCIMX style); a dark variant is available
 *         via setDarkMode().
 * @warning None
 * @since  0.19.0
 * @ingroup home_page
 */
class ModernCalendarWidget : public QCalendarWidget {
    Q_OBJECT

  public:
    /**
     * @brief  Constructs the calendar.
     * @param[in]  parent  Owning Qt parent widget.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    home_page
     */
    explicit ModernCalendarWidget(QWidget* parent = nullptr);

    /**
     * @brief  Switches between the light and dark visual variants.
     * @param[in]  dark_mode  True for the dark variant.
     * @return     None
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    home_page
     */
    void setDarkMode(bool dark_mode);

    /**
     * @brief   Reports whether the dark variant is active.
     * @return  True if dark mode is on.
     * @throws  None
     * @note    None
     * @warning None
     * @since   0.19.0
     */
    bool isDarkMode() const { return dark_mode; }

    /**
     * @brief  Marks a date with a color dot.
     * @param[in]  date   Date to mark.
     * @param[in]  color  Marker color.
     * @return     None
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    home_page
     */
    void setColorForDate(const QDate& date, const QColor& color);

    /**
     * @brief  Removes the marker for a date.
     * @param[in]  date  Date whose marker to remove.
     * @return     None
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    home_page
     */
    void popColorForDate(const QDate& date);

  protected:
    /**
     * @brief  Paints a single calendar cell.
     * @param[in]  painter  Painter to draw with.
     * @param[in]  rect     Cell rectangle.
     * @param[in]  date     Date the cell represents.
     * @return     None
     * @throws     None
     * @note       Selection takes priority, then an event marker, then today.
     * @warning    None
     * @since      0.19.0
     * @ingroup    home_page
     */
    void paintCell(QPainter* painter, const QRect& rect, const QDate date) const override;

  private:
    /// @brief Restyles the navigation bar (transparent background).
    void styleNavigationBar();

    /**
     * @brief   Builds the QSS for the current mode.
     * @param[in]  is_dark_mode  True for the dark variant.
     * @return  The QSS string.
     * @throws  None
     * @note    None
     * @warning None
     * @since   0.19.0
     */
    static QString globalModeQss(bool is_dark_mode = false);

    bool dark_mode{false};             ///< True when the dark variant is active.
    QHash<QDate, QColor> date_colors_; ///< Date -> marker color.
};

} // namespace cf::desktop::desktop_component
