/**
 * @file registered_factory.hpp
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief   Provides registered factory templates for abstract interface creation.
 *
 * Defines factory templates that create objects via registered creator functions,
 * rather than by directly calling constructors. This is the dual of
 * SmartPtrPlainFactory: it works for abstract interfaces where direct
 * construction is impossible.
 *
 * @version 0.1
 * @date 2026-03-30
 * @copyright Copyright (c) 2026
 * @ingroup base_factory
 */
#pragma once

#include "base/singleton/simple_singleton.hpp"
#include <functional>
#include <memory>

namespace cf {

/**
 * @brief  Factory that creates objects via a registered creator function.
 *
 * Unlike SmartPtrPlainFactory which directly calls Result(args...), this
 * factory delegates creation to a std::function registered at startup.
 * This makes it suitable for abstract interfaces where the concrete type
 * is determined by the platform.
 *
 * @tparam Interface  The abstract base type to create.
 *
 * @note None
 * @warning None
 * @since N/A
 * @ingroup base_factory
 *
 * @code
 * RegisteredFactory<IMyInterface> factory;
 * factory.register_creator([](){ return new ConcreteImpl; });
 * auto obj = factory.make_unique();
 * @endcode
 */
template <typename Interface> class RegisteredFactory {
  public:
    /// Function type that creates a raw pointer to Interface.
    using Creator = std::function<Interface*()>;

    /// Function type that destroys a raw pointer to Interface.
    using Destroyer = std::function<void(Interface*)>;

    /// Convenience alias for the unique_ptr type returned by make_unique().
    using unique_ptr_type = std::unique_ptr<Interface, Destroyer>;

    RegisteredFactory() = default;
    virtual ~RegisteredFactory() = default;

    /**
     * @brief  Registers a creator/destroyer pair.
     *
     * Should be called once during application startup by the
     * platform-specific initialization code.
     *
     * @param[in] creator   Function that returns a new Interface*.
     * @param[in] destroyer Optional custom deleter. If null, default delete is used.
     *
     * @throws None
     * @note   Calling this multiple times replaces the previous registration.
     * @warning None
     * @since  0.1
     * @ingroup base_factory
     */
    void register_creator(Creator creator, Destroyer destroyer = nullptr) {
        creator_ = std::move(creator);
        destroyer_ = std::move(destroyer);
    }

    /**
     * @brief  Creates an Interface as std::unique_ptr with custom deleter support.
     *
     * Returns nullptr if no creator has been registered or if the creator
     * returns nullptr.
     *
     * @return std::unique_ptr<Interface, Destroyer> owning the newly created object, or nullptr.
     *
     * @throws None
     * @note   If a destroyer was registered, it is used as the deleter.
     *         Otherwise, a default delete deleter is used.
     * @warning None
     * @since  0.1
     * @ingroup base_factory
     */
    std::unique_ptr<Interface, Destroyer> make_unique() {
        if (!creator_)
            return nullptr;
        Interface* raw = creator_();
        if (!raw)
            return nullptr;
        if (destroyer_) {
            return std::unique_ptr<Interface, Destroyer>(raw, destroyer_);
        }
        return std::unique_ptr<Interface, Destroyer>(raw, [](Interface* p) { delete p; });
    }

    /**
     * @brief  Creates an Interface as std::shared_ptr.
     *
     * Returns nullptr if no creator has been registered or if the creator
     * returns nullptr.
     *
     * @return std::shared_ptr<Interface> sharing ownership, or nullptr.
     *
     * @throws None
     * @note   If a destroyer was registered, it is used as the deleter.
     * @warning None
     * @since  0.1
     * @ingroup base_factory
     */
    std::shared_ptr<Interface> make_shared() {
        if (!creator_)
            return nullptr;
        Interface* raw = creator_();
        if (!raw)
            return nullptr;
        if (destroyer_) {
            auto d = destroyer_;
            return std::shared_ptr<Interface>(raw, [d](Interface* p) { d(p); });
        }
        return std::shared_ptr<Interface>(raw);
    }

    /**
     * @brief  Checks whether a creator has been registered.
     *
     * @return true if a non-empty creator is registered.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.1
     * @ingroup base_factory
     */
    bool has_creator() const { return static_cast<bool>(creator_); }

  protected:
    Creator creator_;
    Destroyer destroyer_;
};

/**
 * @brief  Singleton variant of RegisteredFactory for static access.
 *
 * Combines RegisteredFactory with SimpleSingleton to provide a
 * globally accessible factory instance that uses registered creators.
 *
 * @tparam Interface Type of object to create.
 *
 * @note None
 * @warning None
 * @since N/A
 * @ingroup base_factory
 *
 * @code
 * using MyFactory = StaticRegisteredFactory<IMyInterface>;
 * MyFactory::instance().register_creator([](){ return new Concrete; });
 * auto obj = MyFactory::instance().make_unique();
 * @endcode
 */
template <typename Interface> class StaticRegisteredFactory
    : public RegisteredFactory<Interface>,
      public SimpleSingleton<StaticRegisteredFactory<Interface>> {};

} // namespace cf
