/**
 * @file    floating_policy.h
 * @brief   Picks the initial geometry for a freshly appeared floating window.
 *
 * WindowPlacementPolicy clamps an external window back into the work area when
 * it lands outside; FloatingPolicy decides where a new floating window should
 * appear in the first place — centered in the work area, shrunk to fit when
 * larger than the work area (aspect preserved), and nudged by a small cascade
 * offset per consecutive window so a burst of launches does not stack exactly
 * on top of each other. The cascade offset is clamped so the window never
 * escapes the work area.
 *
 * Like WindowPlacementPolicy, this policy is intentionally pure: it operates
 * only on QRect and QSize. There are zero platform branches and zero
 * preprocessor conditionals — moving the window is the backend's job, behind
 * IWindow::set_geometry. The math is exposed as static free functions so it is
 * unit-testable with no backend.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-07-05
 * @version 0.1
 * @since   0.21
 * @ingroup components
 */

#pragma once

#include <QPoint>
#include <QRect>
#include <QSize>

namespace cf::desktop::placement {

/**
 * @brief  Computes the initial geometry for a new floating window.
 *
 * Used on window appearance to give a freshly arrived window a sensible default
 * position: centered in the work area, shrunk to fit when too large, and nudged
 * by a per-window cascade offset so successive launches do not perfectly
 * overlap. The result is always kept fully inside @p work_area.
 *
 * @ingroup components
 */
class FloatingPolicy {
  public:
    /// @brief Default cascade step per window, in device pixels (both axes).
    static constexpr int kCascadeStep = 24;
    /// @brief Default desired window size when the caller omits it.
    static constexpr int kDefaultWidth = 800;
    static constexpr int kDefaultHeight = 600;
    /// @brief Default minimum window size (so a window stays usable).
    static constexpr int kDefaultMinWidth = 320;
    static constexpr int kDefaultMinHeight = 240;

    /**
     * @brief  Returns the cascade offset for the @p cascade_index-th window.
     *
     * Pure linear offset (index * kCascadeStep on both axes); never wraps on its
     * own. initialGeometry() clamps the offset against the work area so the
     * final window stays on-screen. Exposed so the offset math is testable
     * without a work area.
     *
     * @param[in] cascade_index  Zero-based consecutive-window count.
     *
     * @return The (dx, dy) offset.
     *
     * @throws None
     * @since  0.21
     */
    static QPoint cascadeOffset(int cascade_index);

    /**
     * @brief  Raises @p s to at least the given minimum (component-wise).
     *
     * @param[in] s      The requested size.
     * @param[in] min_w  Minimum width.
     * @param[in] min_h  Minimum height.
     *
     * @return @p s with width/height raised to @p min_w / @p min_h when smaller.
     *
     * @throws None
     * @since  0.21
     */
    static QSize clampToMinimum(const QSize& s, int min_w = kDefaultMinWidth,
                                int min_h = kDefaultMinHeight);

    /**
     * @brief  Computes the initial geometry for a new floating window.
     *
     * Steps: raise @p desired to the minimum size; shrink-to-fit @p work_area
     * (aspect preserved, never enlarged) via the same math as
     * WindowPlacementPolicy::centerInWorkArea; center in @p work_area; then add
     * cascadeOffset(@p cascade_index) and clamp so the result stays fully inside
     * @p work_area. An empty @p work_area yields a rect of the clamped size at
     * the origin.
     *
     * @param[in] work_area      The available work area to place within.
     * @param[in] cascade_index  Zero-based consecutive-window count (0 = centered).
     * @param[in] desired        The desired window size (default 800x600).
     *
     * @return The initial geometry, fully contained in @p work_area when valid.
     *
     * @throws None
     * @since  0.21
     */
    static QRect initialGeometry(const QRect& work_area, int cascade_index = 0,
                                 QSize desired = QSize(kDefaultWidth, kDefaultHeight));
};

} // namespace cf::desktop::placement
