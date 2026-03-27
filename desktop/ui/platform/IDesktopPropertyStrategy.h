/**
 * @file    IDesktopPropertyStrategy.h
 * @brief   Abstract base interface for desktop property strategies.
 *
 * Defines the contract for platform-specific desktop property strategies,
 * including strategy type identification and ABI-friendly naming.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-03-26
 * @version 0.1
 * @since   0.1
 * @ingroup none
 */

#pragma once
#include "../../../export.h"

namespace cf::desktop::platform_strategy {

/**
 * @brief  Abstract base class for desktop property strategies.
 *
 * Provides the common interface that all platform-specific property
 * strategies must implement, including type tracking and name querying
 * for ABI compatibility.
 *
 * @note   Not thread-safe unless externally synchronized.
 * @ingroup none
 */
class CF_DESKTOP_EXPORT IDesktopPropertyStrategy {
  public:
    /**
     * @brief  Defines the available strategy types.
     *
     * Identifies the specific property category that a strategy handles.
     */
    enum class StrategyType {
        Unavailable,       ///< No valid strategy type.
        DisplaySizePolicy, ///< Strategy manages display size and behavior.
        Extensions         ///< Strategy manages desktop extensions.
    };

    /**
     * @brief  Constructs a strategy with the specified type.
     *
     * @param[in] t The strategy type identifier.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.1
     * @ingroup    none
     */
    IDesktopPropertyStrategy(const StrategyType t);

    /**
     * @brief  Destroys the strategy instance.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since   0.1
     * @ingroup none
     */
    virtual ~IDesktopPropertyStrategy() = default;

    /**
     * @brief  Returns the ABI-friendly name of the strategy.
     *
     * Provides a stable string identifier for the strategy, used for
     * runtime type checking across ABI boundaries.
     *
     * @return Null-terminated string containing the strategy name.
     * @throws None
     * @note   The returned string must remain valid for the lifetime of
     *         the instance (typically a static string literal).
     * @warning None
     * @since   0.1
     * @ingroup none
     */
    virtual const char* name() const noexcept = 0;

  private:
    /**
     * @brief  The strategy type identifier.
     *
     * Ownership: owner. Immutable after construction.
     */
    const StrategyType type;
};

} // namespace cf::desktop::platform_strategy
