/**
 * @file    IStatusBar.h
 * @brief   Abstract status bar panel interface.
 *
 * IStatusBar extends IPanel to define the status bar contract.
 * Concrete implementations provide the actual status bar widget
 * and content.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-03-29
 * @version 0.1
 * @since   0.11
 * @ingroup components
 */

#pragma once

#include "components/IPanel.h"
namespace cf::desktop::desktop_component {

/**
 * @brief  Abstract status bar panel.
 *
 * Concrete implementations provide a status bar that attaches
 * to a screen edge and participates in the panel layout.
 *
 * @ingroup components
 */
class IStatusBar : public IPanel {
  public:
};
} // namespace cf::desktop::desktop_component
