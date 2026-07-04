/**
 * @file    cfconfig_layer.h
 * @brief   Defines the Layer enumeration for configuration storage hierarchy.
 *
 * This header specifies the priority levels used when querying configuration
 * values, from temporary memory storage to system-wide fallback defaults.
 *
 * @author  N/A
 * @date    N/A
 * @version N/A
 * @since   N/A
 * @ingroup config
 */

#pragma once

namespace cf::config {

/**
 * @brief  Defines the configuration storage layer priority.
 *
 * The layer enumeration determines the order in which configuration sources
 * are queried. Each layer represents a distinct storage scope with specific
 * semantics:
 *
 * - **System**: Lowest priority; provides fallback defaults.
 * - **User**: User-level configuration settings.
 * - **App**: Application-specific configuration.
 * - **Temp**: Highest priority; volatile memory settings for testing or
 *            runtime overrides.
 *
 * Configuration lookup proceeds from highest to lowest priority (Temp -> App
 * -> User -> System) until a layer provides a value for the requested key.
 *
 * @ingroup config
 */
enum class Layer { System, User, App, Temp };

} // namespace cf::config
