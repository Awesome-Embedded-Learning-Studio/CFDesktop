/**
 * @file    settings_window.h
 * @brief   Quick-settings window (Phase 12 minimal version).
 *
 * SettingsWindow is a frameless popup (AppLauncher pattern) holding a TabView
 * with three tabs: Wallpaper (switch/interval/duration backed by ConfigStore),
 * Theme (light/dark via ThemeManager), and About (version). The remaining
 * Phase 12 tabs (display/sound/network/bluetooth/input/apps/accessibility) and
 * a font/icon subsystem are deferred until their backends exist.
 *
 * @author  CFDesktop Team
 * @date    2026-07-10
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup settings
 */

#pragma once

#include <QColor>
#include <QRect>
#include <QWidget>

class QKeyEvent;
class QPaintEvent;

namespace qw::components::material {
class CFMaterialFadeAnimation;
class CFMaterialSlideAnimation;
} // namespace qw::components::material

namespace cf::desktop::desktop_component {

/**
 * @brief  Tabbed quick-settings popup.
 *
 * @ingroup settings
 */
class SettingsWindow : public QWidget {
    Q_OBJECT

  public:
    /**
     * @brief  Constructs the window (hidden until popup()).
     *
     * @param[in]  parent  Owning widget (the desktop surface).
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    settings
     */
    explicit SettingsWindow(QWidget* parent = nullptr);

    /**
     * @brief  Destructs the window.
     *
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    settings
     */
    ~SettingsWindow() override;

    /**
     * @brief  Centers and shows the window.
     *
     * @param[in]  available  The free screen geometry.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    settings
     */
    void popup(const QRect& available);

    /**
     * @brief  Starts the exit animation; hides when it finishes.
     *
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    settings
     */
    void hidePanel();

    /**
     * @brief  Reports whether the window is currently visible.
     *
     * @return     True when shown.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    settings
     */
    bool isShowing() const noexcept;

  protected:
    /**
     * @brief  Paints the rounded Material surface background.
     *
     * @param[in]  event  The paint event descriptor.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    settings
     */
    void paintEvent(QPaintEvent* event) override;

    /**
     * @brief  Hides the window on Escape.
     *
     * @param[in]  event  The key event descriptor.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    settings
     */
    void keyPressEvent(QKeyEvent* event) override;

  private:
    /// @brief Builds the tab view with wallpaper/theme/about tabs.
    void setupUi();
    /// @brief Creates the MD3 enter/exit fade + slide animations.
    void setupAnimations();
    /// @brief Resolves theme colors, then repaints.
    void applyTheme();
    /// @brief Builds the wallpaper tab (switch/interval/duration -> ConfigStore).
    QWidget* buildWallpaperTab();
    /// @brief Builds the theme tab (light/dark -> ThemeManager).
    QWidget* buildThemeTab();
    /// @brief Builds the about tab (version + repo link).
    QWidget* buildAboutTab();

    qw::components::material::CFMaterialFadeAnimation* enter_fade_{nullptr};
    qw::components::material::CFMaterialSlideAnimation* enter_slide_{nullptr};
    qw::components::material::CFMaterialFadeAnimation* exit_fade_{nullptr};
    qw::components::material::CFMaterialSlideAnimation* exit_slide_{nullptr};

    /// @brief Window surface fill (surface).
    QColor surface_color_;
};

} // namespace cf::desktop::desktop_component
