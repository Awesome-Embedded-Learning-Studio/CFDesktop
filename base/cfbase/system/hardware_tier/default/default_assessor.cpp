/**
 * @file    default_assessor.cpp
 * @brief   Default assessor implementation.
 *
 * Uses a bottleneck-weighted approach: the lowest dimension caps the
 * tier while the average provides a baseline.
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

class DefaultAssessor : public IHardwareAssessor {
  public:
    HardwareTierLevel assess(int cpu, int gpu, int memory, int display) override {
        int minScore = std::min({cpu, gpu, memory, display});
        int avg = (cpu + gpu + memory + display) / 4;

        // 60% bottleneck weight, 40% average
        int effective = static_cast<int>(minScore * 0.6 + avg * 0.4);

        if (effective >= 65)
            return HardwareTierLevel::High;
        if (effective >= 30)
            return HardwareTierLevel::Mid;
        return HardwareTierLevel::Low;
    }
};

std::unique_ptr<IHardwareAssessor> makeDefaultAssessor() {
    return std::make_unique<DefaultAssessor>();
}

} // namespace cf
