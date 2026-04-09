/**
 * @file    desktop/ui/components/wallpaper/WallPaperLayer.h
 * @brief   Abstract wallpaper layer interface.
 *
 * Defines the WallPaperLayer base class that manages wallpaper state
 * (current token, image data, scaling mode) without performing rendering.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-04-09
 * @version 0.1
 * @since   0.15
 * @ingroup wallpaper
 */

#pragma once
#include "WallPaperAccessStorage.h"
#include "wallpaper/WallPaperToken.h"
#include <QColor>
#include <QImage>
#include <functional>
#include <memory>

namespace cf::desktop::wallpaper {

/**
 * @brief  Scaling mode for wallpaper rendering.
 *
 * @ingroup wallpaper
 */
enum class ScalingMode {
    Fill,   ///< Cover entire area, crop excess (default).
    Fit,    ///< Contain within area, letterbox bars.
    Stretch ///< Distort to fill exactly.
};

/**
 * @brief  Abstract wallpaper layer interface.
 *
 * Manages wallpaper state (current token, image data, scaling mode)
 * without performing rendering. Concrete implementations load images
 * from WallPaperToken paths and provide QImage data that can be
 * consumed by any rendering backend (QPainter, RHI, etc.).
 *
 * @ingroup wallpaper
 */
class WallPaperLayer {
  public:
    using ImageChangedCallback = std::function<void()>;

    virtual ~WallPaperLayer() = default;

    /**
     * @brief  Sets the wallpaper token storage.
     *
     * Takes ownership of the storage. Implementations should load
     * the image at the current cursor position if available.
     *
     * @param[in] storage  The wallpaper token collection. Ownership: caller → this.
     */
    virtual void setTokenStorage(std::unique_ptr<WallPaperAccessStorage> storage) = 0;

    /**
     * @brief  Switches to the next wallpaper in the collection.
     *
     * @return True if the switch succeeded, false if at the end or no storage.
     */
    virtual bool showNextOne() = 0;

    /**
     * @brief  Switches to the previous wallpaper in the collection.
     *
     * @return True if the switch succeeded, false if at the beginning or no storage.
     */
    virtual bool showPrevOne() = 0;

    /**
     * @brief  Switches to a specific wallpaper by token ID.
     *
     * @param[in] token  The token ID to switch to.
     * @return True if the switch succeeded, false if token not found.
     */
    virtual bool showTargetOne(const wallpaper_token_id_t& token) = 0;

    /**
     * @brief  Returns the current wallpaper image.
     *
     * QImage uses implicit sharing (COW), so returning by value is cheap.
     *
     * @return Current image, or null QImage if no wallpaper is loaded.
     */
    virtual QImage currentImage() const = 0;

    /**
     * @brief  Returns the current scaling mode.
     *
     * @return The scaling mode for wallpaper rendering.
     */
    virtual ScalingMode scalingMode() const = 0;

    /**
     * @brief  Returns the background fill color.
     *
     * Used for letterbox bars or as fallback when no image is available.
     *
     * @return Background color.
     */
    virtual QColor backgroundColor() const = 0;

    /**
     * @brief  Sets the callback invoked when the wallpaper image changes.
     *
     * @param[in] cb  The callback to invoke on image change.
     */
    void setImageChangedCallback(ImageChangedCallback cb) { on_image_changed_ = std::move(cb); }

  protected:
    /**
     * @brief  Invokes the image-changed callback if set.
     *
     * Subclasses should call this after successfully loading a new image.
     */
    void notifyImageChanged() {
        if (on_image_changed_) {
            on_image_changed_();
        }
    }

  private:
    ImageChangedCallback on_image_changed_;
};

} // namespace cf::desktop::wallpaper
