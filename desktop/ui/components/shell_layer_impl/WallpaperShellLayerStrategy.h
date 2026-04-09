/**
 * @file    WallpaperShellLayerStrategy.h
 * @brief   Shell layer strategy that renders a wallpaper background.
 *
 * WallpaperShellLayerStrategy composes a WallPaperLayer and provides
 * pre-scaled QImage data to the shell layer's paintEvent. It handles
 * image scaling on geometry changes and triggers repaints when the
 * wallpaper image changes.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-04-09
 * @version 0.1
 * @since   0.15
 * @ingroup components
 */

#pragma once
#include "../IShellLayerStrategy.h"
#include <QColor>
#include <QImage>
#include <QRect>
#include <memory>

namespace cf::desktop {

class IShellLayer;
class WindowManager;

namespace wallpaper {
class WallPaperLayer;
}

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
    void activate(WeakPtr<IShellLayer> layer, WeakPtr<WindowManager> wm) override;

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
     * Reloads the raw image, re-scales it, and requests a repaint.
     */
    void onWallpaperChanged();

    struct Private;
    std::unique_ptr<Private> d;
};

} // namespace cf::desktop
