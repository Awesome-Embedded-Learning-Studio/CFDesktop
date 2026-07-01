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

class QGraphicsOpacityEffect;
class QGridLayout;
class QKeyEvent;
class QPaintEvent;
class QRect;
class QVariantAnimation;

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
    /// @brief Creates the opacity effect and the enter/exit animations.
    void setupAnimations();
    /// @brief Applies an animation progress value to the popup: t=0 is fully
    ///        hidden and offset downward, t=1 is at rest (opaque, anchored).
    /// @param[in] t  Progress in [0.0, 1.0].
    void applyAnimProgress(qreal t);

    qw::widget::material::TextField* search_edit_{
        nullptr};                ///< Filter box (ownership: this widget).
    QGridLayout* grid_{nullptr}; ///< Tile grid (ownership: grid container).
    QList<LauncherTile*> tiles_; ///< Current tiles. Ownership: Qt parented.
    QList<AppEntry> apps_;       ///< Backing application list.

    QGraphicsOpacityEffect* opacity_effect_{nullptr}; ///< Whole-popup fade (owned by this).
    QVariantAnimation* enter_anim_{nullptr};          ///< Enter: 0.0 -> 1.0 (fade in + slide up).
    QVariantAnimation* exit_anim_{nullptr};           ///< Exit: 1.0 -> 0.0 (fade out + slide down).
    QPoint rest_pos_;                                 ///< Final anchored position (set in popup()).

    QColor surface_color_; ///< Popup background fill (surface).
    QColor outline_color_; ///< Reserved for future border (outline variant).

    /// Weak pointer factory (must be the last member).
    mutable aex::WeakPtrFactory<AppLauncher> weak_factory_{this};
};

} // namespace cf::desktop::desktop_component
