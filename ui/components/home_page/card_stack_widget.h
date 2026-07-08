/**
 * @file    card_stack_widget.h
 * @brief   QStackedWidget with vertical swipe + slide/fade card transitions.
 *
 * Migrated from CCIMXDesktop CardStackWidget. A finger/mouse drag tracks the
 * neighboring card in real time; on release, a drag past 1/4 of the height
 * commits the switch (animated), otherwise the current card bounces back.
 * Droped from the original: the auto-switch timer, the bounded-property
 * macros, and the QMutex (the widget is single-threaded UI).
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
class QGraphicsOpacityEffect;
class QMouseEvent;

namespace cf::desktop::desktop_component {

/**
 * @brief  Stacked widget with swipe-to-switch and slide/fade transitions.
 *
 * @note   Single-threaded (UI thread only).
 * @warning None
 * @since  0.19.0
 * @ingroup home_page
 */
class CardStackWidget : public QStackedWidget {
    Q_OBJECT

  public:
    /**
     * @brief  Transition animation style between cards.
     * @since  0.19.0
     * @ingroup home_page
     */
    enum class CardTransitionMode {
        Slide, ///< Slide the cards vertically.
        Fade,  ///< Cross-fade opacity.
    };

    /**
     * @brief  Constructs the card stack.
     * @param[in]  parent  Owning Qt parent widget.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    home_page
     */
    explicit CardStackWidget(QWidget* parent = nullptr);

    /**
     * @brief  Sets the transition animation mode.
     * @param[in]  mode  Slide or Fade.
     * @return     None
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    home_page
     */
    void setCardTransitionMode(CardTransitionMode mode);

    /**
     * @brief  Returns the current transition mode.
     * @return     Slide or Fade.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    home_page
     */
    CardTransitionMode cardTransitionMode() const;

    /**
     * @brief  Animates a transition to the given card index.
     * @param[in]  new_index  Target card index.
     * @return     None
     * @throws     None
     * @note       Direction is inferred from the current index.
     * @warning    None
     * @since      0.19.0
     * @ingroup    home_page
     */
    void slideTo(int new_index);

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
     * @brief  Tracks the drag, shifting the neighboring card in real time.
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
     * @brief  Commits the switch past 1/4 height, otherwise bounces back.
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
    void slideTo(int new_index, bool slide_up);

    /// @brief Animates the current card back to its rest position.
    void bounceBack(int dy, int index);

    /// @brief Lazily attaches an opacity effect to a widget and returns it.
    QGraphicsOpacityEffect* ensureOpacity(QWidget* w);

    bool dragging_{false};                                         ///< Drag in progress.
    QPoint start_pos_;                                             ///< Press position (local).
    QPropertyAnimation* current_animation_;                        ///< Animation for outgoing card.
    QPropertyAnimation* next_animation_;                           ///< Animation for incoming card.
    QParallelAnimationGroup* group_;                               ///< Parallel runner.
    QPointer<QWidget> hide_after_animation_;                       ///< Card to hide once finished.
    int animation_duration_{350};                                  ///< Transition duration (ms).
    CardTransitionMode transition_mode_{CardTransitionMode::Fade}; ///< Active mode.
};

} // namespace cf::desktop::desktop_component
