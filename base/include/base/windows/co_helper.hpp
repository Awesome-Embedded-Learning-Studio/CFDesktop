/**
 * @file    base/include/base/windows/co_helper.hpp
 * @brief   Provides helper utilities for Windows COM operations.
 *
 * Contains RAII wrappers and helper functions for managing COM
 * initialization and cleanup.
 *
 * @author  Charliechen114514
 * @date    2026-02-22
 * @version 0.1
 * @since   0.1
 * @ingroup base_windows
 */
#pragma once
#include "../expected/expected.hpp"
#include "../scope_guard/scope_guard.hpp"

#include "common.h"
#include <functional>
#include <objbase.h>  // CoInitializeEx, CoUninitialize
#include <winerror.h> // HRESULT, FAILED

namespace cf {

/**
 * @brief  Helper class for Windows COM operations.
 *
 * Provides static methods for managing COM initialization and
 * executing COM operations with automatic cleanup.
 *
 * @tparam ResourceBack  Type of the resource returned by COM operations.
 * @tparam ErrorCode       Type used for error reporting.
 *
 * @ingroup base_windows
 */
template <typename ResourceBack, typename ErrorCode> class COMHelper {
  public:
    /// Type alias for COM operation function.
    using ContextFunction = std::function<cf::expected<ResourceBack, ErrorCode>()>;

    /**
     * @brief  Executes a COM operation with single-threaded initialization.
     *
     * Initializes COM with the specified concurrency model, executes
     * the provided function, and automatically cleans up COM.
     *
     * @param[in] f          Function to execute after COM initialization.
     * @param[in] coinitFlag COM initialization flags. Default is
     *                       COINIT_APARTMENTTHREADED.
     *
     * @return              Expected containing the operation result or
     *                      an error code.
     *
     * @throws              None (error reporting via expected type).
     *
     * @ingroup base_windows
     */
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

    /**
     * @brief  Executes a COM operation with multi-threaded initialization.
     *
     * Initializes COM with COINIT_MULTITHREADED and executes
     * the provided function with automatic cleanup.
     *
     * @param[in] f Function to execute after COM initialization.
     *
     * @return     Expected containing the operation result or an error code.
     *
     * @throws     None (error reporting via expected type).
     *
     * @ingroup base_windows
     */
    static cf::expected<ResourceBack, ErrorCode> RunComInterfacesMTA(ContextFunction f) {
        return RunComInterfacesOnce(std::move(f), COINIT_MULTITHREADED);
    }
};

} // namespace cf
