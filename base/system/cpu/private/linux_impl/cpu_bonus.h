/**
 * @file cpu_bonus.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief
 * @version 0.1
 * @date 2026-02-22
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once

#include "../cpu_host.h"
#include "base/expected/expected.hpp"
#include "system/cpu/cfcpu_bonus.h"

cf::expected<void, cf::CPUBonusInfoViewError> query_cpu_bonus_info(cf::CPUBonusInfoHost& bonus);
