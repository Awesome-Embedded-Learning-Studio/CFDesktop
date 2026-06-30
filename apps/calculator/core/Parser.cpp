/**
 * @file    desktop/ui/components/builtin_apps/calculator/core/Parser.cpp
 * @brief   Implementation of the recursive-descent parser.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-30
 * @version 0.1
 * @since   0.20
 * @ingroup calculator
 */

#include "Parser.h"

#include "BinaryOpTreeNode.h"
#include "FunctorTreeNode.h"
#include "NumberNode.h"
#include "ParseExceptions.h"
#include "UnaryOpTreeNode.h"

namespace cf::desktop::desktop_component::calculator_core {

void Parser::setParserString(const QString& p) {
    handle_expression = p;
    parse_pos = 0;
}

QString Parser::parserString() const {
    return handle_expression;
}

TreeNodeBase* Parser::parse() {
    TreeNodeBase* node = parseExpression();
    if (parse_pos < handle_expression.length()) {
        throw GeneralParseError();
    }
    return node;
}

QChar Parser::peekPos() {
    return parse_pos < handle_expression.length() ? handle_expression[parse_pos] : QChar();
}

QChar Parser::getChar() {
    return parse_pos < handle_expression.length() ? handle_expression[parse_pos++] : QChar();
}

void Parser::skipIgnored() {
    while (peekPos().isSpace()) {
        ++parse_pos;
    }
}

TreeNodeBase* Parser::parseExpression() {
    TreeNodeBase* node = parseTerm();
    while (true) {
        skipIgnored();
        const QChar op = peekPos();
        if (op == '+' || op == '-') {
            getChar();
            TreeNodeBase* rhs = parseTerm();
            node = new BinaryOpTreeNode(op, node, rhs);
        } else {
            break;
        }
    }
    return node;
}

TreeNodeBase* Parser::parseTerm() {
    TreeNodeBase* node = parseFactor();
    while (true) {
        skipIgnored();
        const QChar op = peekPos();
        if (op == '*' || op == '/' || op == '^') {
            getChar();
            TreeNodeBase* rhs = parseFactor();
            node = new BinaryOpTreeNode(op, node, rhs);
        } else {
            break;
        }
    }
    return node;
}

TreeNodeBase* Parser::parseFactor() {
    skipIgnored();
    const QChar ch = peekPos();

    if (ch == '-') {
        getChar();
        TreeNodeBase* node = parseFactor();
        return new UnaryOpTreeNode('-', node);
    }

    if (ch.isLetter()) {
        const QString func = parseIdentifier();
        skipIgnored();
        if (getChar() != '(') {
            throw UnSymmetryExpression();
        }
        TreeNodeBase* arg = parseExpression();
        skipIgnored();
        if (getChar() != ')') {
            throw UnSymmetryExpression();
        }
        return new FunctorTreeNode(func, arg);
    }

    if (ch == '(') {
        getChar();
        TreeNodeBase* node = parseExpression();
        skipIgnored();
        if (getChar() != ')') {
            throw UnSymmetryExpression();
        }
        return node;
    }

    if (ch.isDigit() || ch == '.') {
        return parseNumber();
    }

    throw UnSupportedSymbol(QString(ch));
}

TreeNodeBase* Parser::parseNumber() {
    skipIgnored();
    const int start = parse_pos;
    while (peekPos().isDigit() || peekPos() == '.') {
        ++parse_pos;
    }
    bool ok = false;
    const double val = handle_expression.mid(start, parse_pos - start).toDouble(&ok);
    if (!ok) {
        throw InvalidNumber();
    }
    return new NumberNode(val);
}

QString Parser::parseIdentifier() {
    skipIgnored();
    const int start = parse_pos;
    while (peekPos().isLetter()) {
        ++parse_pos;
    }
    return handle_expression.mid(start, parse_pos - start);
}

} // namespace cf::desktop::desktop_component::calculator_core
