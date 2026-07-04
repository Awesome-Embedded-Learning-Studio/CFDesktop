#include "console_helper.h"
#include "impl/console_platform.h"

namespace cf::base::device::console {

ConsoleHelper::ConsoleHelper() : size_querier(get_platform_size_policy()) {}

std::optional<console_size_t> ConsoleHelper::size() const {
    if (!size_querier) {
        return {};
    }
    auto result = size_querier->execute();
    if (!result) {
        return {};
    }
    // A degenerate size (non-positive width or height) means the console size
    // is unknown -- e.g. a serial console that reports no window size. Treat it
    // as unknown so callers fall back to fallback_size() instead of rendering
    // into a zero-sized buffer (which throws std::length_error downstream).
    if (result->first <= 0 || result->second <= 0) {
        return {};
    }
    return result;
}

bool ConsoleHelper::query_property(const std::string_view property, std::any* value) {
    if (property == "colorable") {
        *value = platform_console_support();
        return true;
    }
    return false;
}

console_size_t ConsoleHelper::fallback_size() const {
    return {80, 24};
}

} // namespace cf::base::device::console
