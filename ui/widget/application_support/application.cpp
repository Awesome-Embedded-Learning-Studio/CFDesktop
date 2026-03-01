/**
 * @file    ui/widget/application_support/application.cpp
 * @author  N/A
 * @brief   CF Desktop Application class implementation
 * @version 0.1
 * @date    2026-02-28
 *
 * @copyright Copyright (c) 2026
 */

#include "application.h"
#include <stdexcept>

namespace cf::ui::widget::application_support {

Application::Application(int& argc, char** argv) : QApplication(argc, argv), initialized_(false) {
    // Connect to ThemeManager's themeChanged signal
    auto& tm = core::ThemeManager::instance();
    connect(&tm, &core::ThemeManager::themeChanged, this, &Application::onThemeManagerChanged);
}

Application::~Application() {
    // Cleanup: animationFactory_ must be destroyed before theme cleanup
    // (since it holds references to the theme)
    animationFactory_.reset();
}

void Application::init() {
    if (initialized_) {
        throw std::runtime_error(
            "Application::init() already called. Initialization can only be done once.");
    }

    // Initialize animation factory with current theme
    initializeAnimationFactory();
    initialized_ = true;
}

Application* Application::instance() {
    return qobject_cast<Application*>(QApplication::instance());
}

core::ThemeManager* Application::themeManager() {
    return &core::ThemeManager::instance();
}

cf::WeakPtr<components::ICFAnimationManagerFactory> Application::animationFactory() {
    if (auto* app = instance()) {
        return app->animationFactory_->GetWeakPtr();
    }
    return {};
}

const core::ICFTheme& Application::theme(const std::string& themeToken) const {
    return core::ThemeManager::instance().theme(themeToken);
}

void Application::setTheme(const std::string& themeToken) {
    core::ThemeManager::instance().setThemeTo(themeToken);
}

const core::ICFTheme& Application::currentTheme() const {
    auto& tm = core::ThemeManager::instance();
    const auto& name = tm.currentThemeName();

    if (name.empty()) {
        // If no theme has been set, throw exception
        // TODO: Support dynamic loading of default themes
        throw std::runtime_error("No theme set. Use MaterialApplication for automatic theme "
                                 "registration, or call setTheme() before accessing themes.");
    }

    return tm.theme(name);
}

cf::WeakPtr<components::ICFAbstractAnimation>
Application::animation(const std::string& animationToken) {
    if (animationFactory_) {
        return animationFactory_->getAnimation(animationToken.c_str());
    }
    return {};
}

void Application::setAnimationsEnabled(bool enabled) {
    if (animationFactory_) {
        animationFactory_->setEnabledAll(enabled);
        emit animationsEnabledChanged(enabled);
    }
}

bool Application::animationsEnabled() const {
    return animationFactory_ ? animationFactory_->isAllEnabled() : false;
}

void Application::initializeAnimationFactory() {
    auto& tm = core::ThemeManager::instance();
    const auto& themeName = tm.currentThemeName();
    const auto& currentTheme = this->currentTheme();

    animationFactory_ = createAnimationFactory(themeName, currentTheme, this);
}

void Application::onThemeManagerChanged(const core::ICFTheme& newTheme) {
    // Recreate animation factory with new theme
    // This is necessary because factories store a reference to theme
    auto oldEnabled = animationsEnabled();
    auto& tm = core::ThemeManager::instance();
    const auto& themeName = tm.currentThemeName();

    animationFactory_ = createAnimationFactory(themeName, newTheme, this);

    animationFactory_->setEnabledAll(oldEnabled);

    // Forward the signal
    emit themeChanged(newTheme);
}

// =============================================================================
// Animation Factory Registry
// =============================================================================

std::unordered_map<std::string, Application::AnimationFactoryMaker>&
Application::animationFactoryRegistry() {
    static std::unordered_map<std::string, AnimationFactoryMaker> registry;
    return registry;
}

bool Application::registerAnimationFactoryType(const std::string& themePrefix,
                                               AnimationFactoryMaker maker) {
    auto& registry = animationFactoryRegistry();

    if (registry.find(themePrefix) != registry.end()) {
        return false; // Already registered
    }

    registry[themePrefix] = std::move(maker);
    return true;
}

void Application::unregisterAnimationFactoryType(const std::string& themePrefix) {
    auto& registry = animationFactoryRegistry();
    registry.erase(themePrefix);
}

bool Application::registerAnimationFactory(const std::string& themePrefix,
                                           AnimationFactoryMaker maker) {
    return registerAnimationFactoryType(themePrefix, std::move(maker));
}

void Application::unregisterAnimationFactory(const std::string& themePrefix) {
    unregisterAnimationFactoryType(themePrefix);
}

std::unique_ptr<components::ICFAnimationManagerFactory>
Application::createAnimationFactory(const std::string& themeName, const core::ICFTheme& theme,
                                    QObject* parent) {
    auto& registry = animationFactoryRegistry();

    // Find the best matching prefix
    // themeName format: "theme.material.light", "theme.fluent.dark"
    // We try to match "theme.material", "theme.fluent", etc.

    // Split by '.' and check for prefixes
    // Try longest prefixes first (most specific)
    std::string bestMatch;
    for (const auto& [prefix, maker] : registry) {
        if (themeName.size() >= prefix.size() && themeName.compare(0, prefix.size(), prefix) == 0) {
            // Check if this is a better (longer) match
            if (prefix.size() > bestMatch.size()) {
                bestMatch = prefix;
            }
        }
    }

    if (!bestMatch.empty()) {
        // Found a matching factory
        return registry[bestMatch](theme, parent);
    }

    // No matching factory found - throw exception
    // TODO: Support dynamic loading of animation factories
    throw std::runtime_error(
        "No animation factory registered for theme '" + themeName +
        "'. Register an animation factory for this theme prefix, "
        "or use MaterialApplication which includes default Material factory support.");
}

} // namespace cf::ui::widget::application_support
