/**
 * @file    DesktopPropertyStrategyFactory.h
 * @brief   Factory for creating platform-specific desktop property strategies.
 *
 * Provides template methods for creating unique and shared pointers to concrete
 * desktop property strategy implementations based on strategy type.
 *
 * @author  N/A
 * @date    N/A
 * @version N/A
 * @since   N/A
 * @ingroup none
 */

#pragma once
#include "../../../export.h"
#include "IDesktopPropertyStrategy.h"
#include <functional>
#include <memory>

namespace cf::desktop::platform_strategy {

/**
 * @brief  Factory for creating platform-specific desktop property strategies.
 *
 * Provides methods to instantiate concrete strategy implementations with either
 * unique or shared pointer semantics.
 *
 * @note   Not thread-safe unless externally synchronized.
 *
 * @ingroup none
 *
 * @code
 * PlatformFactory factory;
 * auto strategy = factory.create_unique<MyStrategy>(StrategyType::Linux);
 * @endcode
 */
class CF_DESKTOP_EXPORT PlatformFactory {
  public:
    /**
     * @brief  Constructs a new PlatformFactory instance.
     * @throws None
     * @note   None
     * @warning None
     * @since  N/A
     * @ingroup none
     */
    PlatformFactory();

    /**
     * @brief  Destroys the PlatformFactory instance.
     * @throws None
     * @note   None
     * @warning None
     * @since  N/A
     * @ingroup none
     */
    ~PlatformFactory();

    /// @brief Deleter function type for strategy pointers.
    using StrategyDeleter = std::function<void(IDesktopPropertyStrategy*)>;

    /**
     * @brief  Creates a unique pointer to a concrete strategy implementation.
     *
     * Creates a strategy instance of the specified type and returns it as a
     * unique_ptr with the requested concrete policy type.
     *
     * @tparam ConcretePolicy Must derive from IDesktopPropertyStrategy.
     * @param[in] t           The strategy type to create.
     * @return                Unique pointer to the concrete strategy, or nullptr
     *                        if type mismatch or creation fails.
     * @throws                None
     * @note                  Memory ownership is transferred to the caller.
     * @warning               Returns nullptr if dynamic_cast fails.
     * @since                 N/A
     * @ingroup               none
     */
    template <typename ConcretePolicy> std::unique_ptr<ConcretePolicy, StrategyDeleter>
    create_unique(const IDesktopPropertyStrategy::StrategyType t) {
        static_assert(std::is_base_of_v<IDesktopPropertyStrategy, ConcretePolicy>,
                      "ConcretePolicy must derive from IDesktopPropertyStrategy");

        auto base = factorize_unique(t);

        if (!base) {
            return {nullptr, nullptr};
        }
        auto raw = dynamic_cast<ConcretePolicy*>(base.get());
        if (!raw) {
            return {nullptr, nullptr};
        }

        auto deleter = base.get_deleter();

        /* Memory Has been transferred, so can safely releases */
        auto p [[maybe_unused]] = base.release();

        return std::unique_ptr<ConcretePolicy, StrategyDeleter>(raw, deleter);
    }

    /**
     * @brief  Creates a shared pointer to a concrete strategy implementation.
     *
     * Creates a strategy instance of the specified type and returns it as a
     * shared_ptr with the requested concrete policy type.
     *
     * @tparam ConcretePolicy Must derive from IDesktopPropertyStrategy.
     * @param[in] t           The strategy type to create.
     * @return                Shared pointer to the concrete strategy, or
     *                        nullptr if type mismatch or creation fails.
     * @throws                None
     * @note                  None
     * @warning               Returns nullptr if dynamic_pointer_cast fails.
     * @since                 N/A
     * @ingroup               none
     */
    template <typename ConcretePolicy>
    std::shared_ptr<ConcretePolicy> create_shared(const IDesktopPropertyStrategy::StrategyType t) {
        static_assert(std::is_base_of_v<IDesktopPropertyStrategy, ConcretePolicy>,
                      "ConcretePolicy must derive from IDesktopPropertyStrategy");

        auto base = factorize_shared(t);

        if (!base)
            return nullptr;

        return std::dynamic_pointer_cast<ConcretePolicy>(base);
    }

  protected:
    /**
     * @brief  Factory method that creates a unique pointer to a base strategy.
     *
     * @param[in] t The strategy type to create.
     * @return      Unique pointer to the base strategy interface.
     * @throws      None
     * @note        Implementation-specific behavior.
     * @warning     None
     * @since       N/A
     * @ingroup     none
     */
    std::unique_ptr<IDesktopPropertyStrategy, StrategyDeleter>
    factorize_unique(const IDesktopPropertyStrategy::StrategyType t);

    /**
     * @brief  Factory method that creates a shared pointer to a base strategy.
     *
     * @param[in] t The strategy type to create.
     * @return      Shared pointer to the base strategy interface.
     * @throws      None
     * @note        Implementation-specific behavior.
     * @warning     None
     * @since       N/A
     * @ingroup     none
     */
    std::shared_ptr<IDesktopPropertyStrategy>
    factorize_shared(const IDesktopPropertyStrategy::StrategyType t);

  private:
    /// @brief Implementation details (PIMPL idiom). Ownership: shared.
    class PlatformImpl;
    std::shared_ptr<PlatformImpl> impl_;
};

} // namespace cf::desktop::platform_strategy
