/**
 * @file material_application.cpp
 * @brief Material Design Application class implementation
 *
 * @author CFDesktop Team
 * @date 2026-03-01
 * @version 0.1
 */

#include "material_application.h"
#include "components/material/cfmaterial_animation_factory.h"
#include "ui/core/material/material_factory_class.h"
#include "ui/core/token/theme_name/material_theme_name.h"

namespace cf::ui::widget::material {

namespace token_literals = ::cf::ui::core::token::literals;

MaterialApplication::MaterialApplication(int& argc, char** argv)
    : application_support::Application(argc, argv) {
    // Call init to register themes and complete initialization
    // (Derived class constructor body can safely call virtual init())
    init();
}

MaterialApplication::~MaterialApplication() = default;

void MaterialApplication::init() {
    // Register Material animation factory FIRST (before theme registration)
    // This must be registered before calling Application::init() which
    // tries to create the animation factory
    Application::registerAnimationFactoryType("theme.material", [](const core::ICFTheme& theme,
                                                                   QObject* parent) {
        return std::make_unique<components::material::CFMaterialAnimationFactory>(theme, nullptr,
                                                                                  parent);
    });

    // Register Material Design themes
    auto* themeManager = Application::themeManager();
    auto installMaterialTheme = []() { return std::make_unique<cf::ui::core::MaterialFactory>(); };
    themeManager->insert_one(token_literals::MATERIAL_THEME_LIGHT, installMaterialTheme);
    themeManager->insert_one(token_literals::MATERIAL_THEME_DARK, installMaterialTheme);
    themeManager->setThemeTo(DEFAULT_THEME, false);

    // Then call base class init to complete initialization
    Application::init();
}

} // namespace cf::ui::widget::material
