/**
 * @file ui/base/geometry_helper.h
 * @brief Geometry helpers for creating rounded rectangles.
 *
 * Provides utilities for creating rounded rectangles with various
 * corner radius options following Material Design shape scales.
 *
 * @author Charliechen114514
 * @date 2026-02-23
 * @version 0.1
 * @since 0.1
 * @ingroup ui
 */
#pragma once
#include "export.h"
#include <QPainterPath>

namespace cf::ui::base {
namespace geometry {

/**
 * @brief Material Design shape scale categories.
 *
 * Defines standard corner radius values following Material Design
 * shape specifications.
 *
 * @ingroup ui
 */
enum class ShapeScale {
    ShapeNone,       ///< No rounding (0dp).
    ShapeExtraSmall, ///< Extra small rounding (4dp).
    ShapeSmall,      ///< Small rounding (8dp).
    ShapeMedium,     ///< Medium rounding (12dp).
    ShapeLarge,      ///< Large rounding (16dp).
    ShapeExtraLarge, ///< Extra large rounding (28dp).
    ShapeFull        ///< Full rounding (50% of size).
};

/**
 * @brief Creates a rounded rectangle path with Material Design scale.
 *
 * @param[in] rect The base rectangle to round.
 * @param[in] scale The Material Design shape scale for corner radius.
 * @return QPainterPath with rounded rectangle.
 * @throws None
 * @note None
 * @warning None
 * @since 0.1
 * @ingroup ui
 */
CF_UI_EXPORT QPainterPath roundedRect(const QRectF& rect, ShapeScale scale);

/**
 * @brief Creates a rounded rectangle path with uniform corner radius.
 *
 * @param[in] rect The base rectangle to round.
 * @param[in] radius Uniform corner radius in pixels.
 * @return QPainterPath with rounded rectangle.
 * @throws None
 * @note None
 * @warning None
 * @since 0.1
 * @ingroup ui
 */
CF_UI_EXPORT QPainterPath roundedRect(const QRectF& rect, float radius);

/**
 * @brief Creates a rounded rectangle path with individual corner radii.
 *
 * @param[in] rect The base rectangle to round.
 * @param[in] topLeft Top-left corner radius in pixels.
 * @param[in] topRight Top-right corner radius in pixels.
 * @param[in] bottomLeft Bottom-left corner radius in pixels.
 * @param[in] bottomRight Bottom-right corner radius in pixels.
 * @return QPainterPath with rounded rectangle.
 * @throws None
 * @note None
 * @warning None
 * @since 0.1
 * @ingroup ui
 */
CF_UI_EXPORT QPainterPath roundedRect(const QRectF& rect, float topLeft, float topRight,
                                      float bottomLeft, float bottomRight);

} // namespace geometry
} // namespace cf::ui::base
