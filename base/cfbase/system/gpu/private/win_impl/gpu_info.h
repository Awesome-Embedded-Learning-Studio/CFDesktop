/**
 * @file gpu_info.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief GPU and Display Information Query for Windows
 * @version 0.1
 * @date 2026-03-21
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once

#include "../gpu_host.h"

namespace cf {

/**
 * @brief Internal Windows Query for GPU information
 *
 * Queries GPU information using DXGI on Windows platforms.
 *
 * @return GPUInfoHost containing the detected GPU information
 */
GPUInfoHost query_gpu_info_win();

/**
 * @brief Internal Windows Query for display information
 *
 * Queries display information using Windows APIs on Windows platforms.
 *
 * @return DisplayInfoHost containing the detected display information
 */
DisplayInfoHost query_display_info_win();

} // namespace cf
