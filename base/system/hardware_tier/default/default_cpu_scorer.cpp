/**
 * @file    default_cpu_scorer.cpp
 * @brief   Default CPU dimension scorer implementation.
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

class DefaultCpuScorer : public IHardwareScorer {
  public:
    int score(const HardwareData& data) override {
        int s = 0;

        // Core count (max 40 points)
        if (data.logical_cores >= 8)
            s += 40;
        else if (data.logical_cores >= 4)
            s += 25;
        else if (data.logical_cores >= 2)
            s += 15;
        else
            s += 5;

        // Frequency (max 35 points)
        if (data.max_frequency >= 2400)
            s += 35;
        else if (data.max_frequency >= 1800)
            s += 25;
        else if (data.max_frequency >= 1000)
            s += 15;
        else
            s += 5;

        // Architecture bonus (max 25 points)
        if (data.has_big_little && data.big_core_count >= 4)
            s += 25;
        else if (data.has_big_little)
            s += 15;
        else if (data.cpu_arch.find("aarch64") != std::string::npos ||
                 data.cpu_arch.find("x86_64") != std::string::npos)
            s += 20;
        else
            s += 5;

        return std::clamp(s, 0, 100);
    }
};

std::unique_ptr<IHardwareScorer> makeDefaultCpuScorer() {
    return std::make_unique<DefaultCpuScorer>();
}

} // namespace cf
