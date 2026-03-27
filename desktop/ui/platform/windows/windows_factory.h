/**
 * @file    desktop/ui/platform/windows/windows_factory.h
 * @brief   Windows-specific factory for creating desktop property strategies.
 *
 * Implements a singleton factory pattern for creating and managing desktop
 * property strategies on Windows platforms.
 *
 * @author  N/A
 * @date    2026-03-27
 * @version 0.1
 * @since   0.1
 * @ingroup none
 */

#pragma once
#include "../../../export.h"
#include "IDesktopPropertyStrategy.h"
#include "base/singleton/simple_singleton.hpp"
#include <memory>

namespace cf::desktop::platform_strategy::windows {

/// @brief Forward declaration of the display size policy maker.
class WindowsDisplaySizePolicy;

/**
 * @brief  Windows-specific singleton factory for desktop property strategies.
 *
 * Creates and manages the lifecycle of desktop property strategy instances
 * specifically for Windows environments.
 *
 * @note   Inherits singleton semantics from SimpleSingleton base.
 *
 * @ingroup none
 *
 * @code
 * auto& factory = WindowsDeskProStrategyFactory::instance();
 * auto* strategy = factory.create(StrategyType::DisplaySizePolicy);
 * @endcode
 */
class CF_DESKTOP_EXPORT WindowsDeskProStrategyFactory
    : public SimpleSingleton<WindowsDeskProStrategyFactory> {
  public:
    /**
     * @brief  Constructs a new WindowsDeskProStrategyFactory instance.
     * @throws None
     * @note   None
     * @warning None
     * @since  0.1
     * @ingroup none
     */
    WindowsDeskProStrategyFactory();

    /**
     * @brief  Destroys the WindowsDeskProStrategyFactory instance.
     * @throws None
     * @note   None
     * @warning None
     * @since  0.1
     * @ingroup none
     */
    ~WindowsDeskProStrategyFactory();

    /**
     * @brief  Creates a desktop property strategy of the specified type.
     *
     * @param[in] t The strategy type to create.
     * @return      Pointer to the created strategy, or nullptr on failure.
     * @throws      None
     * @note        Caller owns the returned pointer and must call release().
     * @warning     None
     * @since       0.1
     * @ingroup     none
     */
    IDesktopPropertyStrategy* create(IDesktopPropertyStrategy::StrategyType t);

    /**
     * @brief  Releases a previously created strategy.
     *
     * @param[in] policy Pointer to the strategy to release. May be nullptr.
     * @throws           None
     * @note             Safe to call with nullptr.
     * @warning          None
     * @since            0.1
     * @ingroup          none
     */
    void release(IDesktopPropertyStrategy* policy);

  private:
    /// @brief Display size policy maker. Ownership: unique.
    std::unique_ptr<WindowsDisplaySizePolicy> display_size_policy_;
};

} // namespace cf::desktop::platform_strategy::windows
