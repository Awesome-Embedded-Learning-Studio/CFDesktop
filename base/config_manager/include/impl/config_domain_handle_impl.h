/**
 * @file config_domain_handle_impl.h
 * @brief Template method implementations for ConfigDomainHandle.
 *
 * @date 2026-04-12
 * @version 1.0
 */

#pragma once

#include "cfconfig/cfconfig_domain_handle.h"
#include "cfconfig_key.h"
#include "impl/config_impl.h"
#include <any>
#include <optional>

namespace cf::config {

// Forward declaration of the any_cast helper in cfconfig.hpp
namespace detail {
/**
 * @brief  Extracts a typed value from std::any with a fallback.
 *
 * Attempts to cast the stored value to type T. Returns the default
 * if the stored type does not match.
 *
 * @tparam T           The target type to extract.
 * @param[in] value         The std::any value to cast.
 * @param[in] default_value Fallback returned when cast fails.
 * @return             The extracted value, or default_value on failure.
 * @throws None
 * @note   None
 * @warning None
 * @since  N/A
 * @ingroup none
 */
template <typename T> T any_cast(const std::any& value, const T& default_value);
} // namespace detail

inline ConfigDomainHandle::ConfigDomainHandle(std::shared_ptr<ConfigStoreImpl> impl,
                                              const std::string& domain_name)
    : impl_(std::move(impl)), domain_name_(domain_name) {}

inline const std::string& ConfigDomainHandle::domain_name() const {
    return domain_name_;
}

inline void ConfigDomainHandle::clear() {
    impl_->get_domain(domain_name_)->clear();
}

inline void ConfigDomainHandle::clear_layer(Layer layer) {
    impl_->get_domain(domain_name_)->clear_layer(layer);
}

inline WatcherHandle ConfigDomainHandle::watch(const std::string& key_pattern, Watcher callback,
                                               NotifyPolicy policy) {
    return impl_->get_domain(domain_name_)->watch(key_pattern, std::move(callback), policy);
}

inline void ConfigDomainHandle::unwatch(WatcherHandle handle) {
    impl_->get_domain(domain_name_)->unwatch(handle);
}

inline NotifyResult ConfigDomainHandle::notify() {
    return impl_->get_domain(domain_name_)->notify();
}

inline std::size_t ConfigDomainHandle::pending_changes() const {
    return impl_->get_domain(domain_name_)->pending_changes();
}

inline void ConfigDomainHandle::sync() {
    impl_->get_domain(domain_name_)->sync(true);
}

inline void ConfigDomainHandle::reload() {
    impl_->get_domain(domain_name_)->reload();
}

inline bool ConfigDomainHandle::has_key(const KeyView key) {
    KeyHelper helper;
    Key k;
    if (!helper.fromKeyViewToKey(key, k)) {
        return false;
    }
    return impl_->get_domain(domain_name_)->has_key(k.full_key);
}

inline bool ConfigDomainHandle::has_key(const KeyView key, Layer layer) {
    KeyHelper helper;
    Key k;
    if (!helper.fromKeyViewToKey(key, k)) {
        return false;
    }
    return impl_->get_domain(domain_name_)->has_key(k.full_key, layer);
}

inline UnRegisterResult ConfigDomainHandle::unregister_key(const Key& key, Layer layer,
                                                           NotifyPolicy notify_policy) {
    return impl_->get_domain(domain_name_)->unregister_key(key, layer, notify_policy);
}

// ============================================================================
// Template implementations
// ============================================================================

template <typename Value> std::optional<Value> ConfigDomainHandle::query(const KeyView key) {
    KeyHelper helper;
    Key k;
    if (!helper.fromKeyViewToKey(key, k)) {
        return std::nullopt;
    }

    std::any result = impl_->get_domain(domain_name_)->query(k.full_key, std::any());
    if (result.type() == typeid(void)) {
        return std::nullopt;
    }

    Value value = detail::any_cast<Value>(result, Value{});
    return value;
}

template <typename Value>
Value ConfigDomainHandle::query(const KeyView key, const Value& default_value) {
    KeyHelper helper;
    Key k;
    if (!helper.fromKeyViewToKey(key, k)) {
        return default_value;
    }

    std::any result = impl_->get_domain(domain_name_)->query(k.full_key, std::any());
    return detail::any_cast<Value>(result, default_value);
}

template <typename Value>
std::optional<Value> ConfigDomainHandle::query(const KeyView key, Layer layer) {
    KeyHelper helper;
    Key k;
    if (!helper.fromKeyViewToKey(key, k)) {
        return std::nullopt;
    }

    std::any result = impl_->get_domain(domain_name_)->query(k.full_key, layer);
    if (result.type() == typeid(void)) {
        return std::nullopt;
    }

    Value value = detail::any_cast<Value>(result, Value{});
    return value;
}

template <typename Value>
Value ConfigDomainHandle::query(const KeyView key, Layer layer, const Value& default_value) {
    KeyHelper helper;
    Key k;
    if (!helper.fromKeyViewToKey(key, k)) {
        return default_value;
    }

    std::any result = impl_->get_domain(domain_name_)->query(k.full_key, layer);
    if (result.type() == typeid(void)) {
        return default_value;
    }

    return detail::any_cast<Value>(result, default_value);
}

template <typename Value> bool ConfigDomainHandle::set(const KeyView key, const Value& v,
                                                       Layer layer, NotifyPolicy notify_policy) {
    KeyHelper helper;
    Key k;
    if (!helper.fromKeyViewToKey(key, k)) {
        return false;
    }

    std::any value = v;
    return impl_->get_domain(domain_name_)->set(k.full_key, value, layer, notify_policy);
}

template <typename Value>
RegisterResult ConfigDomainHandle::register_key(const Key& key, const Value& init_value,
                                                Layer layer, NotifyPolicy notify_policy) {
    std::any value = init_value;
    return impl_->get_domain(domain_name_)->register_key(key, value, layer, notify_policy);
}

} // namespace cf::config
