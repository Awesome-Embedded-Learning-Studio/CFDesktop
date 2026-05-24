/**
 * @file    hardware_tier_data.h
 * @brief   Defines data structures for hardware tier assessment.
 *
 * Provides the core types used by the hardware tier pipeline: tier levels,
 * per-dimension scores, collected hardware data, assessment results,
 * capability flags, and error codes.
 *
 * @author  Charliechen114514
 * @date    2026-05-23
 * @version 0.19
 * @since   0.19
 * @ingroup system_hardware_tier
 */
#pragma once

#include "base/export.h"
#include <cstdint>
#include <string>

namespace cf {

// ─────────────────────────────────────────────────────────
//  Hardware Tier Level
// ─────────────────────────────────────────────────────────

/**
 * @brief  Hardware capability tier classification.
 *
 * Represents the overall capability tier of the device, derived from
 * multi-dimensional scoring.
 *
 * Tier examples:
 *   - Low:   i.MX6ULL (528MHz A7) -- no animation, linuxfb, soft decode
 *   - Mid:   RK3568 (4xA55, Mali-G52) -- partial animation, optional eglfs
 *   - High:  RK3588 (8xA76/A55, Mali-G610) -- full animation, eglfs+OpenGL
 *
 * @since   0.19
 * @ingroup system_hardware_tier
 */
enum class HardwareTierLevel : uint8_t {
    Unknown = 0, ///< Assessment could not be completed.
    Low = 1,     ///< Minimal capability embedded device.
    Mid = 2,     ///< Mid-range capability SoC.
    High = 3     ///< High capability desktop/workstation grade.
};

/**
 * @brief  Converts HardwareTierLevel to a human-readable string.
 *
 * @param[in] level The tier level.
 * @return          "Unknown", "Low", "Mid", or "High".
 *
 * @since   0.19
 * @ingroup system_hardware_tier
 */
CF_BASE_EXPORT const char* hardwareTierLevelToString(HardwareTierLevel level) noexcept;

// ─────────────────────────────────────────────────────────
//  Per-Dimension Score Types
// ─────────────────────────────────────────────────────────

/**
 * @brief  CPU capability score (0-100).
 *
 * @since   0.19
 * @ingroup system_hardware_tier
 */
struct CpuScore {
    int value = 0; ///< Raw score (0-100).
};

/**
 * @brief  GPU capability score (0-100).
 *
 * @since   0.19
 * @ingroup system_hardware_tier
 */
struct GpuScore {
    int value = 0; ///< Raw score (0-100).
};

/**
 * @brief  Memory capability score (0-100).
 *
 * @since   0.19
 * @ingroup system_hardware_tier
 */
struct MemoryScore {
    int value = 0; ///< Raw score (0-100).
};

/**
 * @brief  Display capability score (0-100).
 *
 * @since   0.19
 * @ingroup system_hardware_tier
 */
struct DisplayScore {
    int value = 0; ///< Raw score (0-100).
};

// ─────────────────────────────────────────────────────────
//  Collected Hardware Data (raw snapshot)
// ─────────────────────────────────────────────────────────

/**
 * @brief  Raw collected hardware data used as scorer input.
 *
 * Aggregate of all probe results gathered by the collector stage.
 * Owns all data independently so it outlives any probe cache.
 * Fields that could not be collected are left at defaults.
 *
 * @since   0.19
 * @ingroup system_hardware_tier
 */
struct HardwareData {
    // CPU
    std::string cpu_model;       ///< CPU model name.
    std::string cpu_arch;        ///< CPU architecture string.
    uint16_t logical_cores = 0;  ///< Logical CPU thread count.
    uint16_t physical_cores = 0; ///< Physical CPU core count.
    uint32_t max_frequency = 0;  ///< Max frequency in MHz.
    bool has_big_little = false; ///< big.LITTLE architecture flag.
    uint32_t big_core_count = 0; ///< Number of performance cores.

    // GPU
    std::string gpu_name;         ///< GPU device name.
    uint32_t gpu_vendor_id = 0;   ///< PCI vendor ID.
    bool gpu_is_discrete = false; ///< Discrete GPU flag.
    bool gpu_is_software = false; ///< Software rendering flag.
    int existing_gpu_score = 0;   ///< Existing EnvironmentScore.gpu value (0-50).

    // Display
    int display_width = 0;          ///< Display width in pixels.
    int display_height = 0;         ///< Display height in pixels.
    double display_refresh = 0.0;   ///< Refresh rate in Hz.
    double display_dpi = 96.0;      ///< DPI.
    int existing_display_score = 0; ///< Existing EnvironmentScore.display value (0-50).

    // Memory
    uint64_t total_physical_bytes = 0; ///< Total physical RAM.
    uint64_t total_swap_bytes = 0;     ///< Total swap space.
};

// ─────────────────────────────────────────────────────────
//  Assessment Result
// ─────────────────────────────────────────────────────────

/**
 * @brief  Complete hardware tier assessment result.
 *
 * Contains per-dimension scores, the overall tier level, and
 * override metadata.
 *
 * @since   0.19
 * @ingroup system_hardware_tier
 */
struct HardwareTierAssessment {
    CpuScore cpu;                                        ///< CPU capability score.
    GpuScore gpu;                                        ///< GPU capability score.
    MemoryScore memory;                                  ///< Memory capability score.
    DisplayScore display;                                ///< Display capability score.
    HardwareTierLevel tier = HardwareTierLevel::Unknown; ///< Overall classification.

    bool is_overridden = false;  ///< True if DeviceConfig override is active.
    std::string override_reason; ///< Override reason (when is_overridden).
};

// ─────────────────────────────────────────────────────────
//  Capability Flags
// ─────────────────────────────────────────────────────────

/**
 * @brief  Capability flags derived from the policy stage.
 *
 * Drives downstream decisions: rendering backend, animation policy,
 * decoder selection, etc.
 *
 * @since   0.19
 * @ingroup system_hardware_tier
 */
struct HardwareTierCapabilities {
    bool use_opengl = false;               ///< Use OpenGL/EGLFS rendering.
    bool use_software_render = false;      ///< Use software rendering / linuxfb.
    bool enable_animation = false;         ///< Enable full UI animations.
    bool enable_partial_animation = false; ///< Enable limited animations.
    bool use_hardware_decode = false;      ///< Use hardware video decoding.
    bool use_eglfs = false;                ///< Use EGLFS platform plugin.
    bool use_linuxfb = false;              ///< Use linuxfb platform plugin.
};

// ─────────────────────────────────────────────────────────
//  Error Codes
// ─────────────────────────────────────────────────────────

/**
 * @brief  Error types for hardware tier assessment.
 *
 * @since   0.19
 * @ingroup system_hardware_tier
 */
enum class HardwareTierError {
    NoError,          ///< Assessment completed successfully.
    CollectionFailed, ///< Hardware data collection failed.
    ScoringFailed,    ///< Scoring stage produced no results.
    AssessmentFailed, ///< Assessment stage failed.
    PolicyFailed,     ///< Policy derivation failed.
};

} // namespace cf
