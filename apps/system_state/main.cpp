/**
 * @file    apps/system_state/main.cpp
 * @brief   Standalone System State application entry point.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-07-01
 * @version 0.1
 * @since   0.20
 * @ingroup system_state
 */

#include "system_state_panel.h"

#include <QApplication>
#include <QCoreApplication>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral("CFDesktop System State"));

    cf::desktop::desktop_component::SystemStatePanel panel;
    panel.setWindowTitle(QStringLiteral("System State"));
    panel.resize(520, 640);
    panel.show();

    return app.exec();
}
