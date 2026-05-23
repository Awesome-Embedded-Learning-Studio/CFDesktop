#include "base/policy_chain/policy_chain.hpp"
#include <cassert>
#include <iostream>
#include <string>

void example_basic_usage() {
    std::cout << "=== Basic Usage ===" << std::endl;

    // Create a chain with multiple fallback policies
    cf::PolicyChain<int, int> chain;

    // Policy 1 (highest priority): return 2x if input > 0
    chain.add_front([](int x) -> std::optional<int> {
        if (x > 0)
            return x * 2;
        return std::nullopt;
    });

    // Policy 2: return -x if input < 0
    chain.add_front([](int x) -> std::optional<int> {
        if (x < 0)
            return -x;
        return std::nullopt;
    });

    // Policy 3 (lowest priority): return 0 as default
    chain.add_back([](int) -> std::optional<int> { return 0; });

    // Test cases
    assert(chain.execute(5) == 10); // Policy 1: 5 * 2 = 10
    assert(chain.execute(-3) == 3); // Policy 2: -(-3) = 3
    assert(chain.execute(0) == 0);  // Policy 3: default fallback

    // Using operator()
    assert(chain(10) == 20);

    std::cout << "All assertions passed!" << std::endl;
}

void example_factory_function() {
    std::cout << "\n=== Using make_policy_chain ===" << std::endl;

    auto chain = cf::make_policy_chain<int, const std::string&>(
        [](const std::string& s) -> std::optional<int> {
            // Try to parse as integer
            try {
                return std::stoi(s);
            } catch (...) {
                return std::nullopt;
            }
        },
        [](const std::string& s) -> std::optional<int> {
            // Fallback: return string length
            if (s.empty()) {
                return {};
            }
            return static_cast<int>(s.length());
        },
        [](const std::string&) -> std::optional<int> {
            // Default fallback
            return -1;
        });

    assert(chain.execute("123") == 123); // Parses to 123
    assert(chain.execute("hello") == 5); // Length is 5
    assert(chain.execute("") == -1);     // Default fallback

    std::cout << "All assertions passed!" << std::endl;
}

void example_builder() {
    std::cout << "\n=== Using Builder API ===" << std::endl;

    auto chain = cf::policy_chain_builder<int, double>()
                     .then([](double x) -> std::optional<int> {
                         // Try to round to nearest int
                         if (x >= 0.0)
                             return static_cast<int>(x + 0.5);
                         return std::nullopt;
                     })
                     .then([](double x) -> std::optional<int> {
                         // Fallback: truncate
                         return static_cast<int>(x);
                     })
                     .then([](double) -> std::optional<int> {
                         // Default
                         return 0;
                     })
                     .build();

    assert(chain.execute(3.7) == 4);   // Round: 3.7 -> 4
    assert(chain.execute(-2.3) == -2); // Truncate: -2.3 -> -2

    std::cout << "All assertions passed!" << std::endl;
}

void example_multiple_args() {
    std::cout << "\n=== Multiple Arguments ===" << std::endl;

    auto chain = cf::make_policy_chain<int, int, int>(
        [](int a, int b) -> std::optional<int> {
            if (b != 0)
                return a / b;
            return std::nullopt;
        },
        [](int a, int) -> std::optional<int> { return a * 2; },
        [](int, int) -> std::optional<int> { return -1; });

    assert(chain.execute(10, 2) == 5);  // Division: 10 / 2 = 5
    assert(chain.execute(10, 0) == 20); // Fallback: 10 * 2 = 20

    std::cout << "All assertions passed!" << std::endl;
}

void example_void_output() {
    std::cout << "\n=== Void-like Output (bool) ===" << std::endl;

    // Using std::optional<bool> for success/failure with fallback
    auto chain = cf::make_policy_chain<bool, int>(
        [](int x) -> std::optional<bool> {
            if (x > 100)
                return true;
            return std::nullopt;
        },
        [](int x) -> std::optional<bool> {
            if (x > 50)
                return false;
            return std::nullopt;
        },
        [](int) -> std::optional<bool> {
            return false; // Default
        });

    assert(chain.execute(150) == true); // First policy matches
    assert(chain.execute(75) == false); // Second policy matches
    assert(chain.execute(10) == false); // Default fallback

    std::cout << "All assertions passed!" << std::endl;
}

int main() {
    example_basic_usage();
    example_factory_function();
    example_builder();
    example_multiple_args();
    example_void_output();

    std::cout << "\n=== All examples completed successfully! ===" << std::endl;
    return 0;
}
