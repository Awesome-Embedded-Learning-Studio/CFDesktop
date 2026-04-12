/**
 * @file cfconfig_path_provider.h
 * @brief Abstract interface for providing config store file paths.
 *
 * @date 2026-03-17
 * @version 1.0
 */

#pragma once

#include <QString>

namespace cf::config {

/**
 * @brief Abstract interface for providing config store file paths.
 *
 * This interface allows different projects to provide their own path logic:
 * - Desktop project: uses standard paths like /etc/cfdesktop, ~/.config/cfdesktop
 * - Other projects: can use custom paths
 * - Tests: can use mock/temporary paths
 *
 * @note Thread-safe: implementations should not mutate state after construction.
 */
class IConfigStorePathProvider {
  public:
    virtual ~IConfigStorePathProvider() = default;

    /**
     * @brief Get the system-level config file path.
     *
     * @return Full path to system config (e.g., "/etc/cfdesktop/system.ini")
     * @return Empty QString if system layer should be disabled
     * @throws None
     * @note None
     * @warning None
     * @since 1.0
     * @ingroup none
     */
    virtual QString system_path() const = 0;

    /**
     * @brief Get the user-level config directory path.
     *
     * Creates the directory if it does not exist.
     *
     * @return Full path to user config directory (e.g., "/home/user/.config/cfdesktop")
     * @return Empty QString if user layer should be disabled
     * @throws None
     * @note None
     * @warning None
     * @since 1.0
     * @ingroup none
     */
    virtual QString user_dir() const = 0;

    /**
     * @brief Get the user-level config file name.
     *
     * Combined with user_dir() to form full path.
     *
     * @return File name (e.g., "user.ini")
     * @throws None
     * @note None
     * @warning None
     * @since 1.0
     * @ingroup none
     */
    virtual QString user_filename() const = 0;

    /**
     * @brief Get the app-level config directory path.
     *
     * Creates the directory if it does not exist.
     *
     * @return Full path to app config directory (e.g., "config")
     * @return Empty QString if app layer should be disabled
     * @throws None
     * @note None
     * @warning None
     * @since 1.0
     * @ingroup none
     */
    virtual QString app_dir() const = 0;

    /**
     * @brief Get the app-level config file name.
     *
     * Combined with app_dir() to form full path.
     *
     * @return File name (e.g., "app.ini")
     * @throws None
     * @note None
     * @warning None
     * @since 1.0
     * @ingroup none
     */
    virtual QString app_filename() const = 0;

    /**
     * @brief Check if a layer should be enabled.
     *
     * @param[in] layer_index The layer index to check.
     * @return true if the layer should be enabled.
     * @throws None
     * @note None
     * @warning None
     * @since 1.0
     * @ingroup none
     */
    virtual bool is_layer_enabled(int layer_index) const = 0;

    /**
     * @brief Get the full file path for a named domain in a specific layer.
     *
     * Used by non-default config domains to resolve their file paths.
     * Return empty QString to disable that layer for the domain.
     *
     * @param[in] layer_index 0=System, 1=User, 2=App (Temp is memory-only).
     * @param[in] domain_name Name of the config domain.
     * @return Full file path for the domain's config file in that layer.
     * @throws None
     * @since 1.1
     */
    virtual QString domain_path(int layer_index, const QString& domain_name) const {
        (void)layer_index;
        (void)domain_name;
        return QString();
    }
};

/**
 * @brief Default path provider for CFDesktop project.
 *
 * Provides standard paths:
 * - System: /etc/cfdesktop/system.ini
 * - User: ~/.config/cfdesktop/user.ini
 * - App: config/app.ini (relative to working directory)
 */
class DesktopConfigStorePathProvider : public IConfigStorePathProvider {
  public:
    /**
     * @brief Construct with default paths.
     *
     * Uses:
     * - system_path: "/etc/cfdesktop/system.ini"
     * - user_dir: "~/.config/cfdesktop"
     * - app_dir: "config"
     * @throws None
     * @note None
     * @warning None
     * @since 1.0
     * @ingroup none
     */
    DesktopConfigStorePathProvider();

    /**
     * @brief Construct with custom base directory.
     *
     * @param[in] app_base Base directory for app config (e.g., "config").
     * @throws None
     * @note None
     * @warning None
     * @since 1.0
     * @ingroup none
     */
    explicit DesktopConfigStorePathProvider(const QString& app_base);

    /**
     * @brief Construct with custom organization name.
     *
     * @param[in] organization Organization name for paths (e.g., "mycompany").
     * @param[in] application Application name for paths (e.g., "myapp").
     * @throws None
     * @note None
     * @warning None
     * @since 1.0
     * @ingroup none
     */
    DesktopConfigStorePathProvider(const QString& organization, const QString& application);

    // IConfigStorePathProvider interface
    /**
     * @brief Get the system-level config file path.
     *
     * @return Full path to system config.
     * @throws None
     * @note None
     * @warning None
     * @since 1.0
     * @ingroup none
     */
    QString system_path() const override;

    /**
     * @brief Get the user-level config directory path.
     *
     * @return Full path to user config directory.
     * @throws None
     * @note None
     * @warning None
     * @since 1.0
     * @ingroup none
     */
    QString user_dir() const override;

    /**
     * @brief Get the user-level config file name.
     *
     * @return File name (e.g., "user.ini").
     * @throws None
     * @note None
     * @warning None
     * @since 1.0
     * @ingroup none
     */
    QString user_filename() const override;

    /**
     * @brief Get the app-level config directory path.
     *
     * @return Full path to app config directory.
     * @throws None
     * @note None
     * @warning None
     * @since 1.0
     * @ingroup none
     */
    QString app_dir() const override;

    /**
     * @brief Get the app-level config file name.
     *
     * @return File name (e.g., "app.ini").
     * @throws None
     * @note None
     * @warning None
     * @since 1.0
     * @ingroup none
     */
    QString app_filename() const override;

    /**
     * @brief Check if a layer should be enabled.
     *
     * @param[in] layer_index The layer index to check.
     * @return true if the layer should be enabled.
     * @throws None
     * @note None
     * @warning None
     * @since 1.0
     * @ingroup none
     */
    bool is_layer_enabled(int layer_index) const override;

    /**
     * @brief Get file path for a named domain in a specific layer.
     *
     * For "default" domain, returns the original single-file paths.
     * For other domains, returns {base_dir}/{domain_name}.ini per layer.
     *
     * @param[in] layer_index 0=System, 1=User, 2=App.
     * @param[in] domain_name Name of the config domain.
     * @return Full file path for the domain.
     * @since 1.1
     */
    QString domain_path(int layer_index, const QString& domain_name) const override;

  private:
    QString system_path_;
    QString user_dir_;
    QString user_filename_ = "user.ini";
    QString app_dir_;
    QString app_filename_ = "app.ini";
};

} // namespace cf::config
