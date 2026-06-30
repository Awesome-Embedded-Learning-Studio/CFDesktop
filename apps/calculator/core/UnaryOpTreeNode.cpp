/**
 * @file    desktop/ui/components/builtin_apps/calculator/core/UnaryOpTreeNode.cpp
 * @brief   Implementation of UnaryOpTreeNode.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-30
 * @version 0.1
 * @since   0.20
 * @ingroup calculator
 */

#include "UnaryOpTreeNode.h"

namespace cf::desktop::desktop_component::calculator_core {

UnaryOpTreeNode::UnaryOpTreeNode(const QChar oper, TreeNodeBase* child)
    : op(oper), operand(child) {}

UnaryOpTreeNode::~UnaryOpTreeNode() {
    delete operand;
}

double UnaryOpTreeNode::evaluate() const {
    const double val = operand->evaluate();
    if (op == '-') {
        return -val;
    }
    throw UnSupportedSymbol(QString(op));
}

} // namespace cf::desktop::desktop_component::calculator_core
