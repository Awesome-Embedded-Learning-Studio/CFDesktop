/**
 * @file    desktop/ui/components/builtin_apps/calculator/core/FunctorTreeNode.cpp
 * @brief   Implementation of FunctorTreeNode.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-30
 * @version 0.1
 * @since   0.20
 * @ingroup calculator
 */

#include "FunctorTreeNode.h"

#include "ParseExceptions.h"

#include <QMap>
#include <cmath>
#include <functional>

namespace cf::desktop::desktop_component::calculator_core {

namespace {
/// Mapping of function name to the single-argument math lambda.
const QMap<QString, std::function<double(double)>> kFunctors = {
    {"sin", [](double x) { return std::sin(x); }},
    {"cos", [](double x) { return std::cos(x); }},
    {"tan", [](double x) { return std::tan(x); }},
    {"sqrt",
     [](double x) {
         if (x < 0) {
             throw BadSqrtValue();
         }
         return std::sqrt(x);
     }},
    {"log", [](double x) { return std::log(x); }},
    {"exp", [](double x) { return std::exp(x); }},
};
} // namespace

FunctorTreeNode::FunctorTreeNode(QString name, TreeNodeBase* arg)
    : name(std::move(name)), argument(arg) {}

FunctorTreeNode::~FunctorTreeNode() {
    delete argument;
}

double FunctorTreeNode::evaluate() const {
    const double val = argument->evaluate();
    const auto it = kFunctors.find(name);
    if (it != kFunctors.end()) {
        return it.value()(val);
    }
    throw UnSupportiveFunction(name);
}

} // namespace cf::desktop::desktop_component::calculator_core
