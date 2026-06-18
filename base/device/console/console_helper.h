#pragma once
#include "aex/policy_chain/policy_chain.hpp"
#include "base/export.h"
#include "console_defines.h"
#include <any>
#include <memory>
#include <string_view>

namespace cf::base::device::console {

using aex::PolicyChain;

/**
 * @file    console_helper.h
 * @brief   Helper class for querying console properties and dimensions.
 *
 * Provides a unified interface for console-related queries such as size and
 * color support capabilities.
 *
 * @author  N/A
 * @date    N/A
 * @version N/A
 * @since   N/A
 * @ingroup device
 */

/**
 * @brief  Helper class for console property queries.
 *
 * Provides methods to query console properties and dimensions. The class
 * maintains fallback mechanisms for when console information is unavailable.
 *
 * @note   Not thread-safe unless externally synchronized.
 *
 * @ingroup device
 */
class CF_BASE_EXPORT ConsoleHelper {
  public:
    ConsoleHelper();

    /**
     * @brief  Queries the console size.
     *
     * Attempts to retrieve the current console dimensions. May return empty
     * optional if size cannot be determined.
     *
     * @return  Console size if available, empty optional otherwise.
     * @throws  None
     * @note    None
     * @warning None
     * @since   N/A
     * @ingroup device
     */
    std::optional<console_size_t> size() const;

    /**
     * @brief  Queries a console property.
     *
     * Retrieves the value of a specified console property. Supported properties
     * include colorable (bool) indicating color support.
     *
     * @param[in]  property  Name of the property to query (e.g., "colorable").
     * @param[out] value     Pointer to store the retrieved value.
     * @return               True if value is available, false otherwise.
     * @throws               None
     * @note                 Acceptable key-value pairs: colorable <-> bool.
     * @warning              Behavior undefined if value is nullptr.
     * @since                N/A
     * @ingroup              device
     */
    bool query_property(const std::string_view property, std::any* value);

    /**
     * @brief  Returns the fallback console size.
     *
     * Provides a default console size when actual size cannot be determined.
     *
     * @return  Default fallback console dimensions.
     * @throws  None
     * @note    None
     * @warning None
     * @since   N/A
     * @ingroup device
     */
    console_size_t fallback_size() const;

  private:
    /// @brief Policy chain for querying console size. Ownership: owner.
    std::unique_ptr<PolicyChain<console_size_t>> size_querier;
};

} // namespace cf::base::device::console
