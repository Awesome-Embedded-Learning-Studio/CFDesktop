#include "cflog/sinks/console_sink.h"
#include "cflog/formatter/default_formatter.h"
#include <cstddef>
#include <cstdio>
#include <memory>

#ifdef _WIN32
#    include <windows.h>
#else
#    include <unistd.h>
#endif
namespace cf::log {

bool ConsoleSink::write(const LogRecord& record) {
    if (!formatter_) {
        formatter_ = std::make_shared<DefaultFormatter>();
    }
    const auto str = formatter_->format_me(record);
    if (str.empty())
        return true;

#ifdef _WIN32
    HANDLE hStderr = ::GetStdHandle(STD_ERROR_HANDLE);
    if (hStderr == INVALID_HANDLE_VALUE || hStderr == nullptr)
        return false;

    DWORD written = 0;
    DWORD mode = 0;
    if (::GetConsoleMode(hStderr, &mode)) {
        // 真实控制台，走 WriteConsoleA
        ::WriteConsoleA(hStderr, str.data(), static_cast<DWORD>(str.size()), &written, nullptr);
    }
    // 非控制台（被重定向）时静默丢弃，保持"只写控制台"语义
    return written > 0;

#else
    // Linux/macOS：直接写 fd=2，绕过 CRT
    const char* ptr = str.data();
    std::size_t remaining = str.size();
    while (remaining > 0) {
        const ssize_t n = ::write(STDERR_FILENO, ptr, remaining);
        if (n < 0)
            return false; // EINTR 也视为失败，调用方可重试
        ptr += n;
        remaining -= static_cast<std::size_t>(n);
    }
    return true;
#endif
}

bool ConsoleSink::flush() {
    std::fflush(stderr);
    return true;
}
} // namespace cf::log
