/**
 * @file    calculator_builtin_panel.cpp
 * @brief   Implementation of CalculatorBuiltinPanel.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-07-01
 * @version 0.1
 * @since   0.20
 * @ingroup components
 */

#include "calculator_builtin_panel.h"

#include "calculator_panel.h"

#include <QGuiApplication>
#include <QScreen>
#include <QVBoxLayout>

namespace cf::desktop::desktop_component {

namespace {
/// Builtin calculator size (matches the standalone window default).
constexpr int kPanelWidth = 360;
constexpr int kPanelHeight = 520;
} // namespace

CalculatorBuiltinPanel::CalculatorBuiltinPanel(QWidget* parent) : QWidget(parent), panel_(nullptr) {
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    setAutoFillBackground(false);

    panel_ = new CalculatorPanel(this);
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(panel_);
    hide(); // Hidden until popup(); Qt would otherwise auto-show this child.
}

CalculatorBuiltinPanel::~CalculatorBuiltinPanel() = default;

QString CalculatorBuiltinPanel::appId() const {
    return QStringLiteral("calculator");
}

QString CalculatorBuiltinPanel::displayName() const {
    return QStringLiteral("Calculator");
}

void CalculatorBuiltinPanel::popup(const QRect& available) {
    QRect avail = available;
    if (!avail.isValid() || avail.width() <= 0 || avail.height() <= 0) {
        if (const auto* screen = QGuiApplication::primaryScreen()) {
            avail = screen->availableGeometry();
        }
    }
    const int x = avail.center().x() - kPanelWidth / 2;
    const int y = avail.center().y() - kPanelHeight / 2;
    setGeometry(x, y, kPanelWidth, kPanelHeight);
    show();
    raise();
}

void CalculatorBuiltinPanel::hidePanel() {
    hide();
}

} // namespace cf::desktop::desktop_component
