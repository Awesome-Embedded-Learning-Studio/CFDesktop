/**
 * @file gpu_info.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief GPU and Display Information Query for Linux
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
 * @brief Internal Linux Query for GPU information
 *
 * Queries GPU information using DRM and sysfs on Linux platforms.
 * Supports both desktop Linux (with DRM) and embedded Linux (using device tree).
 *
 * @return GPUInfoHost containing the detected GPU information
 */
GPUInfoHost query_gpu_info_linux();

/**
 * @brief Internal Linux Query for display information
 *
 * Queries display information using sysfs on Linux platforms.
 *
 * @return DisplayInfoHost containing the detected display information
 */
DisplayInfoHost query_display_info_linux();

} // namespace cf
