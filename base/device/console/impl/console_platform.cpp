#include "console_platform.h"
#include "base/macros.h"
#include <memory>
#include <optional>

#ifdef CFDESKTOP_OS_LINUX
#    include "unix/console_unix_impl.h"
namespace cf::base::device::console {
std::unique_ptr<PolicyChain<console_size_t>> get_platform_size_policy() {
    auto result = std::make_unique<PolicyChain<console_size_t>>();
    result->add_back([]() -> std::optional<console_size_t> {
        auto first_try = impl::Unix::console_size();
        if (first_try == impl::Unix::INVALID_ONE) {
            return {};
        }
        return first_try;
    });

    result->add_back([]() -> std::optional<console_size_t> {
        auto result = impl::Unix::console_size_fallback_v1();
        if (result == impl::Unix::INVALID_ONE) {
            return {};
        }
        return result;
    });

    return result;
}

bool platform_console_support() {
    return impl::Unix::support_colorful();
}

} // namespace cf::base::device::console
#else
#    include "../console_defines.h"
#    include "console_platform.h"
#    include "win/console_win_impl.h"
namespace cf::base::device::console {

std::unique_ptr<PolicyChain<console_size_t>> get_platform_size_policy() {
    auto result = std::make_unique<PolicyChain<console_size_t>>();
    result->add_back([]() -> std::optional<console_size_t> {
        auto first_try = impl::Win::console_size();
        if (first_try == INVALID_ONE) {
            return {};
        }
        return first_try;
    });
    return result;
}

bool platform_console_support() {
    return impl::Win::support_colorful();
}

} // namespace cf::base::device::console
#endif
