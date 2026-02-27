/**
 * @file    ui/core/theme_manager.h
 * @brief   Singleton manager for CF UI theme registration and application.
 *
 * ThemeManager manages theme factory registration, theme creation, and
 * application of themes to widgets. Emits signals when the theme changes.
 *
 * @author  N/A
 * @date    N/A
 * @version N/A
 * @since   N/A
 * @ingroup none
 */

#pragma once
#include "export.h"
#include "theme.h"
#include "theme_factory.h"
#include <QObject>
#include <QtWidgets/qwidget.h>
#include <functional>
#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace cf::ui::core {

/**
 * @brief  Singleton manager for CF UI theme registration and application.
 *
 * ThemeManager manages theme factory registration, theme creation, and
 * application of themes to widgets. Emits signals when the theme changes.
 *
 * @ingroup none
 *
 * @note   Singleton instance accessed via instance().
 *
 * @warning None
 *
 * @code
 * auto& manager = ThemeManager::instance();
 * manager.insert_one("my_theme", []() { return std::make_unique<MyFactory>(); });
 * manager.install_widget(my_widget);
 * manager.setThemeTo("my_theme");
 * @endcode
 */
class CF_UI_EXPORT ThemeManager : public QObject {
    Q_OBJECT
  public:
    /**
     * @brief  Gets the singleton ThemeManager instance.
     *
     * @return Reference to the singleton instance.
     * @throws None
     * @note   Thread-safe initialization.
     * @warning None
     * @since  N/A
     * @ingroup none
     */
    static ThemeManager& instance();

    /**
     * @brief  Gets a theme by name.
     *
     * @param[in] name Name of the theme to retrieve.
     * @return        Reference to the requested theme.
     * @throws        May throw if theme not found.
     * @note          Theme is created on first access and cached.
     * @warning       Theme must have been registered via insert_one().
     * @since         N/A
     * @ingroup       none
     */
    const ICFTheme& theme(const std::string& name) const;

    /// @brief Function type for creating ThemeFactory instances.
    using InstallerMaker = std::function<std::unique_ptr<ThemeFactory>()>;

    /**
     * @brief  Registers a theme factory with a name.
     *
     * @param[in] name Name identifier for the theme.
     * @param[in] make_one Factory function that creates ThemeFactory instances.
     * @return        true if registration succeeded, false on duplicate name.
     * @throws        None
     * @note          The factory function is called when creating themes.
     * @warning       None
     * @since         N/A
     * @ingroup       none
     */
    bool insert_one(const std::string& name, InstallerMaker make_one);

    /**
     * @brief  Removes a theme factory by name.
     *
     * @param[in] name Name of the theme to remove.
     * @throws        None
     * @note          Does not affect currently active theme.
     * @warning       None
     * @since         N/A
     * @ingroup       none
     */
    void remove_one(const std::string& name);

    /**
     * @brief  Installs a widget for theme updates.
     *
     * @param[in] w Pointer to the widget to install.
     * @throws        None
     * @note          Installed widgets receive theme change notifications.
     * @warning       None
     * @since         N/A
     * @ingroup       none
     */
    void install_widget(QWidget* w);

    /**
     * @brief  Removes a widget from theme updates.
     *
     * @param[in] w Pointer to the widget to remove.
     * @throws        None
     * @note          The widget no longer receives theme change notifications.
     * @warning       None
     * @since         N/A
     * @ingroup       none
     */
    void remove_widget(QWidget* w);

    /**
     * @brief  Sets the current active theme.
     *
     * @param[in] name Name of the theme to activate.
     * @throws        None
     * @note          Emits themeChanged signal after successful theme change.
     * @warning       Theme must have been registered via insert_one().
     * @since         N/A
     * @ingroup       none
     */
    void setThemeTo(const std::string& name);

  signals:
    /**
     * @brief  Signal emitted when the theme changes.
     *
     * @param[in] new_theme Reference to the newly activated theme.
     *
     * @note None
     * @warning None
     * @since  N/A
     * @ingroup none
     */
    void themeChanged(const ICFTheme& new_theme);

  private:
    /**
     * @brief  Constructs the ThemeManager.
     *
     * @param[in] parent Optional parent QObject.
     * @throws        None
     * @note          Private constructor for singleton pattern.
     * @warning       None
     * @since         N/A
     * @ingroup       none
     */
    ThemeManager(QObject* parent = nullptr);

    /// @brief Destructor.
    ~ThemeManager() override = default;

    /// @brief Copy constructor: deleted.
    ThemeManager(const ThemeManager&) = delete;

    /// @brief Copy assignment: deleted.
    ThemeManager& operator=(const ThemeManager&) = delete;

    /// @brief Set of widgets receiving theme updates. Ownership: observer.
    std::unordered_set<QWidget*> installed_widget;

    /// @brief Map of registered theme factories. Ownership: owner.
    std::unordered_map<std::string, std::unique_ptr<ThemeFactory>> factories_;

    /// @brief Cache of created theme instances. Ownership: owner.
    std::unordered_map<std::string, std::unique_ptr<ICFTheme>> theme_cache_;

    /// @brief Name of the currently active theme.
    std::string current_theme_name_;
};

} // namespace cf::ui::core
