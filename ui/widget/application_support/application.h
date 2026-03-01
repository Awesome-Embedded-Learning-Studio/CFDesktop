/**
 * @file    ui/widget/application_support/application.h
 * @author  N/A
 * @brief   CF Desktop Application class with integrated theme and animation support
 * @version 0.1
 * @date    2026-02-28
 *
 * @copyright Copyright (c) 2026
 *
 * @details
 * Application extends QApplication to provide unified access to ThemeManager
 * and CFMaterialAnimationFactory. It replaces the standard QApplication in main()
 * and provides token-based APIs for theme and animation access.
 */

#pragma once

#include "components/animation_factory_manager.h"
#include "core/theme_manager.h"
#include "export.h"
#include <QApplication>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace cf::ui::widget::application_support {

/**
 * @brief  Application class with integrated theme and animation management.
 *
 * @details Extends QApplication to provide unified access to ThemeManager and
 *          CFMaterialAnimationFactory. Replaces standard QApplication in main().
 *
 * @note    Thread-safe for concurrent reads.
 * @warning The animation factory is owned by Application; WeakPtr may become
 *          invalid if the application is destroyed.
 * @throws  None (all errors return invalid WeakPtr or throw from ThemeManager)
 * @since   0.1
 * @ingroup ui_widget_application_support
 *
 * @code
 * int main(int argc, char* argv[]) {
 *     using namespace cf::ui::widget::application_support;
 *
 *     Application app(argc, argv);
 *     app.setTheme("theme.material.light");
 *
 *     MyWidget w;
 *     w.show();
 *
 *     return app.exec();
 * }
 *
 * // Access animations from anywhere
 * auto fadeIn = Application::animation("md.animation.fadeIn");
 * if (fadeIn) {
 *     fadeIn->setTargetWidget(myWidget);
 *     fadeIn->start();
 * }
 * @endcode
 */
class CF_UI_EXPORT Application : public QApplication {
    Q_OBJECT

  public:
    /**
     * @brief  Constructor with standard QApplication arguments.
     *
     * @param[in]     argc Argument count (reference for Qt compatibility).
     * @param[in]     argv Argument values.
     *
     * @throws        None
     * @note          Initializes animation factory with current theme.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_application_support
     *
     * @code
     * Application app(argc, argv);
     * @endcode
     */
    Application(int& argc, char** argv);

    /**
     * @brief  Destructor.
     *
     * @details All owned resources are cleaned up. The animation factory
     *          is destroyed before theme cleanup (since it holds theme references).
     *
     * @since 0.1
     */
    ~Application() override;

    // Non-copyable, non-movable
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;

    // ========================================================================
    // Global Access (Static Methods - Convenience API)
    // ========================================================================

    /**
     * @brief  Get the singleton Application instance.
     *
     * @return Pointer to Application instance, or nullptr if not created.
     *
     * @since 0.1
     */
    static Application* instance();

    /**
     * @brief  Get the ThemeManager singleton.
     *
     * @details Convenience method equivalent to ThemeManager::instance().
     *
     * @return Pointer to ThemeManager.
     *
     * @since 0.1
     */
    static core::ThemeManager* themeManager();

    /**
     * @brief  Get the animation factory.
     *
     * @return WeakPtr to the animation factory, or invalid WeakPtr if
     *          Application instance doesn't exist.
     *
     * @since 0.1
     */
    static cf::WeakPtr<components::ICFAnimationManagerFactory> animationFactory();

    // ========================================================================
    // Theme Access (Token-based)
    // ========================================================================

    /**
     * @brief  Get a theme by token name.
     *
     * @param[in]     themeToken Theme identifier (e.g., "theme.material.light").
     *
     * @return        Reference to the theme.
     * @throws        May throw if theme not found.
     * @note          None
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_application_support
     */
    const core::ICFTheme& theme(const std::string& themeToken) const;

    /**
     * @brief  Set the active theme by token.
     *
     * @param[in]     themeToken Theme identifier.
     *
     * @throws        None
     * @note          Emits themeChanged signal after successful theme change.
     * @warning       Theme must have been registered via ThemeManager::insert_one().
     * @since         0.1
     * @ingroup       ui_widget_application_support
     *
     * @code
     * app.setTheme("theme.material.dark");
     * @endcode
     */
    void setTheme(const std::string& themeToken);

    /**
     * @brief  Get the current active theme.
     *
     * @return Reference to the current theme.
     * @throws May throw if no theme is set.
     *
     * @since 0.1
     */
    const core::ICFTheme& currentTheme() const;

    // ========================================================================
    // Animation Access (Token-based)
    // ========================================================================

    /**
     * @brief  Get an animation by token name.
     *
     * @details Retrieves an animation from the animation factory using
     *          token-based lookup. Tokens are resolved through the
     *          token mapping system.
     *
     * @param[in]     animationToken Animation identifier (e.g., "md.animation.fadeIn").
     *
     * @return        WeakPtr to the animation, or invalid WeakPtr if:
     *                - Token is not found in mapping
     *                - Animation type is not supported
     *                - Animations are globally disabled
     *
     * @throws        None
     * @note          The returned WeakPtr may become invalid if the factory
     *                is destroyed or the theme changes.
     * @warning       Always check validity before use.
     * @since         0.1
     * @ingroup       ui_widget_application_support
     *
     * @code
     * auto anim = Application::animation("md.animation.fadeIn");
     * if (anim) {
     *     anim->setTargetWidget(myWidget);
     *     anim->start();
     * }
     * @endcode
     */
    cf::WeakPtr<components::ICFAbstractAnimation> animation(const std::string& animationToken);

    /**
     * @brief  Set global animation enabled state.
     *
     * @details When disabled, animation() returns invalid WeakPtr.
     *          Existing animations continue to run; only new creations are affected.
     *
     * @param[in]     enabled true to enable animations, false to disable.
     *
     * @throws        None
     * @note          Emits animationsEnabledChanged signal.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_application_support
     *
     * @code
     * // Disable animations during heavy processing
     * Application::setAnimationsEnabled(false);
     * // ... do heavy work ...
     * Application::setAnimationsEnabled(true);
     * @endcode
     */
    void setAnimationsEnabled(bool enabled);

    /**
     * @brief  Check if animations are globally enabled.
     *
     * @return true if animations are enabled, false otherwise.
     *
     * @since 0.1
     */
    bool animationsEnabled() const;

    // ========================================================================
    // Animation Factory Registration
    // ========================================================================

    /**
     * @brief  Animation factory creator function type.
     *
     * @details Function signature for creating animation factories dynamically.
     *          Used with registerAnimationFactoryType for registering new
     *          animation factory implementations.
     *
     * @since  0.1
     * @ingroup ui_widget_application_support
     */
    using AnimationFactoryMaker =
        std::function<std::unique_ptr<components::ICFAnimationManagerFactory>(const core::ICFTheme&,
                                                                              QObject*)>;

    /**
     * @brief  Register an animation factory type for a theme prefix.
     *
     * @details Associates a theme prefix (e.g., "theme.material") with a
     *          factory creation function. When a theme matching this prefix
     *          is activated, the registered factory is used.
     *
     * @param[in]     themePrefix Theme prefix to match (e.g., "theme.material").
     *                          This matches all themes starting with this prefix
     *                          (e.g., "theme.material.light", "theme.material.dark").
     * @param[in]     maker       Factory function that creates the animation factory.
     *
     * @return        true if registration succeeded, false if prefix already exists.
     *
     * @throws        None
     * @note          The default Material factory is pre-registered.
     * @warning       Register with a unique prefix to avoid conflicts.
     * @since         0.1
     * @ingroup       ui_widget_application_support
     *
     * @code
     * // Register a Fluent-style animation factory
     * Application::registerAnimationFactoryType("theme.fluent",
     *     [](const ICFTheme& theme, QObject* parent) {
     *         return std::make_unique<FluentAnimationFactory>(theme, parent);
     *     });
     *
     * // Now when theme "theme.fluent.dark" is active, the Fluent factory is used
     * app.setTheme("theme.fluent.dark");
     * @endcode
     */
    static bool registerAnimationFactoryType(const std::string& themePrefix,
                                             AnimationFactoryMaker maker);

    /**
     * @brief  Unregister an animation factory type.
     *
     * @details Removes a previously registered animation factory for the
     *          given theme prefix. If a theme with this prefix is currently
     *          active, the factory is replaced with the default.
     *
     * @param[in]     themePrefix Theme prefix to unregister.
     *
     * @throws        None
     * @note          The default Material factory cannot be unregistered.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_application_support
     *
     * @code
     * Application::unregisterAnimationFactoryType("theme.fluent");
     * @endcode
     */
    static void unregisterAnimationFactoryType(const std::string& themePrefix);

  signals:
    /**
     * @brief  Signal emitted when the theme changes.
     *
     * @param[in]     newTheme Reference to the newly activated theme.
     *
     * @note          Forwards ThemeManager::themeChanged signal.
     * @warning       None
     * @since         0.1
     * @ingroup       ui_widget_application_support
     */
    void themeChanged(const core::ICFTheme& newTheme);

    /**
     * @brief  Signal emitted when animation enabled state changes.
     *
     * @param[in]     enabled The new enabled state.
     *
     * @since         0.1
     * @ingroup       ui_widget_application_support
     */
    void animationsEnabledChanged(bool enabled);

  protected:
    /**
     * @brief  Initialize the application.
     *
     * @details Called by constructor after base class setup.
     *          Derived classes can override this to register themes
     *          before calling base init().
     *
     * @note       The default implementation initializes animation factory.
     * @warning    When overriding, always call base init() at the end.
     * @since      0.1
     * @ingroup    ui_widget_application_support
     *
     * @code
     * class MyApplication : public Application {
     * protected:
     *     void init() override {
     *         // Register themes first
     *         registerMyThemes();
     *
     *         // Then call base init
     *         Application::init();
     *     }
     * };
     * @endcode
     */
    virtual void init();

  private:
    /**
     * @brief  Initialize the default animation factory.
     *
     * @details Creates the appropriate animation factory based on current theme.
     */
    void initializeAnimationFactory();

    /**
     * @brief  Register an animation factory (internal).
     *
     * @param  themePrefix Theme prefix to match.
     * @param  maker       Factory function.
     * @return             true if registration succeeded.
     */
    bool registerAnimationFactory(const std::string& themePrefix, AnimationFactoryMaker maker);

    /**
     * @brief  Unregister an animation factory (internal).
     *
     * @param  themePrefix Theme prefix to unregister.
     */
    void unregisterAnimationFactory(const std::string& themePrefix);

    /**
     * @brief  Create animation factory based on theme name.
     *
     * @details Extracts the prefix from themeName and looks up a matching
     *          registered factory. If no match is found, uses the default
     *          Material factory.
     *
     * @param  themeName Full theme name (e.g., "theme.material.light").
     * @param  theme     Reference to the theme.
     * @param  parent    QObject parent.
     * @return           Unique pointer to the created factory.
     */
    std::unique_ptr<components::ICFAnimationManagerFactory>
    createAnimationFactory(const std::string& themeName, const core::ICFTheme& theme,
                           QObject* parent);

    /**
     * @brief  Handle theme change from ThemeManager.
     *
     * @details Updates animation factory to use new theme. The factory is
     *          recreated because it holds a reference to the theme.
     *
     * @param  newTheme The newly activated theme.
     */
    void onThemeManagerChanged(const core::ICFTheme& newTheme);

    /// Animation factory owned by Application (base type for polymorphism)
    std::unique_ptr<components::ICFAnimationManagerFactory> animationFactory_;

    /// Tracks whether init() has been called
    bool initialized_;

    /// Static registry of animation factory makers (shared across all instances)
    static std::unordered_map<std::string, AnimationFactoryMaker>& animationFactoryRegistry();
};

} // namespace cf::ui::widget::application_support
