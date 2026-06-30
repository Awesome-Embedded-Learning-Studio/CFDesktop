/**
 * @file    desktop/ui/components/builtin_apps/calculator/core/TreeNodeBase.h
 * @brief   Abstract base node of the calculator expression AST.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-30
 * @version 0.1
 * @since   0.20
 * @ingroup calculator
 */

#pragma once

namespace cf::desktop::desktop_component::calculator_core {

/**
 * @brief  Abstract base node of the calculator expression AST.
 *
 * Concrete nodes (NumberNode, BinaryOpTreeNode, UnaryOpTreeNode,
 * FunctorTreeNode) implement evaluate() to compute their subtree.
 *
 * @ingroup calculator
 */
struct TreeNodeBase {
    virtual ~TreeNodeBase() = default;

    /**
     * @brief   Evaluates the expression subtree.
     *
     * @return  The computed value.
     *
     * @throws  Parser exceptions on evaluation errors (div-by-zero,
     *          unsupported symbol/function, bad sqrt, etc.).
     *
     * @since   0.20
     * @ingroup calculator
     */
    virtual double evaluate() const = 0;
};

} // namespace cf::desktop::desktop_component::calculator_core
