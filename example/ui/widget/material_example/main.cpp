/**
 * @file main.cpp
 * @brief Material Widget Gallery - Main Entry Point
 *
 * Entry point for the Material Design 3 Widget Gallery application.
 * Displays all Material Design widgets with various states and configurations.
 *
 * @author CFDesktop Team
 * @date 2026-03-18
 * @version 0.1
 */

#include "MaterialGalleryWindow.h"
#include "ui/widget/material/application/material_application.h"

#include <QApplication>

using namespace cf::ui::widget::material;

int main(int argc, char* argv[]) {
    MaterialApplication app(argc, argv);

    // Set application metadata
    app.setApplicationName("Material Gallery");
    app.setApplicationVersion("0.1");
    app.setOrganizationName("CFDesktop");

    // Create and show main window
    cf::ui::example::MaterialGalleryWindow window;
    window.show();

    return app.exec();
}
