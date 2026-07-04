/**
 * @file    base/include/system/gpu/gpu.h
 * @brief   Provides GPU and display information structures and query functions.
 *
 * Defines structures for GPU device information, display properties, environment
 * capability scoring, and error types used by the GPU/display detection subsystem.
 *
 * @author  N/A
 * @date    N/A
 * @version N/A
 * @since   N/A
 * @ingroup none
 */

#pragma once
#include "aex/expected/expected.hpp"
#include "base/export.h"
#include <cstdint>
#include <string>

namespace cf {

/**
 * @brief  Contains information about a GPU device.
 *
 * Stores GPU properties including device identification, driver information,
 * and capability flags. Tracks whether the GPU is discrete, integrated, or
 * running in a WSL2 environment.
 *
 * @note None
 * @warning None
 * @since N/A
 * @ingroup none
 */
struct GPUInfo {
    std::string name;          ///< Human-readable GPU device name.
    std::string backend;       ///< Graphics backend API name (e.g., OpenGL, Vulkan).
    std::string driverVersion; ///< Driver version string.
    uint32_t vendorId = 0;     ///< PCI vendor ID.
    uint32_t deviceId = 0;     ///< PCI device ID.

    bool isDiscrete = false;   ///< True if GPU is a discrete/dedicated GPU.
    bool isIntegrated = false; ///< True if GPU is an integrated GPU.
    bool hasSoftware = false;  ///< True if software rendering is available.
    bool isWSL = false;        ///< True if running in WSL2 environment.
};

/**
 * @brief  Contains display monitor information.
 *
 * Stores display properties including resolution, refresh rate, DPI,
 * and device pixel ratio. Identifies virtual displays such as WSLg or
 * headless configurations.
 *
 * @note None
 * @warning None
 * @since N/A
 * @ingroup none
 */
struct DisplayInfo {
    int width = 0;                 ///< Display width in pixels.
    int height = 0;                ///< Display height in pixels.
    double refreshRate = 0.0;      ///< Refresh rate in Hz.
    double dpi = 0.0;              ///< Dots per inch (physical pixel density).
    double devicePixelRatio = 1.0; ///< Ratio of logical to physical pixels.
    bool isVirtual = false;        ///< True if virtual display (WSLg, headless).
};

/**
 * @brief  Represents environment capability scoring.
 *
 * Provides numeric scores for GPU and display capabilities along with
 * a qualitative level indicator. Used to assess overall hardware
 * suitability for graphics workloads.
 *
 * @note None
 * @warning None
 * @since N/A
 * @ingroup none
 */
struct EnvironmentScore {
    int gpu = 0;       ///< GPU capability score.
    int display = 0;   ///< Display capability score.
    int total = 0;     ///< Combined total score.
    std::string level; ///< Qualitative capability level (e.g., "low", "medium", "high").
};

/**
 * @brief  Error codes for GPU and display information queries.
 *
 * Represents possible error states when querying GPU and display
 * information. Currently indicates no error.
 *
 * @note None
 * @warning None
 * @since N/A
 * @ingroup none
 */
enum class GpuDisplayInfoError { OK };

/**
 * @brief  Aggregates GPU, display, and environment score information.
 *
 * Combines GPU device information, display properties, and capability
 * scoring into a single structure returned by the hardware detection API.
 *
 * @note None
 * @warning None
 * @since N/A
 * @ingroup none
 */
struct GpuDisplayInfo {
    GPUInfo gpu;            ///< GPU device information.
    DisplayInfo display;    ///< Display monitor information.
    EnvironmentScore score; ///< Environment capability score.
};

/**
 * @brief  Queries GPU and display information for the current system.
 *
 * Detects and returns GPU device properties, display characteristics,
 * and an environment capability score. Returns an error object if
 * detection fails.
 *
 * @return GpuDisplayInfo on success, or GpuDisplayInfoError on failure.
 * @throws None
 * @note   This function is noexcept and does not throw exceptions.
 * @warning None
 * @since  N/A
 * @ingroup none
 */
aex::expected<GpuDisplayInfo, GpuDisplayInfoError> CF_BASE_EXPORT getGpuDisplayInfo() noexcept;

} // namespace cf
