/**
 * @file material_application.h
 * @brief Material Design Application class
 *
 * Application class pre-configured for Material Design 3 theming.
 * Automatically registers Material Design light and dark themes.
 *
 * @author CFDesktop Team
 * @date 2026-03-01
 * @version 0.1
 */

#pragma once

#include "token/theme_name/material_theme_name.h"
#include "ui/export.h"
#include "ui/widget/application_support/application.h"
#include <QApplication>

namespace cf::ui::widget::material {

/**
 * @brief  Material Design 3 Application class.
 *
 * @details Extends Application to provide Material Design 3 theme support.
 *          Automatically registers Material Design light and dark themes
 *          on construction.
 *
 * @note           This class registers the material themes automatically.
 * @warning        Use this instead of Application for Material Design apps.
 * @throws         May throw if theme registration fails.
 * @since          0.1
 * @ingroup        ui_widget_material
 *
 * @code
 * int main(int argc, char* argv[]) {
 *     using namespace cf::ui::widget::material;
 *
 *     MaterialApplication app(argc, argv);
 *     app.setTheme("theme.material.light");
 *
 *     MyWidget w;
 *     w.show();
 *
 *     return app.exec();
 * }
 * @endcode
 */
class CF_UI_EXPORT MaterialApplication : public application_support::Application {
    Q_OBJECT

  public:
    static constexpr const char* DEFAULT_THEME =
        cf::ui::core::token::literals::MATERIAL_THEME_LIGHT;
    /**
     * @brief  Constructor with standard QApplication arguments.
     *
     * @param  argc Argument count (reference for Qt compatibility).
     * @param  argv Argument values.
     *
     * @throws     May throw if theme registration fails.
     * @note       Automatically registers Material Design themes.
     * @warning    None
     * @since      0.1
     * @ingroup    ui_widget_material
     *
     * @code
     * MaterialApplication app(argc, argv);
     * @endcode
     */
    MaterialApplication(int& argc, char** argv);

    /**
     * @brief  Destructor.
     *
     * @since 0.1
     */
    ~MaterialApplication() override;

    // Non-copyable, non-movable
    MaterialApplication(const MaterialApplication&) = delete;
    MaterialApplication& operator=(const MaterialApplication&) = delete;
    MaterialApplication(MaterialApplication&&) = delete;
    MaterialApplication& operator=(MaterialApplication&&) = delete;

  protected:
    /**
     * @brief  Register Material themes and initialize.
     *
     * @details Overrides Application::init() to register Material Design themes
     *          before calling base initialization.
     *
     * @since 0.1
     */
    void init() override;
};

} // namespace cf::ui::widget::material
