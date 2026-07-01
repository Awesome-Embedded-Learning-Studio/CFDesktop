/**
 * @file    apps/alarm_clock/main.cpp
 * @brief   Standalone Alarm Clock application entry point.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-07-01
 * @version 0.1
 * @since   0.20
 * @ingroup alarm_clock
 */

#include "alarm_clock_panel.h"

#include <QApplication>
#include <QCoreApplication>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral("CFDesktop Alarm Clock"));

    cf::desktop::desktop_component::AlarmClockPanel panel;
    panel.setWindowTitle(QStringLiteral("Alarm Clock"));
    panel.resize(380, 560);
    panel.show();

    return app.exec();
}
