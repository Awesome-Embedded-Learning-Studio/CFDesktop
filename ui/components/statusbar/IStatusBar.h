/**
 * @file    IStatusBar.h
 * @brief   Abstract status bar panel interface.
 *
 * IStatusBar extends IPanel to define the status bar contract: a panel
 * anchored to the top edge that renders the clock and system icons.
 * Concrete implementations (e.g. StatusBar) provide the widget and the
 * behavior behind these accessors.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-03-29
 * @version 0.2
 * @since   0.11
 * @ingroup components
 */

#pragma once

#include "components/IPanel.h"
namespace cf::desktop::desktop_component {

/**
 * @brief  Visual layout variant for a status bar.
 *
 * Controls how the clock and system icons are arranged.
 *
 * @ingroup components
 */
enum class StatusBarStyle {
    Centered, ///< Clock centered, minimal iconry.
    Split     ///< Clock and icons distributed to the side regions.
};

/**
 * @brief  Abstract status bar panel.
 *
 * Concrete implementations provide a status bar that attaches to a screen
 * edge and participates in the panel layout. The visibility accessors let
 * callers toggle the clock and the system-icon region independently.
 *
 * @ingroup components
 */
class IStatusBar : public IPanel {
  public:
    /**
     * @brief  Shows or hides the clock region.
     *
     * @param[in] visible  true to show the clock, false to hide it.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  N/A
     * @ingroup components
     */
    virtual void setTimeVisible(bool visible) = 0;

    /**
     * @brief  Reports whether the clock region is shown.
     *
     * @return true when the clock is visible, false otherwise.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  N/A
     * @ingroup components
     */
    virtual bool isTimeVisible() const = 0;

    /**
     * @brief  Shows or hides the system-icon region.
     *
     * @param[in] visible  true to show system icons, false to hide them.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  N/A
     * @ingroup components
     */
    virtual void setIconsVisible(bool visible) = 0;

    /**
     * @brief  Reports whether the system-icon region is shown.
     *
     * @return true when system icons are visible, false otherwise.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  N/A
     * @ingroup components
     */
    virtual bool isIconsVisible() const = 0;

    /**
     * @brief  Selects the clock/icon layout variant.
     *
     * @param[in] style  The layout style to apply.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  N/A
     * @ingroup components
     */
    virtual void setStyle(StatusBarStyle style) = 0;

    /**
     * @brief  Returns the active layout variant.
     *
     * @return The current status bar style.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  N/A
     * @ingroup components
     */
    virtual StatusBarStyle style() const = 0;
};
} // namespace cf::desktop::desktop_component
