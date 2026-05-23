#pragma once

/**
 * @file    base/include/base/policy_chain/policy_chain.hpp
 * @brief   Provides a chain-of-responsibility pattern implementation with fallback.
 *
 * Defines PolicyChain, PolicyChainBuilder, and factory functions for executing
 * a sequence of callable policies in order, falling back to the next policy if
 * the previous one returns std::nullopt.
 *
 * @author  CFDesktop Team
 * @date    2026-03-16
 * @version 0.13.1
 * @since   0.13.0
 * @ingroup base_policy_chain
 */

#include <cstddef>
#include <functional>
#include <list>
#include <memory>
#include <optional>
#include <type_traits>
#include <utility>

namespace cf {

namespace policy_chain_detail {

#if defined(__clang__) && (__clang_major__ < 19)
#    define CF_POLICY_CHAIN_INVOKE_BARRIER __attribute__((noinline, optnone))
#else
#    define CF_POLICY_CHAIN_INVOKE_BARRIER
#endif

// Clang 18 can miscompile inlined std::optional-returning fallback chains under -O3.
template <typename Policy, typename... CallArgs> [[nodiscard]] CF_POLICY_CHAIN_INVOKE_BARRIER auto
invoke_policy(Policy const& policy, CallArgs&&... args)
    -> decltype(policy(std::forward<CallArgs>(args)...)) {
    return policy(std::forward<CallArgs>(args)...);
}

#undef CF_POLICY_CHAIN_INVOKE_BARRIER

} // namespace policy_chain_detail

/**
 * @brief PolicyChain with fallback mechanism
 *
 * Executes functions in chain order (head first).
 * If a function returns std::nullopt, falls back to the next.
 * Stops at first successful result (returns a value).
 *
 * @tparam T The value type wrapped in std::optional
 * @tparam Args Argument types for the policies
 *
 * @example
 * @code
 * // Method 1: Direct construction
 * PolicyChain<int, const std::string&> chain;
 * chain.add_front([](const std::string& s) -> std::optional<int> {
 *     if (!s.empty()) return std::stoi(s);
 *     return std::nullopt;
 * });
 *
 * // Method 2: Using builder
 * auto chain = make_policy_chain<int, const std::string&>(
 *     [](const std::string& s) -> std::optional<int> { return s.length(); },
 *     [](const std::string& s) -> std::optional<int> {
 *         if (!s.empty()) return std::stoi(s);
 *         return std::nullopt;
 *     }
 * );
 *
 * // Execute
 * if (auto result = chain.execute("42")) {
 *     std::cout << "Result: " << *result << std::endl;
 * }
 * @endcode
 */
template <typename Ret, typename... Args> class PolicyChain {
  public:
    using PolicyType = std::function<std::optional<Ret>(Args...)>;
    using ValueType = Ret;
    using SizeType = std::size_t;

    PolicyChain() = default;
    PolicyChain(const PolicyChain&) = default;
    PolicyChain& operator=(const PolicyChain&) = default;
    PolicyChain(PolicyChain&&) noexcept = default;
    PolicyChain& operator=(PolicyChain&&) noexcept = default;

    /**
     * @brief Add a policy to the front of the chain (highest priority)
     */
    template <typename Policy> void add_front(Policy&& policy) {
        policies_.emplace_front(std::forward<Policy>(policy));
    }

    /**
     * @brief Add a policy to the back of the chain (lowest priority)
     */
    template <typename Policy> void add_back(Policy&& policy) {
        policies_.emplace_back(std::forward<Policy>(policy));
    }

    /**
     * @brief Execute policies in chain order until one succeeds
     * @return First non-null result, or std::nullopt if all fail
     */
    [[nodiscard]] std::optional<Ret> execute(Args... args) const {
        for (const auto& policy : policies_) {
            auto result = policy_chain_detail::invoke_policy(policy, args...);
            if (result.has_value()) {
                return std::move(result);
            }
        }
        return std::nullopt;
    }

    /**
     * @brief Execute policies in chain order (operator() overload)
     */
    [[nodiscard]] std::optional<Ret> operator()(Args... args) const { return execute(args...); }

    /**
     * @brief Remove all policies
     */
    void clear() { policies_.clear(); }

    /**
     * @brief Check if chain is empty
     */
    [[nodiscard]] bool empty() const { return policies_.empty(); }

    /**
     * @brief Get number of policies
     */
    [[nodiscard]] SizeType size() const { return policies_.size(); }

    /**
     * @brief Get iterator to beginning (for range-based for)
     */
    [[nodiscard]] auto begin() const { return policies_.begin(); }

    /**
     * @brief Get iterator to end
     */
    [[nodiscard]] auto end() const { return policies_.end(); }

  private:
    struct PolicyConcept {
        virtual ~PolicyConcept() = default;
        [[nodiscard]] virtual std::optional<Ret> invoke(Args... args) const = 0;
        [[nodiscard]] virtual std::unique_ptr<PolicyConcept> clone() const = 0;
    };

    template <typename Policy> struct PolicyModel final : PolicyConcept {
        explicit PolicyModel(Policy policy) : policy_(std::move(policy)) {}

        [[nodiscard]] std::optional<Ret> invoke(Args... args) const override {
            auto result = std::invoke(policy_, args...);
#if defined(__clang__) && (__clang_major__ < 19)
            // Clang 18 can mispack disengaged small std::optional returns under -O3.
            if (!result.has_value()) {
                return std::nullopt;
            }
#endif
            return result;
        }

        [[nodiscard]] std::unique_ptr<PolicyConcept> clone() const override {
            return std::make_unique<PolicyModel>(policy_);
        }

        mutable Policy policy_;
    };

    class PolicyEntry {
      public:
        template <typename Policy> explicit PolicyEntry(Policy&& policy)
            : policy_(make_policy_model(std::forward<Policy>(policy))) {}

        PolicyEntry(const PolicyEntry& other) : policy_(other.policy_->clone()) {}
        PolicyEntry(PolicyEntry&&) noexcept = default;

        PolicyEntry& operator=(const PolicyEntry& other) {
            if (this != &other) {
                policy_ = other.policy_->clone();
            }
            return *this;
        }

        PolicyEntry& operator=(PolicyEntry&&) noexcept = default;

        [[nodiscard]] std::optional<Ret> operator()(Args... args) const {
            return policy_->invoke(args...);
        }

      private:
        template <typename Policy>
        [[nodiscard]] static std::unique_ptr<PolicyConcept> make_policy_model(Policy&& policy) {
            using StoredPolicy = std::decay_t<Policy>;
            static_assert(std::is_copy_constructible_v<StoredPolicy>,
                          "PolicyChain policies must be copy constructible");
            return std::make_unique<PolicyModel<StoredPolicy>>(std::forward<Policy>(policy));
        }

        std::unique_ptr<PolicyConcept> policy_;
    };

    std::list<PolicyEntry> policies_;
};

// ============================================================================
// Factory Functions
// ============================================================================

/**
 * @brief Create a PolicyChain with fluent builder API
 *
 * Policies are executed in order (first = highest priority).
 *
 * @tparam T Return value type
 * @tparam Args Argument types
 * @tparam Policies Types of the policy callables
 *
 * @example
 * @code
 * auto chain = make_policy_chain<int, int>(
 *     [](int x) -> std::optional<int> {
 *         if (x > 0) return x * 2;
 *         return std::nullopt;
 *     },
 *     [](int x) -> std::optional<int> {
 *         if (x < 0) return -x;
 *         return std::nullopt;
 *     },
 *     [](int) -> std::optional<int> {
 *         return 0;  // Default fallback
 *     }
 * );
 *
 * auto result = chain.execute(-5);  // Returns 5
 * @endcode
 */
template <typename T, typename... Args, typename... Policies>
[[nodiscard]] auto make_policy_chain(Policies&&... policies) {
    PolicyChain<T, Args...> chain;
    (chain.add_back(std::forward<Policies>(policies)), ...);
    return chain;
}

/**
 * @brief Builder for creating PolicyChain with fluent API
 *
 * @example
 * @code
 * auto chain = policy_chain_builder<int, const std::string&>()
 *     .then([](const std::string& s) -> std::optional<int> {
 *         if (!s.empty()) return std::stoi(s);
 *         return std::nullopt;
 *     })
 *     .then([](const std::string& s) -> std::optional<int> {
 *         return static_cast<int>(s.length());
 *     })
 *     .then([](const std::string&) -> std::optional<int> {
 *         return 0;  // Default
 *     })
 *     .build();
 * @endcode
 */
template <typename Ret, typename... Args> class PolicyChainBuilder {
  public:
    using PolicyType = std::function<std::optional<Ret>(Args...)>;

    PolicyChainBuilder() = default;

    /**
     * @brief  Adds a policy to the chain.
     *
     * Adds the policy to the back of the chain with the lowest priority.
     * Policies are executed in the order they are added.
     *
     * @param[in] policy The policy callable to add to the chain.
     * @return           Reference to this builder for method chaining.
     * @throws           None
     * @note             The policy is moved into the chain.
     * @warning          None
     * @since            0.13.0
     * @ingroup          base_policy_chain
     */
    template <typename Policy> PolicyChainBuilder& then(Policy&& policy) {
        chain_.add_back(std::forward<Policy>(policy));
        return *this;
    }

    /**
     * @brief Build the PolicyChain
     */
    [[nodiscard]] PolicyChain<Ret, Args...> build() && { return std::move(chain_); }

    /**
     * @brief Build and return by copy
     */
    [[nodiscard]] PolicyChain<Ret, Args...> build() const& { return chain_; }

  private:
    PolicyChain<Ret, Args...> chain_;
};

/**
 * @brief Create a PolicyChain builder
 *
 * @tparam T Return value type
 * @tparam Args Argument types
 *
 * @example
 * @code
 * auto chain = policy_chain_builder<int, int>()
 *     .then([](int x) -> std::optional<int> {
 *         if (x > 0) return x * 2;
 *         return std::nullopt;
 *     })
 *     .then([](int x) -> std::optional<int> {
 *         return 0;
 *     })
 *     .build();
 * @endcode
 */
template <typename T, typename... Args> [[nodiscard]] auto policy_chain_builder() {
    return PolicyChainBuilder<T, Args...>{};
}

} // namespace cf
