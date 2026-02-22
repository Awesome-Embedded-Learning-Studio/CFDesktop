/**
 * @file cpu_info.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief CPU Basic Information Query (Model, Architecture, Manufacturer)
 * @version 0.1
 * @date 2026-02-22
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once

#include "../cpu_host.h"
#include "base/expected/expected.hpp"
#include "system/cpu/cfcpu.h"

/**
 * @brief Internal Windows Query for basic CPU information
 *
 * @param hostInfo Output parameter containing model, manufacturer, and architecture
 * @return cf::expected<void, cf::CPUInfoErrorType>
 */
cf::expected<void, cf::CPUInfoErrorType> query_cpu_basic_info(cf::CPUInfoHost& hostInfo);
