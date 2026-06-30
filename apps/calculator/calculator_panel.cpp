/**
 * @file    apps/calculator/calculator_panel.cpp
 * @brief   Implementation of the Calculator panel.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-30
 * @version 0.1
 * @since   0.20
 * @ingroup calculator
 */

#include "calculator_panel.h"

#include "core/ExpressionEvaluator.h"

#include "ui/widget/material/widget/button/button.h"
#include "ui/widget/material/widget/label/label.h"

#include <QGridLayout>
#include <QKeyEvent>
#include <QPainter>
#include <QPainterPath>
#include <QVBoxLayout>

#include <exception>

namespace cf::desktop::desktop_component {

namespace {
using calculator_core::ExpressionEvaluator::evalute_expression;
using qw::widget::material::Button;
using qw::widget::material::Label;
using qw::widget::material::TypographyStyle;
using Variant = Button::ButtonVariant;

constexpr qreal kCornerRadius = 16.0; ///< Card corner radius (px).

/// @brief Button definition: text + MD3 variant, laid out row-major in 4 cols.
struct BtnDef {
    QString text;
    Variant variant;
};

/// Keypad layout: digits + operators + scientific functions + CLR/DEL.
const QVector<BtnDef> kButtons = {
    {"7", Variant::Filled},     {"8", Variant::Filled},      {"9", Variant::Filled},
    {"/", Variant::Tonal},      {"4", Variant::Filled},      {"5", Variant::Filled},
    {"6", Variant::Filled},     {"*", Variant::Tonal},       {"1", Variant::Filled},
    {"2", Variant::Filled},     {"3", Variant::Filled},      {"-", Variant::Tonal},
    {"0", Variant::Filled},     {".", Variant::Filled},      {"=", Variant::Filled},
    {"+", Variant::Tonal},      {"sin", Variant::Outlined},  {"cos", Variant::Outlined},
    {"tan", Variant::Outlined}, {"sqrt", Variant::Outlined}, {"log", Variant::Outlined},
    {"exp", Variant::Outlined}, {"(", Variant::Tonal},       {")", Variant::Tonal},
    {"^", Variant::Tonal},      {"CLR", Variant::Text},      {"DEL", Variant::Text},
    {"", Variant::Text},
};

/// @brief Returns true if @p token is a named function (auto-appends '(').
bool isFunction(const QString& token) {
    return token == "sin" || token == "cos" || token == "tan" || token == "sqrt" ||
           token == "log" || token == "exp";
}
} // namespace

CalculatorPanel::CalculatorPanel(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(12);

    display_ = new Label("0", TypographyStyle::DisplayMedium, this);
    display_->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    layout->addWidget(display_);

    auto* grid = new QGridLayout;
    grid->setSpacing(8);
    for (int i = 0; i < kButtons.size(); ++i) {
        const auto& def = kButtons[i];
        if (def.text.isEmpty()) {
            continue;
        }
        auto* btn = new Button(def.text, def.variant, this);
        const QString token = def.text;
        connect(btn, &QPushButton::clicked, this, [this, token]() { onButton(token); });
        grid->addWidget(btn, i / 4, i % 4);
    }
    layout->addLayout(grid);
}

CalculatorPanel::~CalculatorPanel() = default;

void CalculatorPanel::onButton(const QString& text) {
    if (text == "=") {
        try {
            const double result = evalute_expression(expression_);
            expression_ = QString::number(result);
        } catch (const std::exception& e) {
            // No silent fallback: show the parser's error message.
            expression_ = QString::fromUtf8(e.what());
        }
    } else if (text == "CLR") {
        expression_.clear();
    } else if (text == "DEL") {
        expression_.chop(1);
    } else if (isFunction(text)) {
        expression_ += text + "(";
    } else {
        expression_ += text;
    }
    refreshDisplay();
}

void CalculatorPanel::refreshDisplay() {
    display_->setText(expression_.isEmpty() ? QStringLiteral("0") : expression_);
}

void CalculatorPanel::paintEvent(QPaintEvent* /*event*/) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    const QColor surface_color(0xF7, 0xF5, 0xF3);
    QPainterPath card;
    card.addRoundedRect(QRectF(rect()), kCornerRadius, kCornerRadius);
    p.fillPath(card, surface_color);
}

void CalculatorPanel::keyPressEvent(QKeyEvent* event) {
    const int key = event->key();
    if (key == Qt::Key_Backspace) {
        onButton("DEL");
        return;
    }
    if (key == Qt::Key_Return || key == Qt::Key_Enter) {
        onButton("=");
        return;
    }
    if (key == Qt::Key_Escape) {
        close();
        return;
    }
    const QString ch = event->text();
    if (ch.length() == 1) {
        const QChar c = ch[0];
        if (c.isDigit() || QString("+-*/^().").contains(c)) {
            expression_ += ch;
            refreshDisplay();
            return;
        }
    }
    QWidget::keyPressEvent(event);
}

} // namespace cf::desktop::desktop_component
