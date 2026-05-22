/**
 * @file config_impl.cpp
 * @brief Internal implementation of ConfigStore (Pimpl).
 *
 * ConfigStoreImpl delegates all per-domain operations to ConfigDomain instances.
 *
 * @date 2026-03-17
 * @version 2.0
 */

#include "impl/config_impl.h"
#include "cfconfig/cfconfig_path_provider.h"
#include "cfconfig/cfconfig_result.h"
#include "cfconfig_key.h"
#include "impl/config_backend_factory.h"
#include <QDir>
#include <QFileInfo>
#include <QString>
#include <memory>
#include <mutex>

namespace cf::config {

// ============================================================================
// Constructor / Destructor
// ============================================================================

ConfigStoreImpl::ConfigStoreImpl()
    : ConfigStoreImpl(std::make_shared<DesktopConfigStorePathProvider>()) {}

ConfigStoreImpl::ConfigStoreImpl(std::shared_ptr<IConfigStorePathProvider> path_provider)
    : path_provider_(std::move(path_provider)) {
    // Always create the default domain eagerly
    domains_.emplace("default", std::make_unique<ConfigDomain>(path_provider_, "default"));
}

ConfigStoreImpl::~ConfigStoreImpl() {
    // Sync all domains
    std::shared_lock lock(mutex_);
    for (auto& [name, domain] : domains_) {
        (void)name;
        domain->sync(false);
    }
}

// ============================================================================
// Domain management
// ============================================================================

ConfigDomain* ConfigStoreImpl::get_domain(const std::string& name) {
    // Fast path: shared lock for lookup
    {
        std::shared_lock lock(mutex_);
        auto it = domains_.find(name);
        if (it != domains_.end()) {
            return it->second.get();
        }
    }

    // Slow path: unique lock for creation (double-checked)
    std::unique_lock lock(mutex_);
    auto it = domains_.find(name);
    if (it != domains_.end()) {
        return it->second.get();
    }

    auto [ins_it, ok] =
        domains_.emplace(name, std::make_unique<ConfigDomain>(path_provider_, name));
    return ins_it->second.get();
}

ConfigDomain* ConfigStoreImpl::default_domain() const {
    // Default domain is always created in constructor, no lock needed
    return domains_.at("default").get();
}

// ============================================================================
// Delegated operations (default domain)
// ============================================================================

std::any ConfigStoreImpl::query(const std::string& key, const std::any& default_value) {
    return default_domain()->query(key, default_value);
}

std::any ConfigStoreImpl::query(const std::string& key, Layer layer) {
    return default_domain()->query(key, layer);
}

bool ConfigStoreImpl::has_key(const std::string& key) {
    return default_domain()->has_key(key);
}

bool ConfigStoreImpl::has_key(const std::string& key, Layer layer) {
    return default_domain()->has_key(key, layer);
}

bool ConfigStoreImpl::set(const std::string& key, const std::any& value, Layer layer,
                          NotifyPolicy notify_policy) {
    return default_domain()->set(key, value, layer, notify_policy);
}

RegisterResult ConfigStoreImpl::register_key(const Key& key, const std::any& init_value,
                                             Layer layer, NotifyPolicy notify_policy) {
    return default_domain()->register_key(key, init_value, layer, notify_policy);
}

UnRegisterResult ConfigStoreImpl::unregister_key(const Key& key, Layer layer,
                                                 NotifyPolicy notify_policy) {
    return default_domain()->unregister_key(key, layer, notify_policy);
}

void ConfigStoreImpl::clear() {
    std::shared_lock lock(mutex_);
    for (auto& [name, domain] : domains_) {
        (void)name;
        domain->clear();
    }
}

void ConfigStoreImpl::clear_layer(Layer layer) {
    default_domain()->clear_layer(layer);
}

WatcherHandle ConfigStoreImpl::watch(const std::string& pattern, Watcher callback,
                                     NotifyPolicy policy) {
    return default_domain()->watch(pattern, std::move(callback), policy);
}

void ConfigStoreImpl::unwatch(WatcherHandle handle) {
    default_domain()->unwatch(handle);
}

NotifyResult ConfigStoreImpl::notify() {
    return default_domain()->notify();
}

std::size_t ConfigStoreImpl::pending_changes() const {
    return default_domain()->pending_changes();
}

void ConfigStoreImpl::sync(bool async) {
    std::shared_lock lock(mutex_);
    for (auto& [name, domain] : domains_) {
        (void)name;
        domain->sync(async);
    }
}

void ConfigStoreImpl::reload() {
    std::shared_lock lock(mutex_);
    for (auto& [name, domain] : domains_) {
        (void)name;
        domain->reload();
    }
}

} // namespace cf::config
