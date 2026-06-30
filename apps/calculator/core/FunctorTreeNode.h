/**
 * @file    desktop/ui/components/builtin_apps/calculator/core/FunctorTreeNode.h
 * @brief   AST node for a named function call (sin, cos, sqrt, ...).
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-30
 * @version 0.1
 * @since   0.20
 * @ingroup calculator
 */

#pragma once

#include "TreeNodeBase.h"
#include <QString>

namespace cf::desktop::desktop_component::calculator_core {

/**
 * @brief  AST node applying a named function to one argument subtree.
 *
 * Supports sin, cos, tan, sqrt, log, exp. Owns its argument node and
 * deletes it on destruction.
 *
 * @ingroup calculator
 */
class FunctorTreeNode : public TreeNodeBase {
  public:
    /**
     * @brief          Constructs the node.
     * @param[in] name  The function name (e.g. "sin").
     * @param[in] arg   The argument subtree. Ownership transfers.
     * @throws         None.
     * @since          0.20
     * @ingroup        calculator
     */
    FunctorTreeNode(QString name, TreeNodeBase* arg);

    ~FunctorTreeNode() override;

    /**
     * @brief   Evaluates the argument and applies the function.
     * @return  The computed value.
     * @throws  UnSupportiveFunction if the name is unknown; BadSqrtValue if
     *          sqrt receives a negative value.
     * @since   0.20
     * @ingroup calculator
     */
    double evaluate() const override;

  private:
    QString name;           ///< The function name.
    TreeNodeBase* argument; ///< The argument subtree (owned).
};

} // namespace cf::desktop::desktop_component::calculator_core
