/**
 * @file    wsl_x11_window.h
 * @brief   IWindow implementation wrapping an X11 xcb_window_t.
 *
 * WSLX11Window adapts a native X11 window handle (xcb_window_t) to the
 * platform-agnostic IWindow interface used by WindowManager.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-03-30
 * @version 0.1
 * @since   0.12
 */

#pragma once
#include "base/macro/system_judge.h"

#ifdef CFDESKTOP_OS_LINUX

#    include "IWindow.h"
#    include <QString>
#    include <xcb/xcb.h>

namespace cf::desktop::backend::wsl {

/**
 * @brief  Cached X11 atoms needed for window property queries.
 */
struct XcbAtoms {
    xcb_atom_t net_wm_name = XCB_ATOM_NONE;
    xcb_atom_t wm_name = XCB_ATOM_NONE;
    xcb_atom_t net_wm_pid = XCB_ATOM_NONE;
    xcb_atom_t wm_protocols = XCB_ATOM_NONE;
    xcb_atom_t wm_delete_window = XCB_ATOM_NONE;
    xcb_atom_t net_wm_window_type = XCB_ATOM_NONE;
    xcb_atom_t net_wm_window_type_dock = XCB_ATOM_NONE;
    xcb_atom_t net_wm_window_type_desktop = XCB_ATOM_NONE;
    xcb_atom_t utf8_string = XCB_ATOM_NONE;
};

/**
 * @brief  IWindow adapter for an X11 xcb_window_t.
 *
 * Wraps an existing external X11 window. All property queries go
 * through XCB protocol calls to the XWayland server.
 */
class WSLX11Window : public IWindow {
    Q_OBJECT
  public:
    /**
     * @brief  Constructs a WSLX11Window wrapping the given X11 window.
     *
     * @param[in]  conn    XCB connection (must outlive this object).
     * @param[in]  root    Root window used for coordinate translation.
     * @param[in]  win     The X11 window to wrap.
     * @param[in]  atoms   Cached X11 atoms for property queries.
     * @param[in]  parent  Optional Qt parent.
     */
    WSLX11Window(xcb_connection_t* conn, xcb_window_t root, xcb_window_t win, const XcbAtoms& atoms,
                 QObject* parent = nullptr);
    ~WSLX11Window() override;

    // ── IWindow interface ─────────────────────────────────

    /**
     * @brief  Returns the platform-specific window identifier.
     * @return The X11 window ID as a win_id_t.
     */
    win_id_t windowID() const override;

    /**
     * @brief  Returns the current window title.
     * @return The window title as a QString, decoded from
     *         _NET_WM_NAME (UTF-8) or WM_NAME (STRING).
     */
    QString title() const override;

    /**
     * @brief  Returns the current window geometry in device pixels.
     * @return QRect describing position and size relative to the root.
     */
    QRect geometry() const override;

    /**
     * @brief  Moves and resizes the window.
     * @param[in] r  The desired geometry in device pixels.
     */
    void set_geometry(const QRect& r) override;

    /**
     * @brief  Requests the window to close gracefully via
     *         WM_DELETE_WINDOW protocol.
     */
    void requestClose() override;

    /**
     * @brief  Raises the window to the top of the stacking order.
     */
    void raise() override;

    /**
     * @brief  Returns the owning process id read from _NET_WM_PID.
     * @return The process id, or 0 when unavailable.
     */
    qint64 pid() const override;

    // ── X11-specific ──────────────────────────────────────

    /**
     * @brief  Returns the native X11 window handle.
     * @return The underlying xcb_window_t.
     */
    xcb_window_t nativeHandle() const { return window_; }

  private:
    /// XCB connection. Ownership: external (WSLDisplayServerBackend).
    xcb_connection_t* conn_;
    /// Root window for coordinate translation.
    xcb_window_t root_;
    /// Wrapped X11 window handle. Ownership: external process.
    xcb_window_t window_;
    /// Cached atoms for property queries.
    XcbAtoms atoms_;
    /// Owning process id, read once from _NET_WM_PID at construction.
    qint64 pid_{0};

    /// @brief Queries _NET_WM_PID once; returns 0 when unavailable.
    qint64 readPid() const;
};

} // namespace cf::desktop::backend::wsl

#endif // CFDESKTOP_OS_LINUX
