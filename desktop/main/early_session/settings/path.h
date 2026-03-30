/**
 * @file desktop/main/early_session/settings/path.h
 * @brief Default paths and template for early settings configuration.
 *
 * Defines the default file path for the early settings configuration file
 * and provides a template content for generating the configuration file.
 *
 * @author CFDesktop Team
 * @date 2026-03-16
 * @version 0.13.1
 * @since 0.13.0
 * @ingroup early_session
 */
#pragma once

namespace cf::desktop::early_stage {
/// @brief Default relative path to the early settings configuration file.
static constexpr const char* EARLY_SETTINGS = "settings/early.ini";

/// @brief Template content for generating the early settings INI file.
static constexpr const char* EARLY_SETTINGS_TEMPLATE = R"([logger]
# Dirent Should be relatives at CFDesktop Itself
dirent="logger/"
)";
} // namespace cf::desktop::early_stage
