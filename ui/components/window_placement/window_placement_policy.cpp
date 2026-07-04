/**
 * @file    window_placement_policy.cpp
 * @brief   WindowPlacementPolicy implementation.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-26
 * @version 0.2
 * @since   0.20
 * @ingroup components
 */

#include "window_placement_policy.h"

#include "IWindow.h"

#include <algorithm>
#include <cmath>

namespace cf::desktop::placement {

QRect WindowPlacementPolicy::centerInWorkArea(const QRect& rect, const QRect& work_area) {
    if (work_area.isEmpty() || rect.isEmpty()) {
        return rect;
    }
    // Proportional shrink-to-fit: take the smaller axis scale so aspect is
    // preserved; never enlarge (scale clamped to 1).
    const qreal scale_x = qreal(work_area.width()) / qreal(rect.width());
    const qreal scale_y = qreal(work_area.height()) / qreal(rect.height());
    qreal scale = std::min(scale_x, scale_y);
    if (scale > 1.0) {
        scale = 1.0;
    }
    const int w = std::max(1, static_cast<int>(std::round(rect.width() * scale)));
    const int h = std::max(1, static_cast<int>(std::round(rect.height() * scale)));
    const int x = work_area.x() + (work_area.width() - w) / 2;
    const int y = work_area.y() + (work_area.height() - h) / 2;
    return {x, y, w, h};
}

std::optional<QRect> WindowPlacementPolicy::computeConstrain(const QRect& current,
                                                             const QRect& work_area, bool enabled) {
    if (!enabled || work_area.isEmpty() || current.isEmpty()) {
        return std::nullopt;
    }
    // Leave windows already fully inside the work area where they are.
    const bool fully_inside = current.x() >= work_area.x() && current.y() >= work_area.y() &&
                              current.right() <= work_area.right() &&
                              current.bottom() <= work_area.bottom();
    if (fully_inside) {
        return std::nullopt;
    }
    return centerInWorkArea(current, work_area);
}

void WindowPlacementPolicy::constrain(IWindow& window, const QRect& work_area, bool enabled) const {
    const auto target = computeConstrain(window.geometry(), work_area, enabled);
    if (target.has_value()) {
        window.set_geometry(*target);
    }
}

} // namespace cf::desktop::placement
