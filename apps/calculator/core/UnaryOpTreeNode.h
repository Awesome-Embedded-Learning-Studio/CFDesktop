/**
 * @file    desktop/ui/components/builtin_apps/calculator/core/UnaryOpTreeNode.h
 * @brief   AST node for a unary operation (negation).
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
 * @brief  AST node applying a unary operator to one subtree.
 *
 * Currently only negation ('-') is supported. Owns its operand and deletes
 * it on destruction.
 *
 * @ingroup calculator
 */
class UnaryOpTreeNode : public TreeNodeBase {
  public:
    /**
     * @brief          Constructs the node.
     * @param[in] oper  The operator character (only '-' is supported).
     * @param[in] child  The operand subtree. Ownership transfers.
     * @throws         None.
     * @since          0.20
     * @ingroup        calculator
     */
    UnaryOpTreeNode(QChar oper, TreeNodeBase* child);

    ~UnaryOpTreeNode() override;

    /**
     * @brief   Evaluates the operand and applies the unary operator.
     * @return  The computed value.
     * @throws  UnSupportedSymbol if the operator is not '-'.
     * @since   0.20
     * @ingroup calculator
     */
    double evaluate() const override;

  private:
    QChar op;              ///< The operator character.
    TreeNodeBase* operand; ///< The operand subtree (owned).
};

} // namespace cf::desktop::desktop_component::calculator_core
