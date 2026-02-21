#pragma once
#include "../expected/expected.hpp"
#include "../scope_guard/scope_guard.hpp"

#include "common.h"
#include <functional>
#include <objbase.h>  // CoInitializeEx, CoUninitialize
#include <winerror.h> // HRESULT, FAILED

namespace cf {
template <typename ResourceBack, typename ErrorCode> class COMHelper {
  public:
    using ContextFunction = std::function<cf::expected<ResourceBack, ErrorCode>()>;
    static cf::expected<ResourceBack, ErrorCode>
    RunComInterfacesOnce(ContextFunction f, DWORD coinitFlag = COINIT_APARTMENTTHREADED) {
        HRESULT hr = ::CoInitializeEx(nullptr, coinitFlag);
        if (FAILED(hr)) {
            return cf::unexpected<ErrorCode>(static_cast<ErrorCode>(hr));
        }
        cf::ScopeGuard guard([]() { ::CoUninitialize(); });

        hr = CoInitializeSecurity(nullptr, -1, nullptr, nullptr, RPC_C_AUTHN_LEVEL_DEFAULT,
                                  RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE, nullptr);

        if (FAILED(hr)) {
            return cf::unexpected<ErrorCode>(static_cast<ErrorCode>(hr));
        }

        cf::expected<ResourceBack, ErrorCode> result = f();
        return result;
    }

    static cf::expected<ResourceBack, ErrorCode> RunComInterfacesMTA(ContextFunction f) {
        return RunComInterfacesOnce(std::move(f), COINIT_MULTITHREADED);
    }
};

} // namespace cf