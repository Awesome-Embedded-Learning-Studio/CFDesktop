/**
 * @file scope_guard.hpp
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief When Needing Clearing Ups, that functions will auto do
 * @version 0.1
 * @date 2026-02-21
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once
#include <functional>

namespace cf {
class ScopeGuard {
  public:
    explicit ScopeGuard(std::function<void()> f) : fn_(std::move(f)) {}
    ~ScopeGuard() {
        if (active_)
            fn_();
    }

    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;

    void dismiss() noexcept { active_ = false; }

  private:
    std::function<void()> fn_;
    bool active_ = true;
};
} // namespace cf