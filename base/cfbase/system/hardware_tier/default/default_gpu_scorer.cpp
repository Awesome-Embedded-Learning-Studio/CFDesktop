/**
 * @file    default_gpu_scorer.cpp
 * @brief   Default GPU dimension scorer implementation.
 *
 * Adapts the existing EnvironmentScore.gpu (0-50) to 0-100 range.
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

class DefaultGpuScorer : public IHardwareScorer {
  public:
    int score(const HardwareData& data) override {
        if (data.gpu_is_software)
            return 5;
        if (data.gpu_vendor_id == 0)
            return 5;

        // Normalize existing 0-50 score to 0-100
        int base = data.existing_gpu_score * 2;
        return std::min(base, 100);
    }
};

std::unique_ptr<IHardwareScorer> makeDefaultGpuScorer() {
    return std::make_unique<DefaultGpuScorer>();
}

} // namespace cf
