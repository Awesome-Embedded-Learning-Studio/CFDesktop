/**
 * @file    hardware_tier_policy.h
 * @brief   Declares the IHardwarePolicy interface.
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
 * @brief  Interface for the policy stage.
 *
 * Maps an HardwareTierLevel to concrete capability flags that drive
 * downstream behavior (rendering backend, animations, etc.).
 *
 * @since   0.19
 * @ingroup system_hardware_tier
 */
class IHardwarePolicy {
  public:
    virtual ~IHardwarePolicy() = default;

    /**
     * @brief  Derives capability flags from the tier level.
     *
     * @param[in] level       The assessed tier level.
     * @param[in] assessment  The full assessment (for dimension-aware policy).
     * @return                HardwareTierCapabilities flags.
     */
    virtual HardwareTierCapabilities
    deriveCapabilities(HardwareTierLevel level, const HardwareTierAssessment& assessment) = 0;
};

} // namespace cf
