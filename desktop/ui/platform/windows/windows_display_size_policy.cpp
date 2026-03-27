#include "windows_display_size_policy.h"
#include "IDesktopDisplaySizeStrategy.h"
#include "base/macro/system_judge.h"
#include "cflog.h"
#include <QSizePolicy>
#include <QWidget>

#ifdef CFDESKTOP_OS_WINDOWS
#    include <windows.h>

namespace {
HWND getHWNDFromWidget(QWidget* widget) {
    if (!widget || !widget->windowHandle()) {
        return nullptr;
    }

    // Get the native window handle
    return reinterpret_cast<HWND>(widget->winId());
}
} // namespace

namespace cf::desktop::platform_strategy::windows {

WindowsDisplaySizePolicy::WindowsDisplaySizePolicy() : IDesktopDisplaySizeStrategy() {}

WindowsDisplaySizePolicy::~WindowsDisplaySizePolicy() = default;

bool WindowsDisplaySizePolicy::action(QWidget* widget_data) {
    if (!widget_data) {
        cf::log::errorftag("WindowsDisplaySizePolicy", "Null widget provided");
        return false;
    }

#    ifdef CFDESKTOP_OS_WINDOWS
    applyWindowsNativeBehavior(widget_data);
    cf::log::traceftag(
        "WindowsDisplaySizePolicy",
        "Windows window configured: frameless, stay-on-bottom, fixed-size, avoid-system-ui");
    return true;
#    else
    cf::log::warningftag("WindowsDisplaySizePolicy",
                         "Not compiled for Windows platform, action failed");
    return false;
#    endif
}

void WindowsDisplaySizePolicy::applyWindowsNativeBehavior(QWidget* widget_data) {
    if (!widget_data)
        return;

    Qt::WindowFlags flags = widget_data->windowFlags();

    flags &= ~Qt::WindowStaysOnTopHint;
    flags &= ~Qt::SplashScreen;
    flags &= ~Qt::Popup;
    flags &= ~Qt::ToolTip;

    flags |= Qt::FramelessWindowHint;
    flags |= Qt::WindowStaysOnBottomHint;

    widget_data->setWindowFlags(flags);

    RECT workArea;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);

    widget_data->setGeometry(workArea.left, workArea.top, workArea.right - workArea.left,
                             workArea.bottom - workArea.top);

    widget_data->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    HWND hwnd = getHWNDFromWidget(widget_data);
    if (hwnd) {
        LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
        exStyle |= WS_EX_TOOLWINDOW;
        SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle);

        SetWindowPos(hwnd, HWND_BOTTOM, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
    }

    widget_data->show();
}

DesktopBehaviors WindowsDisplaySizePolicy::query() const {
    // Windows configuration:
    // frameless = true
    // stay_on_bottom = true
    // avoid_system_ui = true
    // (AllowResize is NOT set = false)
    // (Fullscreen is NOT set = false)
    return DesktopBehaviors(DesktopBehaviorFlag::Frameless | DesktopBehaviorFlag::StayOnBottom |
                            DesktopBehaviorFlag::AvoidSystemUI);
}

} // namespace cf::desktop::platform_strategy::windows

#endif