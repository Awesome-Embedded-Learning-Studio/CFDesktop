/**
 * @file    default_collector.cpp
 * @brief   Default hardware data collector implementation.
 *
 * @author  Charliechen114514
 * @date    2026-05-23
 * @version 0.19
 * @since   0.19
 * @ingroup system_hardware_tier
 */

#include "../default_factories.h"

#include "system/cpu/cfcpu.h"
#include "system/cpu/cfcpu_bonus.h"
#include "system/cpu/cfcpu_profile.h"
#include "system/gpu/gpu.h"
#include "system/memory/memory_info.h"

namespace cf {

class DefaultCollector : public IHardwareCollector {
  public:
    HardwareData collect() override {
        HardwareData data;
        collectCpu(data);
        collectGpu(data);
        collectMemory(data);
        return data;
    }

  private:
    void collectCpu(HardwareData& data) {
        if (auto info = getCPUInfo(); info.has_value()) {
            data.cpu_model = std::string(info->model);
            data.cpu_arch = std::string(info->arch);
        }
        if (auto profile = getCPUProfileInfo(); profile.has_value()) {
            data.logical_cores = profile->logical_cnt;
            data.physical_cores = profile->physical_cnt;
            data.max_frequency = profile->max_frequency;
        }
        if (auto bonus = getCPUBonusInfo(); bonus.has_value()) {
            data.has_big_little = bonus->has_big_little;
            data.big_core_count = bonus->big_core_count;
        }
    }

    void collectGpu(HardwareData& data) {
        if (auto gd = getGpuDisplayInfo(); gd.has_value()) {
            data.gpu_name = gd->gpu.name;
            data.gpu_vendor_id = gd->gpu.vendorId;
            data.gpu_is_discrete = gd->gpu.isDiscrete;
            data.gpu_is_software = gd->gpu.hasSoftware;
            data.existing_gpu_score = gd->score.gpu;

            data.display_width = gd->display.width;
            data.display_height = gd->display.height;
            data.display_refresh = gd->display.refreshRate;
            data.display_dpi = gd->display.dpi;
            data.existing_display_score = gd->score.display;
        }
    }

    void collectMemory(HardwareData& data) {
        MemoryInfo mem;
        getSystemMemoryInfo(mem);
        data.total_physical_bytes = mem.physical.total_bytes;
        data.total_swap_bytes = mem.swap.total_bytes;
    }
};

std::unique_ptr<IHardwareCollector> makeDefaultCollector() {
    return std::make_unique<DefaultCollector>();
}

} // namespace cf
