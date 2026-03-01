/**
 * @file    base/include/base/scope_guard/scope_guard.hpp
 * @brief   Provides RAII-style scope guard for automatic cleanup.
 *
 * Implements a scope guard that executes a provided function upon
 * destruction, ensuring cleanup operations are performed even when
 * exceptions occur.
 *
 * @author  Charliechen114514
 * @date    2026-02-21
 * @version 0.1
 * @since   0.1
 * @ingroup base_utilities
 */
#pragma once
#include <functional>

namespace cf {

/**
 * @brief  RAII scope guard for automatic cleanup operations.
 *
 * Executes a provided function upon destruction unless explicitly
 * dismissed. Useful for ensuring resources are released or
 * cleanup operations are performed when leaving a scope.
 *
 * @ingroup base_utilities
 *
 * @note    The guard is not movable or copyable. This design ensures
 *          cleanup operations are executed exactly once.
 *
 * @warning The stored function is invoked during destruction. Any
 *          exceptions thrown by the function propagate during
 *          stack unwinding.
 *
 * @code
 * {
 *     FILE* f = fopen("data.txt", "r");
 *     cf::ScopeGuard guard([&f]() { fclose(f); });
 *
 *     // Use the file...
 *     // File is closed automatically when leaving scope.
 * }
 * @endcode
 */
class ScopeGuard {
  public:
    /**
     * @brief  Constructs a scope guard with a cleanup function.
     *
     * @param[in] f Function to execute on destruction. Must be callable
     *               with no arguments.
     *
     * @throws     May throw if function copy/move constructor throws.
     *
     * @note       None.
     *
     * @warning    None.
     *
     * @since      0.1
     * @ingroup    base_utilities
     */
    template <typename F> explicit ScopeGuard(F&& f) : fn_(std::forward<F>(f)) {}

    /**
     * @brief  Destroys the scope guard and executes the cleanup function.
     *
     * The cleanup function is executed only if the guard has not been
     * dismissed.
     *
     * @throws     May propagate exceptions thrown by the cleanup function.
     *
     * @note       None.
     *
     * @warning    Any exceptions thrown by the cleanup function
     *             propagate during stack unwinding.
     *
     * @since      0.1
     * @ingroup    base_utilities
     */
    ~ScopeGuard() noexcept(false) {
        if (active_)
            fn_();
    }

    /**
     * @brief  Copy constructor is deleted.
     *
     * @since      0.1
     * @ingroup    base_utilities
     */
    ScopeGuard(const ScopeGuard&) = delete;

    /**
     * @brief  Copy assignment operator is deleted.
     *
     * @since      0.1
     * @ingroup    base_utilities
     */
    ScopeGuard& operator=(const ScopeGuard&) = delete;

    /**
     * @brief  Move constructor is deleted.
     *
     * @since      0.1
     * @ingroup    base_utilities
     */
    ScopeGuard(ScopeGuard&&) = delete;

    /**
     * @brief  Move assignment operator is deleted.
     *
     * @since      0.1
     * @ingroup    base_utilities
     */
    ScopeGuard& operator=(ScopeGuard&&) = delete;

    /**
     * @brief  Dismisses the scope guard.
     *
     * Prevents the cleanup function from being executed upon
     * destruction. Irreversible once called.
     *
     * @throws     None.
     *
     * @note       Once dismissed, the cleanup function cannot be
     *             re-activated.
     *
     * @warning    None.
     *
     * @since      0.1
     * @ingroup    base_utilities
     */
    void dismiss() noexcept { active_ = false; }

  private:
    /// The cleanup function to execute on destruction. Ownership: owner.
    std::function<void()> fn_;

    /// Indicates whether the guard is still active. Ownership: owner.
    bool active_ = true;
};

} // namespace cf
