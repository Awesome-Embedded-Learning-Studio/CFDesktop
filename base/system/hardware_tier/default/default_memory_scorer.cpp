/**
 * @file    default_memory_scorer.cpp
 * @brief   Default memory dimension scorer implementation.
 *
 * @author  Charliechen114514
 * @date    2026-05-23
 * @version 0.19
 * @since   0.19
 * @ingroup system_hardware_tier
 */

#include "../default_factories.h"

#include <cstdint>

namespace cf {

class DefaultMemoryScorer : public IHardwareScorer {
  public:
    int score(const HardwareData& data) override {
        auto gb = static_cast<uint64_t>(data.total_physical_bytes / (1024ULL * 1024 * 1024));
        if (gb >= 16)
            return 100;
        if (gb >= 8)
            return 75;
        if (gb >= 4)
            return 55;
        if (gb >= 2)
            return 35;
        if (gb >= 1)
            return 20;
        return 5;
    }
};

std::unique_ptr<IHardwareScorer> makeDefaultMemoryScorer() {
    return std::make_unique<DefaultMemoryScorer>();
}

} // namespace cf
