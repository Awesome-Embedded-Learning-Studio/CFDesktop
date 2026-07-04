/**
 * @file    null_window_backend.cpp
 * @brief   No-op IWindowBackend implementation for embedded DirectRender mode.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-29
 * @version 0.1
 * @since   0.19.0
 * @ingroup platform_embedded
 */

#include "null_window_backend.h"

namespace cf::desktop::backend::embedded {

NullWindowBackend::NullWindowBackend(QObject* parent) : IWindowBackend(parent) {}

NullWindowBackend::~NullWindowBackend() = default;

aex::WeakPtr<IWindow> NullWindowBackend::createWindow(const QString& /*appId*/) {
    return {};
}

void NullWindowBackend::destroyWindow(aex::WeakPtr<IWindow> /*window*/) {}

QList<aex::WeakPtr<IWindow>> NullWindowBackend::windows() const {
    return {};
}

render::BackendCapabilities NullWindowBackend::capabilities() const {
    // The framebuffer target has no GPU and no multi-window support.
    render::BackendCapabilities caps;
    caps.supportsMultiWindow = false;
    caps.supportsTransparency = false;
    caps.hasHardwareAcceleration = false;
    caps.supportsVSync = false;
    caps.supportsScreenshot = false;
    caps.maxTextureSize = 0;
    return caps;
}

} // namespace cf::desktop::backend::embedded
