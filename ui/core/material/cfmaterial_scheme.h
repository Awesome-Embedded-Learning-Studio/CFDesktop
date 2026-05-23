/**
 * @file cfmaterial_scheme.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Material Design 3 Color Scheme
 * @version 0.1
 * @date 2026-02-25
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once
#include <QColor>
#include <string>
#include <unordered_map>

#include "../../export.h"
#include "color_scheme.h"

namespace cf::ui::core {

/**
 * @brief  Material Design 3 Color Scheme.
 *
 * @details Implements the complete Material Design 3 color system with 26 color tokens.
 * Colors are stored in a flat map keyed by string token names.
 *
 * Factory functions are available in the cf::ui::core::material namespace.
 *
 * @note           None.
 * @warning        None.
 * @throws         None.
 * @since          0.1
 * @ingroup        ui_core
 *
 * @code
 * #include "material_factory.hpp"
 *
 * // Create a light theme
 * auto lightScheme = cf::ui::core::material::light();
 *
 * // Query colors by name
 * QColor primary = lightScheme.queryExpectedColor("md.primary");
 * @endcode
 */
class CF_UI_EXPORT MaterialColorScheme : public ICFColorScheme {
  public:
    MaterialColorScheme();
    ~MaterialColorScheme() override = default;

    // Non-copyable, movable
    MaterialColorScheme(const MaterialColorScheme&) = delete;
    MaterialColorScheme& operator=(const MaterialColorScheme&) = delete;
    MaterialColorScheme(MaterialColorScheme&&) noexcept = default;
    MaterialColorScheme& operator=(MaterialColorScheme&&) noexcept = default;

    /**
     * @brief Query a color by name.
     *
     * @param[in] name Color token name (e.g., "md.primary").
     *
     * @return Reference to the QColor.
     *
     * @throws    None.
     *
     * @note      Returns a reference that is valid for the lifetime
     *            of the color scheme.
     *
     * @warning   None.
     *
     * @since     0.1
     * @ingroup   ui_core
     */
    QColor& queryExpectedColor(const char* name) override;

    /**
     * @brief Query a color by name (const overload).
     *
     * @param[in] name Color token name.
     *
     * @return Copy of the QColor.
     *
     * @throws    None.
     *
     * @note      Returns a default black color if not found.
     *
     * @warning   None.
     *
     * @since     0.1
     * @ingroup   ui_core
     */
    QColor queryColor(const char* name) const;

    /**
     * @brief Register a color by name.
     *
     * @param[in] name  Color token name (e.g., "md.primary").
     * @param[in] color Color value to register.
     *
     * @since     0.1
     * @ingroup   ui_core
     */
    void setColor(const std::string& name, const QColor& color);

    /**
     * @brief Check if a color token exists.
     *
     * @param[in] name Color token name.
     *
     * @return    true if the token exists.
     *
     * @since     0.1
     * @ingroup   ui_core
     */
    bool hasColor(const std::string& name) const;

  private:
    std::unordered_map<std::string, QColor> colors_;
};

} // namespace cf::ui::core
