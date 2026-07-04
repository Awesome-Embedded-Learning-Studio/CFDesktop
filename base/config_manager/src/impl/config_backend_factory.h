/**
 * @file config_backend_factory.h
 * @brief Factory for creating config backends based on file extension.
 *
 * @date 2026-04-11
 * @version 1.0
 */

#pragma once

#include "impl/config_backend.h"
#include <QString>
#include <memory>

namespace cf::config {

/**
 * @brief Create a config backend appropriate for the given file path.
 *
 * Detects format from file extension:
 * - ".json" → JsonConfigBackend
 * - otherwise → QSettingsBackend (INI)
 *
 * @param filePath Full path to the config file.
 * @return Backend instance, or nullptr if path is empty.
 */
std::unique_ptr<IConfigBackend> createBackend(const QString& filePath);

} // namespace cf::config
