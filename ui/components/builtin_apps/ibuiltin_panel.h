/**
 * @file    ibuiltin_panel.h
 * @brief   Interface for in-process builtin application panels.
 *
 * IBuiltinPanel is the contract for apps that render inside the desktop
 * process (as child widgets over the desktop surface) rather than as a
 * detached QProcess. This suits memory-constrained targets (i.MX6ULL) where
 * each extra Qt process costs ~30-50MB, and single-framebuffer linuxfb where
 * an external GUI app would fight the desktop for /dev/fb0.
 *
 * Panels are registered with BuiltinPanelRegistry and surfaced to the launcher
 * grid by app_id; launching one resolves the id to its panel instance and
 * calls popup(). Ownership stays with the desktop (panels are Qt children of
 * the desktop surface); the registry only holds non-owning pointers.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-07-01
 * @version 0.1
 * @since   0.20
 * @ingroup components
 */

#pragma once

#include <QString>

class QRect;

namespace cf::desktop::desktop_component {

/**
 * @brief  Contract for an in-process builtin application panel.
 *
 * Concrete panels (AboutPanel, CalculatorPanel as builtin, ...) implement
 * this interface so the launcher can show them by app_id without QProcess.
 *
 * @ingroup components
 */
class IBuiltinPanel {
  public:
    virtual ~IBuiltinPanel() = default;

    /**
     * @brief   Returns the stable app identifier matching the launcher grid.
     *
     * @return  The app_id (e.g. "about", "calculator").
     *
     * @throws  None
     * @since   0.20
     * @ingroup components
     */
    virtual QString appId() const = 0;

    /**
     * @brief   Returns the human-readable name shown under the icon.
     *
     * @return  The display name.
     *
     * @throws  None
     * @since   0.20
     * @ingroup components
     */
    virtual QString displayName() const = 0;

    /**
     * @brief          Sizes/centers the panel over the area and shows it.
     *
     * @param[in] available  Free screen geometry (excludes docked panels).
     *
     * @throws         None
     * @note           A null/empty rect centers on the primary screen.
     * @since          0.20
     * @ingroup        components
     */
    virtual void popup(const QRect& available) = 0;

    /**
     * @brief   Hides the panel.
     *
     * @throws  None
     * @since   0.20
     * @ingroup components
     */
    virtual void hidePanel() = 0;
};

} // namespace cf::desktop::desktop_component
