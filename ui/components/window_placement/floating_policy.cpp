/**
 * @file    floating_policy.cpp
 * @brief   FloatingPolicy implementation.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-07-05
 * @version 0.1
 * @since   0.21
 * @ingroup components
 */

#include "floating_policy.h"

#include "window_placement_policy.h"

#include <algorithm>

namespace cf::desktop::placement {

QPoint FloatingPolicy::cascadeOffset(int cascade_index) {
    if (cascade_index <= 0) {
        return {};
    }
    return {cascade_index * kCascadeStep, cascade_index * kCascadeStep};
}

QSize FloatingPolicy::clampToMinimum(const QSize& s, int min_w, int min_h) {
    return QSize(std::max(s.width(), min_w), std::max(s.height(), min_h));
}

QRect FloatingPolicy::initialGeometry(const QRect& work_area, int cascade_index, QSize desired) {
    // Step 1: never ship a window smaller than the usable minimum.
    desired = clampToMinimum(desired);

    // Step 2: shrink-to-fit the work area (aspect preserved) and center.
    // WindowPlacementPolicy::centerInWorkArea does exactly this on a
    // position-less rect: it scales @p rect down to fit @p work_area and
    // centers the result.
    const QRect centered = WindowPlacementPolicy::centerInWorkArea(
        QRect(0, 0, desired.width(), desired.height()), work_area);
    if (work_area.isEmpty()) {
        // No work area to place within: hand back the clamped size at origin.
        return centered;
    }

    // Step 3: nudge by the cascade offset.
    const QPoint off = cascadeOffset(cascade_index);
    int x = centered.x() + off.x();
    int y = centered.y() + off.y();

    // Step 4: clamp so the cascaded window stays fully inside the work area.
    const int max_x = work_area.right() - centered.width() + 1;
    const int max_y = work_area.bottom() - centered.height() + 1;
    x = std::min(x, max_x);
    y = std::min(y, max_y);
    x = std::max(x, work_area.x());
    y = std::max(y, work_area.y());

    return {x, y, centered.width(), centered.height()};
}

} // namespace cf::desktop::placement
