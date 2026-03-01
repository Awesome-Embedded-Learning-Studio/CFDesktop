/**
 * @file material_factory_class.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Material Design 3 Theme Factory
 * @version 0.1
 * @date 2026-02-27
 *
 * @copyright Copyright (c) 2026
 *
 * @details
 * MaterialFactory implements ThemeFactory interface for Material Design 3.
 * Supports creating themes by name (light/dark) and from JSON.
 */

#pragma once

#include <QByteArray>
#include <memory>

#include "../export.h"
#include "../theme_factory.h"

namespace cf::ui::core {

/**
 * @brief  Material Design 3 Theme Factory.
 *
 * @details Implements ThemeFactory for Material Design 3 themes.
 * Supports:
 * - Creating predefined themes by name: "theme.material.light", "theme.material.dark"
 * - Creating themes from Material Theme Builder JSON export
 * - Exporting themes to JSON format
 *
 * @note           None.
 * @warning        None.
 * @throws         None.
 * @since          0.1
 * @ingroup        ui_core
 *
 * @code
 * MaterialFactory factory;
 *
 * // Create by name
 * auto lightTheme = factory.fromName("theme.material.light");
 * auto darkTheme = factory.fromName("theme.material.dark");
 *
 * // Create from JSON
 * auto fromJson = factory.fromJson(jsonBytes);
 *
 * // Export to JSON
 * auto json = factory.toJson(lightTheme.get());
 * @endcode
 */
class CF_UI_EXPORT MaterialFactory : public ThemeFactory {
  public:
    ~MaterialFactory() override = default;

    /**
     * @brief  Create theme by name.
     *
     * @param[in] name Theme name:
     *                 - "theme.material.light": Light theme
     *                 - "theme.material.dark": Dark theme
     *
     * @return     Unique pointer to ICFTheme, or nullptr if name not recognized.
     *
     * @since     0.1
     * @ingroup   ui_core
     */
    std::unique_ptr<ICFTheme> fromName(const char* name) override;

    /**
     * @brief  Create theme from Material Theme Builder JSON.
     *
     * @param[in] json JSON export from Material Theme Builder.
     *
     * @return     Unique pointer to ICFTheme, or nullptr if parsing fails.
     *
     * @since     0.1
     * @ingroup   ui_core
     */
    std::unique_ptr<ICFTheme> fromJson(const QByteArray& json) override;

    /**
     * @brief  Export theme to JSON format.
     *
     * @param[in] raw_theme Pointer to ICFTheme (must be MaterialTheme).
     *
     * @return     JSON representation of the theme.
     *
     * @since     0.1
     * @ingroup   ui_core
     */
    QByteArray toJson(ICFTheme* raw_theme) override;
};

} // namespace cf::ui::core
