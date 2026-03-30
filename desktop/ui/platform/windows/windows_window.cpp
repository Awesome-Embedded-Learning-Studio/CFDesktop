/**
 * @file    windows_window.cpp
 * @brief   Implementation of WindowsWindow — IWindow adapter for HWND.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-03-29
 * @version 0.1
 * @since   0.11
 */

#include "windows_window.h"

#ifdef CFDESKTOP_OS_WINDOWS
#    include <QRect>
#    include <QString>

namespace cf::desktop::backend::windows {

WindowsWindow::WindowsWindow(HWND hwnd, QObject* parent) : IWindow(parent), hwnd_(hwnd) {}

WindowsWindow::~WindowsWindow() = default;

win_id_t WindowsWindow::windowID() const {
    // Use hex representation of HWND as the window ID
    return QString::number(reinterpret_cast<quintptr>(hwnd_), 16);
}

QString WindowsWindow::title() const {
    if (!hwnd_ || !IsWindow(hwnd_)) {
        return {};
    }

    const int len = GetWindowTextLengthW(hwnd_);
    if (len <= 0) {
        return {};
    }

    std::wstring buf(static_cast<size_t>(len) + 1, L'\0');
    GetWindowTextW(hwnd_, buf.data(), len + 1);
    return QString::fromWCharArray(buf.c_str());
}

QRect WindowsWindow::geometry() const {
    if (!hwnd_ || !IsWindow(hwnd_)) {
        return {};
    }

    RECT r{};
    if (!GetWindowRect(hwnd_, &r)) {
        return {};
    }
    return QRect(r.left, r.top, r.right - r.left, r.bottom - r.top);
}

void WindowsWindow::set_geometry(const QRect& r) {
    if (!hwnd_ || !IsWindow(hwnd_)) {
        return;
    }

    SetWindowPos(hwnd_, nullptr, r.x(), r.y(), r.width(), r.height(),
                 SWP_NOACTIVATE | SWP_NOZORDER);
}

void WindowsWindow::requestClose() {
    if (!hwnd_ || !IsWindow(hwnd_)) {
        return;
    }
    PostMessageW(hwnd_, WM_CLOSE, 0, 0);
}

void WindowsWindow::raise() {
    if (!hwnd_ || !IsWindow(hwnd_)) {
        return;
    }
    SetForegroundWindow(hwnd_);
    BringWindowToTop(hwnd_);
}

} // namespace cf::desktop::backend::windows

#endif // CFDESKTOP_OS_WINDOWS
