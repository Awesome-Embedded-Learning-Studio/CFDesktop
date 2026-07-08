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
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup home_page
 */

#pragma once

#include <QPoint>
#include <QPointer>
#include <QStackedWidget>

class QPropertyAnimation;
class QParallelAnimationGroup;
class QMouseEvent;

namespace cf::desktop::desktop_component {

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
    /**
     * @brief  Records the drag start position on left-button press.
     * @param[in]  event  Mouse press event.
     * @return     None
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    home_page
     */
    void mousePressEvent(QMouseEvent* event) override;

    /**
     * @brief  Tracks the drag, shifting the neighboring page in real time.
     * @param[in]  event  Mouse move event.
     * @return     None
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    home_page
     */
    void mouseMoveEvent(QMouseEvent* event) override;

    /**
     * @brief  Commits the switch past 1/4 width, otherwise bounces back.
     * @param[in]  event  Mouse release event.
     * @return     None
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    home_page
     */
    void mouseReleaseEvent(QMouseEvent* event) override;

  private:
    /// @brief Animated transition with explicit direction.
    void slideToPage(int new_index, bool to_left);

    /// @brief Animates the current page back to its rest position.
    void bounceBack(int dx, int index);

    bool dragging_{false};                   ///< Drag in progress.
    QPoint start_pos_;                       ///< Press position (local).
    QPropertyAnimation* current_animation_;  ///< Animation for outgoing page.
    QPropertyAnimation* next_animation_;     ///< Animation for incoming page.
    QParallelAnimationGroup* group_;         ///< Parallel runner.
    QPointer<QWidget> hide_after_animation_; ///< Page to hide once finished.
    int animation_duration_{300};            ///< Transition duration (ms).
};

} // namespace cf::desktop::desktop_component
