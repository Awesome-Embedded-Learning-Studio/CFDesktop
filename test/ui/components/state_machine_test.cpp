/**
 * @file state_machine_test.cpp
 * @brief Unit tests for StateMachine (Material Behavior Layer)
 *
 * Test Coverage:
 * 1. State transitions and flags
 * 2. Opacity calculation for each state
 * 3. State priority handling
 * 4. Signal emission
 */

#include "ui/widget/material/base/state_machine.h"
#include <gtest/gtest.h>

using namespace cf::ui::widget::material::base;

// =============================================================================
// Test Suite 1: State Transitions
// =============================================================================

TEST(StateMachineTest, InitialState_IsNormal) {
    // Create state machine with null factory (for testing without animation)
    StateMachine sm(nullptr, nullptr);
    EXPECT_EQ(sm.currentState(), StateMachine::State::StateNormal);
    EXPECT_FALSE(sm.hasState(StateMachine::State::StateHovered));
    EXPECT_FALSE(sm.hasState(StateMachine::State::StatePressed));
}

TEST(StateMachineTest, OnHoverEnter_SetsHoveredState) {
    StateMachine sm(nullptr, nullptr);
    sm.onHoverEnter();
    EXPECT_TRUE(sm.hasState(StateMachine::State::StateHovered));
}

TEST(StateMachineTest, OnHoverLeave_ClearsHoveredState) {
    StateMachine sm(nullptr, nullptr);
    sm.onHoverEnter();
    sm.onHoverLeave();
    EXPECT_FALSE(sm.hasState(StateMachine::State::StateHovered));
}

TEST(StateMachineTest, OnPress_SetsPressedState) {
    StateMachine sm(nullptr, nullptr);
    sm.onPress(QPoint(10, 10));
    EXPECT_TRUE(sm.hasState(StateMachine::State::StatePressed));
}

TEST(StateMachineTest, OnRelease_ClearsPressedState) {
    StateMachine sm(nullptr, nullptr);
    sm.onPress(QPoint(10, 10));
    sm.onRelease();
    EXPECT_FALSE(sm.hasState(StateMachine::State::StatePressed));
}

TEST(StateMachineTest, OnFocusIn_SetsFocusedState) {
    StateMachine sm(nullptr, nullptr);
    sm.onFocusIn();
    EXPECT_TRUE(sm.hasState(StateMachine::State::StateFocused));
}

TEST(StateMachineTest, OnFocusOut_ClearsFocusedState) {
    StateMachine sm(nullptr, nullptr);
    sm.onFocusIn();
    sm.onFocusOut();
    EXPECT_FALSE(sm.hasState(StateMachine::State::StateFocused));
}

TEST(StateMachineTest, OnDisable_SetsDisabledState) {
    StateMachine sm(nullptr, nullptr);
    sm.onDisable();
    EXPECT_TRUE(sm.hasState(StateMachine::State::StateDisabled));
}

TEST(StateMachineTest, OnEnable_ClearsDisabledState) {
    StateMachine sm(nullptr, nullptr);
    sm.onDisable();
    sm.onEnable();
    EXPECT_FALSE(sm.hasState(StateMachine::State::StateDisabled));
}

TEST(StateMachineTest, OnCheckedChanged_SetsCheckedState) {
    StateMachine sm(nullptr, nullptr);
    sm.onCheckedChanged(true);
    EXPECT_TRUE(sm.hasState(StateMachine::State::StateChecked));
}

TEST(StateMachineTest, OnCheckedChangedFalse_ClearsCheckedState) {
    StateMachine sm(nullptr, nullptr);
    sm.onCheckedChanged(true);
    sm.onCheckedChanged(false);
    EXPECT_FALSE(sm.hasState(StateMachine::State::StateChecked));
}

// =============================================================================
// Test Suite 2: Combined States
// =============================================================================

TEST(StateMachineTest, HoverAndPressed_CanCoexist) {
    StateMachine sm(nullptr, nullptr);
    sm.onHoverEnter();
    sm.onPress(QPoint(10, 10));
    EXPECT_TRUE(sm.hasState(StateMachine::State::StateHovered));
    EXPECT_TRUE(sm.hasState(StateMachine::State::StatePressed));
}

TEST(StateMachineTest, FocusAndHover_CanCoexist) {
    StateMachine sm(nullptr, nullptr);
    sm.onFocusIn();
    sm.onHoverEnter();
    EXPECT_TRUE(sm.hasState(StateMachine::State::StateFocused));
    EXPECT_TRUE(sm.hasState(StateMachine::State::StateHovered));
}

// =============================================================================
// Test Suite 3: Opacity Calculation
// =============================================================================

TEST(StateMachineTest, Opacity_NormalState_IsZero) {
    StateMachine sm(nullptr, nullptr);
    EXPECT_FLOAT_EQ(sm.stateLayerOpacity(), 0.0f);
}

TEST(StateMachineTest, Opacity_HoveredState_Is008) {
    StateMachine sm(nullptr, nullptr);
    sm.onHoverEnter();
    // Without animation factory, opacity should be set directly
    EXPECT_FLOAT_EQ(sm.stateLayerOpacity(), 0.08f);
}

TEST(StateMachineTest, Opacity_PressedState_Is012) {
    StateMachine sm(nullptr, nullptr);
    sm.onPress(QPoint(10, 10));
    EXPECT_FLOAT_EQ(sm.stateLayerOpacity(), 0.12f);
}

TEST(StateMachineTest, Opacity_FocusedState_Is012) {
    StateMachine sm(nullptr, nullptr);
    sm.onFocusIn();
    EXPECT_FLOAT_EQ(sm.stateLayerOpacity(), 0.12f);
}

TEST(StateMachineTest, Opacity_CheckedState_Is008) {
    StateMachine sm(nullptr, nullptr);
    sm.onCheckedChanged(true);
    EXPECT_FLOAT_EQ(sm.stateLayerOpacity(), 0.08f);
}

TEST(StateMachineTest, Opacity_DisabledState_IsZero) {
    StateMachine sm(nullptr, nullptr);
    sm.onHoverEnter();
    sm.onDisable();
    // Disabled overrides all other states
    EXPECT_FLOAT_EQ(sm.stateLayerOpacity(), 0.0f);
}

// =============================================================================
// Test Suite 4: State Priority
// =============================================================================

TEST(StateMachineTest, Priority_PressedOverHovered) {
    StateMachine sm(nullptr, nullptr);
    sm.onHoverEnter();
    sm.onPress(QPoint(10, 10));
    // Pressed has higher priority than hovered
    EXPECT_FLOAT_EQ(sm.stateLayerOpacity(), 0.12f); // Pressed value
}

TEST(StateMachineTest, Priority_DisabledOverAll) {
    StateMachine sm(nullptr, nullptr);
    sm.onHoverEnter();
    sm.onPress(QPoint(10, 10));
    sm.onFocusIn();
    sm.onDisable();
    // Disabled overrides everything
    EXPECT_TRUE(sm.hasState(StateMachine::State::StateDisabled));
}

// =============================================================================
// Main
// =============================================================================

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
