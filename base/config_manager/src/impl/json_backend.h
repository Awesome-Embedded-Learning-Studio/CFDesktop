/**
 * @file json_backend.h
 * @brief JSON-based config backend using QJsonDocument.
 *
 * Maps dot-separated keys to nested JSON objects.
 *
 * @date 2026-04-11
 * @version 1.0
 */

#pragma once

#include "impl/config_backend.h"
#include <QJsonObject>
#include <QString>

namespace cf::config {

/**
 * @brief Config backend backed by a JSON file.
 *
 * Key mapping: "app.theme.name" → {"app":{"theme":{"name": value}}}
 *
 * In-memory QJsonObject is serialized to disk on sync().
 * File is read on construction and reload().
 */
class JsonConfigBackend : public IConfigBackend {
  public:
    /**
     * @brief Construct a JSON backend for the given file path.
     *
     * If the file exists, it is parsed. Otherwise root_ starts empty.
     * Parse errors are treated as empty config.
     *
     * @param[in] filePath Full path to the JSON file.
     */
    explicit JsonConfigBackend(const QString& filePath);

    /**
     * @brief Read a value by dot-separated key.
     * @param[in] key Dot-separated key to look up.
     * @return QVariant (invalid if not found).
     */
    QVariant value(const QString& key) const override;

    /**
     * @brief Write a value at the given key.
     * @param[in] key Dot-separated key to write.
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

    /// @brief Remove all keys.
    void clear() override;

    /// @brief Serialize the in-memory JSON object to disk.
    void sync() override;

    /// @brief Re-read and parse the JSON file from disk.
    void reload() override;

  private:
    /// @brief Read and parse the JSON file into root_. Returns true on success.
    bool loadFromFile();

    QString filePath_;
    QJsonObject root_;
};

} // namespace cf::config
