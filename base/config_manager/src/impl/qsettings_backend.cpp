/**
 * @file qsettings_backend.cpp
 * @brief QSettings-based config backend implementation.
 *
 * @date 2026-04-11
 * @version 1.0
 */

#include "qsettings_backend.h"

namespace cf::config {

QSettingsBackend::QSettingsBackend(const QString& filePath)
    : settings_(std::make_unique<QSettings>(filePath, QSettings::IniFormat)) {}

QVariant QSettingsBackend::value(const QString& key) const {
    return settings_->value(toPath(key));
}

void QSettingsBackend::setValue(const QString& key, const QVariant& value) {
    settings_->setValue(toPath(key), value);
}

bool QSettingsBackend::contains(const QString& key) const {
    return settings_->contains(toPath(key));
}

void QSettingsBackend::remove(const QString& key) {
    settings_->remove(toPath(key));
}

void QSettingsBackend::clear() {
    settings_->clear();
}

void QSettingsBackend::sync() {
    settings_->sync();
}

void QSettingsBackend::reload() {
    // QSettings automatically re-reads on next access after sync()
    settings_->sync();
}

QString QSettingsBackend::toPath(const QString& dotKey) {
    return QString(dotKey).replace('.', '/');
}

} // namespace cf::config
