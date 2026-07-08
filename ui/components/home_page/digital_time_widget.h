/**
 * @file    digital_time_widget.h
 * @brief   Digital HH:mm:ss + date display driven by GlobalClockSources.
 *
 * Migrated from CCIMXDesktop DigitalTimeWidget: colors/fonts re-based on
 * ThemeManager MD3 tokens (onSurface / onSurfaceVariant) instead of the
 * original hardcoded white.
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

class QTime;

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
    /// @brief Re-reads colors and fonts from the active theme.
    void applyTheme();

    /// @brief Updates the cached strings and repaints on each tick.
    /// @param[in]  time  Current time.
    void onTimeUpdate(const QTime& time);

    QColor time_color_; ///< Time text color (onSurface).
    QColor date_color_; ///< Date text color (onSurfaceVariant).
    QFont time_font_;   ///< Time font (displayMedium).
    QFont date_font_;   ///< Date font (bodySmall).
    QString time_text_; ///< Formatted "HH:mm:ss".
    QString date_text_; ///< Formatted "yyyy-MM-dd dddd".
};

} // namespace cf::desktop::desktop_component
