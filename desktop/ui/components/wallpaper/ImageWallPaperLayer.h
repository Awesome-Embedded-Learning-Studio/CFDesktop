/**
 * @file    ImageWallPaperLayer.h
 * @brief   QImage-based wallpaper layer implementation.
 *
 * ImageWallPaperLayer loads wallpaper images from WallPaperToken paths
 * and provides them as QImage data. It does not perform any rendering,
 * making it usable by both QPainter and RHI-based backends.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-04-09
 * @version 0.1
 * @since   0.15
 * @ingroup wallpaper
 */

#pragma once
#include "WallPaperLayer.h"
#include <QColor>
#include <QImage>
#include <memory>

namespace cf::desktop::wallpaper {

/**
 * @brief  QImage-based concrete wallpaper layer.
 *
 * Manages a collection of wallpaper tokens via WallPaperAccessStorage
 * and loads images from file paths. Image loading is synchronous for v1;
 * async loading can be layered on later without changing the interface.
 *
 * @ingroup wallpaper
 */
class ImageWallPaperLayer : public WallPaperLayer {
  public:
    /**
     * @brief  Constructs an ImageWallPaperLayer with default settings.
     *
     * @param[in] mode  Scaling mode (default: Fill).
     * @param[in] bg    Background color for letterbox bars (default: dark surface).
     */
    explicit ImageWallPaperLayer(ScalingMode mode = ScalingMode::Fill,
                                 const QColor& bg = QColor(0x1c, 0x1b, 0x1f));
    ~ImageWallPaperLayer() override;

    void setTokenStorage(std::unique_ptr<WallPaperAccessStorage> storage) override;

    /**
     * @brief  Switches to the next wallpaper in the collection.
     *
     * @return  True if the switch succeeded, false if at the end or
     *          no storage.
     *
     * @throws           None.
     *
     * @note             None.
     * @warning          None.
     * @since            0.15
     * @ingroup          wallpaper
     */
    bool showNextOne() override;

    /**
     * @brief  Switches to the previous wallpaper in the collection.
     *
     * @return  True if the switch succeeded, false if at the
     *          beginning or no storage.
     *
     * @throws           None.
     *
     * @note             None.
     * @warning          None.
     * @since            0.15
     * @ingroup          wallpaper
     */
    bool showPrevOne() override;

    /**
     * @brief  Switches to a specific wallpaper by token ID.
     *
     * @param[in] token  The token ID to switch to.
     *
     * @return  True if the switch succeeded, false if token not found.
     *
     * @throws           None.
     *
     * @note             None.
     * @warning          None.
     * @since            0.15
     * @ingroup          wallpaper
     */
    bool showTargetOne(const wallpaper_token_id_t& token) override;

    /**
     * @brief  Returns the current wallpaper image.
     *
     * QImage uses implicit sharing (COW), so returning by value is
     * cheap.
     *
     * @return  Current image, or null QImage if no wallpaper is loaded.
     *
     * @throws           None.
     *
     * @note             None.
     * @warning          None.
     * @since            0.15
     * @ingroup          wallpaper
     */
    QImage currentImage() const override;

    /**
     * @brief  Returns the current scaling mode.
     *
     * @return  The scaling mode for wallpaper rendering.
     *
     * @throws           None.
     *
     * @note             None.
     * @warning          None.
     * @since            0.15
     * @ingroup          wallpaper
     */
    ScalingMode scalingMode() const override;

    /**
     * @brief  Returns the background fill color.
     *
     * Used for letterbox bars or as fallback when no image is available.
     *
     * @return  Background color.
     *
     * @throws           None.
     *
     * @note             None.
     * @warning          None.
     * @since            0.15
     * @ingroup          wallpaper
     */
    QColor backgroundColor() const override;

    /**
     * @brief  Sets the scaling mode for wallpaper rendering.
     *
     * @param[in] mode  The new scaling mode.
     */
    void setScalingMode(ScalingMode mode);

    /**
     * @brief  Sets the background fill color.
     *
     * @param[in] color  The new background color.
     */
    void setBackgroundColor(const QColor& color);

  private:
    /**
     * @brief  Loads the image from the given token's source path.
     *
     * @param[in] token  Weak reference to the wallpaper token.
     * @return True if the image was loaded successfully.
     */
    bool loadImageFromToken(const WeakPtr<WallPaperToken>& token);

    struct Private;
    std::unique_ptr<Private> d;
};

} // namespace cf::desktop::wallpaper
