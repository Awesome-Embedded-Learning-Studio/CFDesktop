/**
 * @file    hardware_tier_scorer.h
 * @brief   Declares the IHardwareScorer interface.
 *
 * @author  Charliechen114514
 * @date    2026-05-23
 * @version 0.19
 * @since   0.19
 * @ingroup system_hardware_tier
 */
#pragma once

#include "system/hardware_tier/hardware_tier_data.h"

namespace cf {

/**
 * @brief  Interface for the per-dimension scoring stage.
 *
 * Each dimension (cpu, gpu, memory, display) gets its own scorer
 * registered via registerScorer(). Scorers map raw collected data
 * to a 0-100 score.
 *
 * @since   0.19
 * @ingroup system_hardware_tier
 */
class IHardwareScorer {
  public:
    virtual ~IHardwareScorer() = default;

    /**
     * @brief  Scores a specific dimension from the collected data.
     *
     * @param[in] data  The collected hardware data.
     * @return          Integer score in the range [0, 100].
     */
    virtual int score(const HardwareData& data) = 0;
};

} // namespace cf
