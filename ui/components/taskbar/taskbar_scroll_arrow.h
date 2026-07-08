/**
 * @file    taskbar_scroll_arrow.h
 * @brief   "‹" / "›" button that scrolls the taskbar icon row by one stride.
 *
 * TaskbarScrollArrow flanks the scrollable icon row when not every TaskbarIcon
 * fits. A click emits clicked(), which CenteredTaskbar turns into a one-icon
 * scroll. It draws only a chevron glyph (no filled background, so it reads as a
 * light indicator beside the tiles), brightens on hover, and dims when disabled
 * (already at the scroll end).
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-07-08
 * @version 0.1
 * @since   0.20
 * @ingroup components
 */

#pragma once

#include <QColor>
#include <QWidget>

class QEnterEvent;
class QEvent;
class QMouseEvent;
class QPaintEvent;

namespace cf::desktop::desktop_component {

/// @brief Which way the arrow points (and thus scrolls).
enum class ScrollDirection {
    Left,  ///< "‹" — scroll toward the row's start.
    Right, ///< "›" — scroll toward the row's end.
};

/**
 * @brief  One scroll-arrow button at the edge of a crowded taskbar.
 *
 * Paints a bare chevron glyph (no filled surface), brightens it on hover, dims
 * when disabled (scroll end reached), and emits clicked() on a left-button
 * release inside the tile.
 *
 * @ingroup components
 */
class TaskbarScrollArrow final : public QWidget {
    Q_OBJECT
  public:
    /**
     * @brief  Constructs the arrow.
     *
     * @param[in] direction  Left ("‹") or Right ("›").
     * @param[in] parent     Owning widget (the taskbar).
     *
     * @throws None
     * @note   Resolves theme colors and starts un-hovered.
     * @warning None
     * @since  0.20
     * @ingroup components
     */
    explicit TaskbarScrollArrow(ScrollDirection direction, QWidget* parent = nullptr);

    /**
     * @brief  Destructs the arrow.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.20
     * @ingroup components
     */
    ~TaskbarScrollArrow() override;

    /**
     * @brief  Returns the preferred (fixed) size.
     *
     * @return A fixed size matching the taskbar tile height.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.20
     * @ingroup components
     */
    QSize sizeHint() const override;

  signals:
    /**
     * @brief  Emitted when the arrow is clicked (left-button release inside).
     *
     * @since 0.20
     * @ingroup components
     */
    void clicked();

  protected:
    /**
     * @brief  Paints the chevron; opacity encodes hover / disabled state.
     *
     * @param[in] event  The paint event descriptor.
     *
     * @throws None
     * @note   Theme color is resolved in applyTheme(); no filled background.
     * @warning None
     * @since  0.20
     * @ingroup components
     */
    void paintEvent(QPaintEvent* event) override;

    /**
     * @brief  Arms the hover state-layer on mouse enter.
     *
     * @param[in] event  The enter event descriptor.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.20
     * @ingroup components
     */
    void enterEvent(QEnterEvent* event) override;

    /**
     * @brief  Clears the hover state-layer on mouse leave.
     *
     * @param[in] event  The leave event descriptor.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.20
     * @ingroup components
     */
    void leaveEvent(QEvent* event) override;

    /**
     * @brief  Accepts the press so the matching release is delivered here.
     *
     * @param[in] event  The mouse press event descriptor.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.20
     * @ingroup components
     */
    void mousePressEvent(QMouseEvent* event) override;

    /**
     * @brief  Emits clicked() when released inside an enabled arrow.
     *
     * @param[in] event  The mouse release event descriptor.
     *
     * @throws None
     * @note   A disabled arrow does not emit.
     * @warning None
     * @since  0.20
     * @ingroup components
     */
    void mouseReleaseEvent(QMouseEvent* event) override;

  private:
    /// @brief Resolves theme colors, then repaints.
    void applyTheme();

    ScrollDirection direction_; ///< Which chevron to draw.
    QColor foreground_color_;   ///< Chevron color (on surface).
    bool hovered_{false};       ///< Whether the hover brighten shows.
};

} // namespace cf::desktop::desktop_component
