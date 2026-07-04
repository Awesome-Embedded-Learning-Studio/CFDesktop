/**
 * @file    desktop/base/config_manager/include/impl/config_impl.h
 * @brief   Internal implementation of ConfigStore (Pimpl).
 *
 * ConfigStoreImpl acts as a multi-domain container. Each domain is a
 * ConfigDomain instance with its own backends, cache, and watchers.
 * Existing ConfigStore methods delegate to the "default" domain for
 * backward compatibility.
 *
 * @author  N/A
 * @date    2026-03-17
 * @version 2.0
 * @since   N/A
 * @ingroup none
 */

#pragma once

#include "cfconfig/cfconfig_path_provider.h"
#include "cfconfig/cfconfig_result.h"
#include "cfconfig/cfconfig_watcher.h"
#include "cfconfig_key.h"
#include "cfconfig_layer.h"
#include "cfconfig_notify_policy.h"
#include "impl/config_backend.h"
#include "impl/config_domain.h"
#include <QString>
#include <any>
#include <atomic>
#include <memory>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace cf::config {

/**
 * @brief  Internal implementation of ConfigStore.
 *
 * Manages multiple named ConfigDomain instances and dispatches
 * all operations to the appropriate domain.
 *
 * @note   Thread-safe for all operations.
 * @note   Not part of the public API.
 *
 * @since  N/A
 * @ingroup none
 */
class ConfigStoreImpl {
  public:
    /**
     * @brief  Default-constructs a ConfigStoreImpl.
     * @throws None
     * @note   None
     * @warning None
     * @since  N/A
     * @ingroup none
     */
    ConfigStoreImpl();
    /**
     * @brief  Constructs with a custom path provider.
     * @param[in] path_provider Path provider for config file locations.
     * @throws None
     * @note   None
     * @warning None
     * @since  N/A
     * @ingroup none
     */
    explicit ConfigStoreImpl(std::shared_ptr<IConfigStorePathProvider> path_provider);
    /**
     * @brief  Destructs the ConfigStoreImpl and releases all domains.
     * @throws None
     * @note   None
     * @warning None
     * @since  N/A
     * @ingroup none
     */
    ~ConfigStoreImpl();

    ConfigStoreImpl(const ConfigStoreImpl&) = delete;
    ConfigStoreImpl& operator=(const ConfigStoreImpl&) = delete;
    ConfigStoreImpl(ConfigStoreImpl&&) = delete;
    ConfigStoreImpl& operator=(ConfigStoreImpl&&) = delete;

    /* ========== Domain management ========== */

    /**
     * @brief  Get or lazily create a named domain.
     *
     * Thread-safe. Uses double-checked locking for fast path.
     *
     * @param[in] name Domain name.
     * @return     Pointer to the domain (never null after creation).
     * @throws None
     * @note   None
     * @warning None
     * @since  N/A
     * @ingroup none
     */
    ConfigDomain* get_domain(const std::string& name);

    /**
     * @brief  Get the default domain (for backward-compat fast path).
     * @return     Pointer to the default domain.
     * @throws None
     * @note   None
     * @warning None
     * @since  N/A
     * @ingroup none
     */
    ConfigDomain* default_domain() const;

    /* ========== Delegated operations (default domain) ========== */

    /**
     * @brief  Queries a configuration value by key with a fallback.
     * @param[in] key           The configuration key to look up.
     * @param[in] default_value Value returned if the key is absent.
     * @return     The stored value, or default_value if not found.
     * @throws None
     * @note   None
     * @warning None
     * @since  N/A
     * @ingroup none
     */
    std::any query(const std::string& key, const std::any& default_value);

    /**
     * @brief  Queries a configuration value from a specific layer.
     * @param[in] key   The configuration key to look up.
     * @param[in] layer The configuration layer to query.
     * @return     The stored value, or empty std::any if not found.
     * @throws None
     * @note   None
     * @warning None
     * @since  N/A
     * @ingroup none
     */
    std::any query(const std::string& key, Layer layer);

    /**
     * @brief  Checks whether a key exists across all layers.
     * @param[in] key The configuration key to check.
     * @return     true if the key exists, false otherwise.
     * @throws None
     * @note   None
     * @warning None
     * @since  N/A
     * @ingroup none
     */
    bool has_key(const std::string& key);

    /**
     * @brief  Checks whether a key exists in a specific layer.
     * @param[in] key   The configuration key to check.
     * @param[in] layer The configuration layer to check.
     * @return     true if the key exists in the layer, false otherwise.
     * @throws None
     * @note   None
     * @warning None
     * @since  N/A
     * @ingroup none
     */
    bool has_key(const std::string& key, Layer layer);

    /**
     * @brief  Sets a configuration value in the specified layer.
     * @param[in] key            The configuration key to set.
     * @param[in] value          The value to store.
     * @param[in] layer          The target configuration layer.
     * @param[in] notify_policy  How and when to notify watchers.
     * @return     true if the value was set successfully.
     * @throws None
     * @note   None
     * @warning None
     * @since  N/A
     * @ingroup none
     */
    bool set(const std::string& key, const std::any& value, Layer layer,
             NotifyPolicy notify_policy);

    /**
     * @brief  Registers a new configuration key with an initial value.
     * @param[in] key            The key descriptor to register.
     * @param[in] init_value     The initial value for the key.
     * @param[in] layer          The layer to store the initial value in.
     * @param[in] notify_policy  How and when to notify watchers.
     * @return     Result indicating success or the reason for failure.
     * @throws None
     * @note   None
     * @warning None
     * @since  N/A
     * @ingroup none
     */
    RegisterResult register_key(const Key& key, const std::any& init_value, Layer layer,
                                NotifyPolicy notify_policy);

    /**
     * @brief  Unregisters a previously registered configuration key.
     * @param[in] key            The key descriptor to unregister.
     * @param[in] layer          The layer to remove the key from.
     * @param[in] notify_policy  How and when to notify watchers.
     * @return     Result indicating success or the reason for failure.
     * @throws None
     * @note   None
     * @warning None
     * @since  N/A
     * @ingroup none
     */
    UnRegisterResult unregister_key(const Key& key, Layer layer, NotifyPolicy notify_policy);

    /**
     * @brief  Clears all configuration values across all layers.
     * @throws None
     * @note   None
     * @warning None
     * @since  N/A
     * @ingroup none
     */
    void clear();

    /**
     * @brief  Clears all configuration values in the specified layer.
     * @param[in] layer The configuration layer to clear.
     * @throws None
     * @note   None
     * @warning None
     * @since  N/A
     * @ingroup none
     */
    void clear_layer(Layer layer);

    /**
     * @brief  Registers a watcher for keys matching a pattern.
     * @param[in] pattern  Glob-like pattern to match key names.
     * @param[in] callback Callback invoked on matching key changes.
     * @param[in] policy   Notification policy for the watcher.
     * @return     Handle identifying this watcher registration.
     * @throws None
     * @note   None
     * @warning None
     * @since  N/A
     * @ingroup none
     */
    WatcherHandle watch(const std::string& pattern, Watcher callback, NotifyPolicy policy);

    /**
     * @brief  Removes a previously registered watcher.
     * @param[in] handle Handle returned by watch().
     * @throws None
     * @note   None
     * @warning None
     * @since  N/A
     * @ingroup none
     */
    void unwatch(WatcherHandle handle);

    /**
     * @brief  Manually triggers pending watcher notifications.
     * @return     Result indicating how many watchers were notified.
     * @throws None
     * @note   None
     * @warning None
     * @since  N/A
     * @ingroup none
     */
    NotifyResult notify();

    /**
     * @brief  Returns the number of pending, unnotified changes.
     * @return     Count of pending changes.
     * @throws None
     * @note   None
     * @warning None
     * @since  N/A
     * @ingroup none
     */
    std::size_t pending_changes() const;

    /**
     * @brief  Persists dirty layers to disk.
     * @param[in] async If true, performs the write asynchronously.
     * @throws None
     * @note   None
     * @warning None
     * @since  N/A
     * @ingroup none
     */
    void sync(bool async);

    /**
     * @brief  Reloads all layers from their persistent storage.
     * @throws None
     * @note   None
     * @warning None
     * @since  N/A
     * @ingroup none
     */
    void reload();

  private:
    mutable std::shared_mutex mutex_;
    std::shared_ptr<IConfigStorePathProvider> path_provider_;
    std::unordered_map<std::string, std::unique_ptr<ConfigDomain>> domains_;
};

} // namespace cf::config
