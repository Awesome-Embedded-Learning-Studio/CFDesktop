/**
 * @file    apps/noter/main.cpp
 * @brief   Standalone Noter application entry point.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-07-01
 * @version 0.1
 * @since   0.20
 * @ingroup noter
 */

#include "noter_panel.h"

#include <QApplication>
#include <QCoreApplication>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral("CFDesktop Noter"));

    cf::desktop::desktop_component::NoterPanel panel;
    panel.setWindowTitle(QStringLiteral("Noter"));
    panel.resize(480, 560);
    panel.show();

    return app.exec();
}
