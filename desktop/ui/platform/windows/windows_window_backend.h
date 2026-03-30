/**
 * @file    windows_window_backend.h
 * @brief   IWindowBackend implementation using Win32 event hooks.
 *
 * WindowsWindowBackend discovers and tracks third-party application
 * windows using SetWinEventHook + EnumWindows, wrapping each HWND
 * as a WindowsWindow (IWindow) for the WindowManager to consume.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-03-29
 * @version 0.1
 * @since   0.11
 */

#pragma once
#include "base/macro/system_judge.h"

#ifdef CFDESKTOP_OS_WINDOWS

#    include "IWindowBackend.h"
#    include "windows_window.h"

#    include <Unknwn.h>
#    include <WinUser.h>
#    include <Windows.h>
#    include <unordered_map>

namespace cf::desktop::backend::windows {

/**
 * @brief  IWindowBackend that tracks external Win32 windows.
 *
 * Uses SetWinEventHook (WINEVENT_OUTOFCONTEXT) to receive
 * EVENT_OBJECT_SHOW / EVENT_OBJECT_DESTROY notifications for
 * top-level windows from all processes.
 *
 * On startTracking(), EnumWindows() scans existing windows so
 * the shell immediately sees what is already open.
 */
class WindowsWindowBackend : public IWindowBackend {
    Q_OBJECT
  public:
    explicit WindowsWindowBackend(QObject* parent = nullptr);
    ~WindowsWindowBackend() override;

    /**
     * @brief  Starts tracking external windows via hooks and initial scan.
     *
     * @return True if hook installation and initial scan succeeded.
     */
    bool startTracking();

    /**
     * @brief  Stops tracking and releases all Win32 event hooks.
     */
    void stopTracking();

    // ── IWindowBackend interface ──────────────────────────

    WeakPtr<IWindow> createWindow(const QString& appId) override;

    /**
     * @brief  Destroys the given window and removes it from tracking.
     *
     * @param[in]  window  Weak reference to the window to destroy.
     */
    void destroyWindow(WeakPtr<IWindow> window) override;

    QList<WeakPtr<IWindow>> windows() const override;

    /**
     * @brief  Returns the capabilities of this Windows backend.
     *
     * @return BackendCapabilities for the Win32 window backend.
     */
    render::BackendCapabilities capabilities() const override;

    // ── Internal: called by the static hook callback ──────

    /**
     * @brief  Handles an external window shown event.
     *
     * @param[in]  hwnd  The native window handle that appeared.
     */
    void onExternalWindowShown(HWND hwnd);

    /**
     * @brief  Handles an external window destroyed event.
     *
     * @param[in]  hwnd  The native window handle that was destroyed.
     */
    void onExternalWindowDestroyed(HWND hwnd);

  private:
    /**
     * @brief  Determines whether a window should be tracked.
     *
     * @param[in]  hwnd  The native window handle to evaluate.
     *
     * @return True if the window passes the tracking filter.
     */
    static bool shouldTrackWindow(HWND hwnd);

    void registerWindow(HWND hwnd);
    void unregisterWindow(HWND hwnd);

    /// Win32 event hook for show/destroy events. Ownership: this.
    HWINEVENTHOOK hook_show_destroy_{nullptr};
    /// Win32 event hook for name change events. Ownership: this.
    HWINEVENTHOOK hook_name_change_{nullptr};
    /// Maps HWND to owned WindowsWindow instances. Ownership: this.
    std::unordered_map<HWND, std::unique_ptr<WindowsWindow>> tracked_windows_;
};

} // namespace cf::desktop::backend::windows

#endif // CFDESKTOP_OS_WINDOWS
