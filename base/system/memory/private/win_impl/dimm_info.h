/**
 * @file dimm_info.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief DIMM (Memory Module) Information Query via SMBIOS (Windows)
 * @version 0.1
 * @date 2026-02-23
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once

#include "system/memory/memory_info.h"
#include <vector>

namespace cf {
namespace win_impl {

/**
 * @brief Query DIMM (memory module) information using GetSystemFirmwareTable (SMBIOS).
 *
 * @param dimms Output vector to be populated with DIMM information
 */
void queryDimmInfo(std::vector<DimmInfo>& dimms);

} // namespace win_impl
} // namespace cf
