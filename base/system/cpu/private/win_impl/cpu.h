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

/**
 * @brief Internal Windows Query
 *
 * @param hostInfo
 * @return cf::expected<void, CPUInfoErrorType>
 */
cf::expected<void, CPUInfoErrorType> query_once_info(CPUInfoHost& hostInfo);
