/**
 * @file dimm_info.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief DIMM (Memory Module) Information Query (Linux)
 * @version 0.1
 * @date 2026-02-27
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once

#include "system/memory/memory_info.h"
#include <vector>

namespace cf {
namespace linux_impl {

/**
 * @brief Query DIMM (memory module) information.
 *
 * First attempts to use dmidecode -t memory command, then falls back to
 * reading /sys/class/dmi/id/ directory if dmidecode fails.
 *
 * @param[out] dimms Vector to store the retrieved DIMM information.
 *
 * @throws     None.
 *
 * @note       Requires root privileges for dmidecode. Falls back to /sys files
 *             if dmidecode is not available.
 *
 * @warning    dmidecode requires root privileges; /sys files may have limited info.
 *
 * @since      0.1
 * @ingroup    system_memory
 */
void queryDimmInfo(std::vector<DimmInfo>& dimms);

} // namespace linux_impl
} // namespace cf
