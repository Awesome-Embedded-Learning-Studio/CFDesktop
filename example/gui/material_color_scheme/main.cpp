/**
 * @file main.cpp
 * @brief Material Color Scheme Gallery - Main Entry Point
 *
 * Entry point for the Material Design 3 Color Scheme Gallery application.
 * Displays all 26 color tokens with light/dark theme switching support.
 *
 * @author CFDesktop Team
 * @date 2026-02-25
 * @version 0.1
 */

#include "MaterialColorSchemeMainWindow.h"

#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Set application metadata
    app.setApplicationName("Material Color Scheme Gallery");
    app.setApplicationVersion("0.1");
    app.setOrganizationName("CFDesktop");

    // Create and show main window
    cf::ui::gallery::MaterialColorSchemeMainWindow window;
    window.show();

    return app.exec();
}
