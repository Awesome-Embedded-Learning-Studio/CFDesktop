/**
 * @file    apps/calculator/main.cpp
 * @brief   Standalone Calculator application entry point.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-30
 * @version 0.1
 * @since   0.20
 * @ingroup calculator
 */

#include "calculator_panel.h"

#include <QApplication>
#include <QCoreApplication>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral("CFDesktop Calculator"));

    cf::desktop::desktop_component::CalculatorPanel panel;
    panel.setWindowTitle(QStringLiteral("Calculator"));
    panel.resize(360, 520);
    panel.show();

    return app.exec();
}
