/**
 * @file    base/include/base/helpers/once_init.hpp
 * @brief   Provides thread-safe lazy initialization utilities.
 *
 * Implements the CallOnceInit template for one-time resource
 * initialization with thread safety and forced re-initialization
 * support.
 *
 * @author  Charliechen114514
 * @date    2026-02-21
 * @version 0.1
 * @since   0.1
 * @ingroup base_helpers
 */
#pragma once
#include <mutex>
#include <utility>

namespace cf {

/**
 * @brief  Thread-safe lazy initialization template.
 *
 * Provides a base class for implementing lazy-initialized resources
 * with thread-safe initialization and forced re-initialization
 * capabilities. Derived classes must implement the initialization
 * logic.
 *
 * @tparam Resources Type of the resource to be lazily initialized.
 *
 * @ingroup base_helpers
 *
 * @note   Not thread-safe during force_reinit() calls. External
 *         synchronization is required if concurrent re-initialization
 *         is possible.
 *
 * @code
 * class MyInitializer : public cf::CallOnceInit<MyData> {
 * protected:
 *     bool init_resources() override {
 *         // Perform initialization
 *         resource.data = fetch_data();
 *         return true;
 *     }
 *     bool force_do_reinit() override {
 *         // Perform re-initialization
 *         resource.data = fetch_data();
 *         return true;
 *     }
 * };
 * @endcode
 */
template <typename Resources> struct CallOnceInit {
  public:
    /**
     * @brief  Constructs with forwarded arguments for the resource.
     *
     * Initializes the resource with the provided arguments and marks
     * it as initialized without calling init_resources().
     *
     * @tparam Args Types of arguments to forward to Resources constructor.
     * @param[in] args Arguments to forward to the Resources constructor.
     *
     * @throws     May throw if Resources constructor throws.
     *
     * @note       None.
     *
     * @warning    None.
     *
     * @since      0.1
     * @ingroup    base_helpers
     */
    template <typename... Args> explicit CallOnceInit(Args&&... args)
        : resource(std::forward<Args>(args)...), initialized(true) {}

    /**
     * @brief  Default constructor. Does not initialize the resource.
     *
     * The resource is initialized on the first call to
     * get_resources().
     *
     * @throws     None.
     *
     * @note       None.
     *
     * @warning    None.
     *
     * @since      0.1
     * @ingroup    base_helpers
     */
    CallOnceInit() = default;

    /**
     * @brief  Retrieves the resource, initializing if necessary.
     *
     * If the resource has not been initialized, this method triggers
     * thread-safe initialization using std::call_once. Subsequent
     * calls return the cached resource.
     *
     * @return     Reference to the initialized resource.
     *
     * @throws     May throw if init_resources() throws.
     *
     * @note       Thread-safe for initialization. Concurrent calls
     *             block until initialization completes.
     *
     * @warning    Concurrent re-initialization via force_reinit()
     *             requires external synchronization.
     *
     * @since      0.1
     * @ingroup    base_helpers
     */
    Resources& get_resources() {
        if (!initialized) {
            std::call_once(init_flag, [this]() {
                init_resources();
                initialized = true;
            });
        }
        return resource;
    }

    /**
     * @brief  Forces re-initialization of the resource.
     *
     * Locks the internal mutex and calls force_do_reinit() to
     * re-initialize the resource, even if it was already initialized.
     *
     * @throws     May throw if force_do_reinit() throws.
     *
     * @note       Blocks until the internal mutex is acquired.
     *
     * @warning    Not thread-safe with respect to concurrent
     *             get_resources() calls. External synchronization is
     *             required.
     *
     * @since      0.1
     * @ingroup    base_helpers
     */
    void force_reinit() {
        std::lock_guard<std::mutex> lock(force_mtx);
        force_do_reinit();
        initialized = true;
    }

    /**
     * @brief  Forces re-initialization with new arguments.
     *
     * Locks the internal mutex, replaces the resource with a new
     * instance constructed from the provided arguments, and marks
     * it as initialized.
     *
     * @tparam Args Types of arguments to forward to Resources constructor.
     * @param[in] args Arguments to forward to the Resources constructor.
     *
     * @throws     May throw if Resources constructor throws.
     *
     * @note       Blocks until the internal mutex is acquired.
     *
     * @warning    Not thread-safe with respect to concurrent
     *             get_resources() calls. External synchronization is
     *             required.
     *
     * @since      0.1
     * @ingroup    base_helpers
     */
    template <typename... Args> void force_reinit(Args&&... args) {
        std::lock_guard<std::mutex> lock(force_mtx);
        resource = Resources(std::forward<Args>(args)...);
        initialized = true;
    }

  protected:
    /// The managed resource. Ownership: owner.
    Resources resource;

    /**
     * @brief  Performs the actual resource initialization.
     *
     * Called during first access to initialize the resource.
     * Implementations should populate the `resource` member.
     *
     * @return     true if initialization succeeded, false otherwise.
     *
     * @throws     Implementation-defined exceptions.
     *
     * @note       Called at most once during the lifetime of the object
     *             unless force_reinit() is called.
     *
     * @warning    None.
     *
     * @since      0.1
     * @ingroup    base_helpers
     */
    virtual bool init_resources() = 0;

    /**
     * @brief  Performs forced re-initialization.
     *
     * Called when force_reinit() is invoked to re-initialize
     * the resource.
     *
     * @return     true if re-initialization succeeded, false otherwise.
     *
     * @throws     Implementation-defined exceptions.
     *
     * @note       May be called multiple times.
     *
     * @warning    None.
     *
     * @since      0.1
     * @ingroup    base_helpers
     */
    virtual bool force_do_reinit() = 0;

  private:
    /// Ensures one-time initialization. Ownership: owner.
    std::once_flag init_flag;

    /// Protects force_reinit() operations. Ownership: owner.
    std::mutex force_mtx;

    /// Tracks initialization state. Ownership: owner.
    bool initialized{false};
};

} // namespace cf
