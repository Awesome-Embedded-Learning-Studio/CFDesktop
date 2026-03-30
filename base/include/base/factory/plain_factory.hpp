/**
 * @file plain_factory.hpp
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief   Provides plain pointer factory templates for cross-ABI compatibility.
 *
 * Defines factory templates that create objects using raw pointers. Useful for
 * scenarios requiring ABI stability across compilation boundaries.
 *
 * @version 0.1
 * @date 2026-03-27
 * @copyright Copyright (c) 2026
 * @ingroup base_factory
 */
#pragma once
#include "base/singleton/simple_singleton.hpp"
#include <utility>
namespace cf {

/**
 * @brief  Factory that creates objects as raw pointers.
 *
 * Provides a virtual interface for constructing objects with raw pointers.
 * Intended for cross-ABI scenarios where smart pointers may not be suitable.
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
 * PlainFactory<MyClass, int, double> factory;
 * MyClass* obj = factory.make(42, 3.14);
 * @endcode
 */
template <typename Result, typename... Args> struct PlainFactory {
    /**
     * @brief  Destroys the factory instance.
     *
     * @throws None
     * @note None
     * @warning None
     * @since 0.1
     * @ingroup base_factory
     */
    virtual ~PlainFactory() = default;

    /**
     * @brief  Creates a new Result object using raw pointer.
     *
     * Constructs a Result instance by forwarding arguments to its constructor.
     * Caller owns the returned pointer and is responsible for deletion.
     *
     * @param[in] args Arguments forwarded to Result constructor.
     *
     * @return Pointer to newly created Result object. Caller owns.
     *
     * @throws None
     * @note Caller must delete the returned pointer to avoid memory leak.
     * @warning Raw pointer ownership; consider smart pointers for new code.
     * @since 0.1
     * @ingroup base_factory
     */
    Result* make(Args&&... args) { return new Result(std::forward<Args>(args)...); }
};

/**
 * @brief  Singleton variant of PlainFactory for static access.
 *
 * Combines PlainFactory with SimpleSingleton to provide a globally
 * accessible factory instance using raw pointers.
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
 * auto& factory = StaticPlainFactory<MyClass, int>::instance();
 * MyClass* obj = factory.make(42);
 * @endcode
 */
template <typename Result, typename... Args> struct StaticPlainFactory
    : public PlainFactory<Result, Args...>,
      public SimpleSingleton<StaticPlainFactory<Result, Args...>> {};

} // namespace cf
