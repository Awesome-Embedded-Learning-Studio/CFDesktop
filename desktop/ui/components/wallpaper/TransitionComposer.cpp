/**
 * @file    desktop/ui/components/wallpaper/TransitionComposer.cpp
 * @brief   Implementation of wallpaper transition frame compositing.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-30
 * @version 0.1
 * @since   0.19
 * @ingroup wallpaper
 */

#include "TransitionComposer.h"

#include <QPainter>
#include <algorithm>
#include <cmath>

namespace cf::desktop::wallpaper {

QImage composeTransitionFrame(const QImage& prev, const QImage& cur, qreal t, SwitchingMode mode,
                              const QSize& target) {
    if (target.isEmpty() || cur.isNull()) {
        return {};
    }
    const qreal progress = std::clamp(t, 0.0, 1.0);

    QImage frame(target, QImage::Format_RGB32);
    frame.fill(Qt::black);

    QPainter painter(&frame);
    switch (mode) {
        case SwitchingMode::Fixed: {
            // Defensive: the engine never requests a transition in Fixed mode.
            painter.drawImage(0, 0, cur);
            break;
        }
        case SwitchingMode::Gradient: {
            // Current fully visible, previous fades out on top as t -> 1.
            painter.drawImage(0, 0, cur);
            if (!prev.isNull()) {
                painter.setOpacity(1.0 - progress);
                painter.drawImage(0, 0, prev);
            }
            break;
        }
        case SwitchingMode::Movement: {
            // Old exits left, new enters from right (matches CCIMXDesktop).
            const double width = static_cast<double>(target.width());
            const int x_prev = static_cast<int>(std::round(std::lerp(0.0, -width, progress)));
            const int x_cur = static_cast<int>(std::round(std::lerp(width, 0.0, progress)));
            if (!prev.isNull()) {
                painter.setOpacity(1.0);
                painter.drawImage(x_prev, 0, prev);
            }
            painter.drawImage(x_cur, 0, cur);
            break;
        }
    }
    return frame;
}

} // namespace cf::desktop::wallpaper
