/**
 * @file cpu.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief CPU At Windows Supports
 * @version 0.1
 * @date 2026-02-21
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once
#include "../cpu_host.h"
#include "base/expected/expected.hpp"
#include "system/cpu/cfcpu.h"
#include "system/cpu/cfcpu_profile.h"

/**
 * @brief Internal Windows Query
 *
 * @param hostInfo
 * @return cf::expected<void, CPUInfoErrorType>
 */
cf::expected<void, cf::CPUInfoErrorType> query_cpu_info(cf::CPUInfoHost& hostInfo);

cf::expected<cf::CPUProfileInfo, cf::CPUProfileInfoError> query_cpu_profile_info();
