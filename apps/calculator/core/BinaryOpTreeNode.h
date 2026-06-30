/**
 * @file    desktop/ui/components/builtin_apps/calculator/core/BinaryOpTreeNode.h
 * @brief   AST node for a binary operation (+, -, *, /, ^).
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-30
 * @version 0.1
 * @since   0.20
 * @ingroup calculator
 */

#pragma once

#include "ParseExceptions.h"
#include "TreeNodeBase.h"
#include <QChar>

namespace cf::desktop::desktop_component::calculator_core {

/**
 * @brief  AST node applying a binary operator to two subtrees.
 *
 * Owns its operand nodes and deletes them on destruction.
 *
 * @ingroup calculator
 */
struct BinaryOpTreeNode : TreeNodeBase {
    /**
     * @brief          Constructs the node.
     * @param[in] op    The operator character (+, -, *, /, ^).
     * @param[in] left_hand  The left operand subtree. Ownership transfers.
     * @param[in] right_hand  The right operand subtree. Ownership transfers.
     * @throws         None.
     * @since          0.20
     * @ingroup        calculator
     */
    BinaryOpTreeNode(QChar op, TreeNodeBase* left_hand, TreeNodeBase* right_hand);

    BinaryOpTreeNode() = delete;
    ~BinaryOpTreeNode() override;

    /**
     * @brief   Evaluates both operands and applies the operator.
     * @return  The computed value.
     * @throws  DivideZeroException on division by zero; UnSupportedSymbol on
     *          an unknown operator.
     * @since   0.20
     * @ingroup calculator
     */
    double evaluate() const override;

  private:
    QChar op;                 ///< The operator character.
    TreeNodeBase* left_hand;  ///< Left operand subtree (owned).
    TreeNodeBase* right_hand; ///< Right operand subtree (owned).
};

} // namespace cf::desktop::desktop_component::calculator_core
