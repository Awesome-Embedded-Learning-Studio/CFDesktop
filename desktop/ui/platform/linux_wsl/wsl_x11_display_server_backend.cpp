/**
 * @file    wsl_x11_display_server_backend.cpp
 * @brief   Implementation of WSLDisplayServerBackend.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-03-30
 * @version 0.1
 * @since   0.12
 */

#include "wsl_x11_display_server_backend.h"

#ifdef CFDESKTOP_OS_LINUX

#    include "cflog.h"

#    include <QApplication>
#    include <QScreen>
#    include <xcb/xcb.h>

namespace cf::desktop::backend::wsl {

WSLDisplayServerBackend::WSLDisplayServerBackend(QObject* parent)
    : IDisplayServerBackend(parent), window_backend_(std::make_unique<WSLX11WindowBackend>(this)) {}

WSLDisplayServerBackend::~WSLDisplayServerBackend() {
    shutdown();
}

// ── IDisplayServerBackend ─────────────────────────────────

DisplayServerRole WSLDisplayServerBackend::role() const {
    // We manage external windows via XCB — treated as Compositor
    // from the shell's perspective, even though XWayland/Weston
    // is the real compositor.
    return DisplayServerRole::Compositor;
}

DisplayServerCapabilities WSLDisplayServerBackend::capabilities() const {
    DisplayServerCapabilities caps;
    caps.role = DisplayServerRole::Compositor;
    caps.canManageExternalWindows = (xcb_conn_ != nullptr);
    caps.needsOwnCompositor = false; // XWayland/Weston handles compositing
    caps.supportsWaylandProtocol = false;
    caps.supportsX11Protocol = true;
    return caps;
}

bool WSLDisplayServerBackend::initialize(int argc, char** argv) {
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    if (initialized_) {
        cf::log::warningftag("WSLDisplayServerBackend", "Already initialized");
        return true;
    }

    // QApplication must exist before we start tracking events.
    if (!QApplication::instance()) {
        cf::log::errorftag("WSLDisplayServerBackend", "QApplication not yet created");
        return false;
    }

    // Connect to XWayland via XCB
    if (!connectToX11()) {
        cf::log::errorftag("WSLDisplayServerBackend",
                           "Failed to connect to X11 display (is DISPLAY set?)");
        // Non-fatal: continue without external window tracking
    }

    // Start monitoring third-party windows if XCB is connected
    if (xcb_conn_) {
        if (!window_backend_->startTracking(xcb_conn_, root_window_)) {
            cf::log::errorftag("WSLDisplayServerBackend", "Failed to start X11 window tracking");
        }
    }

    // Forward external-window signals.
    connect(window_backend_.get(), &IWindowBackend::window_came, this,
            [this](aex::WeakPtr<IWindow> win) { emit externalWindowAppeared(win); });

    connect(window_backend_.get(), &IWindowBackend::window_gone, this,
            [this](aex::WeakPtr<IWindow> win) { emit externalWindowDisappeared(win); });

    // Monitor screen geometry changes.
    auto* guiApp = static_cast<QGuiApplication*>(QGuiApplication::instance());
    connect(guiApp, &QGuiApplication::screenAdded, this, &WSLDisplayServerBackend::outputChanged);
    connect(guiApp, &QGuiApplication::screenRemoved, this, &WSLDisplayServerBackend::outputChanged);
    connect(guiApp, &QGuiApplication::primaryScreenChanged, this,
            &WSLDisplayServerBackend::outputChanged);

    initialized_ = true;

    cf::log::traceftag("WSLDisplayServerBackend", "Initialized — XCB {}",
                       xcb_conn_ ? "connected, tracking external windows"
                                 : "not connected, no window tracking");
    return true;
}

void WSLDisplayServerBackend::shutdown() {
    if (!initialized_) {
        return;
    }

    window_backend_->stopTracking();

    if (xcb_conn_) {
        xcb_disconnect(xcb_conn_);
        xcb_conn_ = nullptr;
        root_window_ = XCB_WINDOW_NONE;
    }

    initialized_ = false;

    cf::log::traceftag("WSLDisplayServerBackend", "Shutdown complete");
}

int WSLDisplayServerBackend::runEventLoop() {
    if (!initialized_) {
        cf::log::errorftag("WSLDisplayServerBackend", "runEventLoop called before initialize()");
        return -1;
    }

    auto* app = QApplication::instance();
    if (!app) {
        cf::log::errorftag("WSLDisplayServerBackend", "No QApplication instance");
        return -1;
    }

    return QApplication::exec();
}

aex::WeakPtr<IWindowBackend> WSLDisplayServerBackend::windowBackend() {
    if (!window_backend_) {
        return {};
    }
    return window_backend_->make_weak();
}

QList<QRect> WSLDisplayServerBackend::outputs() const {
    QList<QRect> result;
    const auto screens = QGuiApplication::screens();
    result.reserve(screens.size());
    for (const auto* screen : screens) {
        result.append(screen->geometry());
    }
    return result;
}

// ── XCB connection ────────────────────────────────────────

bool WSLDisplayServerBackend::connectToX11() {
    // Connect to the display specified by $DISPLAY (XWayland in WSLg).
    // Passing nullptr uses the DISPLAY environment variable.
    int screen_num = 0;
    xcb_conn_ = xcb_connect(nullptr, &screen_num);

    if (xcb_connection_has_error(xcb_conn_) != 0) {
        cf::log::errorftag("WSLDisplayServerBackend", "xcb_connect failed (error={})",
                           xcb_connection_has_error(xcb_conn_));
        xcb_disconnect(xcb_conn_);
        xcb_conn_ = nullptr;
        return false;
    }

    // Get the root window of the default screen
    const xcb_setup_t* setup = xcb_get_setup(xcb_conn_);
    xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);
    for (int i = 0; i < screen_num && iter.rem > 0; ++i) {
        xcb_screen_next(&iter);
    }

    if (iter.rem == 0) {
        cf::log::errorftag("WSLDisplayServerBackend",
                           "No X11 screen available (requested screen {})", screen_num);
        xcb_disconnect(xcb_conn_);
        xcb_conn_ = nullptr;
        return false;
    }

    xcb_screen_t* screen = iter.data;
    root_window_ = screen->root;

    cf::log::traceftag("WSLDisplayServerBackend",
                       "Connected to X11 display: screen={}, root=0x{:08x}", screen_num,
                       static_cast<uint32_t>(root_window_));
    return true;
}

} // namespace cf::desktop::backend::wsl

#endif // CFDESKTOP_OS_LINUX
