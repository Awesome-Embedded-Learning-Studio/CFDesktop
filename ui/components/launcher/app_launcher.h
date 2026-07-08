/**
 * @file    app_launcher.h
 * @brief   Start-menu style application launcher popup.
 *
 * AppLauncher is a frameless popup that shows a centered grid of LauncherTile
 * entries built from an AppEntry list. Opening the popup (popup()) sizes and
 * positions it above the taskbar; a tile click emits appLaunched(app_id) and
 * closes the popup. ESC and an outside click (Qt::Popup) also close it. All
 * colors follow the active Material theme.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date 2026-06-26
 * @version 0.1
 * @since   0.20
 * @ingroup components
 */

#pragma once

#include "aex/weak_ptr/weak_ptr.h"
#include "aex/weak_ptr/weak_ptr_factory.h"
#include "app_entry.h"

#include <QColor>
#include <QList>
#include <QString>
#include <QWidget>

class QGridLayout;
class QKeyEvent;
class QPaintEvent;
class QRect;

namespace qw::components::material {
class CFMaterialFadeAnimation;
class CFMaterialSlideAnimation;
} // namespace qw::components::material
namespace qw::widget::material {
class TextField;
}

namespace cf::desktop::desktop_component {

class LauncherTile;

/**
 * @brief  Frameless Start-menu style application launcher popup.
 *
 * Renders a rounded Material surface holding a grid of application tiles.
 * popup() anchors it centered above the taskbar; tile clicks are forwarded as
 * appLaunched(); ESC and outside clicks dismiss it.
 *
 * @ingroup components
 */
class AppLauncher final : public QWidget {
    Q_OBJECT
  public:
    /**
     * @brief  Constructs the launcher popup.
     *
     * @param[in] parent  Owning widget (the desktop surface).
     *
     * @throws None
     * @note   Configures Qt::Popup flags and resolves the theme.
     * @warning None
     * @since  0.20
     * @ingroup components
     */
    explicit AppLauncher(QWidget* parent = nullptr);

    /**
     * @brief  Destructs the launcher.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.20
     * @ingroup components
     */
    ~AppLauncher() override;

    /**
     * @brief  Rebuilds the tile grid from an application list.
     *
     * @param[in] apps  The applications to show in the grid.
     *
     * @throws None
     * @note   Replaces any previously shown tiles.
     * @warning None
     * @since  0.20
     * @ingroup components
     */
    void setApps(const QList<AppEntry>& apps);

    /**
     * @brief  Sizes, positions (centered, bottom-aligned to the available
     *         area so it sits above the taskbar), and shows the popup.
     *
     * @param[in] available  The free screen geometry (excludes docked panels).
     *
     * @throws None
     * @note   A null/empty rect centers on the available origin.
     * @warning None
     * @since  0.20
     * @ingroup components
     */
    void popup(const QRect& available);

    /**
     * @brief  Hides the popup.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.20
     * @ingroup components
     */
    void hideLauncher();

    /**
     * @brief  Reports whether the popup is currently shown.
     *
     * @return true when the popup is visible.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.20
     * @ingroup components
     */
    bool isShowing() const noexcept;

    /**
     * @brief  Returns a weak reference to this launcher.
     *
     * @return aex::WeakPtr valid for this instance's lifetime.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.20
     * @ingroup components
     */
    aex::WeakPtr<AppLauncher> GetWeak() const { return weak_factory_.GetWeakPtr(); }

  signals:
    /**
     * @brief  Emitted when a tile is clicked, before the popup closes.
     *
     * @param[in] app_id  The clicked application identifier.
     *
     * @since 0.20
     * @ingroup components
     */
    void appLaunched(const QString& app_id);

    /**
     * @brief  Emitted when a tile is long-pressed (pin the app to the desktop).
     *
     * @param[in] app_id  The application to add to the desktop.
     *
     * @since 0.20
     * @ingroup components
     */
    void addToDesktopRequested(const QString& app_id);

  protected:
    /**
     * @brief  Paints the rounded Material surface background.
     *
     * @param[in] event  The paint event descriptor.
     *
     * @throws None
     * @note   Theme values are resolved in applyTheme().
     * @warning None
     * @since  0.20
     * @ingroup components
     */
    void paintEvent(QPaintEvent* event) override;

    /**
     * @brief  Closes the popup on Escape.
     *
     * @param[in] event  The key event descriptor.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.20
     * @ingroup components
     */
    void keyPressEvent(QKeyEvent* event) override;

  private:
    /// @brief Creates the grid + search box layout.
    void setupUi();
    /// @brief Resolves theme colors, then repaints.
    void applyTheme();
    /// @brief Rebuilds the tile grid from apps_.
    void rebuildGrid();
    /// @brief Creates the four MD3 enter/exit animations (fade + slide each),
    /// bound to motion tokens so duration + easing resolve from the theme.
    void setupAnimations();

    qw::widget::material::TextField* search_edit_{
        nullptr};                ///< Filter box (ownership: this widget).
    QGridLayout* grid_{nullptr}; ///< Tile grid (ownership: grid container).
    QList<LauncherTile*> tiles_; ///< Current tiles. Ownership: Qt parented.
    QList<AppEntry> apps_;       ///< Backing application list.

    qw::components::material::CFMaterialFadeAnimation* enter_fade_{
        nullptr}; ///< Enter fade in (0->1, shortEnter).
    qw::components::material::CFMaterialSlideAnimation* enter_slide_{
        nullptr}; ///< Enter slide up (-px->0, mediumEnter).
    qw::components::material::CFMaterialFadeAnimation* exit_fade_{
        nullptr}; ///< Exit fade out (1->0, shortExit); finished -> hide.
    qw::components::material::CFMaterialSlideAnimation* exit_slide_{
        nullptr};     ///< Exit slide down (0->+px, mediumExit).
    QPoint rest_pos_; ///< Final anchored position (set in popup()).

    QColor surface_color_; ///< Popup background fill (surface).
    QColor outline_color_; ///< Reserved for future border (outline variant).

    /// Weak pointer factory (must be the last member).
    mutable aex::WeakPtrFactory<AppLauncher> weak_factory_{this};
};

} // namespace cf::desktop::desktop_component
