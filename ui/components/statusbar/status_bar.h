/**
 * @file    status_bar.h
 * @brief   Concrete status bar panel.
 *
 * StatusBar is the top-edge panel implementation behind IStatusBar. It
 * renders the clock and a row of system-icon glyphs, follows the active
 * Material theme, and updates the clock once per second.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-15
 * @version 0.1
 * @since   0.19
 * @ingroup components
 */

#pragma once

#include "IStatusBar.h"
#include "aex/weak_ptr/weak_ptr.h"
#include "aex/weak_ptr/weak_ptr_factory.h"
#include "components/IShellLayer.h"
#include "components/frosted_backdrop/frosted_backdrop.h"

#include <QColor>
#include <QFont>
#include <QPixmap>
#include <QWidget>

class QTimer;

namespace cf::desktop::desktop_component {

/**
 * @brief  QWidget-based status bar.
 *
 * Renders the clock and system-icon glyphs directly in paintEvent so the
 * active Material theme fully controls colors and typography. A one-second
 * timer keeps the clock current.
 *
 * @ingroup components
 */
class StatusBar final : public QWidget, public IStatusBar {
    Q_OBJECT
  public:
    /**
     * @brief  Constructs the status bar.
     *
     * @param[in] parent  Owning widget (typically the desktop surface).
     *
     * @throws None
     * @note   Starts the clock timer and applies the current theme.
     * @warning None
     * @since  0.19
     * @ingroup components
     */
    explicit StatusBar(QWidget* parent = nullptr);

    /**
     * @brief  Destructs the status bar.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.19
     * @ingroup components
     */
    ~StatusBar() override;

    // -- IPanel ---------------------------------------------------------------
    /**
     * @brief  Returns the anchoring edge.
     *
     * @return Always PanelPosition::Top.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.19
     * @ingroup components
     */
    PanelPosition position() const override;

    /**
     * @brief  Returns the layout priority.
     *
     * @return Priority value (outermost on the edge).
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.19
     * @ingroup components
     */
    int priority() const override;

    /**
     * @brief  Returns the bar thickness in device pixels.
     *
     * @return Preferred height in device pixels.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.19
     * @ingroup components
     */
    int preferredSize() const override;

    /**
     * @brief  Returns the widget positioned by the layout engine.
     *
     * @return This status bar widget.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.19
     * @ingroup components
     */
    QWidget* widget() const override;

    // -- IStatusBar -----------------------------------------------------------
    /**
     * @brief  Shows or hides the clock.
     *
     * @param[in] visible  true to show the clock, false to hide it.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.19
     * @ingroup components
     */
    void setTimeVisible(bool visible) override;

    /**
     * @brief  Reports clock visibility.
     *
     * @return true when the clock is visible.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.19
     * @ingroup components
     */
    bool isTimeVisible() const override;

    /**
     * @brief  Shows or hides the system-icon cluster.
     *
     * @param[in] visible  true to show icons, false to hide them.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.19
     * @ingroup components
     */
    void setIconsVisible(bool visible) override;

    /**
     * @brief  Reports icon-cluster visibility.
     *
     * @return true when system icons are visible.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.19
     * @ingroup components
     */
    bool isIconsVisible() const override;

    /**
     * @brief  Selects the clock/icon layout variant.
     *
     * @param[in] style  The layout style to apply.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.19
     * @ingroup components
     */
    void setStyle(StatusBarStyle style) override;

    /**
     * @brief  Returns the active layout variant.
     *
     * @return The current status bar style.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.19
     * @ingroup components
     */
    StatusBarStyle style() const override;

    /**
     * @brief  Returns a weak reference to this status bar.
     *
     * @return aex::WeakPtr valid for this instance's lifetime.
     *
     * @throws None
     * @note   None
     * @warning None
     * @since  0.19
     * @ingroup components
     */
    aex::WeakPtr<StatusBar> GetWeak() const { return weak_factory_.GetWeakPtr(); }

    /**
     * @brief  Sets the backdrop source for the frosted-glass surface.
     *
     * @param[in] source  Shell layer supplying the wallpaper image (non-owning).
     *
     * @throws None
     * @note   @p source must outlive this status bar.
     * @since  0.20
     */
    void setBackdropSource(cf::desktop::IShellLayer* source);

  protected:
    /**
     * @brief  Paints the background, clock, and icon glyphs.
     *
     * @param[in] event  The paint event descriptor.
     *
     * @throws None
     * @note   Theme colors and fonts are resolved in applyTheme().
     * @warning None
     * @since  0.19
     * @ingroup components
     */
    void paintEvent(QPaintEvent* event) override;

    /**
     * @brief  Coalesces geometry changes into a debounced backdrop reblur.
     *
     * @param[in] event  The resize event descriptor.
     *
     * @throws None
     * @since  0.20
     */
    void resizeEvent(QResizeEvent* event) override;

  private slots:
    /// @brief Refreshes the clock text each second.
    void onTimeout();

  private:
    /// @brief Wires signals and starts the clock timer.
    void setupUi();
    /// @brief Resolves theme colors and typography, then repaints.
    void applyTheme();
    /// @brief Returns the formatted current time string (HH:mm).
    QString currentTimeText() const;
    /// @brief Returns the formatted current date string (yyyy/MM/dd).
    QString currentDateText() const;
    /// @brief Plays the boot fade-in (opacity 0 -> 1).
    void startFadeIn();
    /// @brief Loads monochrome icon masks from compiled resources (qrc).
    /// @note  Masks stay null when the resources are absent; paintEvent then
    ///        falls back to the hand-drawn vector glyphs.
    void loadIconMasks();
    /// @brief Recolors a monochrome mask to a target color, preserving alpha.
    /// @param[in] mask   Single-color icon on a transparent background.
    /// @param[in] color  Target fill color.
    /// @return           Tinted pixmap matching the mask's alpha shape.
    static QPixmap tintedPixmap(const QPixmap& mask, const QColor& color);

    /// One-second clock timer. Ownership: this widget (Qt parent).
    QTimer* timer_{nullptr};
    bool time_visible_{true};
    bool icons_visible_{true};
    StatusBarStyle style_{StatusBarStyle::Split};
    QString cached_time_;
    QString cached_date_;

    // Resolved theme values (refreshed by applyTheme()).
    QColor background_color_;
    QColor surface_top_color_; ///< HCT tone-lifted surface, for the top of the gradient.
    QColor foreground_color_;
    QColor icon_color_;
    QColor divider_color_;
    QFont clock_font_;

    /// Boot fade-in opacity in [0, 1]; applied as painter opacity.
    qreal fade_opacity_{0.0};

    /// Cached monochrome icon masks (index = StatusIcon). Null entries fall
    /// back to vector drawing in paintEvent.
    QPixmap icon_masks_[4];

    /// Shell layer backing the frosted surface (non-owning; may be null).
    cf::desktop::IShellLayer* backdrop_source_{nullptr};
    /// Frosted-glass renderer with its own cache (one instance per bar).
    cf::desktop::FrostedBackdrop frosted_;
    /// Resolved frosted parameters; the tint tracks the active theme surface.
    cf::desktop::FrostedParams frosted_params_{};
    /// Coalesces rapid resizes into a single backdrop reblur. Ownership: this.
    QTimer* reblur_debounce_{nullptr};
    /// Guards the null-backdrop warning so it fires once per transition.
    bool backdrop_null_warned_{false};

    /// Weak pointer factory (must be the last member).
    mutable aex::WeakPtrFactory<StatusBar> weak_factory_{this};
};

} // namespace cf::desktop::desktop_component
