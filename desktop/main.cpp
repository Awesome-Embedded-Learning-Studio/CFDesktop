#include "desktop_entry.h"
#include "ui/widget/material/application/material_application.h"

int main(int argc, char* argv[]) {
    // MaterialApplication registers the MD3 themes (light/dark) into ThemeManager
    // on construction, so panels and widgets resolve real theme tokens at runtime.
    cf::ui::widget::material::MaterialApplication cf_desktop_app(argc, argv);
    return cf::desktop::run_desktop_session();
}
