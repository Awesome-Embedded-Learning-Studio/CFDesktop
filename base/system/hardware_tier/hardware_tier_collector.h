/**
 * @file    hardware_tier_collector.h
 * @brief   Declares the IHardwareCollector interface.
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
 * @brief  Interface for the hardware data collection stage.
 *
 * Implementations gather raw hardware data from platform probes.
 * The default implementation calls the existing cf::getCPUInfo(),
 * cf::getGpuDisplayInfo(), cf::getSystemMemoryInfo() probes.
 *
 * @since   0.19
 * @ingroup system_hardware_tier
 */
class IHardwareCollector {
  public:
    virtual ~IHardwareCollector() = default;

    /**
     * @brief  Collects raw hardware data from all subsystems.
     *
     * @return HardwareData populated with probe results.
     *         Fields that could not be collected are left at defaults.
     */
    virtual HardwareData collect() = 0;
};

} // namespace cf
