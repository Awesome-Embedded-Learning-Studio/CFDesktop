/**
 * @file cfcpu_profile.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief CPU Profiles Info
 * @version 0.1
 * @date 2026-02-21
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once
#include "base/expected/expected.hpp"
#include "system/cpu/cfcpu_profile.h"
#include <cstdint>
#include <string_view>

namespace cf {
enum class CPUProfileInfoError { CPUProfileInfoGeneralError };

struct CPUProfileInfo {
    uint8_t logical_cnt;
    uint8_t physical_cnt;
    uint32_t current_frequecy;
    uint32_t max_frequency;
    float cpu_usage_percentage;
};

/**
 * @brief Interfaces will feedback the CPU ProfileInfo
 *
 * @return expected<CPUProfileInfo, CPUProfileInfoError>
 */
expected<CPUProfileInfo, CPUProfileInfoError> getCPUProfileInfo();

} // namespace cf
