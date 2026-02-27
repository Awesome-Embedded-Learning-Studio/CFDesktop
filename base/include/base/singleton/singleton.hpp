/**
 * @file    base/include/base/singleton/singleton.hpp
 * @brief   Thread-safe singleton with explicit initialization.
 *
 * Provides a singleton implementation that requires explicit initialization
 * via init() before accessing the instance. Uses std::call_once for
 * thread-safe initialization.
 *
 * @author  N/A
 * @date    N/A
 * @version N/A
 * @since   N/A
 * @ingroup none
 */

#pragma once
#include <memory>
#include <mutex>
#include <stdexcept>

namespace cf {

/**
 * @brief  Thread-safe singleton with explicit initialization.
 *
 * Provides a singleton implementation that requires explicit initialization
 * via init() before accessing the instance. This pattern allows for
 * parameterized construction and explicit control over initialization timing.
 *
 * @tparam T Type of the singleton instance.
 *
 * @ingroup none
 *
 * @note   Thread-safe. Uses std::call_once for initialization.
 *
 * @warning Accessing instance() before calling init() throws std::logic_error.
 *
 * @code
 * class MyClass {
 * public:
 *     MyClass(int value) : value_(value) {}
 *     void doSomething() {}
 * private:
 *     int value_;
 * };
 *
 * using MySingleton = Singleton<MyClass>;
 * MySingleton::init(42);
 * MySingleton::instance().doSomething();
 * @endcode
 */
template <typename T> class Singleton {
  public:
    /**
     * @brief  Initializes the singleton with the provided arguments.
     *
     * Constructs the singleton instance once, subsequent calls return the
     * existing instance. Thread-safe via std::call_once.
     *
     * @tparam  Args Types of constructor arguments.
     * @param[in] args Arguments to forward to T's constructor.
     * @return        Reference to the initialized singleton instance.
     * @throws        None
     * @note          Thread-safe initialization guaranteed by std::call_once.
     * @warning       Calling init() twice has no effect; the first instance is kept.
     * @since         N/A
     * @ingroup       none
     */
    template <typename... Args>
    static T& init(Args&&... args) {
        std::call_once(flag_, [&] {
            instance_.reset(new T(std::forward<Args>(args)...));
        });
        return *instance_;
    }

    /**
     * @brief  Returns the singleton instance.
     *
     * Requires that init() has been called first.
     *
     * @return Reference to the singleton instance.
     * @throws std::logic_error if init() has not been called.
     * @note   None
     * @warning Must call init() before accessing instance().
     * @since  N/A
     * @ingroup none
     */
    static T& instance() {
        if (!instance_) {
            throw std::logic_error("Singleton not initialized. Call init() first.");
        }
        return *instance_;
    }

    /**
     * @brief  Resets the singleton instance.
     *
     * Destroys the current instance and allows re-initialization via init().
     *
     * @throws None
     * @note   After calling reset(), init() must be called again before
     *         accessing instance().
     * @warning None
     * @since  N/A
     * @ingroup none
     */
    static void reset() { instance_.reset(); }

    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

  protected:
    Singleton() = default;
    virtual ~Singleton() = default;

  private:
    inline static std::unique_ptr<T> instance_ = nullptr;
    inline static std::once_flag flag_;
};

} // namespace cf
