/**
 * @file smartptr_plain_factory.hpp
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief   Provides smart pointer factory templates for automatic lifetime
 *          management.
 *
 * Defines factory templates that create objects using std::unique_ptr and
 * std::shared_ptr. Eliminates the need for manual memory management.
 *
 * @version 0.1
 * @date 2026-03-27
 * @copyright Copyright (c) 2026
 * @ingroup base_factory
 */
#pragma once

#include "base/singleton/simple_singleton.hpp"
#include <memory>
namespace cf {

/**
 * @brief  Factory that creates objects using smart pointers.
 *
 * Provides methods for constructing objects with automatic lifetime
 * management via std::unique_ptr and std::shared_ptr.
 *
 * @tparam Result Type of object to create. Must be constructible from Args.
 * @tparam Args   Argument types forwarded to Result constructor.
 *
 * @note None
 * @warning None
 * @since 0.1
 * @ingroup base_factory
 *
 * @code
 * SmartPtrPlainFactory<MyClass, int> factory;
 * auto unique = factory.make_unique(42);
 * auto shared = factory.make_shared(42);
 * @endcode
 */
template <typename Result, typename... Args> struct SmartPtrPlainFactory {
  public:
    /**
     * @brief  Default constructs the factory.
     *
     * @throws None
     * @note None
     * @warning None
     * @since 0.1
     * @ingroup base_factory
     */
    SmartPtrPlainFactory() = default;

    /**
     * @brief  Destroys the factory instance.
     *
     * @throws None
     * @note None
     * @warning None
     * @since 0.1
     * @ingroup base_factory
     */
    virtual ~SmartPtrPlainFactory() = default;

    /**
     * @brief  Creates a new Result object as std::unique_ptr.
     *
     * Constructs a Result instance with unique ownership by forwarding
     * arguments to its constructor.
     *
     * @param[in] args Arguments forwarded to Result constructor.
     *
     * @return std::unique_ptr<Result> owning the newly created object.
     *
     * @throws None
     * @note None
     * @warning None
     * @since 0.1
     * @ingroup base_factory
     */
    std::unique_ptr<Result> make_unique(Args&&... args) {
        return std::make_unique<Result>(std::forward<Args>(args)...);
    }

    /**
     * @brief  Creates a new Result object as std::shared_ptr.
     *
     * Constructs a Result instance with shared ownership by forwarding
     * arguments to its constructor.
     *
     * @param[in] args Arguments forwarded to Result constructor.
     *
     * @return std::shared_ptr<Result> sharing ownership of the new object.
     *
     * @throws None
     * @note None
     * @warning None
     * @since 0.1
     * @ingroup base_factory
     */
    std::shared_ptr<Result> make_shared(Args&&... args) {
        return std::make_shared<Result>(std::forward<Args>(args)...);
    }
};

/**
 * @brief  Singleton variant of SmartPtrPlainFactory for static access.
 *
 * Combines SmartPtrPlainFactory with SimpleSingleton to provide a
 * globally accessible factory instance using smart pointers.
 *
 * @tparam Result Type of object to create. Must be constructible from Args.
 * @tparam Args   Argument types forwarded to Result constructor.
 *
 * @note None
 * @warning None
 * @since 0.1
 * @ingroup base_factory
 *
 * @code
 * auto& factory = StaticSmartPtrPlainFactory<MyClass, int>::instance();
 * auto obj = factory.make_unique(42);
 * @endcode
 */
template <typename Result, typename... Args> struct StaticSmartPtrPlainFactory
    : public SmartPtrPlainFactory<Result, Args...>,
      public SimpleSingleton<StaticSmartPtrPlainFactory<Result, Args...>> {};

} // namespace cf
