/**
 * @file cpu_profile.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief CPU Profile Information Query (Core count, frequency, usage)
 * @version 0.1
 * @date 2026-02-22
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once

#include "base/expected/expected.hpp"
#include "system/cpu/cfcpu_profile.h"

/**
 * @brief Internal Windows Query for CPU profile information
 *
 * @return cf::expected<cf::CPUProfileInfo, cf::CPUProfileInfoError>
 */
cf::expected<cf::CPUProfileInfo, cf::CPUProfileInfoError> query_cpu_profile_info();