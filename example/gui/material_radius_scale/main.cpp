/**
 * @file main.cpp
 * @brief Material Radius Scale Gallery - Main Entry Point
 *
 * Entry point for the Material Design 3 Radius Scale Gallery application.
 * Displays all 7 corner radius tokens with visual preview.
 *
 * @author CFDesktop Team
 * @date 2026-02-26
 * @version 0.1
 */

#include "MaterialRadiusScaleMainWindow.h"

#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Set application metadata
    app.setApplicationName("Material Radius Scale Gallery");
    app.setApplicationVersion("0.1");
    app.setOrganizationName("CFDesktop");

    // Create and show main window
    cf::ui::gallery::MaterialRadiusScaleMainWindow window;
    window.show();

    return app.exec();
}
