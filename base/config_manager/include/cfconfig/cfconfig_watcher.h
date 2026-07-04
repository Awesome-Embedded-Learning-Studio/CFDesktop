/**
 * @file cfconfig_watcher.h
 * @brief Watcher callback definitions for ConfigStore.
 *
 * Defines the callback function type and handle type for
 * configuration change watchers.
 *
 * @date 2026-03-17
 * @version 1.0
 */

#pragma once

#include <any>
#include <functional>

namespace cf::config {

// Forward declarations
struct Key;
/** @brief Configuration layer precedence. */
enum class Layer : int;

/**
 * @brief Configuration change callback function type.
 *
 * This callback is invoked when a watched configuration key changes.
 *
 * @param k           The configuration key that changed
 * @param old_value   Pointer to the old value (nullptr for new keys)
 * @param new_value   Pointer to the new value (nullptr for deleted keys)
 * @param from_layer  The layer where the change originated
 *
 * @note Both old_value and new_value are pointers to std::any stored internally.
 *       They remain valid only during the callback execution.
 * @note Avoid calling ConfigStore::set() within the callback to prevent deadlocks.
 */
using Watcher = std::function<void(const Key& k, const std::any* old_value,
                                   const std::any* new_value, Layer from_layer)>;

/**
 * @brief Watcher management handle.
 *
 * Opaque handle type used to cancel a watcher.
 * Returned by ConfigStore::watch() and used with ConfigStore::unwatch().
 */
using WatcherHandle = std::size_t;

} // namespace cf::config
