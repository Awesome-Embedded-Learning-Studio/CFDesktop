/**
 * @file    base/system/gpu/private/gpu_host.h
 * @brief   Internal data structures for GPU information storage.
 *
 * Defines host-side storage structures used by platform-specific
 * implementations. These structures own the underlying data that
 * is exposed in the public API.
 *
 * @warning  This is an internal header. Do not include directly.
 *
 * @author  Charliechen114514
 * @date    2026-03-21
 * @version 0.1
 * @since   0.1
 * @ingroup system_gpu_internal
 */
#pragma once
#include <cstdint>
#include <string>

namespace cf {

/**
 * @brief  Internal storage for GPU information.
 *
 * Owns the string data for GPU properties. Used by platform-specific
 * implementations to store query results.
 *
 * @ingroup system_gpu_internal
 */
struct GPUInfoHost {
    std::string name;          ///< GPU name. Ownership: owner.
    std::string backend;       ///< Backend type (DXGI, DRM, DeviceTree, None). Ownership: owner.
    std::string driverVersion; ///< Driver version string. Ownership: owner.
    uint32_t vendorId = 0;     ///< PCI vendor ID.
    uint32_t deviceId = 0;     ///< PCI device ID.
    bool isDiscrete = false;   ///< True if discrete GPU.
    bool isIntegrated = false; ///< True if integrated GPU.
    bool hasSoftware = false;  ///< True if software rendering only.
    bool isWSL = false;        ///< True if running in WSL2 environment.
};

/**
 * @brief  Internal storage for display information.
 *
 * Used by platform-specific implementations to store display query results.
 *
 * @ingroup system_gpu_internal
 */
struct DisplayInfoHost {
    int width = 0;                 ///< Display width in pixels.
    int height = 0;                ///< Display height in pixels.
    double refreshRate = 0.0;      ///< Refresh rate in Hz.
    double dpi = 0.0;              ///< Dots per inch.
    double devicePixelRatio = 1.0; ///< Device pixel ratio.
    bool isVirtual = false;        ///< True if virtual display (WSLg, headless).
};

} // namespace cf
