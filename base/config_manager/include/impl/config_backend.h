/**
 * @file config_backend.h
 * @brief Abstract interface for config storage backends.
 *
 * Provides a uniform API that abstracts over different file formats (INI, JSON).
 * Each backend handles its own key-to-storage mapping internally.
 *
 * Thread safety: callers must synchronize access. ConfigStoreImpl holds
 * a shared_mutex around all backend calls.
 *
 * @date 2026-04-11
 * @version 1.0
 */

#pragma once

#include <QString>
#include <QVariant>
#include <memory>

namespace cf::config {

/**
 * @brief Abstract backend for persistent key-value storage.
 *
 * Key format: dot-separated (e.g. "app.theme.name").
 * Each backend maps this to its internal representation.
 * - QSettingsBackend: "app/theme/name" (QSettings path)
 * - JsonConfigBackend: nested objects {"app":{"theme":{"name":...}}}
 */
class IConfigBackend {
  public:
    virtual ~IConfigBackend() = default;

    /**
     * @brief Read a value.
     *
     * @param[in] key Dot-separated config key.
     * @return The stored value, or an invalid QVariant if the key is not found.
     */
    virtual QVariant value(const QString& key) const = 0;

    /// @brief Write a value.
    virtual void setValue(const QString& key, const QVariant& value) = 0;

    /**
     * @brief Check if a key exists.
     *
     * @param[in] key Dot-separated config key.
     * @return true if the key exists, false otherwise.
     */
    virtual bool contains(const QString& key) const = 0;

    /// @brief Remove a key.
    virtual void remove(const QString& key) = 0;

    /// @brief Remove all keys.
    virtual void clear() = 0;

    /// @brief Flush in-memory state to disk.
    virtual void sync() = 0;

    /// @brief Re-read from disk (discard in-memory changes).
    virtual void reload() = 0;
};

} // namespace cf::config
