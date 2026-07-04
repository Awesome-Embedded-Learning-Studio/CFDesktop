/**
 * @file json_backend.cpp
 * @brief JSON-based config backend implementation.
 *
 * @date 2026-04-11
 * @version 1.0
 */

#include "json_backend.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonParseError>

namespace cf::config {

// ============================================================================
// Recursive helpers for QJsonObject's value-based semantics
// ============================================================================

/// @brief Set a value at a nested key path, creating intermediate objects as needed.
static void setValueNested(QJsonObject& obj, const QStringList& parts, int index,
                           const QJsonValue& value) {
    if (index == parts.size() - 1) {
        obj.insert(parts[index], value);
        return;
    }
    // Extract or create child object
    QJsonObject child = obj.value(parts[index]).toObject();
    setValueNested(child, parts, index + 1, value);
    obj.insert(parts[index], child);
}

/// @brief Remove a key at a nested path. Does NOT prune empty parents.
static void removeNested(QJsonObject& obj, const QStringList& parts, int index) {
    if (index == parts.size() - 1) {
        obj.remove(parts[index]);
        return;
    }
    QJsonValue child = obj.value(parts[index]);
    if (!child.isObject()) {
        return;
    }
    QJsonObject childObj = child.toObject();
    removeNested(childObj, parts, index + 1);
    obj.insert(parts[index], childObj);
}

/// @brief Traverse nested objects and return the value at the leaf. Returns Undefined if not found.
static QJsonValue valueNested(const QJsonObject& obj, const QStringList& parts) {
    QJsonValue current = obj;
    for (const QString& part : parts) {
        if (!current.isObject()) {
            return QJsonValue();
        }
        current = current.toObject().value(part);
        if (current.isUndefined()) {
            return QJsonValue();
        }
    }
    return current;
}

// ============================================================================
// JsonConfigBackend
// ============================================================================

JsonConfigBackend::JsonConfigBackend(const QString& filePath) : filePath_(filePath) {
    loadFromFile();
}

QVariant JsonConfigBackend::value(const QString& key) const {
    const QStringList parts = key.split('.');
    QJsonValue v = valueNested(root_, parts);
    if (v.isUndefined() || v.isNull()) {
        // Distinguish "key exists with null value" from "key not found"
        // contains() handles existence check; value() just returns the data
        if (v.isNull()) {
            return QVariant(); // null JSON value → invalid QVariant (consistent with "not set")
        }
        return QVariant();
    }
    return v.toVariant();
}

void JsonConfigBackend::setValue(const QString& key, const QVariant& value) {
    const QStringList parts = key.split('.');
    if (parts.isEmpty() || key.isEmpty()) {
        return;
    }
    QJsonValue leaf = QJsonValue::fromVariant(value);
    setValueNested(root_, parts, 0, leaf);
}

bool JsonConfigBackend::contains(const QString& key) const {
    const QStringList parts = key.split('.');
    QJsonValue current = root_;

    for (const QString& part : parts) {
        if (!current.isObject()) {
            return false;
        }
        QJsonObject obj = current.toObject();
        if (!obj.contains(part)) {
            return false;
        }
        current = obj.value(part);
    }
    return true;
}

void JsonConfigBackend::remove(const QString& key) {
    const QStringList parts = key.split('.');
    if (parts.isEmpty() || key.isEmpty()) {
        return;
    }
    removeNested(root_, parts, 0);
}

void JsonConfigBackend::clear() {
    root_ = QJsonObject();
}

void JsonConfigBackend::sync() {
    if (filePath_.isEmpty()) {
        return;
    }

    QFileInfo info(filePath_);
    QDir dir = info.absoluteDir();
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QJsonDocument doc(root_);
    QFile file(filePath_);
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
    }
}

void JsonConfigBackend::reload() {
    loadFromFile();
}

bool JsonConfigBackend::loadFromFile() {
    if (filePath_.isEmpty()) {
        root_ = QJsonObject();
        return true;
    }

    QFile file(filePath_);
    if (!file.exists()) {
        root_ = QJsonObject();
        return true;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        root_ = QJsonObject();
        return false;
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
    file.close();

    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        root_ = QJsonObject();
        return false;
    }

    root_ = doc.object();
    return true;
}

} // namespace cf::config
