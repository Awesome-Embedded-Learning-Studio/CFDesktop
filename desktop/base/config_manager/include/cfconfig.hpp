/**
 * @file cfconfig.hpp
 * @brief Main configuration storage interface for CFDesktop.
 *
 * Provides the ConfigStore class which is the main entry point for
 * configuration management in CFDesktop.
 *
 * @date 2026-03-17
 * @version 1.0
 */

#pragma once

#include "base/singleton/simple_singleton.hpp"
#include "cfconfig/cfconfig_domain_handle.h"
#include "cfconfig/cfconfig_result.h"
#include "cfconfig/cfconfig_watcher.h"
#include "cfconfig_key.h"
#include "cfconfig_layer.h"
#include "cfconfig_notify_policy.h"
#include <QVariant>
#include <any>
#include <memory>
#include <optional>

namespace cf::config {

class ConfigStoreImpl;          // Pimpl forward declaration
class IConfigStorePathProvider; // Path provider forward declaration

/**
 * @brief Configuration storage center.
 *
 * Provides four-layer configuration storage, structured key naming,
 * change monitoring, and persistence capabilities.
 *
 * @note Thread-safe: All API calls can be used in multi-threaded environments
 * @note Singleton pattern: Use ConfigStore::instance() to access
 * @note Auto-initialization: Loads from paths on first access
 *
 * @code
 * using namespace cf::config;
 *
 * // Register a key
 * Key theme_key{.full_key = "app.theme.name", .full_description = "Application theme name"};
 * ConfigStore::instance().register_key(theme_key, std::string("default"), Layer::App);
 *
 * // Query configuration (using KeyView)
 * KeyView kv{.group = "app.theme", .key = "name"};
 * std::string theme = ConfigStore::instance().query<std::string>(kv, "default");
 *
 * // Modify configuration
 * ConfigStore::instance().set(kv, std::string("dark"), Layer::App);
 *
 * // Sync to disk
 * ConfigStore::instance().sync(SyncMethod::Async);
 * @endcode
 */
class ConfigStore : public SimpleSingleton<ConfigStore> {
  public:
    ConfigStore();
    ~ConfigStore();

    // Delete copy and move
    ConfigStore(const ConfigStore&) = delete;
    ConfigStore& operator=(const ConfigStore&) = delete;
    ConfigStore(ConfigStore&&) = delete;
    ConfigStore& operator=(ConfigStore&&) = delete;

    /**
     * @brief Initialize with custom path provider.
     *
     * Must be called before any other ConfigStore operations.
     * Only takes effect on first call; subsequent calls are ignored.
     *
     * @param[in] path_provider Custom path provider for config file locations.
     *
     * @code
     * auto mock_provider = std::make_shared<test::MockPathProvider>("/tmp/test_config");
     * ConfigStore::instance().initialize(mock_provider);
     * @endcode
     */
    void initialize(std::shared_ptr<IConfigStorePathProvider> path_provider);

    /**
     * @brief Get a handle to a named config domain.
     *
     * Each domain has its own set of backend files per layer.
     * The "default" domain is used by all non-domain operations.
     * Domains are lazily created on first access.
     *
     * @param[in] name Domain name (e.g., "wallpaper", "ui", "network").
     * @return ConfigDomainHandle scoped to the named domain.
     *
     * @code
     * auto wallpaper = ConfigStore::instance().domain("wallpaper");
     * wallpaper.set(KeyView{.group="source", .key="path"}, std::string("/pics/bg.jpg"));
     * @endcode
     */
    ConfigDomainHandle domain(const std::string& name);

    /* ========== Query operations ========== */

    /**
     * @brief Query configuration value (by priority).
     *
     * Searches layers in order: Temp → App → User → System
     *
     * @tparam Value Value type
     * @param[in] key Key view
     * @return std::optional<Value> with value if exists, std::nullopt otherwise
     */
    template <typename Value> [[nodiscard]] std::optional<Value> query(const KeyView key);

    /**
     * @brief Query configuration value (by priority) with default.
     *
     * Searches layers in order: Temp → App → User → System
     *
     * @tparam Value Value type
     * @param[in] key Key view
     * @param[in] default_value Default value if not found
     * @return Configuration value or default_value
     */
    template <typename Value>
    [[nodiscard]] Value query(const KeyView key, const Value& default_value);

    /**
     * @brief Query configuration value from a specific layer.
     *
     * Does not use priority merge, directly queries the specified layer.
     *
     * @return std::optional<Value> with value if exists, std::nullopt otherwise
     */
    template <typename Value>
    [[nodiscard]] std::optional<Value> query(const KeyView key, Layer layer);

    /**
     * @brief Query configuration value from a specific layer with default.
     *
     * Does not use priority merge, directly queries the specified layer.
     *
     * @return Configuration value or default_value
     */
    template <typename Value>
    [[nodiscard]] Value query(const KeyView key, Layer layer, const Value& default_value);

    /**
     * @brief Check if a configuration key exists (recursive search all layers).
     *
     * Searches layers in order: Temp → App → User → System
     */
    [[nodiscard]] bool has_key(const KeyView key);

    /**
     * @brief Check if a key exists in a specific layer.
     */
    [[nodiscard]] bool has_key(const KeyView key, Layer layer);

    /* ========== Write operations ========== */

    /**
     * @brief Set a configuration value.
     *
     * @param[in] key Key view
     * @param[in] v Configuration value
     * @param[in] layer Target layer (default App layer)
     * @param[in] notify_policy Notification policy (default immediate)
     * @return true if successful, false otherwise
     */
    template <typename Value>
    [[nodiscard]] bool set(const KeyView key, const Value& v, Layer layer = Layer::App,
                           NotifyPolicy notify_policy = NotifyPolicy::Immediate);

    /**
     * @brief Register a key (explicit declaration).
     *
     * After registration, the key can be used for configuration management.
     *
     * @param[in] key Persistent key
     * @param[in] init_value Initial value
     * @param[in] layer Target layer
     * @param[in] notify_policy Notification policy
     * @return RegisterResult Registration result
     */
    template <typename Value>
    [[nodiscard]] RegisterResult register_key(const Key& key, const Value& init_value,
                                              Layer layer = Layer::App,
                                              NotifyPolicy notify_policy = NotifyPolicy::Immediate);

    /**
     * @brief Unregister a key.
     *
     * @param[in] key Key to unregister
     * @param[in] layer Target layer
     * @param[in] notify_policy Notification policy
     * @return UnRegisterResult Unregistration result
     */
    [[nodiscard]] UnRegisterResult
    unregister_key(const Key& key, Layer layer = Layer::App,
                   NotifyPolicy notify_policy = NotifyPolicy::Immediate);

    /**
     * @brief Clear all configurations.
     *
     * @warning Use with caution, this operation cannot be undone
     */
    void clear();

    /**
     * @brief Clear all configurations in a specific layer.
     *
     * @param[in] layer Target layer to clear
     * @warning Use with caution, this operation cannot be undone
     */
    void clear_layer(Layer layer);

    /* ========== Watcher operations ========== */

    /**
     * @brief Watch for configuration changes.
     *
     * @param[in] key_pattern Key pattern, supports wildcards
     * @param[in] callback Callback function
     * @param[in] policy Notification policy
     * @return WatcherHandle Watcher handle for unwatch()
     */
    WatcherHandle watch(const std::string& key_pattern, Watcher callback,
                        NotifyPolicy policy = NotifyPolicy::Immediate);

    /**
     * @brief Cancel watching.
     *
     * @param[in] handle Handle returned by watch()
     */
    void unwatch(WatcherHandle handle);

    /**
     * @brief Manually trigger notification.
     *
     * Triggers all Watchers with NotifyPolicy::Manual.
     *
     * @return NotifyResult Notification result
     */
    [[nodiscard]] NotifyResult notify();

    /**
     * @brief Get count of pending changes.
     *
     * For diagnostics, counts changes not yet notified.
     */
    [[nodiscard]] std::size_t pending_changes() const;

    /* ========== Persistence operations ========== */

    /**
     * @brief Sync method.
     */
    enum class SyncMethod { Sync, Async };

    /**
     * @brief Sync configuration to disk.
     *
     * Only writes layers with changes.
     *
     * @param[in] m Sync method
     */
    void sync(const SyncMethod m = SyncMethod::Async);

    /**
     * @brief Reload configuration from disk.
     *
     * Clears cache and re-reads all layer configuration files.
     * Discards all temporary configurations in Temp layer.
     */
    void reload();

  private:
    std::shared_ptr<ConfigStoreImpl> impl;
};

} // namespace cf::config

// Include implementation for template methods
#include "impl/config_impl.h"

namespace cf::config {

// ============================================================================
// Template Implementations
// ============================================================================

namespace detail {

/**
 * @brief Helper to convert std::any to a specific type.
 */
template <typename T> T any_cast(const std::any& value, const T& default_value) {
    if (value.type() == typeid(void)) {
        return default_value;
    }

    // Direct type match
    if (value.type() == typeid(T)) {
        try {
            return std::any_cast<T>(value);
        } catch (const std::bad_any_cast&) {
            return default_value;
        }
    }

    // QVariant conversion path
    if (value.type() == typeid(QVariant)) {
        try {
            QVariant qvar = std::any_cast<QVariant>(value);
            if constexpr (std::is_same_v<T, std::string>) {
                return qvar.toString().toStdString();
            } else if constexpr (std::is_integral_v<T>) {
                return static_cast<T>(qvar.toLongLong());
            } else if constexpr (std::is_floating_point_v<T>) {
                return static_cast<T>(qvar.toDouble());
            } else if constexpr (std::is_same_v<T, bool>) {
                return qvar.toBool();
            }
        } catch (const std::bad_any_cast&) {
            // Fall through to default
        }
    }

    // String to numeric conversion
    if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
        if (value.type() == typeid(std::string)) {
            try {
                std::string str = std::any_cast<std::string>(value);
                if constexpr (std::is_integral_v<T>) {
                    return static_cast<T>(std::stoll(str));
                } else {
                    return static_cast<T>(std::stod(str));
                }
            } catch (...) {
                return default_value;
            }
        }
    }

    // Numeric to string conversion
    if constexpr (std::is_same_v<T, std::string>) {
        if (value.type() == typeid(int)) {
            return std::to_string(std::any_cast<int>(value));
        } else if (value.type() == typeid(double)) {
            return std::to_string(std::any_cast<double>(value));
        } else if (value.type() == typeid(bool)) {
            return std::any_cast<bool>(value) ? "true" : "false";
        }
    }

    return default_value;
}

} // namespace detail

template <typename Value> std::optional<Value> ConfigStore::query(const KeyView key) {
    KeyHelper helper;
    Key k;
    if (!helper.fromKeyViewToKey(key, k)) {
        return std::nullopt;
    }

    std::any result = impl->query(k.full_key, std::any());
    if (result.type() == typeid(void)) {
        return std::nullopt;
    }

    Value value = detail::any_cast<Value>(result, Value{});
    return value;
}

template <typename Value> Value ConfigStore::query(const KeyView key, const Value& default_value) {
    KeyHelper helper;
    Key k;
    if (!helper.fromKeyViewToKey(key, k)) {
        return default_value;
    }

    std::any result = impl->query(k.full_key, std::any());
    return detail::any_cast<Value>(result, default_value);
}

template <typename Value> std::optional<Value> ConfigStore::query(const KeyView key, Layer layer) {
    KeyHelper helper;
    Key k;
    if (!helper.fromKeyViewToKey(key, k)) {
        return std::nullopt;
    }

    std::any result = impl->query(k.full_key, layer);
    if (result.type() == typeid(void)) {
        return std::nullopt;
    }

    Value value = detail::any_cast<Value>(result, Value{});
    return value;
}

template <typename Value>
Value ConfigStore::query(const KeyView key, Layer layer, const Value& default_value) {
    KeyHelper helper;
    Key k;
    if (!helper.fromKeyViewToKey(key, k)) {
        return default_value;
    }

    std::any result = impl->query(k.full_key, layer);
    if (result.type() == typeid(void)) {
        return default_value;
    }

    return detail::any_cast<Value>(result, default_value);
}

template <typename Value>
bool ConfigStore::set(const KeyView key, const Value& v, Layer layer, NotifyPolicy notify_policy) {
    KeyHelper helper;
    Key k;
    if (!helper.fromKeyViewToKey(key, k)) {
        return false;
    }

    // Convert Value to std::any
    std::any value = v;
    return impl->set(k.full_key, value, layer, notify_policy);
}

template <typename Value>
RegisterResult ConfigStore::register_key(const Key& key, const Value& init_value, Layer layer,
                                         NotifyPolicy notify_policy) {
    std::any value = init_value;
    return impl->register_key(key, value, layer, notify_policy);
}

} // namespace cf::config

// Include ConfigDomainHandle template implementations (needs detail::any_cast above)
#include "impl/config_domain_handle_impl.h"
