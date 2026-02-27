/**
 * @file main.cpp
 * @brief Material Design 3 Gallery - Main Entry Point
 *
 * @author CFDesktop Team
 * @date 2026-02-28
 * @version 0.1
 */

#include "MaterialGalleryMainWindow.h"

#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Set application metadata
    app.setApplicationName("Material Design 3 Gallery");
    app.setApplicationVersion("0.1");
    app.setOrganizationName("CFDesktop");

    // Create and show main window
    cf::ui::gallery::MaterialGalleryMainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
