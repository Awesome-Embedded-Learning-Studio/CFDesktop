/**
 * @file main.cpp
 * @brief Material Typography Gallery - Main Entry Point
 *
 * Entry point for the Material Design 3 Typography Gallery application.
 * Displays all 15 Type Scale tokens organized in 5 categories.
 *
 * @author CFDesktop Team
 * @date 2026-02-26
 * @version 0.1
 */

#include "MaterialTypographyMainWindow.h"

#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Set application metadata
    app.setApplicationName("Material Typography Gallery");
    app.setApplicationVersion("0.1");
    app.setOrganizationName("CFDesktop");

    // Create and show main window
    cf::ui::gallery::MaterialTypographyMainWindow window;
    window.resize(1200, 800);
    window.show();

    return app.exec();
}
