/**
 * @file    default_factories.h
 * @brief   Declares factory functions for default pipeline implementations.
 *
 * Each default implementation .cpp file provides a factory function
 * that creates the appropriate unique_ptr, avoiding the need for
 * full class definitions in the pipeline executor.
 *
 * @author  Charliechen114514
 * @date    2026-05-23
 * @version 0.19
 * @since   0.19
 * @ingroup system_hardware_tier
 */
#pragma once

#include "hardware_tier_assessor.h"
#include "hardware_tier_collector.h"
#include "hardware_tier_policy.h"
#include "hardware_tier_scorer.h"

#include <memory>

namespace cf {

std::unique_ptr<IHardwareCollector> makeDefaultCollector();
std::unique_ptr<IHardwareScorer> makeDefaultCpuScorer();
std::unique_ptr<IHardwareScorer> makeDefaultGpuScorer();
std::unique_ptr<IHardwareScorer> makeDefaultMemoryScorer();
std::unique_ptr<IHardwareScorer> makeDefaultDisplayScorer();
std::unique_ptr<IHardwareAssessor> makeDefaultAssessor();
std::unique_ptr<IHardwarePolicy> makeDefaultPolicy();

} // namespace cf
