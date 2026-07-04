/**
 * @file    default_display_scorer.cpp
 * @brief   Default display dimension scorer implementation.
 *
 * Adapts the existing EnvironmentScore.display (0-50) to 0-100 range.
 *
 * @author  Charliechen114514
 * @date    2026-05-23
 * @version 0.19
 * @since   0.19
 * @ingroup system_hardware_tier
 */

#include "../default_factories.h"

#include <algorithm>

namespace cf {

class DefaultDisplayScorer : public IHardwareScorer {
  public:
    int score(const HardwareData& data) override {
        if (data.existing_display_score > 0) {
            return std::min(data.existing_display_score * 2, 100);
        }

        // Fallback: compute from raw display data
        auto pixels = static_cast<long long>(data.display_width) * data.display_height;
        int s = 0;

        // Resolution (max 50 points)
        if (pixels >= 3840LL * 2160)
            s += 50;
        else if (pixels >= 2560LL * 1440)
            s += 40;
        else if (pixels >= 1920LL * 1080)
            s += 30;
        else if (pixels >= 1280LL * 720)
            s += 15;
        else
            s += 5;

        // Refresh rate (max 30 points)
        if (data.display_refresh >= 120)
            s += 30;
        else if (data.display_refresh >= 90)
            s += 25;
        else if (data.display_refresh >= 60)
            s += 20;
        else
            s += 5;

        // DPI (max 20 points)
        if (data.display_dpi >= 200)
            s += 20;
        else if (data.display_dpi >= 150)
            s += 15;
        else if (data.display_dpi >= 96)
            s += 10;
        else
            s += 3;

        return std::min(s, 100);
    }
};

std::unique_ptr<IHardwareScorer> makeDefaultDisplayScorer() {
    return std::make_unique<DefaultDisplayScorer>();
}

} // namespace cf
