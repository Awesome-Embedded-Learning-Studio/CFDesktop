/**
 * @file    windows_display_server_backend.h
 * @brief   IDisplayServerBackend implementation for Windows pseudo-desktop.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-03-29
 * @version 0.1
 * @since   0.11
 */

#pragma once

#ifdef CFDESKTOP_OS_WINDOWS

#    include "../../components/IDisplayServerBackend.h"
#    include "windows_window_backend.h"

namespace cf::desktop::backend::windows {

/**
 * @brief  Windows display server backend (pseudo-desktop mode).
 *
 * Runs as a Client of the Windows DWM compositor, but sets
 * canManageExternalWindows = true because it uses SetWinEventHook
 * to track third-party application windows.
 */
class WindowsDisplayServerBackend : public IDisplayServerBackend {
    Q_OBJECT
  public:
    explicit WindowsDisplayServerBackend(QObject* parent = nullptr);
    ~WindowsDisplayServerBackend() override;

    /**
     * @brief  Returns the role this backend operates in.
     *
     * @return Always DisplayServerRole::Client.
     */
    DisplayServerRole role() const override;

    /**
     * @brief  Returns the capabilities of this backend.
     *
     * @return DisplayServerCapabilities for the Windows pseudo-desktop.
     */
    DisplayServerCapabilities capabilities() const override;

    /**
     * @brief  Initializes the Windows display server backend.
     *
     * @param[in]  argc  Argument count (forwarded from main).
     * @param[in]  argv  Argument vector (forwarded from main).
     *
     * @return True if initialization succeeded.
     */
    bool initialize(int argc, char** argv) override;

    /**
     * @brief  Shuts down the backend and releases resources.
     */
    void shutdown() override;

    /**
     * @brief  Runs the Qt event loop.
     *
     * @return Exit code from the event loop.
     */
    int runEventLoop() override;

    WeakPtr<IWindowBackend> windowBackend() override;
    QList<QRect> outputs() const override;

  private:
    std::unique_ptr<WindowsWindowBackend> window_backend_;
    bool initialized_{false};
};

} // namespace cf::desktop::backend::windows

#endif // CFDESKTOP_OS_WINDOWS
