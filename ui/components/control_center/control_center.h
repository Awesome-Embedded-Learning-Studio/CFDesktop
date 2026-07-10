/**
 * @file    control_center.h
 * @brief   Slide-in quick-settings panel (brightness, volume, toggles, theme).
 *
 * ControlCenter is a frameless popup anchored near the top-right that bundles
 * the quick controls: brightness and volume sliders, Wi-Fi / Bluetooth / DND
 * switches, and a theme toggle. It follows the AppLauncher frameless-child
 * pattern (no Qt::Popup, so it does not fight the window manager on windowless
 * targets). Sliders and the Wi-Fi/Bluetooth toggles are UI-only in this phase
 * (real hardware backends arrive with the aels-power / aels-network repos);
 * the DND switch is wired to the ConfigStore via NotificationService, and the
 * theme toggle switches the active Material theme for real.
 *
 * @author  CFDesktop Team
 * @date    2026-07-10
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup control_center
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
namespace qw::widget::material {
class Button;
class Slider;
class Switch;
} // namespace qw::widget::material

namespace cf::desktop::desktop_component {

/**
 * @brief  Quick-settings popup (MS6 minimal version).
 *
 * Frameless child widget animated with the QuarkWidgets MD3 fade + slide
 * pair. Hardware-backed brightness/volume/Wi-Fi/Bluetooth are deferred; DND
 * and theme switching are live.
 *
 * @ingroup control_center
 */
class ControlCenter : public QWidget {
    Q_OBJECT

  public:
    /**
     * @brief  Constructs the panel (hidden until popup()).
     *
     * @param[in]  parent  Owning widget (the desktop surface).
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    control_center
     */
    explicit ControlCenter(QWidget* parent = nullptr);

    /**
     * @brief  Destructs the panel.
     *
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    control_center
     */
    ~ControlCenter() override;

    /**
     * @brief  Positions (top-right) and shows the panel.
     *
     * @param[in]  available  The free screen geometry (excludes docked panels).
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    control_center
     */
    void popup(const QRect& available);

    /**
     * @brief  Starts the exit animation; hides when it finishes.
     *
     * @throws     None
     * @note       No-op when already hidden.
     * @warning    None
     * @since      0.19.0
     * @ingroup    control_center
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
     * @ingroup    control_center
     */
    bool isShowing() const noexcept;

  protected:
    /**
     * @brief  Paints the rounded Material surface background.
     *
     * @param[in]  event  The paint event descriptor.
     * @throws     None
     * @note       Theme colors resolve in applyTheme().
     * @warning    None
     * @since      0.19.0
     * @ingroup    control_center
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
     * @ingroup    control_center
     */
    void keyPressEvent(QKeyEvent* event) override;

  private:
    /// @brief Builds the slider / switch / button layout.
    void setupUi();
    /// @brief Creates the MD3 enter/exit fade + slide animations.
    void setupAnimations();
    /// @brief Resolves theme colors, then repaints.
    void applyTheme();

    /// @brief Brightness slider (UI-only this phase).
    qw::widget::material::Slider* brightness_{nullptr};
    /// @brief Volume slider (UI-only this phase).
    qw::widget::material::Slider* volume_{nullptr};
    /// @brief Wi-Fi toggle (UI-only this phase).
    qw::widget::material::Switch* wifi_{nullptr};
    /// @brief Bluetooth toggle (UI-only this phase).
    qw::widget::material::Switch* bluetooth_{nullptr};
    /// @brief Do-Not-Disturb toggle (wired to ConfigStore via NotificationService).
    qw::widget::material::Switch* dnd_{nullptr};
    /// @brief Theme toggle button (switches the active Material theme).
    qw::widget::material::Button* theme_btn_{nullptr};
    /// @brief Screenshot placeholder button.
    qw::widget::material::Button* screenshot_btn_{nullptr};
    /// @brief Test stub: posts a "Hello World" notification (end-to-end check).
    qw::widget::material::Button* test_notif_btn_{nullptr};

    qw::components::material::CFMaterialFadeAnimation* enter_fade_{nullptr};
    qw::components::material::CFMaterialSlideAnimation* enter_slide_{nullptr};
    qw::components::material::CFMaterialFadeAnimation* exit_fade_{nullptr};
    qw::components::material::CFMaterialSlideAnimation* exit_slide_{nullptr};

    /// @brief Panel surface fill (surface).
    QColor surface_color_;
};

} // namespace cf::desktop::desktop_component
