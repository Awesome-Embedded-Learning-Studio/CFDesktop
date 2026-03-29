/**
 * @file    IDisplayServerBackend.h
 * @brief   Abstract interface for display server / compositor backends.
 *
 * IDisplayServerBackend is the top-level abstraction that determines
 * CFDesktop's role in the display stack:
 *
 * - **Client mode**: CFDesktop runs as an application inside an
 *   existing desktop environment (Windows, Linux with Gnome/KDE).
 * - **Compositor mode**: CFDesktop IS the display server / compositor,
 *   managing external application windows (bare-metal X11 or Wayland).
 * - **DirectRender mode**: CFDesktop renders directly to the framebuffer
 *   without any windowing system (embedded EGLFS / linuxfb).
 *
 * This interface unifies the three modes so that the Shell, PanelManager,
 * and WindowManager can work identically regardless of role.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-03-29
 * @version 0.1
 * @since   0.11
 * @ingroup components
 */

#pragma once

#include "IWindow.h"
#include "IWindowBackend.h"
#include "base/weak_ptr/weak_ptr.h"

#include <QList>
#include <QObject>
#include <QRect>

namespace cf::desktop {

/**
 * @brief  Describes CFDesktop's role in the display stack.
 *
 * @ingroup components
 */
enum class DisplayServerRole {
    Client,      ///< Running as an app inside an existing desktop.
    Compositor,  ///< CFDesktop is the display server / compositor.
    DirectRender ///< Rendering directly to framebuffer (EGLFS / linuxfb).
};

/**
 * @brief  Describes what a specific display server backend can do.
 *
 * @ingroup components
 */
struct DisplayServerCapabilities {
    /// The role this backend operates in.
    DisplayServerRole role = DisplayServerRole::Client;

    /// Whether the backend can manage windows from external applications.
    /// Client mode: false (only internal windows).
    /// Compositor mode: true.
    /// DirectRender mode: false.
    bool canManageExternalWindows = false;

    /// Whether the backend needs its own compositor scene.
    /// Client mode: false (the OS compositor handles this).
    /// Compositor / DirectRender: true.
    bool needsOwnCompositor = false;

    /// Whether the backend speaks the Wayland protocol.
    bool supportsWaylandProtocol = false;

    /// Whether the backend speaks the X11 / XCB protocol.
    bool supportsX11Protocol = false;
};

/**
 * @brief  Abstract interface for the display server / compositor backend.
 *
 * Implementations:
 * - **WindowsPseudoDesktop**: Client mode on Windows (fullscreen overlay).
 * - **X11ClientBackend**: Client mode on X11 with an existing WM.
 * - **X11WMBackend**: Compositor mode — CFDesktop acts as X11 window manager.
 * - **WaylandCompositorBackend**: Compositor mode — CFDesktop acts as
 *   Wayland compositor via QtWaylandCompositor.
 * - **EGLFSBackend**: DirectRender mode — Qt EGLFS platform plugin.
 * - **LinuxFBBackend**: DirectRender mode — Qt linuxfb platform plugin.
 *
 * @ingroup components
 */
class IDisplayServerBackend : public QObject {
    Q_OBJECT

  public:
    ~IDisplayServerBackend() override = default;

    /**
     * @brief  Returns the role this backend operates in.
     *
     * @return The current DisplayServerRole.
     */
    virtual DisplayServerRole role() const = 0;

    /**
     * @brief  Returns the capabilities of this backend.
     *
     * @return DisplayServerCapabilities describing supported features.
     */
    virtual DisplayServerCapabilities capabilities() const = 0;

    /**
     * @brief  Initializes the display server backend.
     *
     * In Compositor mode this sets up the compositor, acquires DRM
     * outputs, and starts accepting client connections.
     * In Client mode this is a lightweight setup.
     * In DirectRender mode this initializes the framebuffer or EGL surface.
     *
     * @param[in] argc  Argument count (forwarded from main).
     * @param[in] argv  Argument vector (forwarded from main).
     * @return          True if initialization succeeded.
     */
    virtual bool initialize(int argc, char** argv) = 0;

    /**
     * @brief  Shuts down the display server backend and releases resources.
     */
    virtual void shutdown() = 0;

    /**
     * @brief  Runs the backend's event loop.
     *
     * In Compositor mode this processes Wayland/X11 client events.
     * In Client mode this calls QApplication::exec().
     * In DirectRender mode this runs the Qt event loop.
     *
     * @return Exit code from the event loop.
     */
    virtual int runEventLoop() = 0;

    /**
     * @brief  Returns the window backend for managing windows.
     *
     * The IWindowBackend is the per-window abstraction. In Compositor
     * mode it manages external application windows; in Client mode it
     * manages internal QWidget-based windows.
     *
     * @return Weak pointer to the IWindowBackend, or null if not
     *         yet initialized.
     */
    virtual WeakPtr<IWindowBackend> windowBackend() = 0;

    /**
     * @brief  Returns the list of output rectangles (screens).
     *
     * In multi-monitor setups this returns one QRect per output.
     * In Client mode these are the QScreen geometries.
     * In Compositor mode these are the DRM/KMS outputs.
     *
     * @return List of output rectangles in device pixels.
     */
    virtual QList<QRect> outputs() const = 0;

  signals:
    /**
     * @brief  Emitted when an output is added, removed, or resized.
     */
    void outputChanged();

    /**
     * @brief  Emitted when an external application window appears.
     *
     * Only fires in Compositor mode when a new client connects.
     *
     * @param[in]  window  Weak reference to the new window.
     */
    void externalWindowAppeared(WeakPtr<IWindow> window);

    /**
     * @brief  Emitted when an external application window disappears.
     *
     * Only fires in Compositor mode when a client disconnects.
     *
     * @param[in]  window  Weak reference to the disappearing window.
     */
    void externalWindowDisappeared(WeakPtr<IWindow> window);
};

} // namespace cf::desktop
