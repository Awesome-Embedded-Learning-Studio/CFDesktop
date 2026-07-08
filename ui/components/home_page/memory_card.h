/**
 * @file    memory_card.h
 * @brief   Home-screen card showing live RAM usage.
 *
 * A rounded Material card (surfaceContainer) with a "Memory" label, a large
 * percentage, a progress bar (primary on surfaceVariant), and a "used / total"
 * line. Polls cfbase's getSystemMemoryInfo on a coarse 2 s timer.
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
 * @brief  Live RAM usage card for the home screen card stack.
 *
 * @note   Repaints on theme switches; refreshes usage every 2 s.
 * @warning None
 * @since  0.19.0
 * @ingroup home_page
 */
class MemoryCard : public QWidget {
    Q_OBJECT

  public:
    /**
     * @brief  Constructs the memory card.
     * @param[in]  parent  Owning Qt parent widget.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    home_page
     */
    explicit MemoryCard(QWidget* parent = nullptr);

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

    /// @brief Re-polls cfbase for current RAM usage.
    void refresh();

    QTimer* timer_{nullptr}; ///< 2 s refresh timer.
    QColor surface_color_;   ///< Card background (surface).
    QColor track_color_;     ///< Progress bar track (surfaceVariant).
    QColor fill_color_;      ///< Progress bar fill (primary).
    QColor label_color_;     ///< "Memory" label (onSurfaceVariant).
    QColor value_color_;     ///< Percentage text (onSurface).
    QColor detail_color_;    ///< used/total text (onSurfaceVariant).
    QFont label_font_;       ///< Label font (labelMedium).
    QFont value_font_;       ///< Percentage font (displaySmall).
    QFont detail_font_;      ///< Detail font (bodySmall).
    QString detail_text_;    ///< "X.X / Y.Y GB".
    int pct_{0};             ///< Used percentage (0-100).
};

} // namespace cf::desktop::desktop_component
