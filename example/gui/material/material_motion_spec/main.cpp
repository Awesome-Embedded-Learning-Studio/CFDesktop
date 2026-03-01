/**
 * @file main.cpp
 * @brief Material Motion Spec Gallery - Main Entry Point
 *
 * Entry point for the Material Design 3 Motion Spec Gallery application.
 * Displays all 9 motion presets with interactive animation demonstrations.
 *
 * @author CFDesktop Team
 * @date 2026-02-26
 * @version 0.1
 */

#include "MaterialMotionSpecMainWindow.h"

#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Set application metadata
    app.setApplicationName("Material Motion Spec Gallery");
    app.setApplicationVersion("0.1");
    app.setOrganizationName("CFDesktop");

    // Create and show main window
    cf::ui::gallery::MaterialMotionSpecMainWindow window;
    window.show();

    return app.exec();
}
