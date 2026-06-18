/**
 * @file    hardware_tier.cpp
 * @brief   Implements the hardware tier assessment pipeline.
 *
 * Contains the registry, caching logic, and pipeline orchestration
 * for the Collect -> Score -> Assess -> Policy workflow.
 *
 * @author  Charliechen114514
 * @date    2026-05-23
 * @version 0.19
 * @since   0.19
 * @ingroup system_hardware_tier
 */

#include "system/hardware_tier/hardware_tier.h"

#include "hardware_tier_assessor.h"
#include "hardware_tier_collector.h"
#include "hardware_tier_policy.h"
#include "hardware_tier_scorer.h"

#include "default_factories.h"

#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>

namespace cf {

// ─────────────────────────────────────────────────────────
//  Pipeline Registry
// ─────────────────────────────────────────────────────────
struct HardwareTierRegistry {
    std::unique_ptr<IHardwareCollector> collector;
    std::unique_ptr<IHardwareScorer> cpu_scorer;
    std::unique_ptr<IHardwareScorer> gpu_scorer;
    std::unique_ptr<IHardwareScorer> memory_scorer;
    std::unique_ptr<IHardwareScorer> display_scorer;
    std::unique_ptr<IHardwareAssessor> assessor;
    std::unique_ptr<IHardwarePolicy> policy;

    std::optional<HardwareTierLevel> override_level;
    std::string override_reason;

    HardwareTierRegistry();
};

static HardwareTierRegistry& getRegistry() {
    static HardwareTierRegistry reg;
    return reg;
}

// ─────────────────────────────────────────────────────────
//  Cached Assessment State
// ─────────────────────────────────────────────────────────
struct CachedAssessment {
    HardwareTierAssessment assessment;
    HardwareTierCapabilities capabilities;
    bool valid = false;
};

static CachedAssessment g_cached;
static std::mutex g_cache_mutex;

// ─────────────────────────────────────────────────────────
//  Public API
// ─────────────────────────────────────────────────────────

const char* hardwareTierLevelToString(HardwareTierLevel level) noexcept {
    switch (level) {
        case HardwareTierLevel::Low:
            return "Low";
        case HardwareTierLevel::Mid:
            return "Mid";
        case HardwareTierLevel::High:
            return "High";
        default:
            return "Unknown";
    }
}

void invalidateCache() {
    std::lock_guard lock(g_cache_mutex);
    g_cached.valid = false;
}

void registerCollector(std::unique_ptr<IHardwareCollector> collector) {
    getRegistry().collector = std::move(collector);
    invalidateCache();
}

void registerScorer(std::string_view dimension, std::unique_ptr<IHardwareScorer> scorer) {
    auto& reg = getRegistry();
    if (dimension == "cpu")
        reg.cpu_scorer = std::move(scorer);
    else if (dimension == "gpu")
        reg.gpu_scorer = std::move(scorer);
    else if (dimension == "memory")
        reg.memory_scorer = std::move(scorer);
    else if (dimension == "display")
        reg.display_scorer = std::move(scorer);
    invalidateCache();
}

void registerAssessor(std::unique_ptr<IHardwareAssessor> assessor) {
    getRegistry().assessor = std::move(assessor);
    invalidateCache();
}

void registerPolicy(std::unique_ptr<IHardwarePolicy> policy) {
    getRegistry().policy = std::move(policy);
    invalidateCache();
}

void setDeviceConfigOverride(HardwareTierLevel level, std::string reason) {
    auto& reg = getRegistry();
    reg.override_level = level;
    reg.override_reason = std::move(reason);
    invalidateCache();
}

void clearDeviceConfigOverride() {
    auto& reg = getRegistry();
    reg.override_level.reset();
    reg.override_reason.clear();
    invalidateCache();
}

aex::expected<HardwareTierAssessment, HardwareTierError> assessHardware(bool force_refresh) {
    if (force_refresh) {
        invalidateCache();
    }

    {
        std::lock_guard lock(g_cache_mutex);
        if (g_cached.valid) {
            return g_cached.assessment;
        }
    }

    auto& reg = getRegistry();
    HardwareTierAssessment result;

    // ── DeviceConfig override short-circuit ──
    if (reg.override_level.has_value()) {
        result.tier = *reg.override_level;
        result.is_overridden = true;
        result.override_reason = reg.override_reason;

        HardwareTierCapabilities caps;
        if (reg.policy) {
            caps = reg.policy->deriveCapabilities(result.tier, result);
        }
        std::lock_guard lock(g_cache_mutex);
        g_cached.assessment = result;
        g_cached.capabilities = caps;
        g_cached.valid = true;
        return result;
    }

    // ── Stage 1: Collect ──
    if (!reg.collector) {
        return aex::unexpected(HardwareTierError::CollectionFailed);
    }
    HardwareData data = reg.collector->collect();

    // ── Stage 2: Score ──
    if (!reg.cpu_scorer || !reg.gpu_scorer || !reg.memory_scorer || !reg.display_scorer) {
        return aex::unexpected(HardwareTierError::ScoringFailed);
    }

    result.cpu.value = reg.cpu_scorer->score(data);
    result.gpu.value = reg.gpu_scorer->score(data);
    result.memory.value = reg.memory_scorer->score(data);
    result.display.value = reg.display_scorer->score(data);

    // ── Stage 3: Assess ──
    if (!reg.assessor) {
        return aex::unexpected(HardwareTierError::AssessmentFailed);
    }
    result.tier = reg.assessor->assess(result.cpu.value, result.gpu.value, result.memory.value,
                                       result.display.value);

    // ── Stage 4: Policy ──
    HardwareTierCapabilities caps;
    if (reg.policy) {
        caps = reg.policy->deriveCapabilities(result.tier, result);
    }

    std::lock_guard lock(g_cache_mutex);
    g_cached.assessment = result;
    g_cached.capabilities = caps;
    g_cached.valid = true;
    return result;
}

aex::expected<HardwareTierCapabilities, HardwareTierError> getHardwareTierCapabilities() {
    std::lock_guard lock(g_cache_mutex);
    if (!g_cached.valid) {
        return aex::unexpected(HardwareTierError::PolicyFailed);
    }
    return g_cached.capabilities;
}

// ─────────────────────────────────────────────────────────
//  Registry Constructor — installs all defaults
// ─────────────────────────────────────────────────────────
HardwareTierRegistry::HardwareTierRegistry()
    : collector(makeDefaultCollector()), cpu_scorer(makeDefaultCpuScorer()),
      gpu_scorer(makeDefaultGpuScorer()), memory_scorer(makeDefaultMemoryScorer()),
      display_scorer(makeDefaultDisplayScorer()), assessor(makeDefaultAssessor()),
      policy(makeDefaultPolicy()) {}

} // namespace cf
