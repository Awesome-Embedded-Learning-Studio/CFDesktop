/**
 * @file    window_placement_policy.h
 * @brief   Places external windows centered inside the desktop work area.
 *
 * Real desktop window managers place application windows inside the work area
 * (the rect between the top status bar and the bottom taskbar) so they never
 * overlap a bar or fly off-screen. WindowPlacementPolicy gives CFDesktop the
 * same behavior from the client side: when an external window appears outside
 * the work area — or falls outside after the desktop is resized — it is moved
 * to the center of the work area, proportionally shrunk to fit when it is
 * larger than the work area (aspect preserved). Windows already fully inside
 * are left where the app/user put them.
 *
 * The policy is intentionally pure: it operates only through IWindow (read
 * geometry / set geometry) and QRect. There are zero platform branches and zero
 * preprocessor conditionals — moving the window is the backend's job, already
 * implemented per platform behind IWindow::set_geometry. The placement math is
 * exposed as a free static function so it is unit-testable with no backend.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-26
 * @version 0.2
 * @since   0.20
 * @ingroup components
 */

#pragma once

#include <QRect>
#include <optional>

namespace cf::desktop {
class IWindow;
}

namespace cf::desktop::placement {

/**
 * @brief  Places external windows centered in the desktop work area.
 *
 * Used on window appearance and when the work area changes: any external window
 * found outside the work area is re-centered into it (shrunk to fit when too
 * large), the way a real desktop WM keeps windows inside the work area.
 *
 * @ingroup components
 */
class WindowPlacementPolicy {
  public:
    /**
     * @brief  Centers @p rect in @p work_area, shrinking to fit (pure math).
     *
     * If @p rect is larger than @p work_area in either axis it is proportionally
     * shrunk (aspect ratio preserved) until it fits; it is never enlarged. The
     * result is then centered in @p work_area. An empty @p work_area is treated
     * as "no constraint" and @p rect is returned unchanged. The input rect's
     * position is ignored — only its size participates.
     *
     * Exposed as a static free function so the placement math is testable
     * without any IWindow or backend.
     *
     * @param[in] rect       The window's current geometry (only size matters).
     * @param[in] work_area  The work area to center within.
     *
     * @return The centered, fit-to-work-area rectangle.
     *
     * @throws None
     * @since  0.20
     */
    static QRect centerInWorkArea(const QRect& rect, const QRect& work_area);

    /**
     * @brief  Decides the geometry to apply to a window, if any (pure).
     *
     * Encapsulates the full placement decision without touching IWindow, so it
     * is unit-testable with no backend. Returns the centered target rectangle
     * (see centerInWorkArea()) when @p enabled is true, @p work_area is
     * non-empty, @p current is non-empty, and @p current is NOT fully contained
     * in @p work_area (i.e. it overlaps a bar or sits off-screen). Returns
     * std::nullopt otherwise — in particular windows already fully inside the
     * work area are left where they are (no yank).
     *
     * @param[in] current   The window's current geometry.
     * @param[in] work_area The work area to place within.
     * @param[in] enabled   Whether placement is active (runtime toggle).
     *
     * @return The geometry to set, or std::nullopt when no move is needed.
     *
     * @throws None
     * @since  0.20
     */
    static std::optional<QRect> computeConstrain(const QRect& current, const QRect& work_area,
                                                 bool enabled);

    /**
     * @brief  Centers @p window into @p work_area when @p enabled and outside.
     *
     * Reads the window's current geometry; if computeConstrain() yields a target
     * that differs from it, applies the result with IWindow::set_geometry. No-op
     * when disabled, when the work area is empty, or when the window already
     * sits fully inside the work area.
     *
     * Callers invoke this on window appearance and on work-area change, so the
     * policy re-centers windows that fall outside after the desktop is resized
     * without disturbing windows the user already placed inside.
     *
     * @param[in,out] window    The window to place.
     * @param[in]     work_area The work area to place within.
     * @param[in]     enabled   Whether placement is active (runtime toggle).
     *
     * @throws None
     * @note   No-op (no set_geometry call) when the window already fits inside.
     * @since  0.20
     */
    void constrain(IWindow& window, const QRect& work_area, bool enabled) const;
};

} // namespace cf::desktop::placement
