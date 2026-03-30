/**
 * @file desktop/main/early_session/settings/early_settings.h
 * @brief Early settings configuration loader and manager.
 *
 * Handles loading and accessing early desktop initialization configuration
 * from INI files using Qt's QSettings mechanism.
 *
 * @author CFDesktop Team
 * @date 2026-03-16
 * @version 0.13.1
 * @since 0.13.0
 * @ingroup early_session
 */
#pragma once

#include <QSettings>
#include <QString>
#include <memory>

namespace cf::desktop::early_settings {
/**
 * @brief Manages early desktop configuration settings.
 *
 * Loads and provides access to early initialization configuration from an
 * INI-format configuration file. Uses Qt's QSettings for parsing.
 *
 * @note Not thread-safe unless externally synchronized.
 * @ingroup early_session
 *
 * @code
 * EarlySettings settings("settings/early.ini");
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
     * @param[in] early_config_path Path to the early configuration INI file.
     * @throws None
     * @note None
     * @warning None
     * @since 0.13.0
     * @ingroup early_session
     */
    EarlySettings(const QString& early_config_path);

    ~EarlySettings() = default;

    /**
     * @brief Checks if the settings were loaded successfully.
     *
     * @return True if settings are valid and loaded without error, false otherwise.
     * @throws None
     * @note None
     * @warning None
     * @since 0.13.0
     * @ingroup early_session
     */
    bool valid() const;

    /**
     * @brief Gets the file path from which settings were loaded.
     *
     * @return Absolute path to the configuration file.
     * @throws None
     * @note None
     * @warning None
     * @since 0.13.0
     * @ingroup early_session
     */
    QString loadFrom() const;

    /**
     * @brief Gets the boot logger directory path from settings.
     *
     * Reads the logger directory configuration from the early settings file.
     *
     * @return Path to the boot logger directory.
     * @throws None
     * @note The result is cached after the first call.
     * @warning None
     * @since 0.13.0
     * @ingroup early_session
     */
    QString get_boot_logger_path() const;

    std::unique_ptr<QSettings> unlock_early_settings();

  private:
    /// @brief Cached path to the log file for the current session. Ownership: owner.
    mutable QString this_session_logfile_path{};
    /// @brief Qt settings object for reading the INI configuration. Ownership: owner.
    std::unique_ptr<QSettings> early_settings;
};
} // namespace cf::desktop::early_settings
