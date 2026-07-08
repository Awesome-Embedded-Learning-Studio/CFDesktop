/**
 * @file    cpu_card.h
 * @brief   Home-screen card showing live CPU usage.
 *
 * Rounded Material card with a "CPU" label, usage percentage, progress bar,
 * and a "cores · frequency" detail line. Polls cfbase's getCPUProfileInfo on a
 * coarse 5 s timer (the usage probe blocks ~100 ms per sample).
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

class QTimer;

namespace cf::desktop::desktop_component {

/**
 * @brief  Live CPU usage card for the home screen card stack.
 *
 * @note   Repaints on theme switches; refreshes usage every 5 s.
 * @warning None
 * @since  0.19.0
 * @ingroup home_page
 */
class CpuCard : public QWidget {
    Q_OBJECT

  public:
    /**
     * @brief  Constructs the CPU card.
     * @param[in]  parent  Owning Qt parent widget.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    home_page
     */
    explicit CpuCard(QWidget* parent = nullptr);

  protected:
    /**
     * @brief  Paints the rounded surface, label, percentage, bar, and detail.
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

    /// @brief Re-polls cfbase for current CPU usage.
    void refresh();

    QTimer* timer_{nullptr}; ///< 5 s refresh timer.
    QColor surface_color_;   ///< Card background (surface).
    QColor track_color_;     ///< Progress bar track (surfaceVariant).
    QColor fill_color_;      ///< Progress bar fill (primary).
    QColor label_color_;     ///< "CPU" label (onSurfaceVariant).
    QColor value_color_;     ///< Percentage text (onSurface).
    QColor detail_color_;    ///< cores/frequency text (onSurfaceVariant).
    QFont label_font_;       ///< Label font (labelMedium).
    QFont value_font_;       ///< Percentage font (displaySmall).
    QFont detail_font_;      ///< Detail font (bodySmall).
    QString detail_text_;    ///< "N cores · MMMM MHz".
    int pct_{0};             ///< Usage percentage (0-100).
};

} // namespace cf::desktop::desktop_component
