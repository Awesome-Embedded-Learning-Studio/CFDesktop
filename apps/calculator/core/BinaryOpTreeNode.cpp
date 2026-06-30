/**
 * @file    desktop/ui/components/builtin_apps/calculator/core/BinaryOpTreeNode.cpp
 * @brief   Implementation of BinaryOpTreeNode.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-30
 * @version 0.1
 * @since   0.20
 * @ingroup calculator
 */

#include "BinaryOpTreeNode.h"

#include <QMap>
#include <cmath>
#include <functional>

namespace cf::desktop::desktop_component::calculator_core {

namespace {
/// Mapping of operator character to the binary arithmetic lambda.
const QMap<QChar, std::function<double(double, double)>> kMappings = {
    {'+', [](double a, double b) { return a + b; }},
    {'-', [](double a, double b) { return a - b; }},
    {'*', [](double a, double b) { return a * b; }},
    {'/', [](double a, double b) { return b != 0 ? a / b : throw DivideZeroException(); }},
    {'^', [](double a, double b) { return std::pow(a, b); }},
};
} // namespace

BinaryOpTreeNode::BinaryOpTreeNode(const QChar op, TreeNodeBase* left_hand,
                                   TreeNodeBase* right_hand)
    : op(op), left_hand(left_hand), right_hand(right_hand) {}

BinaryOpTreeNode::~BinaryOpTreeNode() {
    delete left_hand;
    delete right_hand;
}

double BinaryOpTreeNode::evaluate() const {
    const double l = left_hand->evaluate();
    const double r = right_hand->evaluate();
    const auto it = kMappings.find(op);
    if (it != kMappings.end()) {
        return it.value()(l, r);
    }
    throw UnSupportedSymbol(QString(op));
}

} // namespace cf::desktop::desktop_component::calculator_core
