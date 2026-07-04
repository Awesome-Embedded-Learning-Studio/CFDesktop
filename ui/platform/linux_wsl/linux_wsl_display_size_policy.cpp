#include "linux_wsl_display_size_policy.h"
#include "IDesktopDisplaySizeStrategy.h"
#include "cflog.h"
#include "qt_backend.h"
#include <QSizePolicy>
#include <QWidget>

namespace cf::desktop::platform_strategy::wsl {

DisplaySizePolicyMaker::DisplaySizePolicyMaker() : IDesktopDisplaySizeStrategy() {}

DisplaySizePolicyMaker::~DisplaySizePolicyMaker() = default;

bool DisplaySizePolicyMaker::action(QWidget* widget_data) {
    if (!widget_data) {
        return false;
    }

    backend::LinuxQtBackend backend = backend::GetBackend();
    switch (backend) {
        case backend::LinuxQtBackend::X11:
            cf::log::traceftag("DisplaySizePolicy", "X11 Backend Action");
            act_with_x11(widget_data);
            break;
        case backend::LinuxQtBackend::Wayland:
            // Wayland backend support placeholder
            cf::log::warningftag("DisplaySizePolicy",
                                 "Wayland Backend has not been implemented, silent failed");
            return false;
        default:
            cf::log::errorftag("DisplaySizePolicy",
                               "Can not defer the platform type, please contact the "
                               "CFDesktop Issue looking for support!");
            break;
    }

    return true;
}

void DisplaySizePolicyMaker::act_with_x11(QWidget* widget_data) {
    if (!widget_data) {
        return;
    }

    // Get current window flags
    Qt::WindowFlags flags = widget_data->windowFlags();

    // Ensure standard window type with decorations (not frameless)
    flags |= Qt::Window;
    flags &= ~Qt::FramelessWindowHint;
    flags &= ~Qt::SplashScreen;
    flags &= ~Qt::ToolTip;
    flags &= ~Qt::Popup;

    // Ensure normal window order (not stay on bottom)
    flags &= ~Qt::WindowStaysOnBottomHint;

    // Apply the window flags
    widget_data->setWindowFlags(flags);

    // Ensure normal window mode (not fullscreen)
    widget_data->setWindowState(widget_data->windowState() | Qt::WindowMaximized);

    // Enable resizing - set size policy to allow user to resize
    widget_data->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    cf::log::traceftag("DisplaySizePolicy",
                       "X11 window configured: standard window with resize enabled");
}

DesktopBehaviors DisplaySizePolicyMaker::query() const {
    backend::LinuxQtBackend backend = backend::GetBackend();
    switch (backend) {
        case backend::LinuxQtBackend::X11:
            return x11_query();
        case backend::LinuxQtBackend::Wayland:
        default:
            return IDesktopDisplaySizeStrategy::query();
    }
}

DesktopBehaviors DisplaySizePolicyMaker::x11_query() const {
    // WSL X11 default behaviors:
    return DesktopBehaviors(DesktopBehaviorFlag::AllowResize);
}

} // namespace cf::desktop::platform_strategy::wsl
