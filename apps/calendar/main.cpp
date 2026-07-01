/**
 * @file    apps/calendar/main.cpp
 * @brief   Standalone Calendar application entry point.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-07-01
 * @version 0.1
 * @since   0.20
 * @ingroup calendar
 */

#include "calendar_panel.h"

#include <QApplication>
#include <QCoreApplication>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral("CFDesktop Calendar"));

    cf::desktop::desktop_component::CalendarPanel panel;
    panel.setWindowTitle(QStringLiteral("Calendar"));
    panel.resize(720, 480);
    panel.show();

    return app.exec();
}
