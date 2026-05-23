/**
 * @file    test_hardware_tier.cpp
 * @brief   Unit tests for the hardware tier assessment pipeline.
 *
 * @author  Charliechen114514
 * @date    2026-05-23
 * @version 0.19
 * @since   0.19
 * @ingroup system_hardware_tier
 */

#include "system/hardware_tier/hardware_tier.h"

#include <gtest/gtest.h>

using namespace cf;

// ── Basic Assessment ────────────────────────────────────

TEST(HardwareTierTest, AssessReturnsValidResult) {
    auto result = assessHardware(true);
    ASSERT_TRUE(result.has_value());

    const auto& a = *result;
    EXPECT_GE(a.cpu.value, 0);
    EXPECT_LE(a.cpu.value, 100);
    EXPECT_GE(a.gpu.value, 0);
    EXPECT_LE(a.gpu.value, 100);
    EXPECT_GE(a.memory.value, 0);
    EXPECT_LE(a.memory.value, 100);
    EXPECT_GE(a.display.value, 0);
    EXPECT_LE(a.display.value, 100);

    EXPECT_NE(a.tier, HardwareTierLevel::Unknown);
}

// ── Tier Level Strings ──────────────────────────────────

TEST(HardwareTierTest, TierLevelStrings) {
    EXPECT_STREQ(hardwareTierLevelToString(HardwareTierLevel::Low), "Low");
    EXPECT_STREQ(hardwareTierLevelToString(HardwareTierLevel::Mid), "Mid");
    EXPECT_STREQ(hardwareTierLevelToString(HardwareTierLevel::High), "High");
    EXPECT_STREQ(hardwareTierLevelToString(HardwareTierLevel::Unknown), "Unknown");
}

// ── Capabilities Accessible ─────────────────────────────

TEST(HardwareTierTest, CapabilitiesAccessible) {
    assessHardware(true);

    auto caps = getHardwareTierCapabilities();
    ASSERT_TRUE(caps.has_value());
    EXPECT_TRUE(caps->use_opengl || caps->use_software_render || caps->use_eglfs ||
                caps->use_linuxfb);
}

// ── DeviceConfig Override ───────────────────────────────

TEST(HardwareTierTest, DeviceConfigOverride) {
    setDeviceConfigOverride(HardwareTierLevel::Mid, "test override");

    auto result = assessHardware(true);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->tier, HardwareTierLevel::Mid);
    EXPECT_TRUE(result->is_overridden);
    EXPECT_EQ(result->override_reason, "test override");

    clearDeviceConfigOverride();
    assessHardware(true);
}

// ── Caching Behavior ────────────────────────────────────

TEST(HardwareTierTest, CachingReturnsSameResult) {
    auto first = assessHardware();
    ASSERT_TRUE(first.has_value());

    auto second = assessHardware();
    ASSERT_TRUE(second.has_value());

    EXPECT_EQ(first->cpu.value, second->cpu.value);
    EXPECT_EQ(first->tier, second->tier);
}

// ── Force Refresh ───────────────────────────────────────

TEST(HardwareTierTest, ForceRefreshProducesResult) {
    auto result = assessHardware(true);
    ASSERT_TRUE(result.has_value());
    EXPECT_NE(result->tier, HardwareTierLevel::Unknown);
}

// ── Override Clear Restores Normal Assessment ───────────

TEST(HardwareTierTest, OverrideClearRestoresNormal) {
    setDeviceConfigOverride(HardwareTierLevel::Low, "forced low");
    auto overridden = assessHardware(true);
    ASSERT_TRUE(overridden.has_value());
    EXPECT_EQ(overridden->tier, HardwareTierLevel::Low);

    clearDeviceConfigOverride();
    auto normal = assessHardware(true);
    ASSERT_TRUE(normal.has_value());
    EXPECT_FALSE(normal->is_overridden);
}
