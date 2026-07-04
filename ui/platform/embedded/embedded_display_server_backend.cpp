/**
 * @file    embedded_display_server_backend.cpp
 * @brief   DirectRender IDisplayServerBackend implementation for embedded targets.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-29
 * @version 0.1
 * @since   0.19.0
 * @ingroup platform_embedded
 */

#include "embedded_display_server_backend.h"

#include "cflog.h"

#include <QGuiApplication>
#include <QScreen>

namespace cf::desktop::backend::embedded {

EmbeddedDisplayServerBackend::EmbeddedDisplayServerBackend(QObject* parent)
    : IDisplayServerBackend(parent),
      window_backend_(std::make_unique<NullWindowBackend>(this)) {}

EmbeddedDisplayServerBackend::~EmbeddedDisplayServerBackend() = default;

DisplayServerRole EmbeddedDisplayServerBackend::role() const {
    return DisplayServerRole::DirectRender;
}

DisplayServerCapabilities EmbeddedDisplayServerBackend::capabilities() const {
    DisplayServerCapabilities caps;
    caps.role = DisplayServerRole::DirectRender;
    caps.canManageExternalWindows = false;
    caps.needsOwnCompositor = false;
    caps.supportsWaylandProtocol = false;
    caps.supportsX11Protocol = false;
    return caps;
}

bool EmbeddedDisplayServerBackend::initialize(int /*argc*/, char** /*argv*/) {
    initialized_ = true;
    cf::log::traceftag("EmbeddedBackend", "Initialized - DirectRender (linuxfb/EGLFS)");
    return true;
}

void EmbeddedDisplayServerBackend::shutdown() {
    initialized_ = false;
}

int EmbeddedDisplayServerBackend::runEventLoop() {
    // The Qt event loop is driven by the desktop session (QApplication::exec()).
    return 0;
}

aex::WeakPtr<IWindowBackend> EmbeddedDisplayServerBackend::windowBackend() {
    if (!window_backend_) {
        return {};
    }
    return window_backend_->make_weak();
}

QList<QRect> EmbeddedDisplayServerBackend::outputs() const {
    QList<QRect> result;
    if (auto* screen = QGuiApplication::primaryScreen()) {
        result.append(screen->geometry());
    }
    return result;
}

} // namespace cf::desktop::backend::embedded
