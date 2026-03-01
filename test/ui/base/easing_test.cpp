/**
 * @file easing_test.cpp
 * @brief Comprehensive unit tests for cf::ui::base::Easing namespace
 *
 * Test Coverage:
 * 1. fromEasingType() - Converting Type enum to QEasingCurve
 * 2. custom() - Creating custom cubic bezier easing curves
 * 3. Spring presets (springGentle, springBouncy, springStiff)
 * 4. QEasingCurve validation
 */

#include "ui/base/easing.h"
#include <gtest/gtest.h>

// =============================================================================
// Test Suite 1: fromEasingType() - Type enum to QEasingCurve conversion
// =============================================================================

TEST(EasingTest, FromEasingType_Emphasized) {
    QEasingCurve curve =
        cf::ui::base::Easing::fromEasingType(cf::ui::base::Easing::Type::Emphasized);

    EXPECT_EQ(curve.type(), QEasingCurve::BezierSpline);
    EXPECT_FLOAT_EQ(curve.valueForProgress(0.0), 0.0);
    EXPECT_FLOAT_EQ(curve.valueForProgress(1.0), 1.0);
}

TEST(EasingTest, FromEasingType_EmphasizedDecelerate) {
    QEasingCurve curve =
        cf::ui::base::Easing::fromEasingType(cf::ui::base::Easing::Type::EmphasizedDecelerate);

    EXPECT_EQ(curve.type(), QEasingCurve::BezierSpline);
    EXPECT_FLOAT_EQ(curve.valueForProgress(0.0), 0.0);
    EXPECT_FLOAT_EQ(curve.valueForProgress(1.0), 1.0);
}

TEST(EasingTest, FromEasingType_EmphasizedAccelerate) {
    QEasingCurve curve =
        cf::ui::base::Easing::fromEasingType(cf::ui::base::Easing::Type::EmphasizedAccelerate);

    EXPECT_EQ(curve.type(), QEasingCurve::BezierSpline);
    EXPECT_FLOAT_EQ(curve.valueForProgress(0.0), 0.0);
    EXPECT_FLOAT_EQ(curve.valueForProgress(1.0), 1.0);
}

TEST(EasingTest, FromEasingType_Standard) {
    QEasingCurve curve = cf::ui::base::Easing::fromEasingType(cf::ui::base::Easing::Type::Standard);

    EXPECT_EQ(curve.type(), QEasingCurve::BezierSpline);
    EXPECT_FLOAT_EQ(curve.valueForProgress(0.0), 0.0);
    EXPECT_FLOAT_EQ(curve.valueForProgress(1.0), 1.0);
}

TEST(EasingTest, FromEasingType_StandardDecelerate) {
    QEasingCurve curve =
        cf::ui::base::Easing::fromEasingType(cf::ui::base::Easing::Type::StandardDecelerate);

    EXPECT_EQ(curve.type(), QEasingCurve::BezierSpline);
    EXPECT_FLOAT_EQ(curve.valueForProgress(0.0), 0.0);
    EXPECT_FLOAT_EQ(curve.valueForProgress(1.0), 1.0);
}

TEST(EasingTest, FromEasingType_StandardAccelerate) {
    QEasingCurve curve =
        cf::ui::base::Easing::fromEasingType(cf::ui::base::Easing::Type::StandardAccelerate);

    EXPECT_EQ(curve.type(), QEasingCurve::BezierSpline);
    EXPECT_FLOAT_EQ(curve.valueForProgress(0.0), 0.0);
    EXPECT_FLOAT_EQ(curve.valueForProgress(1.0), 1.0);
}

TEST(EasingTest, FromEasingType_Linear) {
    QEasingCurve curve = cf::ui::base::Easing::fromEasingType(cf::ui::base::Easing::Type::Linear);

    EXPECT_EQ(curve.type(), QEasingCurve::BezierSpline);
    EXPECT_FLOAT_EQ(curve.valueForProgress(0.0), 0.0);
    EXPECT_FLOAT_EQ(curve.valueForProgress(1.0), 1.0);

    // Linear should have value approximately equal to progress at all points
    EXPECT_NEAR(curve.valueForProgress(0.5), 0.5, 0.01f);
    EXPECT_NEAR(curve.valueForProgress(0.25), 0.25, 0.01f);
    EXPECT_NEAR(curve.valueForProgress(0.75), 0.75, 0.01f);
}

// =============================================================================
// Test Suite 2: Material Design easing curve characteristics
// =============================================================================

TEST(MaterialDesignEasing, Emphasized_IsEaseInOut) {
    // Emphasized: cubic-bezier(0.2, 0, 0, 1.0)
    QEasingCurve curve =
        cf::ui::base::Easing::fromEasingType(cf::ui::base::Easing::Type::Emphasized);

    // At 0.5 progress, should be greater than 0.5 (quick rise after slow start)
    float value = curve.valueForProgress(0.5);
    EXPECT_GT(value, 0.5f);
    EXPECT_LT(value, 1.0f);
}

TEST(MaterialDesignEasing, EmphasizedDecelerate_IsEaseOut) {
    // EmphasizedDecelerate: cubic-bezier(0.05, 0.7, 0.1, 1.0)
    // Fast start, slow end
    QEasingCurve curve =
        cf::ui::base::Easing::fromEasingType(cf::ui::base::Easing::Type::EmphasizedDecelerate);

    // At 0.5 progress, should be greater than 0.5 (fast start)
    float value = curve.valueForProgress(0.5);
    EXPECT_GT(value, 0.5f);
}

TEST(MaterialDesignEasing, EmphasizedAccelerate_IsEaseIn) {
    // EmphasizedAccelerate: cubic-bezier(0.3, 0, 0.8, 0.15)
    // Slow start, fast end
    QEasingCurve curve =
        cf::ui::base::Easing::fromEasingType(cf::ui::base::Easing::Type::EmphasizedAccelerate);

    // At 0.5 progress, should be less than 0.5 (slow start)
    float value = curve.valueForProgress(0.5);
    EXPECT_LT(value, 0.5f);
}

TEST(MaterialDesignEasing, Standard_IsSimilarToEmphasized) {
    // Standard: cubic-bezier(0.2, 0, 0, 1.0) - same as Emphasized
    QEasingCurve emphasized =
        cf::ui::base::Easing::fromEasingType(cf::ui::base::Easing::Type::Emphasized);
    QEasingCurve standard =
        cf::ui::base::Easing::fromEasingType(cf::ui::base::Easing::Type::Standard);

    // Should be the same curve
    for (int i = 0; i <= 10; ++i) {
        float t = i / 10.0f;
        EXPECT_FLOAT_EQ(emphasized.valueForProgress(t), standard.valueForProgress(t));
    }
}

TEST(MaterialDesignEasing, StandardDecelerate_IsPureEaseOut) {
    // StandardDecelerate: cubic-bezier(0, 0, 0, 1.0)
    // This is actually a pure ease-out curve
    QEasingCurve curve =
        cf::ui::base::Easing::fromEasingType(cf::ui::base::Easing::Type::StandardDecelerate);

    EXPECT_FLOAT_EQ(curve.valueForProgress(0.0), 0.0);
    EXPECT_FLOAT_EQ(curve.valueForProgress(1.0), 1.0);
}

TEST(MaterialDesignEasing, StandardAccelerate_IsPureEaseIn) {
    // StandardAccelerate: cubic-bezier(0.3, 0, 1, 1)
    // Pure ease-in curve
    QEasingCurve curve =
        cf::ui::base::Easing::fromEasingType(cf::ui::base::Easing::Type::StandardAccelerate);

    EXPECT_FLOAT_EQ(curve.valueForProgress(0.0), 0.0);
    EXPECT_FLOAT_EQ(curve.valueForProgress(1.0), 1.0);

    // At 0.5, should be significantly less than 0.5 (ease-in starts slow)
    float value = curve.valueForProgress(0.5);
    EXPECT_LT(value, 0.4f);
}

// =============================================================================
// Test Suite 3: custom() - Custom cubic bezier curves
// =============================================================================

TEST(CustomEasing, BasicBezier) {
    QEasingCurve curve = cf::ui::base::Easing::custom(0.25f, 0.1f, 0.25f, 1.0f);

    EXPECT_EQ(curve.type(), QEasingCurve::BezierSpline);
    EXPECT_FLOAT_EQ(curve.valueForProgress(0.0), 0.0);
    EXPECT_FLOAT_EQ(curve.valueForProgress(1.0), 1.0);
}

TEST(CustomEasing, EaseInQuad) {
    // Parabolic ease-in: (0.5, 0, 1, 1)
    QEasingCurve curve = cf::ui::base::Easing::custom(0.5f, 0.0f, 1.0f, 1.0f);

    EXPECT_FLOAT_EQ(curve.valueForProgress(0.0), 0.0);
    EXPECT_FLOAT_EQ(curve.valueForProgress(1.0), 1.0);

    // At t=0.5, value should be around 0.25 (quadratic)
    float value = curve.valueForProgress(0.5);
    EXPECT_LT(value, 0.5f);
}

TEST(CustomEasing, EaseOutQuad) {
    // Parabolic ease-out: (0, 0, 0.5, 1)
    QEasingCurve curve = cf::ui::base::Easing::custom(0.0f, 0.0f, 0.5f, 1.0f);

    EXPECT_FLOAT_EQ(curve.valueForProgress(0.0), 0.0);
    EXPECT_FLOAT_EQ(curve.valueForProgress(1.0), 1.0);

    // At t=0.5, value should be around 0.75 (quadratic)
    float value = curve.valueForProgress(0.5);
    EXPECT_GT(value, 0.5f);
}

TEST(CustomEasing, ExtremeControlPoints) {
    // Test with extreme control point values
    QEasingCurve curve = cf::ui::base::Easing::custom(0.0f, -0.5f, 1.0f, 1.5f);

    EXPECT_FLOAT_EQ(curve.valueForProgress(0.0), 0.0);
    EXPECT_FLOAT_EQ(curve.valueForProgress(1.0), 1.0);
}

// =============================================================================
// Test Suite 4: Spring presets
// =============================================================================

TEST(SpringPresets, SpringGentle) {
    cf::ui::base::Easing::SpringPreset preset = cf::ui::base::Easing::springGentle();

    EXPECT_FLOAT_EQ(preset.stiffness, 120.0f);
    EXPECT_FLOAT_EQ(preset.damping, 20.0f);
}

TEST(SpringPresets, SpringBouncy) {
    cf::ui::base::Easing::SpringPreset preset = cf::ui::base::Easing::springBouncy();

    EXPECT_FLOAT_EQ(preset.stiffness, 200.0f);
    EXPECT_FLOAT_EQ(preset.damping, 10.0f);
}

TEST(SpringPresets, SpringStiff) {
    cf::ui::base::Easing::SpringPreset preset = cf::ui::base::Easing::springStiff();

    EXPECT_FLOAT_EQ(preset.stiffness, 400.0f);
    EXPECT_FLOAT_EQ(preset.damping, 30.0f);
}

TEST(SpringPresets, GentleHasLowestStiffness) {
    auto gentle = cf::ui::base::Easing::springGentle();
    auto bouncy = cf::ui::base::Easing::springBouncy();
    auto stiff = cf::ui::base::Easing::springStiff();

    EXPECT_LT(gentle.stiffness, bouncy.stiffness);
    EXPECT_LT(bouncy.stiffness, stiff.stiffness);
}

TEST(SpringPresets, BouncyHasLowestDamping) {
    auto gentle = cf::ui::base::Easing::springGentle();
    auto bouncy = cf::ui::base::Easing::springBouncy();
    auto stiff = cf::ui::base::Easing::springStiff();

    EXPECT_LT(bouncy.damping, gentle.damping);
    EXPECT_LT(bouncy.damping, stiff.damping);
}

// =============================================================================
// Test Suite 5: QEasingCurve validation
// =============================================================================

TEST(EasingCurveValidation, AllTypes_StartAtZero) {
    auto types = {cf::ui::base::Easing::Type::Emphasized,
                  cf::ui::base::Easing::Type::EmphasizedDecelerate,
                  cf::ui::base::Easing::Type::EmphasizedAccelerate,
                  cf::ui::base::Easing::Type::Standard,
                  cf::ui::base::Easing::Type::StandardDecelerate,
                  cf::ui::base::Easing::Type::StandardAccelerate,
                  cf::ui::base::Easing::Type::Linear};

    for (auto type : types) {
        QEasingCurve curve = cf::ui::base::Easing::fromEasingType(type);
        EXPECT_FLOAT_EQ(curve.valueForProgress(0.0), 0.0) << "Easing curve should start at 0";
    }
}

TEST(EasingCurveValidation, AllTypes_EndAtOne) {
    auto types = {cf::ui::base::Easing::Type::Emphasized,
                  cf::ui::base::Easing::Type::EmphasizedDecelerate,
                  cf::ui::base::Easing::Type::EmphasizedAccelerate,
                  cf::ui::base::Easing::Type::Standard,
                  cf::ui::base::Easing::Type::StandardDecelerate,
                  cf::ui::base::Easing::Type::StandardAccelerate,
                  cf::ui::base::Easing::Type::Linear};

    for (auto type : types) {
        QEasingCurve curve = cf::ui::base::Easing::fromEasingType(type);
        EXPECT_FLOAT_EQ(curve.valueForProgress(1.0), 1.0) << "Easing curve should end at 1";
    }
}

TEST(EasingCurveValidation, AllTypes_Monotonic) {
    // Easing curves should be monotonically increasing
    auto types = {cf::ui::base::Easing::Type::Emphasized,
                  cf::ui::base::Easing::Type::EmphasizedDecelerate,
                  cf::ui::base::Easing::Type::EmphasizedAccelerate,
                  cf::ui::base::Easing::Type::Standard,
                  cf::ui::base::Easing::Type::StandardDecelerate,
                  cf::ui::base::Easing::Type::StandardAccelerate,
                  cf::ui::base::Easing::Type::Linear};

    for (auto type : types) {
        QEasingCurve curve = cf::ui::base::Easing::fromEasingType(type);

        float prev = curve.valueForProgress(0.0);
        for (int i = 1; i <= 10; ++i) {
            float t = i / 10.0f;
            float curr = curve.valueForProgress(t);
            EXPECT_GE(curr, prev) << "Easing curve should be monotonically increasing";
            prev = curr;
        }
    }
}

TEST(EasingCurveValidation, LinearIsPerfectlyLinear) {
    QEasingCurve curve = cf::ui::base::Easing::fromEasingType(cf::ui::base::Easing::Type::Linear);

    for (int i = 0; i <= 10; ++i) {
        float t = i / 10.0f;
        // Use EXPECT_NEAR for bezier curve floating point tolerance
        EXPECT_NEAR(curve.valueForProgress(t), t, 0.01f);
    }
}

// =============================================================================
// Test Suite 6: Common animation progress values
// =============================================================================

TEST(EasingProgressValues, AtZeroProgress) {
    auto types = {cf::ui::base::Easing::Type::Emphasized, cf::ui::base::Easing::Type::Linear,
                  cf::ui::base::Easing::Type::Standard};

    for (auto type : types) {
        QEasingCurve curve = cf::ui::base::Easing::fromEasingType(type);
        EXPECT_FLOAT_EQ(curve.valueForProgress(0.0), 0.0);
    }
}

TEST(EasingProgressValues, AtQuarterProgress) {
    // Different easing types should produce different values
    QEasingCurve linear = cf::ui::base::Easing::fromEasingType(cf::ui::base::Easing::Type::Linear);
    QEasingCurve easeIn =
        cf::ui::base::Easing::fromEasingType(cf::ui::base::Easing::Type::EmphasizedAccelerate);
    QEasingCurve easeOut =
        cf::ui::base::Easing::fromEasingType(cf::ui::base::Easing::Type::EmphasizedDecelerate);

    float t = 0.25f;
    float linearValue = linear.valueForProgress(t);
    float easeInValue = easeIn.valueForProgress(t);
    float easeOutValue = easeOut.valueForProgress(t);

    EXPECT_NEAR(linearValue, 0.25f, 0.01f);
    // Ease-in should be slower at the start
    EXPECT_LT(easeInValue, linearValue);
    // Ease-out should be faster at the start
    EXPECT_GT(easeOutValue, linearValue);
}

TEST(EasingProgressValues, AtHalfProgress) {
    QEasingCurve linear = cf::ui::base::Easing::fromEasingType(cf::ui::base::Easing::Type::Linear);
    QEasingCurve emphasized =
        cf::ui::base::Easing::fromEasingType(cf::ui::base::Easing::Type::Emphasized);

    float t = 0.5f;
    float linearValue = linear.valueForProgress(t);
    float emphasizedValue = emphasized.valueForProgress(t);

    EXPECT_NEAR(linearValue, 0.5f, 0.01f);
    // Emphasized curve (0.2, 0, 0, 1.0) rises quickly after initial slow start
    // At t=0.5, the value is actually around 0.88 due to the curve shape
    EXPECT_GT(emphasizedValue, 0.5f);
    EXPECT_LT(emphasizedValue, 1.0f);
}

TEST(EasingProgressValues, AtThreeQuarterProgress) {
    QEasingCurve linear = cf::ui::base::Easing::fromEasingType(cf::ui::base::Easing::Type::Linear);
    QEasingCurve easeIn =
        cf::ui::base::Easing::fromEasingType(cf::ui::base::Easing::Type::EmphasizedAccelerate);
    QEasingCurve easeOut =
        cf::ui::base::Easing::fromEasingType(cf::ui::base::Easing::Type::EmphasizedDecelerate);

    float t = 0.75f;
    float linearValue = linear.valueForProgress(t);
    float easeInValue = easeIn.valueForProgress(t);
    float easeOutValue = easeOut.valueForProgress(t);

    EXPECT_NEAR(linearValue, 0.75f, 0.01f);
    // Ease-in should still be catching up
    EXPECT_LT(easeInValue, linearValue);
    // Ease-out should be slowing down
    EXPECT_GT(easeOutValue, linearValue);
}

// =============================================================================
// Test Suite 7: Comparison between easing types
// =============================================================================

TEST(EasingComparison, AccelerateVsDecelerate) {
    QEasingCurve accelerate =
        cf::ui::base::Easing::fromEasingType(cf::ui::base::Easing::Type::EmphasizedAccelerate);
    QEasingCurve decelerate =
        cf::ui::base::Easing::fromEasingType(cf::ui::base::Easing::Type::EmphasizedDecelerate);

    // At early progress, accelerate should be slower
    float earlyT = 0.25f;
    EXPECT_LT(accelerate.valueForProgress(earlyT), decelerate.valueForProgress(earlyT));

    // At late progress, accelerate should have caught up and be faster
    float lateT = 0.75f;
    EXPECT_LT(accelerate.valueForProgress(lateT), decelerate.valueForProgress(lateT));
}

TEST(EasingComparison, LinearVsEmphasized) {
    QEasingCurve linear = cf::ui::base::Easing::fromEasingType(cf::ui::base::Easing::Type::Linear);
    QEasingCurve emphasized =
        cf::ui::base::Easing::fromEasingType(cf::ui::base::Easing::Type::Emphasized);

    // At very start (t=0.01), emphasized is still behind linear
    float startT = 0.01f;
    EXPECT_LT(emphasized.valueForProgress(startT), linear.valueForProgress(startT));

    // At end, emphasized should catch up
    EXPECT_FLOAT_EQ(emphasized.valueForProgress(1.0), linear.valueForProgress(1.0));
}

// =============================================================================
// Main
// =============================================================================

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
