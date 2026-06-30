/**
 * @file    desktop/ui/components/builtin_apps/calculator/core/Parser.h
 * @brief   Recursive-descent parser that builds the calculator AST.
 *
 * Ported from CCIMXDesktop's Caculator; parse logic unchanged, wrapped in
 * the calculator_core namespace.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-30
 * @version 0.1
 * @since   0.20
 * @ingroup calculator
 */

#pragma once

#include <QString>

namespace cf::desktop::desktop_component::calculator_core {

class TreeNodeBase;

/**
 * @brief  Parses a mathematical expression string into an AST.
 *
 * Grammar (lowest to highest precedence): expression = term (('+'|'-') term)*;
 * term = factor (('*'|'/'|'^') factor)*; factor = '-' factor | func '(' expr
 * ')' | '(' expr ')' | number.
 *
 * @ingroup calculator
 */
class Parser {
  public:
    /**
     * @brief   Default-constructs the parser.
     * @throws  None.
     * @since   0.20
     * @ingroup calculator
     */
    Parser() = default;

    /**
     * @brief   Destructs the parser.
     * @throws  None.
     * @since   0.20
     * @ingroup calculator
     */
    ~Parser() = default;

    /**
     * @brief          Sets the expression string to parse.
     * @param[in] p     The input expression.
     * @throws         None.
     * @since          0.20
     * @ingroup        calculator
     */
    void setParserString(const QString& p);

    /**
     * @brief   Returns the currently set expression string.
     * @return  The expression string.
     * @throws  None.
     * @since   0.20
     * @ingroup calculator
     */
    QString parserString() const;

    /**
     * @brief   Parses the full expression and returns the AST root.
     * @return  Pointer to the root node; caller owns the tree.
     * @throws  GeneralParseError on trailing input; UnSupportedSymbol,
     *          UnSymmetryExpression, InvalidNumber on malformed input.
     * @since   0.20
     * @ingroup calculator
     */
    TreeNodeBase* parse();

  private:
    /// @brief Peeks the character at the current position without advancing.
    QChar peekPos();
    /// @brief Returns the current character and advances the position.
    QChar getChar();
    /// @brief Skips whitespace at the current position.
    void skipIgnored();
    /// @brief Parses an expression (handles +, -).
    TreeNodeBase* parseExpression();
    /// @brief Parses a term (handles *, /, ^).
    TreeNodeBase* parseTerm();
    /// @brief Parses a factor (unary, function, parenthesized, number).
    TreeNodeBase* parseFactor();
    /// @brief Parses a numeric literal.
    TreeNodeBase* parseNumber();
    /// @brief Parses an identifier (function name).
    QString parseIdentifier();

    QString handle_expression; ///< The expression being parsed.
    int parse_pos{0};          ///< Current parse position.
};

} // namespace cf::desktop::desktop_component::calculator_core
