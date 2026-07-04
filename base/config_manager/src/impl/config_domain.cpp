/**
 * @file config_domain.cpp
 * @brief Per-domain configuration storage engine implementation.
 *
 * @date 2026-04-12
 * @version 1.0
 */

#include "impl/config_domain.h"
#include "cfconfig/cfconfig_path_provider.h"
#include "cfconfig/cfconfig_result.h"
#include "cfconfig_key.h"
#include "impl/config_backend_factory.h"
#include <QDir>
#include <QFileInfo>
#include <QString>
#include <array>
#include <memory>
#include <mutex>
#include <regex>

namespace cf::config {

namespace detail {

bool match_pattern(const std::string& pattern, const std::string& key) {
    std::string regex_pattern = pattern;
    size_t pos = 0;
    while ((pos = regex_pattern.find_first_of("^.${}+|()[]\\", pos)) != std::string::npos) {
        regex_pattern.insert(pos, "\\");
        pos += 2;
    }
    pos = 0;
    while ((pos = regex_pattern.find('*', pos)) != std::string::npos) {
        regex_pattern.replace(pos, 1, ".*");
        pos += 2;
    }
    regex_pattern = "^" + regex_pattern + "$";

    try {
        std::regex re(regex_pattern);
        return std::regex_match(key, re);
    } catch (const std::regex_error&) {
        return false;
    }
}

} // namespace detail

// ============================================================================
// Constructor / Destructor
// ============================================================================

ConfigDomain::ConfigDomain(std::shared_ptr<IConfigStorePathProvider> path_provider,
                           const std::string& domain_name)
    : path_provider_(std::move(path_provider)), domain_name_(domain_name) {

    QString domain_qname = QString::fromStdString(domain_name_);

    if (domain_name == "default") {
        // Backward-compatible: use the original path provider methods
        QString systemPath = path_provider_->system_path();
        if (!systemPath.isEmpty() && QFileInfo::exists(systemPath)) {
            settings_system_ = createBackend(systemPath);
        }

        QString userDir = path_provider_->user_dir();
        if (!userDir.isEmpty()) {
            QDir().mkpath(userDir);
            QString userFilePath = userDir + "/" + path_provider_->user_filename();
            settings_user_ = createBackend(userFilePath);
        }

        QString appDir = path_provider_->app_dir();
        if (!appDir.isEmpty()) {
            QDir().mkpath(appDir);
            QString appFilePath = appDir + "/" + path_provider_->app_filename();
            settings_app_ = createBackend(appFilePath);
        }
    } else {
        // Named domain: use domain_path() from provider
        // System layer (index 0)
        QString sysPath = path_provider_->domain_path(0, domain_qname);
        if (!sysPath.isEmpty() && QFileInfo::exists(sysPath)) {
            settings_system_ = createBackend(sysPath);
        }

        // User layer (index 1)
        QString userPath = path_provider_->domain_path(1, domain_qname);
        if (!userPath.isEmpty()) {
            QDir().mkpath(QFileInfo(userPath).absolutePath());
            settings_user_ = createBackend(userPath);
        }

        // App layer (index 2)
        QString appPath = path_provider_->domain_path(2, domain_qname);
        if (!appPath.isEmpty()) {
            QDir().mkpath(QFileInfo(appPath).absolutePath());
            settings_app_ = createBackend(appPath);
        }
    }
}

ConfigDomain::~ConfigDomain() {
    sync(false);
}

// ============================================================================
// Query operations
// ============================================================================

std::any ConfigDomain::query(const std::string& key, const std::any& default_value) {
    std::shared_lock lock(mutex_);

    auto it = cache_.find(key);
    if (it != cache_.end()) {
        return it->second;
    }

    QString qkey = QString::fromStdString(key);

    if (settings_app_) {
        QVariant value = settings_app_->value(qkey);
        if (!value.isNull()) {
            cache_[key] = value;
            return value;
        }
    }

    if (settings_user_) {
        QVariant value = settings_user_->value(qkey);
        if (!value.isNull()) {
            cache_[key] = value;
            return value;
        }
    }

    if (settings_system_) {
        QVariant value = settings_system_->value(qkey);
        if (!value.isNull()) {
            cache_[key] = value;
            return value;
        }
    }

    return default_value;
}

std::any ConfigDomain::query(const std::string& key, Layer layer) {
    std::shared_lock lock(mutex_);

    if (layer == Layer::Temp) {
        auto it = cache_.find(key);
        if (it != cache_.end()) {
            return it->second;
        }
        return std::any();
    }

    IConfigBackend* backend = get_backend(layer);
    if (!backend) {
        return std::any();
    }

    QString qkey = QString::fromStdString(key);
    QVariant value = backend->value(qkey);

    if (value.isNull()) {
        return std::any();
    }

    return value;
}

bool ConfigDomain::has_key(const std::string& key) {
    std::shared_lock lock(mutex_);

    if (cache_.find(key) != cache_.end()) {
        return true;
    }

    QString qkey = QString::fromStdString(key);

    if (settings_app_ && settings_app_->contains(qkey)) {
        return true;
    }
    if (settings_user_ && settings_user_->contains(qkey)) {
        return true;
    }
    if (settings_system_ && settings_system_->contains(qkey)) {
        return true;
    }

    return false;
}

bool ConfigDomain::has_key(const std::string& key, Layer layer) {
    std::shared_lock lock(mutex_);

    if (layer == Layer::Temp) {
        return cache_.find(key) != cache_.end();
    }

    IConfigBackend* backend = get_backend(layer);
    if (!backend) {
        return false;
    }

    QString qkey = QString::fromStdString(key);
    return backend->contains(qkey);
}

// ============================================================================
// Write operations (public - acquire locks)
// ============================================================================

bool ConfigDomain::set(const std::string& key, const std::any& value, Layer layer,
                       NotifyPolicy notify_policy) {
    std::unique_lock lock(mutex_);
    bool result = set_impl(key, value, layer, notify_policy);
    if (result) {
        lock.unlock();
        execute_deferred_watchers();
    }
    return result;
}

RegisterResult ConfigDomain::register_key(const Key& key, const std::any& init_value, Layer layer,
                                          NotifyPolicy notify_policy) {
    std::unique_lock lock(mutex_);
    RegisterResult result = register_key_impl(key, init_value, layer, notify_policy);
    lock.unlock();
    execute_deferred_watchers();
    return result;
}

UnRegisterResult ConfigDomain::unregister_key(const Key& key, Layer layer,
                                              NotifyPolicy notify_policy) {
    std::unique_lock lock(mutex_);
    UnRegisterResult result = unregister_key_impl(key, layer, notify_policy);
    lock.unlock();
    execute_deferred_watchers();
    return result;
}

void ConfigDomain::clear() {
    std::unique_lock lock(mutex_);
    clear_impl();
    lock.unlock();
    execute_deferred_watchers();
}

void ConfigDomain::clear_layer(Layer layer) {
    std::unique_lock lock(mutex_);
    clear_layer_impl(layer);
    lock.unlock();
    execute_deferred_watchers();
}

// ============================================================================
// Write operations (internal lock-free implementations)
// ============================================================================

bool ConfigDomain::set_impl(const std::string& key, const std::any& value, Layer layer,
                            NotifyPolicy notify_policy) {
    std::any old_value;
    bool had_old = false;
    auto it = cache_.find(key);
    if (it != cache_.end()) {
        old_value = it->second;
        had_old = true;
    }

    if (layer == Layer::Temp) {
        cache_[key] = value;

        if (notify_policy == NotifyPolicy::Immediate) {
            Key k{.full_key = key, .full_description = key};
            trigger_watchers(k, had_old ? &old_value : nullptr, &value, layer);
        } else {
            pending_changes_.push_back({Key{.full_key = key, .full_description = key},
                                        had_old ? old_value : std::any(), value, layer});
        }
        return true;
    }

    IConfigBackend* backend = get_backend(layer);
    if (!backend) {
        return false;
    }

    QString qkey = QString::fromStdString(key);

    if (!had_old && backend->contains(qkey)) {
        old_value = backend->value(qkey);
        had_old = true;
    }

    QVariant qvalue = anyToQVariant(value);

    backend->setValue(qkey, qvalue);
    cache_[key] = value;
    mark_dirty(layer);

    if (notify_policy == NotifyPolicy::Immediate) {
        Key k{.full_key = key, .full_description = key};
        trigger_watchers(k, had_old ? &old_value : nullptr, &value, layer);
    } else {
        pending_changes_.push_back({Key{.full_key = key, .full_description = key},
                                    had_old ? old_value : std::any(), value, layer});
    }

    return true;
}

RegisterResult ConfigDomain::register_key_impl(const Key& key, const std::any& init_value,
                                               Layer layer, NotifyPolicy notify_policy) {
    if (cache_.find(key.full_key) != cache_.end()) {
        return RegisterResult::KeyAlreadyIn;
    }

    if (layer != Layer::Temp) {
        IConfigBackend* backend = get_backend(layer);
        if (backend) {
            QString qkey = QString::fromStdString(key.full_key);
            if (backend->contains(qkey)) {
                return RegisterResult::KeyAlreadyIn;
            }
        }
    }

    set_impl(key.full_key, init_value, layer, notify_policy);
    return RegisterResult::KeyRegisteredSuccess;
}

UnRegisterResult ConfigDomain::unregister_key_impl(const Key& key, Layer layer,
                                                   NotifyPolicy notify_policy) {
    bool existed = false;
    std::any old_value;

    auto it = cache_.find(key.full_key);
    if (it != cache_.end()) {
        old_value = it->second;
        cache_.erase(it);
        existed = true;
    }

    if (layer != Layer::Temp) {
        IConfigBackend* backend = get_backend(layer);
        if (backend) {
            QString qkey = QString::fromStdString(key.full_key);
            if (backend->contains(qkey)) {
                if (!existed) {
                    old_value = backend->value(qkey);
                }
                backend->remove(qkey);
                existed = true;
                mark_dirty(layer);
            }
        }
    }

    if (!existed) {
        return UnRegisterResult::KeyUnexisted;
    }

    if (notify_policy == NotifyPolicy::Immediate) {
        trigger_watchers(key, &old_value, nullptr, layer);
    } else {
        pending_changes_.push_back({key, old_value, std::any(), layer});
    }

    return UnRegisterResult::KeyUnRegisteredSuccess;
}

void ConfigDomain::clear_impl() {
    cache_.clear();

    if (settings_system_) {
        settings_system_->clear();
    }
    if (settings_user_) {
        settings_user_->clear();
    }
    if (settings_app_) {
        settings_app_->clear();
    }

    dirty_flags_.fill(true);
    pending_changes_.clear();
}

void ConfigDomain::clear_layer_impl(Layer layer) {
    if (layer == Layer::Temp) {
        cache_.clear();
        return;
    }

    IConfigBackend* backend = get_backend(layer);
    if (backend) {
        backend->clear();
        mark_dirty(layer);
    }
}

// ============================================================================
// Watcher operations
// ============================================================================

WatcherHandle ConfigDomain::watch(const std::string& pattern, Watcher callback,
                                  NotifyPolicy policy) {
    std::unique_lock lock(mutex_);
    WatcherHandle handle = next_handle_.fetch_add(1);
    watchers_.push_back({pattern, std::move(callback), policy, handle});
    return handle;
}

void ConfigDomain::unwatch(WatcherHandle handle) {
    std::unique_lock lock(mutex_);
    auto it = std::remove_if(watchers_.begin(), watchers_.end(),
                             [handle](const WatcherEntry& e) { return e.handle == handle; });
    watchers_.erase(it, watchers_.end());
}

NotifyResult ConfigDomain::notify() {
    std::unique_lock lock(mutex_);

    if (pending_changes_.empty()) {
        return NotifyResult::NothingWorthNotify;
    }

    for (const auto& change : pending_changes_) {
        trigger_watchers(change.key,
                         change.old_value.type() == typeid(void) ? nullptr : &change.old_value,
                         change.new_value.type() == typeid(void) ? nullptr : &change.new_value,
                         change.from_layer);
    }

    pending_changes_.clear();
    lock.unlock();

    execute_deferred_watchers();
    return NotifyResult::NotifySuccess;
}

std::size_t ConfigDomain::pending_changes() const {
    std::shared_lock lock(mutex_);
    return pending_changes_.size();
}

// ============================================================================
// Persistence operations
// ============================================================================

void ConfigDomain::sync(bool async) {
    (void)async;

    std::shared_lock lock(mutex_);

    if (settings_system_ && dirty_flags_[0]) {
        settings_system_->sync();
        dirty_flags_[0] = false;
    }
    if (settings_user_ && dirty_flags_[1]) {
        settings_user_->sync();
        dirty_flags_[1] = false;
    }
    if (settings_app_ && dirty_flags_[2]) {
        settings_app_->sync();
        dirty_flags_[2] = false;
    }
}

void ConfigDomain::reload() {
    std::unique_lock lock(mutex_);

    cache_.clear();
    pending_changes_.clear();

    if (settings_system_) {
        settings_system_->reload();
    }
    if (settings_user_) {
        settings_user_->reload();
    }
    if (settings_app_) {
        settings_app_->reload();
    }
}

// ============================================================================
// Private helpers
// ============================================================================

bool ConfigDomain::match_pattern(const std::string& pattern, const std::string& key) {
    return detail::match_pattern(pattern, key);
}

IConfigBackend* ConfigDomain::get_backend(Layer layer) {
    switch (layer) {
        case Layer::System:
            return settings_system_.get();
        case Layer::User:
            return settings_user_.get();
        case Layer::App:
            return settings_app_.get();
        case Layer::Temp:
            return nullptr;
    }
    return nullptr;
}

QVariant ConfigDomain::anyToQVariant(const std::any& value) {
    if (value.type() == typeid(int)) {
        return QVariant::fromValue(std::any_cast<int>(value));
    } else if (value.type() == typeid(double)) {
        return QVariant::fromValue(std::any_cast<double>(value));
    } else if (value.type() == typeid(bool)) {
        return QVariant::fromValue(std::any_cast<bool>(value));
    } else if (value.type() == typeid(std::string)) {
        return QString::fromStdString(std::any_cast<std::string>(value));
    } else {
        try {
            return std::any_cast<QVariant>(value);
        } catch (const std::bad_any_cast&) {
            return QVariant();
        }
    }
}

void ConfigDomain::trigger_watchers(const Key& key, const std::any* old_value,
                                    const std::any* new_value, Layer layer) {
    for (const auto& watcher : watchers_) {
        if (detail::match_pattern(watcher.pattern, key.full_key)) {
            DeferredWatcherEvent event;
            event.callback = watcher.callback;
            event.key = key;
            event.from_layer = layer;
            event.has_old_value = (old_value != nullptr);
            event.has_new_value = (new_value != nullptr);
            if (old_value)
                event.old_value = *old_value;
            if (new_value)
                event.new_value = *new_value;
            deferred_events_.push_back(std::move(event));
        }
    }
}

void ConfigDomain::execute_deferred_watchers() {
    std::vector<DeferredWatcherEvent> events;
    {
        std::lock_guard<std::mutex> lock(deferred_mutex_);
        events = std::move(deferred_events_);
        deferred_events_.clear();
    }

    for (const auto& event : events) {
        event.callback(event.key, event.has_old_value ? &event.old_value : nullptr,
                       event.has_new_value ? &event.new_value : nullptr, event.from_layer);
    }
}

void ConfigDomain::mark_dirty(Layer layer) {
    int index = static_cast<int>(layer);
    if (index >= 0 && index < 4) {
        dirty_flags_[index] = true;
    }
}

} // namespace cf::config
