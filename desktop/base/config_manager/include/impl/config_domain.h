/**
 * @file    desktop/base/config_manager/include/impl/config_domain.h
 * @brief   Per-domain configuration storage engine.
 *
 * Each ConfigDomain owns its own backends, cache, watchers, and dirty flags,
 * providing complete isolation between configuration domains.
 *
 * @author  N/A
 * @date    2026-04-12
 * @version 1.0
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
 * @brief Watcher entry for pattern matching.
 */
struct WatcherEntry {
    std::string pattern;
    Watcher callback;
    NotifyPolicy policy;
    WatcherHandle handle;
};

/**
 * @brief Pending change for manual notification.
 */
struct PendingChange {
    Key key;
    std::any old_value;
    std::any new_value;
    Layer from_layer;
};

/**
 * @brief Deferred watcher event for callback execution after lock release.
 */
struct DeferredWatcherEvent {
    Watcher callback;
    Key key;
    std::any old_value;
    std::any new_value;
    Layer from_layer;
    bool has_old_value;
    bool has_new_value;
};

/**
 * @brief  Per-domain configuration storage engine.
 *
 * Manages four-layer configuration storage, caching, watchers,
 * and persistence operations for a single named domain.
 *
 * @note   Thread-safe for all operations.
 * @note   Not part of the public API.
 *
 * @since  N/A
 * @ingroup none
 */
class ConfigDomain {
  public:
    /**
     * @brief  Construct with path provider and domain name.
     *
     * For domain "default", uses the original path provider methods
     * (system_path, user_dir/filename, app_dir/filename).
     * For other domains, uses domain_path() from the provider.
     *
     * @param[in] path_provider Path provider for config file locations.
     * @param[in] domain_name   Name of this domain.
     */
    ConfigDomain(std::shared_ptr<IConfigStorePathProvider> path_provider,
                 const std::string& domain_name);

    ~ConfigDomain();

    ConfigDomain(const ConfigDomain&) = delete;
    ConfigDomain& operator=(const ConfigDomain&) = delete;
    ConfigDomain(ConfigDomain&&) = delete;
    ConfigDomain& operator=(ConfigDomain&&) = delete;

    /* ========== Query operations ========== */

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

    /* ========== Write operations ========== */

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

    /* ========== Watcher operations ========== */

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

    /* ========== Persistence operations ========== */

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

    /**
     * @brief  Gets the domain name.
     * @return Reference to the domain name string.
     * @throws None
     * @note   None
     * @warning None
     * @since  N/A
     * @ingroup none
     */
    const std::string& domain_name() const { return domain_name_; }

  private:
    /**
     * @brief  Tests whether a key matches a glob-like pattern.
     * @param[in] pattern The glob pattern to test against.
     * @param[in] key     The key string to test.
     * @return     true if the key matches the pattern.
     * @throws None
     * @note   None
     * @warning None
     * @since  N/A
     * @ingroup none
     */
    static bool match_pattern(const std::string& pattern, const std::string& key);
    IConfigBackend* get_backend(Layer layer);
    void mark_dirty(Layer layer);
    static QVariant anyToQVariant(const std::any& value);

    bool set_impl(const std::string& key, const std::any& value, Layer layer,
                  NotifyPolicy notify_policy);
    RegisterResult register_key_impl(const Key& key, const std::any& init_value, Layer layer,
                                     NotifyPolicy notify_policy);
    UnRegisterResult unregister_key_impl(const Key& key, Layer layer, NotifyPolicy notify_policy);
    void clear_layer_impl(Layer layer);
    void clear_impl();

    void trigger_watchers(const Key& key, const std::any* old_value, const std::any* new_value,
                          Layer layer);
    void execute_deferred_watchers();

  private:
    mutable std::shared_mutex mutex_;
    std::mutex deferred_mutex_;

    std::shared_ptr<IConfigStorePathProvider> path_provider_;
    std::string domain_name_;

    std::unordered_map<std::string, std::any> cache_;

    std::unique_ptr<IConfigBackend> settings_system_;
    std::unique_ptr<IConfigBackend> settings_user_;
    std::unique_ptr<IConfigBackend> settings_app_;

    std::array<bool, 4> dirty_flags_{false, false, false, false};

    std::vector<WatcherEntry> watchers_;
    std::atomic<WatcherHandle> next_handle_{1};

    std::vector<PendingChange> pending_changes_;
    std::vector<DeferredWatcherEvent> deferred_events_;
};

} // namespace cf::config
