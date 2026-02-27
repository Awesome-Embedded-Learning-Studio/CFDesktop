/**
 * @file    ui/core/theme_factory.h
 * @brief   Abstract factory for creating and serializing CF UI themes.
 *
 * Defines the ThemeFactory interface for creating ICFTheme instances
 * from names or JSON data, and for serializing themes to JSON format.
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
#include <QByteArray>
#include <memory>

namespace cf::ui::core {

/**
 * @brief  Abstract factory for creating and serializing CF UI themes.
 *
 * ThemeFactory provides an interface for creating ICFTheme instances from
 * theme names or JSON data, and for serializing existing themes to JSON.
 * Implementations of this interface handle specific theme formats.
 *
 * @ingroup none
 *
 * @note   None
 *
 * @warning None
 *
 * @code
 * class MyThemeFactory : public ThemeFactory {
 * public:
 *     std::unique_ptr<ICFTheme> fromName(const char* name) override {
 *         // Create theme by name
 *     }
 *     std::unique_ptr<ICFTheme> fromJson(const QByteArray& json) override {
 *         // Parse JSON and create theme
 *     }
 *     QByteArray toJson(ICFTheme* raw_theme) override {
 *         // Serialize theme to JSON
 *     }
 * };
 * @endcode
 */
class CF_UI_EXPORT ThemeFactory {
  public:
    /// @brief Virtual destructor.
    virtual ~ThemeFactory() = default;

    /**
     * @brief  Creates a theme from its name.
     *
     * @param[in] name Name identifier of the theme to create.
     * @return        Unique pointer to the created theme, or nullptr on error.
     * @throws        May throw exceptions depending on implementation.
     * @note          Implementation-specific behavior for unknown names.
     * @warning       None
     * @since         N/A
     * @ingroup       none
     */
    virtual std::unique_ptr<ICFTheme> fromName(const char* name) = 0;

    /**
     * @brief  Creates a theme from JSON data.
     *
     * @param[in] json JSON byte array containing theme definition.
     * @return        Unique pointer to the created theme, or nullptr on error.
     * @throws        May throw exceptions depending on implementation.
     * @note          Implementation-specific JSON format validation.
     * @warning       None
     * @since         N/A
     * @ingroup       none
     */
    virtual std::unique_ptr<ICFTheme> fromJson(const QByteArray& json) = 0;

    /**
     * @brief  Serializes a theme to JSON format.
     *
     * @param[in] raw_theme Pointer to the theme to serialize.
     * @return              JSON byte array representing the theme.
     * @throws              May throw exceptions depending on implementation.
     * @note                None
     * @warning             None
     * @since               N/A
     * @ingroup             none
     */
    virtual QByteArray toJson(ICFTheme* raw_theme) = 0;
};

} // namespace cf::ui::core
