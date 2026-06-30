/**
 * @file    WallpaperShellLayerStrategy.h
 * @brief   Shell layer strategy that renders a wallpaper background.
 *
 * WallpaperShellLayerStrategy composes a WallPaperLayer and provides
 * pre-scaled QImage data to the shell layer's paintEvent. It handles
 * image scaling on geometry changes and triggers repaints when the
 * wallpaper image changes.
 *
 * A WallPaperEngine drives timed rotation. When a switch is due the
 * strategy runs a per-frame transition: the outgoing frame is captured,
 * the layer advances, and a QVariantAnimation blends old and new images
 * into cached_scaled_image via composeTransitionFrame() until the new
 * image is fully revealed.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-04-09
 * @version 0.1
 * @since   0.15
 * @ingroup components
 */

#pragma once
#include "../IShellLayerStrategy.h"
#include "../wallpaper/TransitionComposer.h"
#include <QColor>
#include <QImage>
#include <QRect>
#include <memory>

namespace cf::desktop {

class IShellLayer;
class WindowManager;

namespace wallpaper {
class WallPaperLayer;
class WallPaperEngine;
} // namespace wallpaper

/**
 * @brief  Shell layer strategy with wallpaper background support.
 *
 * Holds a WallPaperLayer instance and provides pre-scaled QImage
 * data through the IShellLayerStrategy interface. The shell layer
 * queries currentBackgroundImage() in its paintEvent and renders
 * with its own backend (QPainter for QWidget, RHI for Wayland).
 *
 * Falls back to a solid color when no wallpaper image is available.
 *
 * @ingroup components
 */
class WallpaperShellLayerStrategy : public IShellLayerStrategy {
  public:
    /**
     * @brief  Constructs a wallpaper strategy with the given layer.
     *
     * @param[in] wallpaper_layer  The wallpaper layer to render. Ownership transfers.
     */
    explicit WallpaperShellLayerStrategy(
        std::unique_ptr<wallpaper::WallPaperLayer> wallpaper_layer);
    ~WallpaperShellLayerStrategy() override;

    /**
     * @brief  Activates the strategy for the given shell layer.
     *
     * @param[in] layer  Weak reference to the shell layer.
     * @param[in] wm     Weak reference to the window manager.
     *
     * @throws           None.
     *
     * @note             None.
     * @warning          None.
     * @since            0.15
     * @ingroup          components
     */
    void activate(aex::WeakPtr<IShellLayer> layer, aex::WeakPtr<WindowManager> wm) override;

    /**
     * @brief  Deactivates the strategy and releases resources.
     *
     * @throws           None.
     *
     * @note             None.
     * @warning          None.
     * @since            0.15
     * @ingroup          components
     */
    void deactivate() override;

    /**
     * @brief  Handles geometry changes by re-scaling the wallpaper.
     *
     * @param[in] available  The newly available geometry rectangle.
     *
     * @throws               None.
     *
     * @note             None.
     * @warning          None.
     * @since            0.15
     * @ingroup          components
     */
    void onGeometryChanged(const QRect& available) override;

    /**
     * @brief  Returns the current pre-scaled background image.
     *
     * @return  The current wallpaper image, or null QImage if none.
     *
     * @throws           None.
     *
     * @note             None.
     * @warning          None.
     * @since            0.15
     * @ingroup          components
     */
    QImage currentBackgroundImage() const override;

    /**
     * @brief  Returns the fallback solid background color.
     *
     * @return  The background color used when no wallpaper is set.
     *
     * @throws           None.
     *
     * @note             None.
     * @warning          None.
     * @since            0.15
     * @ingroup          components
     */
    QColor backgroundColor() const override;

    /**
     * @brief  Manually triggers a transition to the next wallpaper.
     *
     * Uses the engine's configured switching mode and selector. Provided
     * as the future hook for a wallpaper chooser UI; the same path is
     * used by automatic timed rotation.
     *
     * @throws           None.
     *
     * @note             None.
     * @warning          None.
     * @since            0.19
     * @ingroup          components
     */
    void triggerNextWallpaper();

  private:
    /**
     * @brief  Re-scales the raw image to fit the current geometry.
     *
     * Uses the scaling mode from the wallpaper layer.
     */
    void rescaleImage();

    /**
     * @brief  Callback invoked when the wallpaper layer's image changes.
     *
     * Reloads the raw image, re-scales it, and requests a repaint. During
     * an active transition it instead captures the new frame and starts
     * the per-frame animation.
     */
    void onWallpaperChanged();

    /**
     * @brief  Requests a repaint from the bound shell layer (no-op if unset).
     */
    void requestLayerRepaint();

    /**
     * @brief  Begins a transition to a new wallpaper.
     *
     * Captures the current frame as the outgoing image, arms the
     * transition state, and advances the layer to the next wallpaper.
     *
     * @param[in] mode  Compositing mode for this transition.
     */
    void beginTransition(wallpaper::SwitchingMode mode);

    /**
     * @brief  Advances the layer to the next wallpaper per the selector.
     *
     * @return True if the layer switched (and thus fired onWallpaperChanged);
     *         false if there was nothing to switch to.
     */
    bool advanceLayerToNext();

    /**
     * @brief  Builds and starts the per-frame transition animation.
     */
    void startTransitionAnim();

    /**
     * @brief  Composes and publishes one transition frame at @p progress.
     *
     * @param[in] progress  Transition progress in [0, 1].
     */
    void composeFrame(qreal progress);

    /**
     * @brief  Finalizes a transition: settles on the new frame, clears state.
     */
    void finishTransition();

    /**
     * @brief  Aborts any in-flight transition and clears buffers.
     *
     * Used by deactivate() and on geometry changes mid-transition.
     */
    void resetTransition();

    struct Private;
    std::unique_ptr<Private> d;
};

} // namespace cf::desktop
