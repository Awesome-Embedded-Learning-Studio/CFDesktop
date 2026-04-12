/**
 * @file cfconfig_domain_handle.h
 * @brief Public handle for a named config domain.
 *
 * Provides the same query/set/watch API as ConfigStore but scoped to
 * a single named domain. Obtained via ConfigStore::domain("name").
 *
 * @date 2026-04-12
 * @version 1.0
 */

#pragma once

#include "cfconfig/cfconfig_result.h"
#include "cfconfig/cfconfig_watcher.h"
#include "cfconfig_key.h"
#include "cfconfig_layer.h"
#include "cfconfig_notify_policy.h"
#include <any>
#include <memory>
#include <optional>
#include <string>

namespace cf::config {

class ConfigStoreImpl;

/**
 * @brief Handle to a named config domain.
 *
 * Mirrors the ConfigStore API surface but scoped to one domain.
 * Obtain via ConfigStore::domain("name").
 *
 * @note Lightweight: holds only a shared_ptr to the impl and a domain name.
 * @note Thread-safe: operations are thread-safe via the underlying ConfigDomain.
 *
 * @code
 * auto wallpaper = ConfigStore::instance().domain("wallpaper");
 * wallpaper.set(KeyView{.group="source", .key="path"}, std::string("/pics/bg.jpg"), Layer::App);
 * auto path = wallpaper.query<std::string>(KeyView{.group="source", .key="path"}, "");
 * @endcode
 */
class ConfigDomainHandle {
  public:
    /* ========== Query operations ========== */

    template <typename Value> [[nodiscard]] std::optional<Value> query(const KeyView key);

    template <typename Value>
    [[nodiscard]] Value query(const KeyView key, const Value& default_value);

    template <typename Value>
    [[nodiscard]] std::optional<Value> query(const KeyView key, Layer layer);

    template <typename Value>
    [[nodiscard]] Value query(const KeyView key, Layer layer, const Value& default_value);

    [[nodiscard]] bool has_key(const KeyView key);
    [[nodiscard]] bool has_key(const KeyView key, Layer layer);

    /* ========== Write operations ========== */

    template <typename Value>
    [[nodiscard]] bool set(const KeyView key, const Value& v, Layer layer = Layer::App,
                           NotifyPolicy notify_policy = NotifyPolicy::Immediate);

    template <typename Value>
    [[nodiscard]] RegisterResult register_key(const Key& key, const Value& init_value,
                                              Layer layer = Layer::App,
                                              NotifyPolicy notify_policy = NotifyPolicy::Immediate);

    [[nodiscard]] UnRegisterResult
    unregister_key(const Key& key, Layer layer = Layer::App,
                   NotifyPolicy notify_policy = NotifyPolicy::Immediate);

    /**
     * @brief  Clears all configuration values in this domain.
     * @throws None
     * @note   None
     * @warning None
     * @since  N/A
     * @ingroup none
     */
    void clear();
    /**
     * @brief  Clears configuration values in the specified layer.
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
     * @param[in] key_pattern Glob-like pattern to match key names.
     * @param[in] callback    Callback invoked on matching key changes.
     * @param[in] policy      Notification policy for the watcher.
     * @return     Handle identifying this watcher registration.
     * @throws None
     * @note   None
     * @warning None
     * @since  N/A
     * @ingroup none
     */
    WatcherHandle watch(const std::string& key_pattern, Watcher callback,
                        NotifyPolicy policy = NotifyPolicy::Immediate);
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
    [[nodiscard]] NotifyResult notify();
    [[nodiscard]] std::size_t pending_changes() const;

    /* ========== Persistence operations ========== */

    /**
     * @brief  Persists configuration changes to disk asynchronously.
     * @throws None
     * @note   None
     * @warning None
     * @since  N/A
     * @ingroup none
     */
    void sync();
    /**
     * @brief  Reloads configuration values from disk.
     * @throws None
     * @note   None
     * @warning None
     * @since  N/A
     * @ingroup none
     */
    void reload();

    /**
     * @brief  Gets the domain name this handle is bound to.
     * @return Reference to the domain name string.
     * @throws None
     * @note   None
     * @warning None
     * @since  N/A
     * @ingroup none
     */
    const std::string& domain_name() const;

  private:
    friend class ConfigStore;
    ConfigDomainHandle(std::shared_ptr<ConfigStoreImpl> impl, const std::string& domain_name);

    std::shared_ptr<ConfigStoreImpl> impl_;
    std::string domain_name_;
};

} // namespace cf::config
