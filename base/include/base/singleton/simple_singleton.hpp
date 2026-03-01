/**
 * @file    base/include/base/singleton/simple_singleton.hpp
 * @brief   Simple thread-safe singleton using Meyer's singleton pattern.
 *
 * Provides a minimal singleton implementation that leverages C++11 guarantees
 * for thread-safe static local variable initialization.
 *
 * @author  N/A
 * @date    N/A
 * @version N/A
 * @since   N/A
 * @ingroup none
 */

#pragma once

namespace cf {

/**
 * @brief  Simple thread-safe singleton using Meyer's singleton pattern.
 *
 * Provides a minimal singleton implementation that leverages C++11 guarantees
 * for thread-safe static local variable initialization. The instance is created
 * on first access and destroyed on application exit.
 *
 * @tparam SingleTargetClass Type of the singleton instance.
 *
 * @ingroup none
 *
 * @note   Thread-safe. C++11 guarantees thread-safe initialization of function-local
 *         static variables.
 *
 * @warning None
 *
 * @code
 * class MyClass {
 * public:
 *     void doSomething() {}
 * };
 *
 * using MySingleton = SimpleSingleton<MyClass>;
 * MySingleton::instance().doSomething();
 * @endcode
 */
template <typename SingleTargetClass> class SimpleSingleton {
  public:
    /**
     * @brief  Returns the singleton instance.
     *
     * Creates the instance on first call, subsequent calls return the same
     * instance.
     *
     * @return Reference to the singleton instance.
     * @throws None
     * @note   Thread-safe initialization guaranteed by C++11 standard.
     * @warning None
     * @since  N/A
     * @ingroup none
     */
    static SingleTargetClass& instance() {
        static SingleTargetClass target;
        return target;
    }

  private:
    SimpleSingleton(const SimpleSingleton&) = delete;
    SimpleSingleton& operator=(const SimpleSingleton&) = delete;
    SimpleSingleton(SimpleSingleton&&) = delete;
    SimpleSingleton& operator=(SimpleSingleton&&) = delete;
};

} // namespace cf
