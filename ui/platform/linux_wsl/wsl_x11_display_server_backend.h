/**
 * @file    wsl_x11_display_server_backend.h
 * @brief   IDisplayServerBackend implementation for WSL via X11/XCB.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-03-30
 * @version 0.1
 * @since   0.12
 */

#pragma once
#include "aex/macro/system_judge.h"

#ifdef CFDESKTOP_OS_LINUX

#    include "../../components/IDisplayServerBackend.h"
#    include "wsl_x11_window_backend.h"

#    include <xcb/xcb.h>

namespace cf::desktop::backend::wsl {

/**
 * @brief  WSL display server backend using X11/XCB.
 *
 * Runs as a Client of the XWayland compositor inside WSLg, tracking
 * external application windows via XCB SubstructureNotify events.
 * This mirrors the WindowsDisplayServerBackend pattern where CFDesktop
 * acts as a pseudo-compositor by monitoring third-party windows.
 */
class WSLDisplayServerBackend : public IDisplayServerBackend {
    Q_OBJECT
  public:
    explicit WSLDisplayServerBackend(QObject* parent = nullptr);
    ~WSLDisplayServerBackend() override;

    // ── IDisplayServerBackend ────────────────────────────

    /**
     * @brief  Returns the display server role for this backend.
     * @return DisplayServerRole::Client — CFDesktop acts as a client
     *         of the XWayland compositor.
     */
    DisplayServerRole role() const override;

    /**
     * @brief  Returns the capabilities supported by this backend.
     * @return DisplayServerCapabilities describing available features.
     */
    DisplayServerCapabilities capabilities() const override;

    /**
     * @brief  Initializes the XCB connection and window backend.
     * @param[in] argc  Argument count from the application entry point.
     * @param[in] argv  Argument vector from the application entry point.
     * @return True if initialization succeeded.
     */
    bool initialize(int argc, char** argv) override;

    /**
     * @brief  Disconnects from XCB and releases all held resources.
     */
    void shutdown() override;

    /**
     * @brief  Runs the Qt event loop for the display server.
     * @return Exit code from the event loop.
     */
    int runEventLoop() override;
    aex::WeakPtr<IWindowBackend> windowBackend() override;
    QList<QRect> outputs() const override;

  private:
    /**
     * @brief  Establishes an XCB connection to the XWayland server.
     * @return True if connection succeeded.
     */
    bool connectToX11();

    /// Window backend for tracking external X11 windows.
    std::unique_ptr<WSLX11WindowBackend> window_backend_;
    /// XCB connection to the XWayland server. Ownership: this.
    xcb_connection_t* xcb_conn_{nullptr};
    /// Root window of the default screen.
    xcb_window_t root_window_{XCB_WINDOW_NONE};
    /// Whether initialize() has been called successfully.
    bool initialized_{false};
};

} // namespace cf::desktop::backend::wsl

#endif // CFDESKTOP_OS_LINUX
