/**
 * @file qsettings_backend.h
 * @brief QSettings-based config backend (INI format).
 *
 * @date 2026-04-11
 * @version 1.0
 */

#pragma once

#include "impl/config_backend.h"
#include <QSettings>
#include <memory>

namespace cf::config {

/**
 * @brief Config backend backed by QSettings (INI format).
 *
 * Maps dot-separated keys to QSettings paths by replacing '.' with '/'.
 */
class QSettingsBackend : public IConfigBackend {
  public:
    /**
     * @brief Construct a QSettings backend for the given file path.
     * @param[in] filePath Full path to the INI file.
     */
    explicit QSettingsBackend(const QString& filePath);

    /**
     * @brief Read a value by dot-separated key.
     * @param[in] key Dot-separated key to look up.
     * @return QVariant (invalid if not found).
     */
    QVariant value(const QString& key) const override;

    /**
     * @brief Write a value at the given key.
     * @param[in] key Dot-separated key.
     * @param[in] value Value to store.
     */
    void setValue(const QString& key, const QVariant& value) override;

    /**
     * @brief Check whether a key exists.
     * @param[in] key Dot-separated key to check.
     * @return True if the key is present.
     */
    bool contains(const QString& key) const override;

    /**
     * @brief Remove a key and its value.
     * @param[in] key Dot-separated key to remove.
     */
    void remove(const QString& key) override;

    /**
     * @brief Remove all keys.
     */
    void clear() override;

    /**
     * @brief Flush in-memory state to the INI file.
     */
    void sync() override;

    /**
     * @brief Re-read the INI file from disk.
     */
    void reload() override;

  private:
    /// @brief Convert dot-separated key to QSettings path: "app.theme.name" → "app/theme/name"
    static QString toPath(const QString& dotKey);

    std::unique_ptr<QSettings> settings_;
};

} // namespace cf::config
