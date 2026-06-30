/**
 * @file    desktop/ui/components/builtin_apps/calculator/core/ExpressionEvaluator.cpp
 * @brief   Implementation of the expression evaluator facade.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-30
 * @version 0.1
 * @since   0.20
 * @ingroup calculator
 */

#include "ExpressionEvaluator.h"

#include "Parser.h"
#include "TreeNodeBase.h"

#include <memory>

namespace cf::desktop::desktop_component::calculator_core {

namespace ExpressionEvaluator {

double evalute_expression(const QString& expr) {
    Parser parser;
    parser.setParserString(expr);
    std::unique_ptr<TreeNodeBase> root(parser.parse());
    return root->evaluate();
}

} // namespace ExpressionEvaluator

} // namespace cf::desktop::desktop_component::calculator_core
