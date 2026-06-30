/**
 * @file    test/desktop/calculator/parser_test.cpp
 * @brief   GoogleTest port of CCIMXDesktop's Caculator test_parser.
 *
 * Covers the same cases as the upstream test_parser.cpp (basic arithmetic,
 * precedence, unary negation, functions, nesting, edge cases, and error
 * throws) to confirm the port is behaviorally equivalent.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-30
 * @version 0.1
 * @since   0.20
 * @ingroup calculator
 */

#include "ExpressionEvaluator.h"

#include <QString>
#include <gtest/gtest.h>

#include <exception>

namespace {
namespace eval = cf::desktop::desktop_component::calculator_core::ExpressionEvaluator;

/// Evaluates @p expr and expects it near @p expected; fails on any throw.
void expectEq(const QString& expr, double expected, double tol = 1e-3) {
    double result = 0;
    try {
        result = eval::evalute_expression(expr);
    } catch (const std::exception& e) {
        FAIL() << expr.toStdString() << " threw: " << e.what();
    }
    EXPECT_NEAR(result, expected, tol) << expr.toStdString();
}
} // namespace

TEST(CalculatorParser, BasicArithmetic) {
    expectEq("1 + 2", 3);
    expectEq("5 - 3", 2);
    expectEq("2 * 4", 8);
    expectEq("8 / 2", 4);
}

TEST(CalculatorParser, Precedence) {
    expectEq("2 + 3 * 4", 14);
    expectEq("(2 + 3) * 4", 20);
    expectEq("10 - 2 - 3", 5);
}

TEST(CalculatorParser, UnaryNegation) {
    expectEq("-5", -5);
    expectEq("-(2 + 3)", -5);
    expectEq("4 + -2", 2);
}

TEST(CalculatorParser, Functions) {
    expectEq("sin(3.1415926 / 2)", 1.0);
    expectEq("cos(0)", 1.0);
    expectEq("sqrt(16)", 4.0);
    expectEq("log(1)", 0.0);
}

TEST(CalculatorParser, NestedExpressions) {
    expectEq("((2+3)*(4+1))", 25.0);
    expectEq("sqrt(4 + sqrt(16))", 2.828);
}

TEST(CalculatorParser, BoundaryValues) {
    expectEq("0", 0.0);
    expectEq("0.00001 + 1", 1.00001);
    expectEq("999999 + 1", 1000000.0);
}

TEST(CalculatorParser, DeepNesting) {
    expectEq("(((((((1+2)))))))", 3);
    expectEq("(((((3+5)*2)-4)/2)+1)", 7);
}

TEST(CalculatorParser, FunctionNesting) {
    expectEq("sqrt(sin(3.1415926/2) + cos(0))", std::sqrt(1.0 + 1.0));
    expectEq("sqrt(sqrt(16))", 2.0);
}

TEST(CalculatorParser, UnaryWithFunction) {
    expectEq("-sin(3.1415926)", -std::sin(3.1415926));
    expectEq("-sqrt(9)", -3.0);
    expectEq("sqrt(-3 * -3)", 3.0);
}

TEST(CalculatorParser, MultipleNegation) {
    expectEq("--2", 2.0);
    expectEq("---2", -2.0);
    expectEq("4 + --1", 5.0);
}

TEST(CalculatorParser, LargeAndSmallNumbers) {
    expectEq("1.0000000001 + 1", 2.0000000001);
}

TEST(CalculatorParser, DecimalTolerance) {
    expectEq(".5 + .5", 1.0);
    expectEq("5.", 5.0);
    expectEq("0.1 + 0.2", 0.3);
}

TEST(CalculatorParser, MultiDivision) {
    expectEq("100 / 2 / 5", 10.0);
    expectEq("8 / 2 * (2 + 2)", 16.0);
}

TEST(CalculatorParser, TrigIdentity) {
    expectEq("sin(0)^2 + cos(0)^2", 1.0);
    expectEq("log(exp(3))", 3.0);
}

TEST(CalculatorParser, ThrowsOnMalformed) {
    EXPECT_ANY_THROW(eval::evalute_expression("2 +"));
    EXPECT_ANY_THROW(eval::evalute_expression("((3+2)"));
    EXPECT_ANY_THROW(eval::evalute_expression("abc(3)"));
    EXPECT_ANY_THROW(eval::evalute_expression("2 + * 3"));
    EXPECT_ANY_THROW(eval::evalute_expression("3 / 0"));
    EXPECT_ANY_THROW(eval::evalute_expression("10 / (5 - 5)"));
    EXPECT_ANY_THROW(eval::evalute_expression("sqrt(-1)"));
    EXPECT_ANY_THROW(eval::evalute_expression("sin"));
    EXPECT_ANY_THROW(eval::evalute_expression("sin()"));
    EXPECT_ANY_THROW(eval::evalute_expression("()"));
    EXPECT_ANY_THROW(eval::evalute_expression("..2 + 1"));
    EXPECT_ANY_THROW(eval::evalute_expression("1 + 2 3"));
}
