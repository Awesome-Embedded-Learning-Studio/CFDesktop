/**
 * @file    desktop/ui/components/builtin_apps/calculator/core/NumberNode.cpp
 * @brief   Implementation of NumberNode.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-30
 * @version 0.1
 * @since   0.20
 * @ingroup calculator
 */

#include "NumberNode.h"

namespace cf::desktop::desktop_component::calculator_core {

NumberNode::NumberNode(const double val) : stored_value(val) {}

double NumberNode::evaluate() const {
    return stored_value;
}

} // namespace cf::desktop::desktop_component::calculator_core
