/**
 * @file    desktop/ui/components/builtin_apps/calculator/core/ParseExceptions.h
 * @brief   Exception types thrown by the calculator expression parser.
 *
 * Ported from CCIMXDesktop's Caculator; parse logic unchanged, wrapped in
 * the calculator_core namespace. Each exception forwards its message to
 * std::runtime_error, so what() returns the constructed string directly.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-30
 * @version 0.1
 * @since   0.20
 * @ingroup calculator
 */

#pragma once

#include <QString>
#include <stdexcept>

namespace cf::desktop::desktop_component::calculator_core {

/**
 * @brief  Thrown when a division by zero is attempted.
 *
 * @ingroup calculator
 */
class DivideZeroException : public std::runtime_error {
  public:
    /**
     * @brief   Constructs the exception with a fixed message.
     * @throws  None.
     * @since   0.20
     * @ingroup calculator
     */
    DivideZeroException() : std::runtime_error("Divide Zero is not permitted!") {}
};

/**
 * @brief  Thrown when an unsupported symbol is encountered during parsing.
 *
 * @ingroup calculator
 */
class UnSupportedSymbol : public std::runtime_error {
  public:
    /**
     * @brief            Constructs the exception for @p symbol.
     * @param[in] symbol  The unsupported symbol.
     * @throws           None.
     * @since            0.20
     * @ingroup          calculator
     */
    explicit UnSupportedSymbol(const QString& symbol)
        : std::runtime_error(("Meeting unsolved symbol: " + symbol).toStdString()) {}
};

/**
 * @brief  Thrown when a numeric token cannot be parsed.
 *
 * @ingroup calculator
 */
class InvalidNumber : public std::runtime_error {
  public:
    /**
     * @brief   Constructs the exception with a fixed message.
     * @throws  None.
     * @since   0.20
     * @ingroup calculator
     */
    InvalidNumber() : std::runtime_error("Meeting InvalidNumber!") {}
};

/**
 * @brief  Thrown when an expression has unbalanced parentheses or structure.
 *
 * @ingroup calculator
 */
class UnSymmetryExpression : public std::runtime_error {
  public:
    /**
     * @brief   Constructs the exception with a fixed message.
     * @throws  None.
     * @since   0.20
     * @ingroup calculator
     */
    UnSymmetryExpression() : std::runtime_error("UnSymmetry Expression!") {}
};

/**
 * @brief  General-purpose exception for parse errors not covered by other types.
 *
 * @ingroup calculator
 */
class GeneralParseError : public std::runtime_error {
  public:
    /**
     * @brief   Constructs the exception with a fixed message.
     * @throws  None.
     * @since   0.20
     * @ingroup calculator
     */
    GeneralParseError() : std::runtime_error("Some Parse Error!") {}
};

/**
 * @brief  Thrown when an unsupported function name is encountered.
 *
 * @ingroup calculator
 */
class UnSupportiveFunction : public std::runtime_error {
  public:
    /**
     * @brief            Constructs the exception for @p function.
     * @param[in] function  The unsupported function name.
     * @throws           None.
     * @since            0.20
     * @ingroup          calculator
     */
    explicit UnSupportiveFunction(const QString& function)
        : std::runtime_error(("Meeting unsolved function: " + function).toStdString()) {}
};

/**
 * @brief  Thrown when sqrt receives a negative value.
 *
 * @ingroup calculator
 */
class BadSqrtValue : public std::runtime_error {
  public:
    /**
     * @brief   Constructs the exception with a fixed message.
     * @throws  None.
     * @since   0.20
     * @ingroup calculator
     */
    BadSqrtValue() : std::runtime_error("value sqrt is less then 0, which is not allowed!") {}
};

} // namespace cf::desktop::desktop_component::calculator_core
