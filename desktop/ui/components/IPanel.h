/**
 * @file    IPanel.h
 * @brief   Abstract interface for edge-occupying desktop panels.
 *
 * IPanel defines the contract for panels that attach to screen edges
 * (status bar, dock, sidebar, etc.) and participate in the layout
 * engine managed by PanelManager.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-03-29
 * @version 0.1
 * @since   0.11
 * @ingroup components
 */

#pragma once

class QWidget;

namespace cf::desktop {

/**
 * @brief  Screen edge where a panel is anchored.
 *
 * @ingroup components
 */
enum class PanelPosition {
    Top,    ///< Anchored to the top edge.
    Bottom, ///< Anchored to the bottom edge.
    Left,   ///< Anchored to the left edge.
    Right   ///< Anchored to the right edge.
};

/**
 * @brief  Abstract interface for edge-occupying panels.
 *
 * Panels (status bar, dock, sidebar) implement this interface so
 * the PanelManager can lay them out along screen edges.
 *
 * @ingroup components
 */
class IPanel {
  public:
    virtual ~IPanel() = default;

    /**
     * @brief  Returns the screen edge this panel occupies.
     *
     * @return The panel's anchoring position.
     */
    virtual PanelPosition position() const = 0;

    /**
     * @brief  Returns the layout priority of this panel.
     *
     * Higher values place the panel closer to the screen edge
     * (outermost layer).
     *
     * @return Priority value (higher is outer).
     */
    virtual int priority() const = 0;

    /**
     * @brief  Returns the desired panel thickness in pixels.
     *
     * A return value of 0 signals the panel opts out of layout.
     *
     * @return Preferred size in device pixels.
     */
    virtual int preferredSize() const = 0;

    /**
     * @brief  Returns the QWidget positioned by the layout engine.
     *
     * @return The panel's widget, or nullptr if not yet created.
     */
    virtual QWidget* widget() const = 0;
};

} // namespace cf::desktop
