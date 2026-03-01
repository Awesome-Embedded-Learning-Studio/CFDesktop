/**
 * @file ui/base/device_pixel.h
 * @brief Device pixel ratio conversion utilities for canvas units.
 *
 * Provides helper structures and functions for converting between device-independent
 * pixels (dp), scalable pixels (sp), and physical pixels based on device pixel ratio.
 *
 * @author Charliechen114514
 * @date 2026-02-23
 * @version 0.1
 * @since 0.1
 * @ingroup ui
 */
#pragma once
#include <QtTypes>

namespace cf::ui::base {
namespace device {

/**
 * @brief Helper for converting between canvas units and device pixels.
 *
 * Provides conversion functions for device-independent pixels (dp),
 * scalable pixels (sp), and physical pixels based on the device pixel ratio.
 *
 * @note Not thread-safe unless externally synchronized.
 *
 * @code
 * CanvasUnitHelper helper(2.0);
 * qreal pixels = helper.dpToPx(16.0); // Converts 16dp to pixels
 * @endcode
 *
 * @ingroup ui
 */
struct CanvasUnitHelper {
    /**
     * @brief Constructs a CanvasUnitHelper with the specified device pixel ratio.
     *
     * @param[in] devicePixelRatio The device pixel ratio (e.g., 1.0, 2.0, 3.0).
     *
     * @return     None (constructor).
     *
     * @throws     None.
     *
     * @note       None.
     *
     * @warning    None.
     *
     * @since      0.1
     * @ingroup    ui
     */
    CanvasUnitHelper(const qreal devicePixelRatio);

    /**
     * @brief Converts device-independent pixels to physical pixels.
     *
     * @param[in] dp Value in device-independent pixels.
     * @return Value in physical pixels.
     * @throws None
     * @note None
     * @warning None
     * @since 0.1
     * @ingroup ui
     */
    qreal dpToPx(qreal dp) const;

    /**
     * @brief Converts scalable pixels to physical pixels.
     *
     * @param[in] sp Value in scalable pixels.
     * @return Value in physical pixels.
     * @throws None
     * @note None
     * @warning None
     * @since 0.1
     * @ingroup ui
     */
    qreal spToPx(qreal sp) const;

    /**
     * @brief Converts physical pixels to device-independent pixels.
     *
     * @param[in] px Value in physical pixels.
     * @return Value in device-independent pixels.
     * @throws None
     * @note None
     * @warning None
     * @since 0.1
     * @ingroup ui
     */
    qreal pxToDp(qreal px) const;

    /**
     * @brief Returns the device pixel ratio.
     *
     * @return The device pixel ratio.
     * @throws None
     * @note None
     * @warning None
     * @since 0.1
     * @ingroup ui
     */
    qreal dpi() const;

    /**
     * @brief Window width breakpoints for responsive layouts.
     *
     * Defines breakpoints for different screen size categories
     * following Material Design guidelines.
     *
     * @ingroup ui
     */
    enum class BreakPoint {
        Compact, ///< Compact width: < 600dp
        Medium,  ///< Medium width: 600dp - 839dp
        Expanded ///< Expanded width: >= 840dp
    };

    /**
     * @brief Determines the breakpoint category for a given width.
     *
     * @param[in] widthDp Width in device-independent pixels.
     * @return The breakpoint category for the specified width.
     * @throws None
     * @note None
     * @warning None
     * @since 0.1
     * @ingroup ui
     */
    BreakPoint breakPoint(qreal widthDp);

  private:
    qreal devicePixelRatio; ///< Device pixel ratio used for conversions.
};
} // namespace device
} // namespace cf::ui::base
