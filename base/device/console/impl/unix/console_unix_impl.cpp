#include "aex/macro/system_judge.h"

#ifndef CFDESKTOP_OS_LINUX
#    error "Unexpected Includes As This File is using for Linux/Unix Implementations"
#else
#    include "console_unix_impl.h"
#    include <cstdlib>
#    include <cstring>
#    include <optional>
#    include <stdexcept>
#    include <string>
#    include <sys/ioctl.h>
#    include <unistd.h>
namespace cf::base::device::impl::Unix {
console::console_size_t console_size() {
    struct winsize console_window_size{};
    const auto result = ioctl(STDOUT_FILENO, TIOCGWINSZ, &console_window_size);
    if (result == 0) {
        return {console_window_size.ws_col, console_window_size.ws_row};
    }
    return INVALID_ONE;
}

std::optional<int> parse_int(const char* s) {
    if (s == nullptr || *s == '\0')
        return std::nullopt;
    try {
        std::size_t pos;
        int val = std::stoi(s, &pos);

        if (pos != std::strlen(s))
            return std::nullopt;

        return val;
    } catch (const std::invalid_argument&) {
        return std::nullopt;
    } catch (const std::out_of_range&) {
        return std::nullopt;
    }
}

console::console_size_t console_size_fallback_v1() {
    // Fallback to environment variables
    // OK, these one read the env from shell env. you know, that sucks :)
    return {parse_int(getenv("COLUMNS")).value_or(-1), parse_int(getenv("LINES")).value_or(-1)};
}

bool support_colorful() {
    if (getenv("NO_COLOR"))
        return false;
    if (!isatty(STDOUT_FILENO))
        return false;
    const char* term = getenv("TERM");
    if (!term || !strcmp(term, "dumb"))
        return false;
    return true;
}

} // namespace cf::base::device::impl::Unix
#endif
