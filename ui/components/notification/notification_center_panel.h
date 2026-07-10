/**
 * @file    notification_center_panel.h
 * @brief   Slide-in notification center panel listing active notifications.
 *
 * NotificationCenterPanel is a frameless popup anchored to the right edge
 * that lists every active notification as a stack of cards. It subscribes to
 * NotificationService and rebuilds the list on every posted / dismissed /
 * cleared signal, so it stays live while open. A header "clear all" hotspot
 * dismisses the whole list.
 *
 * @author  CFDesktop Team
 * @date    2026-07-10
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup notification
 */

#pragma once

#include <QColor>
#include <QRect>
#include <QWidget>

class QKeyEvent;
class QMouseEvent;
class QPaintEvent;
class QScrollArea;
class QVBoxLayout;

namespace qw::components::material {
class CFMaterialFadeAnimation;
class CFMaterialSlideAnimation;
} // namespace qw::components::material

namespace cf::desktop::desktop_component {

/**
 * @brief  Slide-in panel listing all active notifications.
 *
 * Frameless child widget animated with the QuarkWidgets MD3 fade + slide
 * pair. Reads its contents from NotificationService::all() and rebuilds on
 * every service change while visible.
 *
 * @ingroup notification
 */
class NotificationCenterPanel : public QWidget {
    Q_OBJECT

  public:
    /**
     * @brief  Constructs the panel (hidden until popup()).
     *
     * @param[in]  parent  Owning widget (the desktop surface).
     * @throws     None
     * @note       Subscribes to NotificationService immediately.
     * @warning    None
     * @since      0.19.0
     * @ingroup    notification
     */
    explicit NotificationCenterPanel(QWidget* parent = nullptr);

    /**
     * @brief  Destructs the panel.
     *
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    notification
     */
    ~NotificationCenterPanel() override;

    /**
     * @brief  Positions (right edge) and shows the panel, then rebuilds.
     *
     * @param[in]  available  The free screen geometry (excludes docked panels).
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    notification
     */
    void popup(const QRect& available);

    /**
     * @brief  Starts the exit animation; hides when it finishes.
     *
     * @throws     None
     * @note       No-op when already hidden.
     * @warning    None
     * @since      0.19.0
     * @ingroup    notification
     */
    void hidePanel();

    /**
     * @brief  Reports whether the panel is currently visible.
     *
     * @return     True when the panel is shown.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    notification
     */
    bool isShowing() const noexcept;

  protected:
    /**
     * @brief  Paints the rounded surface, header title, and clear-all hotspot.
     *
     * @param[in]  event  The paint event descriptor.
     * @throws     None
     * @note       Theme colors resolve in applyTheme().
     * @warning    None
     * @since      0.19.0
     * @ingroup    notification
     */
    void paintEvent(QPaintEvent* event) override;

    /**
     * @brief  Hides the panel on Escape.
     *
     * @param[in]  event  The key event descriptor.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    notification
     */
    void keyPressEvent(QKeyEvent* event) override;

    /**
     * @brief  Records a press on the clear-all hotspot.
     *
     * @param[in]  event  The mouse event descriptor.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    notification
     */
    void mousePressEvent(QMouseEvent* event) override;

    /**
     * @brief  Activates clear-all when the press releases on it.
     *
     * @param[in]  event  The mouse event descriptor.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    notification
     */
    void mouseReleaseEvent(QMouseEvent* event) override;

  private:
    /// @brief Builds the header + scroll area layout.
    void setupUi();
    /// @brief Creates the MD3 enter/exit fade + slide animations.
    void setupAnimations();
    /// @brief Resolves theme colors, then repaints.
    void applyTheme();
    /// @brief Rebuilds the card list from NotificationService::all().
    void rebuildList();

    /// @brief Scroll area hosting the notification cards.
    QScrollArea* scroll_{nullptr};
    /// @brief Container widget holding the card QVBoxLayout.
    QWidget* list_container_{nullptr};
    /// @brief Layout stacking one card per active notification.
    QVBoxLayout* list_layout_{nullptr};

    /// @brief Clear-all hotspot geometry, in widget coords.
    QRect clear_all_rect_;
    /// @brief True while a press is active inside the clear-all hotspot.
    bool pressed_on_clear_{false};

    qw::components::material::CFMaterialFadeAnimation* enter_fade_{nullptr};
    qw::components::material::CFMaterialSlideAnimation* enter_slide_{nullptr};
    qw::components::material::CFMaterialFadeAnimation* exit_fade_{nullptr};
    qw::components::material::CFMaterialSlideAnimation* exit_slide_{nullptr};

    /// @brief Panel surface fill (surface).
    QColor surface_color_;
    /// @brief Header title color (onSurface).
    QColor title_color_;
    /// @brief Clear-all glyph color (primary).
    QColor clear_color_;
    /// @brief Empty-state hint color (onSurfaceVariant).
    QColor hint_color_;
};

} // namespace cf::desktop::desktop_component
