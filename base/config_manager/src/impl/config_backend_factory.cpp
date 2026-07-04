/**
 * @file config_backend_factory.cpp
 * @brief Factory for creating config backends based on file extension.
 *
 * @date 2026-04-11
 * @version 1.0
 */

#include "config_backend_factory.h"
#include "json_backend.h"
#include "qsettings_backend.h"

namespace cf::config {

std::unique_ptr<IConfigBackend> createBackend(const QString& filePath) {
    if (filePath.isEmpty()) {
        return nullptr;
    }

    if (filePath.endsWith(".json", Qt::CaseInsensitive)) {
        return std::make_unique<JsonConfigBackend>(filePath);
    }

    // Default: INI format via QSettings
    return std::make_unique<QSettingsBackend>(filePath);
}

} // namespace cf::config
