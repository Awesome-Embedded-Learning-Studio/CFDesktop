/**
 * @file    hardware_tier_assessor.h
 * @brief   Declares the IHardwareAssessor interface.
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
 * @brief  Interface for the assessment stage.
 *
 * Maps multi-dimensional scores to an overall HardwareTierLevel.
 *
 * @since   0.19
 * @ingroup system_hardware_tier
 */
class IHardwareAssessor {
  public:
    virtual ~IHardwareAssessor() = default;

    /**
     * @brief  Determines the overall tier level from per-dimension scores.
     *
     * @param[in] cpu     CPU dimension score.
     * @param[in] gpu     GPU dimension score.
     * @param[in] memory  Memory dimension score.
     * @param[in] display Display dimension score.
     * @return            HardwareTierLevel classification.
     */
    virtual HardwareTierLevel assess(int cpu, int gpu, int memory, int display) = 0;
};

} // namespace cf
