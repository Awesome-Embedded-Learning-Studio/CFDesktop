/**
 * @file    windows_display_server_backend.cpp
 * @brief   Implementation of WindowsDisplayServerBackend.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-03-29
 * @version 0.1
 * @since   0.11
 */

#include "windows_display_server_backend.h"

#ifdef CFDESKTOP_OS_WINDOWS
#    ifdef ERROR
#        undef ERROR
#    endif
#    include "cflog.h"

#    include <QApplication>
#    include <QScreen>

namespace cf::desktop::backend::windows {

WindowsDisplayServerBackend::WindowsDisplayServerBackend(QObject* parent)
    : IDisplayServerBackend(parent), window_backend_(std::make_unique<WindowsWindowBackend>(this)) {
}

WindowsDisplayServerBackend::~WindowsDisplayServerBackend() {
    shutdown();
}

// ── IDisplayServerBackend ─────────────────────────────────

DisplayServerRole WindowsDisplayServerBackend::role() const {
    // We manage external windows via Win32 hooks — treated as Compositor
    // from the shell's perspective, even though DWM is the real OS compositor.
    return DisplayServerRole::Compositor;
}

DisplayServerCapabilities WindowsDisplayServerBackend::capabilities() const {
    DisplayServerCapabilities caps;
    caps.role = DisplayServerRole::Compositor;
    caps.canManageExternalWindows = true; // SetWinEventHook + EnumWindows
    caps.needsOwnCompositor = false;      // Windows DWM handles compositing
    caps.supportsWaylandProtocol = false;
    caps.supportsX11Protocol = false;
    return caps;
}

bool WindowsDisplayServerBackend::initialize(int argc, char** argv) {
    if (initialized_) {
        cf::log::warningftag("WinDisplayServerBackend", "Already initialized");
        return true;
    }

    // QApplication must exist before we start hooking events.
    if (!QApplication::instance()) {
        cf::log::errorftag("WinDisplayServerBackend", "QApplication not yet created");
        return false;
    }

    // Start monitoring third-party windows.
    if (!window_backend_->startTracking()) {
        cf::log::errorftag("WinDisplayServerBackend", "Failed to start window tracking");
        return false;
    }

    // Forward external-window signals.
    connect(window_backend_.get(), &IWindowBackend::window_came, this,
            [this](WeakPtr<IWindow> win) { emit externalWindowAppeared(win); });

    connect(window_backend_.get(), &IWindowBackend::window_gone, this,
            [this](WeakPtr<IWindow> win) { emit externalWindowDisappeared(win); });

    // Monitor screen geometry changes.
    auto* guiApp = static_cast<QGuiApplication*>(QGuiApplication::instance());
    connect(guiApp, &QGuiApplication::screenAdded, this,
            &WindowsDisplayServerBackend::outputChanged);
    connect(guiApp, &QGuiApplication::screenRemoved, this,
            &WindowsDisplayServerBackend::outputChanged);
    connect(guiApp, &QGuiApplication::primaryScreenChanged, this,
            &WindowsDisplayServerBackend::outputChanged);

    initialized_ = true;

    cf::log::traceftag("WinDisplayServerBackend", "Initialized — tracking external windows");
    return true;
}

void WindowsDisplayServerBackend::shutdown() {
    if (!initialized_) {
        return;
    }

    window_backend_->stopTracking();
    initialized_ = false;

    cf::log::traceftag("WinDisplayServerBackend", "Shutdown complete");
}

int WindowsDisplayServerBackend::runEventLoop() {
    if (!initialized_) {
        cf::log::errorftag("WinDisplayServerBackend", "runEventLoop called before initialize()");
        return -1;
    }

    auto* app = QApplication::instance();
    if (!app) {
        cf::log::errorftag("WinDisplayServerBackend", "No QApplication instance");
        return -1;
    }

    return QApplication::exec();
}

WeakPtr<IWindowBackend> WindowsDisplayServerBackend::windowBackend() {
    if (!window_backend_) {
        return {};
    }
    return window_backend_->make_weak();
}

QList<QRect> WindowsDisplayServerBackend::outputs() const {
    QList<QRect> result;
    const auto screens = QGuiApplication::screens();
    result.reserve(screens.size());
    for (const auto* screen : screens) {
        result.append(screen->geometry());
    }
    return result;
}

} // namespace cf::desktop::backend::windows

#endif // CFDESKTOP_OS_WINDOWS
