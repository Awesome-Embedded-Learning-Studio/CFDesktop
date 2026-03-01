/**
 * @file math_helper.cpp
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Math Helpers for the UI Widgets
 * @version 0.1
 * @date 2026-02-23
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "math_helper.h"
#include <cmath>

namespace cf::ui {
namespace math {

float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

float clamp(float value, float min, float max) {
    if (value < min)
        return min;
    if (value > max)
        return max;
    return value;
}

float remap(float value, float inMin, float inMax, float outMin, float outMax) {
    // Handle edge case where input range is zero
    // When input range is zero, value at or below min returns output max, value above min returns
    // output min
    if (inMax == inMin) {
        return (value <= inMin) ? outMax : outMin;
    }
    return outMin + (value - inMin) * (outMax - outMin) / (inMax - inMin);
}

float cubicBezier(float x1, float y1, float x2, float y2, float t) {
    // For proper CSS cubic-bezier behavior, we need to solve x(t) = input_time
    // and return y(t). This is a simplified implementation that handles
    // common cases correctly.

    // Special case: linear bezier - returns input directly
    if (std::abs(x1 - y1) < 0.001f && std::abs(x2 - y2) < 0.001f) {
        return t;
    }

    // For other cases, use Newton's method to solve for the parameter
    // that gives x(t) = input, then return y(t)
    // Start with initial guess equal to input
    float guess = t;

    // Newton iteration: guess_{n+1} = guess_n - (x(guess_n) - t) / x'(guess_n)
    for (int i = 0; i < 8; ++i) {
        float mt = 1.0f - guess;
        float mt2 = mt * mt;
        float t2 = guess * guess;

        // x(guess) = 3(1-t)²t*x1 + 3(1-t)t²*x2 + t³
        float x_guess = 3.0f * mt2 * guess * x1 + 3.0f * mt * t2 * x2 + guess * t2;

        // x'(guess) = 3(1-t)²*x1 + 6(1-t)t*(x2-x1) + 3t²*(1-x2)
        float x_derivative =
            3.0f * mt2 * x1 + 6.0f * mt * guess * (x2 - x1) + 3.0f * t2 * (1.0f - x2);

        float delta = (x_guess - t) / x_derivative;
        guess -= delta;

        if (std::abs(delta) < 0.0001f) {
            break;
        }
    }

    // Clamp guess to [0, 1]
    if (guess < 0.0f)
        guess = 0.0f;
    if (guess > 1.0f)
        guess = 1.0f;

    // Compute y at the solved parameter
    float mt = 1.0f - guess;
    float mt2 = mt * mt;
    float t2 = guess * guess;

    float y = 3.0f * mt2 * guess * y1 + 3.0f * mt * t2 * y2 + guess * t2;
    return y;
}

std::pair<float, float> springStep(float position, float velocity, float target, float stiffness,
                                   float damping, float dt) {
    // Damped spring using semi-implicit Euler integration
    float acceleration = stiffness * (target - position) - damping * velocity;
    float newVelocity = velocity + acceleration * dt;
    float newPosition = position + newVelocity * dt;
    return {newPosition, newVelocity};
}

float lerpAngle(float a, float b, float t) {
    constexpr float PI = 3.14159265358979323846f;
    constexpr float TWO_PI = 2.0f * PI;
    constexpr float DEG_TO_RAD = PI / 180.0f;
    constexpr float RAD_TO_DEG = 180.0f / PI;

    float aRad = a * DEG_TO_RAD;
    float bRad = b * DEG_TO_RAD;

    // Calculate the shortest path difference from a to b
    float diff = bRad - aRad;

    // Normalize to [-PI, PI]
    while (diff <= -PI)
        diff += TWO_PI;
    while (diff > PI)
        diff -= TWO_PI;

    // Handle the exact 180 degree case - prefer positive direction
    if (std::abs(diff - PI) < 0.0001f) {
        diff = PI;
    } else if (std::abs(diff + PI) < 0.0001f) {
        diff = PI;
    }

    float result = aRad + diff * t;

    // Normalize result to [0, TWO_PI)
    while (result < 0)
        result += TWO_PI;
    while (result >= TWO_PI)
        result -= TWO_PI;

    return result * RAD_TO_DEG;
}

} // namespace math
} // namespace cf::ui