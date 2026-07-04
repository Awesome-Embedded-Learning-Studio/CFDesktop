/**
 * @file    boot_widget_factory.cpp
 * @brief   Factory function for creating boot progress widgets.
 *
 * @author  CFDesktop Team
 * @date    2025-03-25
 * @version 0.1
 * @since   0.1
 * @ingroup desktop_ui_widget_init_session
 */

#include "boot_progress_widget.h"
#include "simple_boot_widget.h"

namespace cf::desktop::init_session {

BootProgressWidget* GetBootWidget(BootstrapStyle s) {
    switch (s) {
        case BootstrapStyle::Simple:
            return new SimpleBootWidget();
        case BootstrapStyle::Perfect:
        case BootstrapStyle::SelfDefined:
        default:
            return nullptr;
    }
}

} // namespace cf::desktop::init_session
