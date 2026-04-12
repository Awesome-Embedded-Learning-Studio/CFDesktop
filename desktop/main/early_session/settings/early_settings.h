/**
 * @file desktop/main/early_session/settings/early_settings.h
 * @brief Early settings configuration loader and manager.
 *
 * Handles loading and accessing early desktop initialization configuration
 * from JSON files using Qt's JSON parsing.
 *
 * @author CFDesktop Team
 * @date 2026-03-16
 * @version 0.14.0
 * @since 0.13.0
 * @ingroup early_session
 */
#pragma once

#include <QString>
#include <memory>

class QJsonObject;

namespace cf::desktop::early_settings {
/**
 * @brief Manages early desktop configuration settings.
 *
 * Loads and provides access to early initialization configuration from a
 * JSON-format configuration file.
 *
 * @note Not thread-safe unless externally synchronized.
 * @ingroup early_session
 *
 * @code
 * EarlySettings settings("settings/early.json");
 * if (settings.valid()) {
 *     QString logPath = settings.get_boot_logger_path();
 * }
 * @endcode
 */
class EarlySettings {
  public:
    /**
     * @brief Constructs EarlySettings and loads from the specified config path.
     *
     * @param[in] early_config_path Path to the early configuration JSON file.
     */
    EarlySettings(const QString& early_config_path);

    ~EarlySettings();

    /**
     * @brief Checks if the settings were loaded successfully.
     *
     * @return True if settings are valid and loaded without error, false otherwise.
     */
    bool valid() const;

    /**
     * @brief Gets the file path from which settings were loaded.
     *
     * @return Absolute path to the configuration file.
     */
    QString loadFrom() const;

    /**
     * @brief Gets the boot logger directory path from settings.
     *
     * @return Path to the boot logger directory.
     * @note The result is cached after the first call.
     */
    QString get_boot_logger_path() const;

    /**
     * @brief Gets the Desktop Root, creating it if the target does not exist.
     *
     * @return QString
     */
    QString get_desktop_root() const;

    /**
     * @brief Gets the desktop configuration folder path (relative to app runtime dir).
     *
     * @return Relative config folder path, defaults to "settings/desktop/".
     */
    QString get_config_folder() const;

    /**
     * @brief Unlock the Early Settings, used in releasing the sessions.
     *
     * @return std::unique_ptr<QJsonObject> containing the early settings data.
     *         Returns nullptr if settings are invalid.
     */
    std::unique_ptr<QJsonObject> unlock_early_settings();

  private:
    /// @brief Cached path to the log file for the current session.
    mutable QString this_session_logfile_path{};

    /// @brief Cached Desktop Root; only changes when the desktop path is switched.
    mutable QString desktop_root{};

    /// @brief Cached config folder path.
    mutable QString config_folder_cache{};

    /// @brief Parsed JSON object containing the configuration data. Ownership: owner.
    std::unique_ptr<QJsonObject> early_settings_obj_;

    /// @brief Whether the JSON file was loaded successfully.
    bool valid_ = false;

    /// @brief Path to the configuration file.
    QString filepath_{};
};
} // namespace cf::desktop::early_settings
