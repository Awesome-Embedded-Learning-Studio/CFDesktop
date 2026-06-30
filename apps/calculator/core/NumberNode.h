/**
 * @file    desktop/ui/components/builtin_apps/calculator/core/NumberNode.h
 * @brief   AST leaf node holding a numeric literal.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-30
 * @version 0.1
 * @since   0.20
 * @ingroup calculator
 */

#pragma once

#include "TreeNodeBase.h"

namespace cf::desktop::desktop_component::calculator_core {

/**
 * @brief  AST leaf node holding a numeric literal.
 *
 * @ingroup calculator
 */
struct NumberNode : TreeNodeBase {
    /**
     * @brief   Constructs the node with value @p val.
     * @param[in] val  The numeric value to store.
     * @throws  None.
     * @since   0.20
     * @ingroup calculator
     */
    explicit NumberNode(double val);

    NumberNode() = delete;
    NumberNode(const NumberNode&) = delete;
    NumberNode& operator=(const NumberNode&) = delete;
    NumberNode(NumberNode&&) = delete;
    NumberNode& operator=(NumberNode&&) = delete;

    /**
     * @brief   Destructs the node.
     * @throws  None.
     * @since   0.20
     * @ingroup calculator
     */
    ~NumberNode() override = default;

    /**
     * @brief   Returns the stored value.
     * @return  The numeric value.
     * @throws  None.
     * @since   0.20
     * @ingroup calculator
     */
    double evaluate() const override;

  private:
    double stored_value{0.0}; ///< The literal value.
};

} // namespace cf::desktop::desktop_component::calculator_core
