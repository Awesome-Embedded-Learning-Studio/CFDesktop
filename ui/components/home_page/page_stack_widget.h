/**
 * @file    page_stack_widget.h
 * @brief   QStackedWidget with horizontal swipe + slide page transitions.
 *
 * Holds the desktop pages (HomePage, icon layer) and flips between them with a
 * horizontal drag: a drag past 1/4 of the width commits the switch (animated),
 * otherwise the current page bounces back. Mirrors CardStackWidget (which does
 * the same for vertical card swipes); the two coexist by Qt event propagation
 * -- HomePage and its clock widgets do not accept mouse press, so it bubbles
 * here, while CardStackWidget accepts press and keeps the gesture.
 *
 * @details Rendering during a swipe is snapshot-based: at gesture start each
 * page is rendered once into a QPixmap, the live page widgets are hidden, and a
 * child overlay widget paints only the two cached pixmaps translated by the
 * drag offset. This turns a per-frame full-page widget-tree repaint (very costly
 * in software rendering on a slow core such as i.MX6ULL) into a pair of cheap
 * pixmap blits. On commit/bounce a single QPropertyAnimation drives the overlay
 * offset; the live widgets are restored when the animation finishes.
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup home_page
 */

#pragma once

#include <QImage>
#include <QPixmap>
#include <QPoint>
#include <QPropertyAnimation>
#include <QStackedWidget>
#include <QVector>

#include <functional>

class QMouseEvent;
class QPaintEvent;
class QResizeEvent;
class QShowEvent;

namespace cf::desktop::desktop_component {

class SwipeOverlay;

/**
 * @brief  Stacked widget of desktop pages with horizontal swipe transitions.
 *
 * @note   Single-threaded (UI thread only).
 * @warning None
 * @since  0.19.0
 * @ingroup home_page
 */
class PageStackWidget : public QStackedWidget {
    Q_OBJECT
    /// @brief Horizontal offset of the current-page snapshot during a swipe
    ///        (animated by swipe_anim_). Negative slides left (next page in),
    ///        positive slides right (previous page in).
    Q_PROPERTY(int swipeOffset READ swipeOffset WRITE setSwipeOffset)

  public:
    /**
     * @brief  Constructs the page stack.
     * @param[in]  parent  Owning Qt parent widget.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    home_page
     */
    explicit PageStackWidget(QWidget* parent = nullptr);

    /**
     * @brief  Animates a transition to the given page index.
     * @param[in]  new_index  Target page index.
     * @return     None
     * @throws     None
     * @note       Direction is inferred from the current index.
     * @warning    None
     * @since      0.19.0
     * @ingroup    home_page
     */
    void slideToPage(int new_index);

    /**
     * @brief  Sets the transition animation duration.
     * @param[in]  duration_ms  Duration in milliseconds.
     * @return     None
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    home_page
     */
    void setAnimationDuration(int duration_ms);

    /**
     * @brief  Sets the desktop background image provider used behind pages.
     * @param[in]  provider  Callback returning the current shell background.
     * @return     None
     * @throws     None
     * @note       The returned image may be full-desktop or page-sized.
     * @warning    None
     * @since      0.19.0
     * @ingroup    home_page
     */
    void setBackgroundProvider(std::function<QImage()> provider);

    /**
     * @brief  Returns the animation duration.
     * @return     Duration in milliseconds.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    home_page
     */
    int animationDuration() const;

    /**
     * @brief  Animates to the next (higher-index) page if any.
     * @return     None
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    home_page
     */
    void toNextPage();

    /**
     * @brief  Animates to the previous (lower-index) page if any.
     * @return     None
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    home_page
     */
    void toPrevPage();

  protected:
    /// @brief Records the drag start position on left-button press.
    void mousePressEvent(QMouseEvent* event) override;
    /// @brief Drags the neighboring page in via snapshot overlay.
    void mouseMoveEvent(QMouseEvent* event) override;
    /// @brief Commits the switch past 1/4 width, otherwise bounces back.
    void mouseReleaseEvent(QMouseEvent* event) override;
    /// @brief Paints the wallpaper backdrop under transparent page widgets.
    void paintEvent(QPaintEvent* event) override;
    /// @brief Captures the initially-visible page snapshot on first show.
    void showEvent(QShowEvent* event) override;
    /// @brief Invalidates the snapshot cache on resize (e.g. the display probing
    ///        geometry change 640x480 -> 1024x600 during boot) and re-captures
    ///        the current page at the new size.
    void resizeEvent(QResizeEvent* event) override;

  private:
    /// @brief Snapshot-based transition with explicit direction.
    void slideToPage(int new_index, bool to_left);

    /// @brief Captures both pages to pixmaps, hides the live pages, shows the
    ///        overlay. Idempotent within an active swipe.
    void beginSwipe(bool to_next);

    /// @brief Animates the overlay offset to @p target_offset, then restores the
    ///        live page at @p new_index.
    void animateSwipeTo(int target_offset, int new_index);

    /// @brief Tears down the overlay and shows the destination live page.
    void endSwipe(int new_index);

    /// @brief Re-renders page @p idx into the snapshot cache. Only meaningful
    ///        when the page is currently visible (warm); cheap because Qt can
    ///        reuse the on-screen backing store.
    void refreshSnapshot(int idx);

    /// @brief Returns a pixmap of the wallpaper area under this page stack.
    QPixmap backgroundSnapshot() const;

    /// @brief Rearms the stale-input guard while a swipe is active.
    void armSwipeIdleWatchdog();

    int swipeOffset() const { return swipe_offset_; }
    void setSwipeOffset(int dx);

    bool dragging_{false};                   ///< Drag in progress.
    bool swiping_{false};                    ///< Snapshot overlay active.
    bool other_is_next_{false};              ///< Swipe direction (true = next).
    bool retouch_during_swipe_{false};       ///< Resistive panel emitted a second press.
    int swipe_offset_{0};                    ///< Current overlay offset (px).
    int swipe_peak_extent_{0};               ///< Farthest distance seen in the locked direction.
    int swipe_idle_generation_{0};           ///< Cancels stale-input watchdog callbacks.
    int source_index_{-1};                   ///< Index hidden when the swipe began.
    int target_index_{-1};                   ///< Index the running animation lands on.
    QPoint start_pos_;                       ///< Press position (local).

    SwipeOverlay* overlay_;                  ///< Paints the two cached pixmaps.
    QPropertyAnimation* swipe_anim_;         ///< Animates swipeOffset.
    int animation_duration_{300};            ///< Transition duration (ms).
    std::function<QImage()> background_provider_; ///< Supplies shell wallpaper.
    QVector<QPixmap> snap_cache_;            ///< Per-page warm snapshots (avoids
                                             ///< re-rendering a hidden, heavy page
                                             ///< at gesture start on slow cores).
};

} // namespace cf::desktop::desktop_component
