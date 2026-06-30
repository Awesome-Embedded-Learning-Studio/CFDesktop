/**
 * @file    desktop/ui/components/builtin_apps/calculator/core/ExpressionEvaluator.h
 * @brief   High-level facade that parses and evaluates an expression string.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-30
 * @version 0.1
 * @since   0.20
 * @ingroup calculator
 */

#pragma once

#include <QString>

namespace cf::desktop::desktop_component::calculator_core {

/**
 * @brief  High-level facade for parsing and evaluating an expression.
 *
 * @ingroup calculator
 */
namespace ExpressionEvaluator {

/**
 * @brief          Parses and evaluates @p expr in one shot.
 * @param[in] expr  The expression string (e.g. "1 + 2 * sin(0)").
 * @return         The computed value.
 * @throws         Parser exceptions on malformed input or evaluation errors
 *                 (div-by-zero, unsupported symbol/function, bad sqrt, ...).
 * @since          0.20
 * @ingroup        calculator
 */
double evalute_expression(const QString& expr);

} // namespace ExpressionEvaluator

} // namespace cf::desktop::desktop_component::calculator_core
