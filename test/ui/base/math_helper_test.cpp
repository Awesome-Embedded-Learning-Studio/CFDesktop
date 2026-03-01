/**
 * @file math_helper_test.cpp
 * @brief Comprehensive unit tests for cf::ui::math helper functions
 *
 * Test Coverage:
 * 1. lerp - Linear interpolation
 * 2. clamp - Value clamping
 * 3. remap - Value range remapping
 * 4. cubicBezier - Cubic Bezier curve evaluation
 * 5. springStep - Spring physics simulation
 * 6. lerpAngle - Angle interpolation with 0-360 boundary handling
 */

#include "ui/base/math_helper.h"
#include <cmath>
#include <gtest/gtest.h>

// =============================================================================
// Test Suite 1: lerp (Linear Interpolation)
// =============================================================================

TEST(MathHelperTest, Lerp_T0_ReturnsA) {
    EXPECT_FLOAT_EQ(cf::ui::math::lerp(10.0f, 20.0f, 0.0f), 10.0f);
    EXPECT_FLOAT_EQ(cf::ui::math::lerp(-5.0f, 5.0f, 0.0f), -5.0f);
}

TEST(MathHelperTest, Lerp_T1_ReturnsB) {
    EXPECT_FLOAT_EQ(cf::ui::math::lerp(10.0f, 20.0f, 1.0f), 20.0f);
    EXPECT_FLOAT_EQ(cf::ui::math::lerp(-5.0f, 5.0f, 1.0f), 5.0f);
}

TEST(MathHelperTest, Lerp_T05_ReturnsMidpoint) {
    EXPECT_FLOAT_EQ(cf::ui::math::lerp(0.0f, 10.0f, 0.5f), 5.0f);
    EXPECT_FLOAT_EQ(cf::ui::math::lerp(10.0f, 20.0f, 0.5f), 15.0f);
    EXPECT_FLOAT_EQ(cf::ui::math::lerp(-10.0f, 10.0f, 0.5f), 0.0f);
}

TEST(MathHelperTest, Lerp_NegativeT) {
    // t < 0 extrapolates beyond a
    EXPECT_FLOAT_EQ(cf::ui::math::lerp(10.0f, 20.0f, -0.5f), 5.0f);
    EXPECT_FLOAT_EQ(cf::ui::math::lerp(0.0f, 10.0f, -1.0f), -10.0f);
}

TEST(MathHelperTest, Lerp_TGreaterThan1) {
    // t > 1 extrapolates beyond b
    EXPECT_FLOAT_EQ(cf::ui::math::lerp(10.0f, 20.0f, 1.5f), 25.0f);
    EXPECT_FLOAT_EQ(cf::ui::math::lerp(0.0f, 10.0f, 2.0f), 20.0f);
}

TEST(MathHelperTest, Lerp_ReverseOrder) {
    // When a > b, interpolation works in reverse
    EXPECT_FLOAT_EQ(cf::ui::math::lerp(20.0f, 10.0f, 0.5f), 15.0f);
    EXPECT_FLOAT_EQ(cf::ui::math::lerp(20.0f, 10.0f, 0.0f), 20.0f);
    EXPECT_FLOAT_EQ(cf::ui::math::lerp(20.0f, 10.0f, 1.0f), 10.0f);
}

TEST(MathHelperTest, Lerp_EqualEndpoints) {
    EXPECT_FLOAT_EQ(cf::ui::math::lerp(5.0f, 5.0f, 0.5f), 5.0f);
    EXPECT_FLOAT_EQ(cf::ui::math::lerp(5.0f, 5.0f, 0.0f), 5.0f);
    EXPECT_FLOAT_EQ(cf::ui::math::lerp(5.0f, 5.0f, 1.0f), 5.0f);
}

// =============================================================================
// Test Suite 2: clamp
// =============================================================================

TEST(MathHelperTest, Clamp_ValueInRange) {
    EXPECT_FLOAT_EQ(cf::ui::math::clamp(5.0f, 0.0f, 10.0f), 5.0f);
    EXPECT_FLOAT_EQ(cf::ui::math::clamp(0.5f, 0.0f, 1.0f), 0.5f);
    EXPECT_FLOAT_EQ(cf::ui::math::clamp(-5.0f, -10.0f, 0.0f), -5.0f);
}

TEST(MathHelperTest, _Clamp_ValueBelowMin) {
    EXPECT_FLOAT_EQ(cf::ui::math::clamp(-5.0f, 0.0f, 10.0f), 0.0f);
    EXPECT_FLOAT_EQ(cf::ui::math::clamp(2.0f, 5.0f, 10.0f), 5.0f);
    EXPECT_FLOAT_EQ(cf::ui::math::clamp(-100.0f, -50.0f, 50.0f), -50.0f);
}

TEST(MathHelperTest, Clamp_ValueAboveMax) {
    EXPECT_FLOAT_EQ(cf::ui::math::clamp(15.0f, 0.0f, 10.0f), 10.0f);
    EXPECT_FLOAT_EQ(cf::ui::math::clamp(100.0f, 0.0f, 50.0f), 50.0f);
    EXPECT_FLOAT_EQ(cf::ui::math::clamp(60.0f, -50.0f, 50.0f), 50.0f);
}

TEST(MathHelperTest, _Clamp_BoundaryValues) {
    EXPECT_FLOAT_EQ(cf::ui::math::clamp(0.0f, 0.0f, 10.0f), 0.0f);
    EXPECT_FLOAT_EQ(cf::ui::math::clamp(10.0f, 0.0f, 10.0f), 10.0f);
    EXPECT_FLOAT_EQ(cf::ui::math::clamp(-5.0f, -5.0f, 5.0f), -5.0f);
    EXPECT_FLOAT_EQ(cf::ui::math::clamp(5.0f, -5.0f, 5.0f), 5.0f);
}

TEST(MathHelperTest, Clamp_NegativeRange) {
    EXPECT_FLOAT_EQ(cf::ui::math::clamp(-15.0f, -20.0f, -10.0f), -15.0f);
    EXPECT_FLOAT_EQ(cf::ui::math::clamp(-25.0f, -20.0f, -10.0f), -20.0f);
    EXPECT_FLOAT_EQ(cf::ui::math::clamp(-5.0f, -20.0f, -10.0f), -10.0f);
}

TEST(MathHelperTest, Clamp_EqualMinMax) {
    EXPECT_FLOAT_EQ(cf::ui::math::clamp(5.0f, 10.0f, 10.0f), 10.0f);
    EXPECT_FLOAT_EQ(cf::ui::math::clamp(10.0f, 10.0f, 10.0f), 10.0f);
    EXPECT_FLOAT_EQ(cf::ui::math::clamp(15.0f, 10.0f, 10.0f), 10.0f);
}

// =============================================================================
// Test Suite 3: remap
// =============================================================================

TEST(MathHelperTest, Remap_Basic) {
    EXPECT_FLOAT_EQ(cf::ui::math::remap(5.0f, 0.0f, 10.0f, 0.0f, 100.0f), 50.0f);
    EXPECT_FLOAT_EQ(cf::ui::math::remap(0.0f, 0.0f, 10.0f, 0.0f, 100.0f), 0.0f);
    EXPECT_FLOAT_EQ(cf::ui::math::remap(10.0f, 0.0f, 10.0f, 0.0f, 100.0f), 100.0f);
}

TEST(MathHelperTest, Remap_ReverseInputRange) {
    EXPECT_FLOAT_EQ(cf::ui::math::remap(7.5f, 10.0f, 0.0f, 0.0f, 100.0f), 25.0f);
    EXPECT_FLOAT_EQ(cf::ui::math::remap(10.0f, 10.0f, 0.0f, 0.0f, 100.0f), 0.0f);
    EXPECT_FLOAT_EQ(cf::ui::math::remap(0.0f, 10.0f, 0.0f, 0.0f, 100.0f), 100.0f);
}

TEST(MathHelperTest, Remap_ReverseOutputRange) {
    EXPECT_FLOAT_EQ(cf::ui::math::remap(5.0f, 0.0f, 10.0f, 100.0f, 0.0f), 50.0f);
    EXPECT_FLOAT_EQ(cf::ui::math::remap(0.0f, 0.0f, 10.0f, 100.0f, 0.0f), 100.0f);
    EXPECT_FLOAT_EQ(cf::ui::math::remap(10.0f, 0.0f, 10.0f, 100.0f, 0.0f), 0.0f);
}

TEST(MathHelperTest, Remap_DifferentScaleFactors) {
    EXPECT_FLOAT_EQ(cf::ui::math::remap(0.5f, 0.0f, 1.0f, 0.0f, 1000.0f), 500.0f);
    EXPECT_FLOAT_EQ(cf::ui::math::remap(50.0f, 0.0f, 100.0f, -100.0f, 100.0f), 0.0f);
}

TEST(MathHelperTest, Remap_OutOfInputRange) {
    // Values outside input range are extrapolated
    EXPECT_FLOAT_EQ(cf::ui::math::remap(-5.0f, 0.0f, 10.0f, 0.0f, 100.0f), -50.0f);
    EXPECT_FLOAT_EQ(cf::ui::math::remap(15.0f, 0.0f, 10.0f, 0.0f, 100.0f), 150.0f);
}

TEST(MathHelperTest, Remap_ZeroInputRange_ValueAtMin) {
    // When input range is zero, value at or below min returns output max
    EXPECT_FLOAT_EQ(cf::ui::math::remap(5.0f, 5.0f, 5.0f, 0.0f, 100.0f), 100.0f);
    EXPECT_FLOAT_EQ(cf::ui::math::remap(3.0f, 5.0f, 5.0f, 0.0f, 100.0f), 100.0f);
}

TEST(MathHelperTest, Remap_ZeroInputRange_ValueAboveMin) {
    // When input range is zero, value above min returns output min
    EXPECT_FLOAT_EQ(cf::ui::math::remap(6.0f, 5.0f, 5.0f, 0.0f, 100.0f), 0.0f);
    EXPECT_FLOAT_EQ(cf::ui::math::remap(10.0f, 5.0f, 5.0f, 0.0f, 100.0f), 0.0f);
}

TEST(MathHelperTest, Remap_NegativeRanges) {
    EXPECT_FLOAT_EQ(cf::ui::math::remap(-5.0f, -10.0f, 0.0f, -100.0f, 0.0f), -50.0f);
    EXPECT_FLOAT_EQ(cf::ui::math::remap(-10.0f, -10.0f, 0.0f, -100.0f, 0.0f), -100.0f);
    EXPECT_FLOAT_EQ(cf::ui::math::remap(0.0f, -10.0f, 0.0f, -100.0f, 0.0f), 0.0f);
}

// =============================================================================
// Test Suite 4: cubicBezier
// =============================================================================

TEST(MathHelperTest, CubicBezier_T0_Returns0) {
    EXPECT_FLOAT_EQ(cf::ui::math::cubicBezier(0.2f, 0.0f, 0.0f, 1.0f, 0.0f), 0.0f);
    EXPECT_FLOAT_EQ(cf::ui::math::cubicBezier(0.5f, 0.5f, 0.5f, 0.5f, 0.0f), 0.0f);
}

TEST(MathHelperTest, CubicBezier_T1_Returns1) {
    EXPECT_FLOAT_EQ(cf::ui::math::cubicBezier(0.2f, 0.0f, 0.0f, 1.0f, 1.0f), 1.0f);
    EXPECT_FLOAT_EQ(cf::ui::math::cubicBezier(0.5f, 0.5f, 0.5f, 0.5f, 1.0f), 1.0f);
}

TEST(MathHelperTest, CubicBezier_Linear) {
    // Linear bezier: (0,0) to (1,1) with control points at (0,0) and (1,1) is linear
    // Actually with P1=(0,0), P2=(1,1), the curve is y=x
    float tolerance = 0.001f;
    for (int i = 0; i <= 10; ++i) {
        float t = i / 10.0f;
        EXPECT_NEAR(cf::ui::math::cubicBezier(0.0f, 0.0f, 1.0f, 1.0f, t), t, tolerance);
    }
}

TEST(MathHelperTest, CubicBezier_EaseIn) {
    // Ease in: slow start, fast end
    // At t=0.5, should be less than 0.5
    float result = cf::ui::math::cubicBezier(0.5f, 0.0f, 1.0f, 1.0f, 0.5f);
    EXPECT_LT(result, 0.5f);
}

TEST(MathHelperTest, CubicBezier_EaseOut) {
    // Ease out: fast start, slow end
    // At t=0.5, should be greater than 0.5
    float result = cf::ui::math::cubicBezier(0.0f, 0.0f, 0.5f, 1.0f, 0.5f);
    EXPECT_GT(result, 0.5f);
}

TEST(MathHelperTest, CubicBezier_MaterialDesignStandard) {
    // Material Design Standard: cubic-bezier(0.2, 0, 0, 1)
    // This is an ease-out curve (fast start, slow end)
    // At t=0.5, y should be > 0.8 (curve rises quickly then plateaus)
    float result = cf::ui::math::cubicBezier(0.2f, 0.0f, 0.0f, 1.0f, 0.5f);
    EXPECT_GT(result, 0.8f);  // Ease-out rises quickly
    EXPECT_LT(result, 0.95f); // But shouldn't reach near-1 yet
}

TEST(MathHelperTest, CubicBezier_Monotonicity) {
    // For proper easing curves, output should increase with t
    float prev = cf::ui::math::cubicBezier(0.2f, 0.0f, 0.0f, 1.0f, 0.0f);
    for (int i = 1; i <= 10; ++i) {
        float t = i / 10.0f;
        float curr = cf::ui::math::cubicBezier(0.2f, 0.0f, 0.0f, 1.0f, t);
        EXPECT_GE(curr, prev);
        prev = curr;
    }
}

// =============================================================================
// Test Suite 5: springStep
// =============================================================================

TEST(SpringStepTest, BasicSpringBehavior) {
    // Spring should move towards target
    auto [pos, vel] = cf::ui::math::springStep(0.0f, 0.0f, 10.0f, 100.0f, 10.0f, 0.016f);
    EXPECT_GT(pos, 0.0f); // Position should increase
    EXPECT_GT(vel, 0.0f); // Velocity should increase
}

TEST(SpringStepTest, SpringAtTarget_NoMovement) {
    // Already at target with zero velocity should stay
    auto [pos, vel] = cf::ui::math::springStep(10.0f, 0.0f, 10.0f, 100.0f, 10.0f, 0.016f);
    EXPECT_FLOAT_EQ(pos, 10.0f);
    EXPECT_FLOAT_EQ(vel, 0.0f);
}

TEST(SpringStepTest, SpringOvershoot) {
    // With low damping, spring should overshoot
    float stiffness = 200.0f;
    float damping = 5.0f;
    float dt = 0.016f;

    float pos = 0.0f;
    float vel = 0.0f;
    float target = 100.0f;

    bool overshot = false;
    for (int i = 0; i < 100; ++i) {
        auto result = cf::ui::math::springStep(pos, vel, target, stiffness, damping, dt);
        pos = result.first;
        vel = result.second;
        if (pos > target) {
            overshot = true;
            break;
        }
    }
    EXPECT_TRUE(overshot);
}

TEST(SpringStepTest, SpringConvergence) {
    // With proper damping, spring should converge
    float stiffness = 200.0f;
    float damping = 20.0f;
    float dt = 0.016f;

    float pos = 0.0f;
    float vel = 0.0f;
    float target = 100.0f;

    // Simulate for 3 seconds
    for (int i = 0; i < 187; ++i) {
        auto result = cf::ui::math::springStep(pos, vel, target, stiffness, damping, dt);
        pos = result.first;
        vel = result.second;
    }

    // Should be close to target
    EXPECT_NEAR(pos, target, 1.0f);
    // Velocity should be small
    EXPECT_NEAR(std::abs(vel), 0.0f, 5.0f);
}

TEST(SpringStepTest, SpringVelocityDecay) {
    // High damping should cause velocity to decay quickly
    float stiffness = 100.0f;
    float damping = 50.0f;
    float dt = 0.016f;

    float pos = 0.0f;
    float vel = 100.0f; // Initial velocity toward target
    float target = 10.0f;

    auto [pos1, vel1] = cf::ui::math::springStep(pos, vel, target, stiffness, damping, dt);
    EXPECT_LT(std::abs(vel1), std::abs(vel));
}

TEST(SpringStepTest, SpringZeroTimeStep) {
    // dt=0 should not change state
    auto [pos, vel] = cf::ui::math::springStep(5.0f, 10.0f, 100.0f, 100.0f, 10.0f, 0.0f);
    EXPECT_FLOAT_EQ(pos, 5.0f);
    EXPECT_FLOAT_EQ(vel, 10.0f);
}

TEST(SpringStepTest, SpringNegativeStiffness) {
    // Negative stiffness should move away from target (unstable)
    auto [pos, vel] = cf::ui::math::springStep(50.0f, 0.0f, 100.0f, -100.0f, 10.0f, 0.016f);
    EXPECT_LT(pos, 50.0f); // Should move away from target
}

TEST(SpringStepTest, SpringHighDamping) {
    // High damping should prevent overshoot
    float stiffness = 400.0f;
    float damping = 50.0f;
    float dt = 0.016f;

    float pos = 0.0f;
    float vel = 0.0f;
    float target = 100.0f;

    bool overshot = false;
    for (int i = 0; i < 200; ++i) {
        auto result = cf::ui::math::springStep(pos, vel, target, stiffness, damping, dt);
        pos = result.first;
        vel = result.second;
        if (pos > target) {
            overshot = true;
            break;
        }
    }
    EXPECT_FALSE(overshot);
}

// =============================================================================
// Test Suite 6: lerpAngle
// =============================================================================

TEST(LerpAngleTest, NormalInterpolation) {
    // Normal angle interpolation
    EXPECT_NEAR(cf::ui::math::lerpAngle(0.0f, 90.0f, 0.5f), 45.0f, 0.01f);
    EXPECT_NEAR(cf::ui::math::lerpAngle(0.0f, 180.0f, 0.5f), 90.0f, 0.01f);
}

TEST(LerpAngleTest, T0_ReturnsStartAngle) {
    EXPECT_FLOAT_EQ(cf::ui::math::lerpAngle(45.0f, 135.0f, 0.0f), 45.0f);
    EXPECT_FLOAT_EQ(cf::ui::math::lerpAngle(180.0f, 270.0f, 0.0f), 180.0f);
}

TEST(LerpAngleTest, T1_ReturnsEndAngle) {
    EXPECT_NEAR(cf::ui::math::lerpAngle(45.0f, 135.0f, 1.0f), 135.0f, 0.01f);
    EXPECT_NEAR(cf::ui::math::lerpAngle(180.0f, 270.0f, 1.0f), 270.0f, 0.01f);
}

TEST(LerpAngleTest, CrossZeroBoundaryForward) {
    // Interpolating from 350° to 10° should go through 0°
    float result = cf::ui::math::lerpAngle(350.0f, 10.0f, 0.5f);
    // Should be close to 0° (or 360°)
    EXPECT_TRUE(result < 10.0f || result > 350.0f);
}

TEST(LerpAngleTest, CrossZeroBoundaryBackward) {
    // Interpolating from 10° to 350° should go the short way through 0°
    float result = cf::ui::math::lerpAngle(10.0f, 350.0f, 0.5f);
    // Should be close to 0° or 360°
    EXPECT_TRUE(result < 20.0f || result > 340.0f);
}

TEST(LerpAngleTest, ShortestPath180Degrees) {
    // 0° to 180° - either direction is the same
    float result = cf::ui::math::lerpAngle(0.0f, 180.0f, 0.5f);
    EXPECT_NEAR(result, 90.0f, 0.1f);
}

TEST(LerpAngleTest, LargeAngleDifference) {
    // Interpolating across large angles
    float result = cf::ui::math::lerpAngle(30.0f, 330.0f, 0.5f);
    // Should take shortest path through 0°
    EXPECT_NEAR(result, 0.0f, 10.0f);
}

TEST(LerpAngleTest, NegativeAngles) {
    // Negative angles should work
    float result = cf::ui::math::lerpAngle(-90.0f, 90.0f, 0.5f);
    EXPECT_NEAR(result, 0.0f, 0.1f);
}

TEST(LerpAngleTest, SameAngle) {
    EXPECT_NEAR(cf::ui::math::lerpAngle(45.0f, 45.0f, 0.5f), 45.0f, 0.01f);
    EXPECT_NEAR(cf::ui::math::lerpAngle(0.0f, 0.0f, 0.5f), 0.0f, 0.01f);
}

TEST(LerpAngleTest, ClockwiseRotation) {
    // Small clockwise rotation
    float result = cf::ui::math::lerpAngle(0.0f, 45.0f, 0.5f);
    EXPECT_NEAR(result, 22.5f, 0.1f);
}

TEST(LerpAngleTest, CounterClockwiseRotation) {
    // Counter-clockwise rotation (short path)
    float result = cf::ui::math::lerpAngle(45.0f, 0.0f, 0.5f);
    // Should go through 22.5°
    EXPECT_NEAR(result, 22.5f, 0.1f);
}

TEST(LerpAngleTest, Extrapolation) {
    // t > 1 should extrapolate
    float result = cf::ui::math::lerpAngle(0.0f, 90.0f, 1.5f);
    EXPECT_NEAR(result, 135.0f, 0.1f);
}

// =============================================================================
// Main
// =============================================================================

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
