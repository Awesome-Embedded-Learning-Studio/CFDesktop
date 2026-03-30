/**
 * @file    windows_window.h
 * @brief   IWindow implementation wrapping a Win32 HWND.
 *
 * WindowsWindow adapts a native Win32 window handle (HWND) to the
 * platform-agnostic IWindow interface used by WindowManager.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-03-29
 * @version 0.1
 * @since   0.11
 */

#pragma once
#include "base/macro/system_judge.h"

#ifdef CFDESKTOP_OS_WINDOWS

#    include "IWindow.h"
#    include <Windows.h>

namespace cf::desktop::backend::windows {

/**
 * @brief  IWindow adapter for a Win32 HWND.
 *
 * Wraps an existing external window (owned by another process) or
 * an internal window. All property queries go through Win32 API calls.
 */
class WindowsWindow : public IWindow {
    Q_OBJECT
  public:
    explicit WindowsWindow(HWND hwnd, QObject* parent = nullptr);
    ~WindowsWindow() override;

    /**
     * @brief  Returns the unique identifier of this window.
     *
     * @return A string representation of the HWND.
     */
    win_id_t windowID() const override;

    /**
     * @brief  Returns the current window title.
     *
     * @return The window title as a Unicode string.
     */
    QString title() const override;

    /**
     * @brief  Returns the current window geometry in device pixels.
     *
     * @return The window rectangle (position and size).
     */
    QRect geometry() const override;

    /**
     * @brief  Moves and resizes the window.
     *
     * @param[in]  r  The new geometry in device pixels.
     */
    void set_geometry(const QRect& r) override;

    /**
     * @brief  Requests the window to close gracefully.
     */
    void requestClose() override;

    /**
     * @brief  Brings this window to the top of the Z-order.
     */
    void raise() override;

    /**
     * @brief  Returns the native Win32 window handle.
     *
     * @return The underlying HWND.
     */
    HWND nativeHandle() const { return hwnd_; }

  private:
    /// Native Win32 window handle. Ownership: external process.
    HWND hwnd_;
};

} // namespace cf::desktop::backend::windows

#endif // CFDESKTOP_OS_WINDOWS
