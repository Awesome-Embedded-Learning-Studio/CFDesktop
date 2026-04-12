/**
 * @file cfconfig.cpp
 * @brief Main configuration storage implementation.
 *
 * @date 2026-03-17
 * @version 1.0
 */

#include "cfconfig.hpp"
#include "cfconfig/cfconfig_path_provider.h"
#include "cfconfig_key.h"
#include "impl/config_impl.h"
#include <mutex>

namespace cf::config {

// ============================================================================
// Constructor / Destructor
// ============================================================================

ConfigStore::ConfigStore() : impl(std::make_shared<ConfigStoreImpl>()) {}

ConfigStore::~ConfigStore() = default;

void ConfigStore::initialize(std::shared_ptr<IConfigStorePathProvider> path_provider) {
    // Use call_once to ensure initialization happens only once
    static std::once_flag init_flag;
    static std::shared_ptr<IConfigStorePathProvider> custom_provider;

    std::call_once(init_flag, [&]() {
        custom_provider = std::move(path_provider);
        // Replace the impl with custom provider
        impl = std::make_shared<ConfigStoreImpl>(custom_provider);
    });
}

// ============================================================================
// Query operations
// ============================================================================

bool ConfigStore::has_key(const KeyView key) {
    KeyHelper helper;
    Key k;
    if (!helper.fromKeyViewToKey(key, k)) {
        return false;
    }
    return impl->has_key(k.full_key);
}

bool ConfigStore::has_key(const KeyView key, Layer layer) {
    KeyHelper helper;
    Key k;
    if (!helper.fromKeyViewToKey(key, k)) {
        return false;
    }
    return impl->has_key(k.full_key, layer);
}

// ============================================================================
// Write operations
// ============================================================================

UnRegisterResult ConfigStore::unregister_key(const Key& key, Layer layer,
                                             NotifyPolicy notify_policy) {
    return impl->unregister_key(key, layer, notify_policy);
}

void ConfigStore::clear() {
    impl->clear();
}

void ConfigStore::clear_layer(Layer layer) {
    impl->clear_layer(layer);
}

// ============================================================================
// Watcher operations
// ============================================================================

WatcherHandle ConfigStore::watch(const std::string& key_pattern, Watcher callback,
                                 NotifyPolicy policy) {
    return impl->watch(key_pattern, std::move(callback), policy);
}

void ConfigStore::unwatch(WatcherHandle handle) {
    impl->unwatch(handle);
}

NotifyResult ConfigStore::notify() {
    return impl->notify();
}

std::size_t ConfigStore::pending_changes() const {
    return impl->pending_changes();
}

// ============================================================================
// Persistence operations
// ============================================================================

void ConfigStore::sync(const SyncMethod m) {
    impl->sync(m == SyncMethod::Async);
}

void ConfigStore::reload() {
    impl->reload();
}

ConfigDomainHandle ConfigStore::domain(const std::string& name) {
    return ConfigDomainHandle(impl, name);
}

} // namespace cf::config
