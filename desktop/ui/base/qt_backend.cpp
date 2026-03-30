#include "qt_backend.h"
#include <QGuiApplication>
#include <QProcessEnvironment>

#include <fstream>

namespace cf::desktop::backend {

#ifdef CFDESKTOP_OS_LINUX

LinuxQtBackend GetBackend() {
    QString platformName = QGuiApplication::platformName();
    if (platformName == "xcb" || platformName == "x11") {
        return LinuxQtBackend::X11;
    } else if (platformName == "wayland") {
        return LinuxQtBackend::Wayland;
    } else if (platformName == "eglfs") {
        return LinuxQtBackend::EGLFS;
    } else if (platformName == "linuxfb") {
        return LinuxQtBackend::LinuxFB;
    }
    return LinuxQtBackend::Unknown;
}

DisplayServerMode DetectDisplayServerMode() {
    // Priority 1: Environment variable override
    auto env = QProcessEnvironment::systemEnvironment();
    QString override = env.value("CFDESKTOP_DISPLAY_SERVER").toLower();
    if (!override.isEmpty()) {
        if (override == "client")
            return DisplayServerMode::Client;
        if (override == "compositor")
            return DisplayServerMode::NeedCompositor;
        if (override == "direct")
            return DisplayServerMode::DirectRender;
    }

    // Priority 2: Wayland display exists
    if (!env.value("WAYLAND_DISPLAY").isEmpty()) {
        return DisplayServerMode::Client;
    }

    // Priority 3: X11 display exists
    if (!env.value("DISPLAY").isEmpty()) {
        // TODO: Check if a window manager is already running via XCB.
        // For now, assume Client mode when DISPLAY is set.
        return DisplayServerMode::Client;
    }

    // Priority 4: DRM device exists (EGLFS possible)
    {
        std::ifstream drm("/dev/dri/card0");
        if (drm.good()) {
            return DisplayServerMode::DirectRender;
        }
    }

    // Priority 5: Linux framebuffer
    {
        std::ifstream fb("/dev/fb0");
        if (fb.good()) {
            return DisplayServerMode::DirectRender;
        }
    }

    // Default: Client mode
    return DisplayServerMode::Client;
}

#endif

} // namespace cf::desktop::backend
