#include "base/macro/system_judge.h"

#ifndef CFDESKTOP_OS_WINDOWS
#    error "Unexpected Includes As This File is using for Windows Implementations"
#else
#    include "../console_platform.h"
#    include "console_win_impl.h"
#    include <windows.h>
namespace cf::base::device::impl::Win {

console::console_size_t console_size() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    if (h == INVALID_HANDLE_VALUE || !GetConsoleScreenBufferInfo(h, &csbi))
        return {80, 24};
    return {csbi.srWindow.Right - csbi.srWindow.Left + 1,
            csbi.srWindow.Bottom - csbi.srWindow.Top + 1};
}

bool support_colorful() {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    if (h == INVALID_HANDLE_VALUE)
        return false;

    // 不是真正的控制台（被重定向）
    DWORD mode;
    if (!GetConsoleMode(h, &mode))
        return false;

    // 尝试开启 ANSI 转义序列支持（Win10 1511+）
    return SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}

} // namespace cf::base::device::impl::Win
#endif