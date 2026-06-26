/**
 * @file    wsl_x11_window_backend.h
 * @brief   IWindowBackend implementation using XCB event monitoring.
 *
 * WSLX11WindowBackend discovers and tracks third-party application
 * windows via XCB SubstructureNotify on the root window, wrapping
 * each xcb_window_t as a WSLX11Window (IWindow).
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-03-30
 * @version 0.1
 * @since   0.12
 */

#pragma once
#include "aex/macro/system_judge.h"

#ifdef CFDESKTOP_OS_LINUX

#    include "IWindowBackend.h"
#    include "wsl_x11_window.h"

#    include <QSocketNotifier>
#    include <unordered_map>

namespace cf::desktop::backend::wsl {

/**
 * @brief  IWindowBackend that tracks external X11 windows via XCB.
 *
 * Uses XCB SubstructureNotifyMask on the root window to receive
 * CreateNotify / DestroyNotify events for windows from all processes.
 *
 * On startTracking(), xcb_query_tree scans existing windows so the
 * shell immediately sees what is already open.
 *
 * XCB events are integrated into the Qt event loop via QSocketNotifier
 * on the XCB file descriptor.
 */
class WSLX11WindowBackend : public IWindowBackend {
    Q_OBJECT
  public:
    explicit WSLX11WindowBackend(QObject* parent = nullptr);
    ~WSLX11WindowBackend() override;

    /**
     * @brief  Starts tracking external X11 windows.
     *
     * @param[in]  conn  XCB connection (must remain valid until stopTracking).
     * @param[in]  root  Root window to monitor.
     * @return True if event mask installation and initial scan succeeded.
     */
    bool startTracking(xcb_connection_t* conn, xcb_window_t root);

    /**
     * @brief  Stops tracking and releases all XCB event subscriptions.
     */
    void stopTracking();

    // ── IWindowBackend interface ──────────────────────────

    aex::WeakPtr<IWindow> createWindow(const QString& appId) override;

    /**
     * @brief  Destroys a previously tracked window.
     * @param[in] window  Weak reference to the window to destroy.
     */
    void destroyWindow(aex::WeakPtr<IWindow> window) override;

    QList<aex::WeakPtr<IWindow>> windows() const override;

    /**
     * @brief  Returns the rendering capabilities of this backend.
     * @return BackendCapabilities describing supported render features.
     */
    render::BackendCapabilities capabilities() const override;

    // ── X11-specific ──────────────────────────────────────

    /**
     * @brief  Returns the cached X11 atoms.
     * @return Const reference to the interned atoms.
     */
    const XcbAtoms& atoms() const { return atoms_; }

  private:
    /**
     * @brief  Interns commonly used X11 atoms.
     * @return True if all critical atoms were interned successfully.
     */
    bool internAtoms();

    /**
     * @brief  Interns a single X11 atom by name.
     * @param[in]  name  The atom name.
     * @return The interned atom, or XCB_ATOM_NONE on failure.
     */
    xcb_atom_t internAtom(const char* name);

    /**
     * @brief  Processes pending XCB events (called by QSocketNotifier).
     */
    void processXcbEvents();

    /**
     * @brief  Determines whether a window should be tracked.
     * @param[in]  win  The X11 window to evaluate.
     * @return True if the window passes the tracking filter.
     */
    bool shouldTrackWindow(xcb_window_t win);

    /**
     * @brief  Creates a WSLX11Window for the given X11 window and emits window_came.
     * @param[in]  win  The X11 window to register.
     */
    void registerWindow(xcb_window_t win);

    /**
     * @brief  Removes the WSLX11Window for the given X11 window and emits window_gone.
     * @param[in]  win  The X11 window to unregister.
     */
    void unregisterWindow(xcb_window_t win);

    /// XCB connection. Ownership: external (WSLDisplayServerBackend).
    xcb_connection_t* xcb_conn_{nullptr};
    /// Root window being monitored.
    xcb_window_t root_window_{XCB_WINDOW_NONE};
    /// Cached X11 atoms for property queries.
    XcbAtoms atoms_;
    /// Qt socket notifier for XCB fd integration.
    QSocketNotifier* notifier_{nullptr};
    /// Maps xcb_window_t to owned WSLX11Window instances.
    std::unordered_map<xcb_window_t, std::unique_ptr<WSLX11Window>> tracked_windows_;
};

} // namespace cf::desktop::backend::wsl

#endif // CFDESKTOP_OS_LINUX
