#include "desktop_entry.h"

#include <QtGlobal>

#include "ui/widget/material/application/material_application.h"

int main(int argc, char* argv[]) {
    // MaterialApplication registers the MD3 themes (light/dark) into ThemeManager
    // on construction, so panels and widgets resolve real theme tokens at runtime.
    qw::widget::material::MaterialApplication cf_desktop_app(argc, argv);

    // Refuse to start a second desktop shell -- two fullscreen shells on one
    // screen fight over geometry and WindowManagers cross-track each other's
    // windows. See acquireSingleInstanceLock() docs.
    if (!cf::desktop::acquireSingleInstanceLock()) {
        qWarning("CFDesktop: another instance is already running; exiting.");
        return 0;
    }

    return cf::desktop::run_desktop_session();
}
