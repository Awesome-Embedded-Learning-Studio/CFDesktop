/**
 * @file    hardware_tier.h
 * @brief   Declares the hardware tier assessment public API.
 *
 * Provides functions for registering pluggable pipeline stages,
 * configuring DeviceConfig overrides, and performing hardware
 * tier assessments.
 *
 * @author  Charliechen114514
 * @date    2026-05-23
 * @version 0.19
 * @since   0.19
 * @ingroup system_hardware_tier
 */
#pragma once

#include "base/expected/expected.hpp"
#include "base/export.h"
#include "system/hardware_tier/hardware_tier_data.h"

#include <memory>
#include <string_view>

namespace cf {

class IHardwareCollector;
class IHardwareScorer;
class IHardwareAssessor;
class IHardwarePolicy;

// ─────────────────────────────────────────────────────────
//  Pipeline Registry
// ─────────────────────────────────────────────────────────

/**
 * @brief  Registers a custom hardware data collector.
 *
 * Replaces the default collector. Must be called before the first
 * assessHardware() invocation.
 *
 * @param[in] collector  Unique pointer to the collector implementation.
 *
 * @return              void (replaces the registered collector).
 *
 * @since   0.19
 * @ingroup system_hardware_tier
 */
CF_BASE_EXPORT void registerCollector(std::unique_ptr<IHardwareCollector> collector);

/**
 * @brief  Registers a custom scorer for a specific dimension.
 *
 * @param[in] dimension  One of "cpu", "gpu", "memory", "display".
 * @param[in] scorer     Unique pointer to the scorer implementation.
 *
 * @return              void (replaces the registered scorer for the dimension).
 *
 * @since   0.19
 * @ingroup system_hardware_tier
 */
CF_BASE_EXPORT void registerScorer(std::string_view dimension,
                                   std::unique_ptr<IHardwareScorer> scorer);

/**
 * @brief  Registers a custom assessor implementation.
 *
 * @param[in] assessor  Unique pointer to the assessor implementation.
 *
 * @return              void (replaces the registered assessor).
 *
 * @since   0.19
 * @ingroup system_hardware_tier
 */
CF_BASE_EXPORT void registerAssessor(std::unique_ptr<IHardwareAssessor> assessor);

/**
 * @brief  Registers a custom policy implementation.
 *
 * @param[in] policy  Unique pointer to the policy implementation.
 *
 * @return            void (replaces the registered policy).
 *
 * @since   0.19
 * @ingroup system_hardware_tier
 */
CF_BASE_EXPORT void registerPolicy(std::unique_ptr<IHardwarePolicy> policy);

// ─────────────────────────────────────────────────────────
//  DeviceConfig Override
// ─────────────────────────────────────────────────────────

/**
 * @brief  Sets a manual tier override from DeviceConfig.
 *
 * When set, the pipeline short-circuits: collection and scoring are
 * skipped, and the specified tier level is used directly.
 *
 * @param[in] level   The tier level to enforce.
 * @param[in] reason  Optional human-readable reason string.
 *
 * @return            void (activates the tier override).
 *
 * @since   0.19
 * @ingroup system_hardware_tier
 */
CF_BASE_EXPORT void setDeviceConfigOverride(HardwareTierLevel level, std::string reason = {});

/**
 * @brief  Clears any active DeviceConfig override.
 *
 * @return            void (restores normal pipeline behavior).
 *
 * @since   0.19
 * @ingroup system_hardware_tier
 */
CF_BASE_EXPORT void clearDeviceConfigOverride();

// ─────────────────────────────────────────────────────────
//  Assessment API
// ─────────────────────────────────────────────────────────

/**
 * @brief  Performs a complete hardware tier assessment.
 *
 * Executes the full pipeline: Collect -> Score -> Assess -> Policy.
 * If a DeviceConfig override is active, the pipeline short-circuits
 * and returns the overridden tier.
 *
 * The result is cached after the first successful call. Pass
 * force_refresh = true to re-run the pipeline.
 *
 * @param[in] force_refresh  If true, forces re-running the pipeline.
 *
 * @return  expected containing HardwareTierAssessment on success,
 *          or HardwareTierError on failure.
 *
 * @since   0.19
 * @ingroup system_hardware_tier
 */
CF_BASE_EXPORT expected<HardwareTierAssessment, HardwareTierError>
assessHardware(bool force_refresh = false);

/**
 * @brief  Queries capability flags from the last assessment.
 *
 * Must be called after a successful assessHardware().
 *
 * @return  expected containing HardwareTierCapabilities on success,
 *          or HardwareTierError if no assessment exists.
 *
 * @since   0.19
 * @ingroup system_hardware_tier
 */
CF_BASE_EXPORT expected<HardwareTierCapabilities, HardwareTierError> getHardwareTierCapabilities();

} // namespace cf
