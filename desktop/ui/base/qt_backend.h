/**
 * @file    desktop/ui/base/qt_backend.h
 * @brief   Platform-specific Qt backend enumeration and detection.
 *
 * Declares enum types for Qt backend variants on Linux (X11/Wayland) and
 * Windows, along with a runtime backend detection function.
 *
 * @author  N/A
 * @date    N/A
 * @version N/A
 * @since   N/A
 * @ingroup none
 */

#pragma once
#include "base/macro/system_judge.h"

namespace cf::desktop::backend {
#ifdef CFDESKTOP_OS_LINUX

/**
 * @brief  Qt backend types available on Linux platforms.
 *
 * Represents the underlying windowing system used by the Qt framework.
 * The backend affects rendering, input handling, and platform integration.
 *
 * @ingroup none
 */
enum class LinuxQtBackend {
    X11,     ///< X11 windowing system backend.
    Wayland, ///< Wayland compositor backend.
    EGLFS,   ///< EGL + OpenGL ES without a windowing system (embedded).
    LinuxFB, ///< Linux framebuffer, pure software rendering (embedded).
    Unknown  ///< Backend could not be determined or is unsupported.
};

/**
 * @brief  Detects the current Qt backend at runtime.
 *
 * Queries the Qt platform integration to determine whether the application
 * is running under X11, Wayland, EGLFS, LinuxFB, or an unknown backend.
 *
 * @return          Detected Qt backend for the current session.
 * @throws          None
 * @note            Result may vary depending on environment variables and
 *                  session configuration.
 * @warning         None
 * @since           N/A
 * @ingroup         none
 */
LinuxQtBackend GetBackend();

/**
 * @brief  Determines whether CFDesktop should act as a display server.
 *
 * Probes the runtime environment to decide the operating mode:
 * - Client:         A display server (X11/Wayland) is already running.
 * - NeedCompositor: No display server detected — CFDesktop should be one.
 * - DirectRender:   No windowing system at all — render to framebuffer.
 *
 * The detection priority is:
 * 1. CFDESKTOP_DISPLAY_SERVER environment variable (force override).
 * 2. Presence of WAYLAND_DISPLAY or DISPLAY (Client mode).
 * 3. Presence of /dev/dri/card* (DirectRender / EGLFS).
 * 4. Presence of /dev/fb0 (DirectRender / LinuxFB).
 * 5. Default: Client.
 *
 * @return          The recommended display server mode.
 * @throws          None
 * @since           0.11
 * @ingroup         none
 */
enum class DisplayServerMode {
    Client,         ///< A windowing system is available; run as an app.
    NeedCompositor, ///< No WM detected; CFDesktop should be the compositor.
    DirectRender    ///< No windowing system; render directly to hardware.
};

DisplayServerMode DetectDisplayServerMode();

#elifdef CFDESKTOP_OS_WINDOWS

/**
 * @brief  Qt backend type for Windows platforms.
 *
 * Represents the standard desktop backend used by Qt on Windows.
 *
 * @ingroup none
 */
enum class WindowsQtBackend {
    Typical ///< Standard Windows desktop backend.
};

/**
 * @brief  Returns the Qt backend for Windows platforms.
 *
 * Provides a consistent interface with Linux, returning the standard
 * Windows backend.
 *
 * @return          Always returns WindowsQtBackend::Typical.
 * @throws          None
 * @note            Windows uses a single consistent backend type.
 * @warning         None
 * @since           N/A
 * @ingroup         none
 */
inline WindowsQtBackend GetBackend() {
    return WindowsQtBackend::Typical;
}

#endif

} // namespace cf::desktop::backend
