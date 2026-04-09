/**
 * @file ui/base/qt_format.h
 * @brief std::formatter specializations for Qt geometry types.
 *
 * Provides formatters for QPoint, QPointF, QSize, QSizeF, QRect, QRectF,
 * QLine, QLineF, QMargins, and QMarginsF so they can be used directly
 * with std::format and the project's logging macros (log::traceftag, etc.).
 *
 * @author Charliechen114514
 * @date 2026-03-31
 * @version 0.14
 * @since 0.14
 * @ingroup ui
 */
#pragma once

#include <QLine>
#include <QMargins>
#include <QPoint>
#include <QRect>
#include <QSize>
#include <QString>
#include <format>

// ============================================================================
// QString
// ============================================================================

/**
 * @brief  std::formatter specialization for QString.
 *
 * Formats a QString by converting to UTF-8 std::string.
 *
 * @ingroup ui
 */
template <> struct std::formatter<QString> : std::formatter<std::string> {
    auto format(const QString& s, std::format_context& ctx) const {
        return std::formatter<std::string>::format(s.toStdString(), ctx);
    }
};

// ============================================================================
// Point types
// ============================================================================

/**
 * @brief  std::formatter specialization for QPoint.
 *
 * Formats a QPoint as "QPoint(x, y)".
 *
 * @ingroup ui
 */
template <> struct std::formatter<QPoint> : std::formatter<std::string> {
    auto format(const QPoint& p, std::format_context& ctx) const {
        return std::formatter<std::string>::format(std::format("QPoint({}, {})", p.x(), p.y()),
                                                   ctx);
    }
};

/**
 * @brief  std::formatter specialization for QPointF.
 *
 * Formats a QPointF as "QPointF(x, y)".
 *
 * @ingroup ui
 */
template <> struct std::formatter<QPointF> : std::formatter<std::string> {
    auto format(const QPointF& p, std::format_context& ctx) const {
        return std::formatter<std::string>::format(std::format("QPointF({}, {})", p.x(), p.y()),
                                                   ctx);
    }
};

// ============================================================================
// Size types
// ============================================================================

/**
 * @brief  std::formatter specialization for QSize.
 *
 * Formats a QSize as "QSize(width, height)".
 *
 * @ingroup ui
 */
template <> struct std::formatter<QSize> : std::formatter<std::string> {
    auto format(const QSize& s, std::format_context& ctx) const {
        return std::formatter<std::string>::format(
            std::format("QSize({}, {})", s.width(), s.height()), ctx);
    }
};

/**
 * @brief  std::formatter specialization for QSizeF.
 *
 * Formats a QSizeF as "QSizeF(width, height)".
 *
 * @ingroup ui
 */
template <> struct std::formatter<QSizeF> : std::formatter<std::string> {
    auto format(const QSizeF& s, std::format_context& ctx) const {
        return std::formatter<std::string>::format(
            std::format("QSizeF({}, {})", s.width(), s.height()), ctx);
    }
};

// ============================================================================
// Rectangle types
// ============================================================================

/**
 * @brief  std::formatter specialization for QRect.
 *
 * Formats a QRect as "QRect(x, y, width, height)".
 *
 * @ingroup ui
 */
template <> struct std::formatter<QRect> : std::formatter<std::string> {
    auto format(const QRect& r, std::format_context& ctx) const {
        return std::formatter<std::string>::format(
            std::format("QRect({}, {}, {}, {})", r.x(), r.y(), r.width(), r.height()), ctx);
    }
};

/**
 * @brief  std::formatter specialization for QRectF.
 *
 * Formats a QRectF as "QRectF(x, y, width, height)".
 *
 * @ingroup ui
 */
template <> struct std::formatter<QRectF> : std::formatter<std::string> {
    auto format(const QRectF& r, std::format_context& ctx) const {
        return std::formatter<std::string>::format(
            std::format("QRectF({}, {}, {}, {})", r.x(), r.y(), r.width(), r.height()), ctx);
    }
};

// ============================================================================
// Line types
// ============================================================================

/**
 * @brief  std::formatter specialization for QLine.
 *
 * Formats a QLine as "QLine(x1, y1, x2, y2)".
 *
 * @ingroup ui
 */
template <> struct std::formatter<QLine> : std::formatter<std::string> {
    auto format(const QLine& l, std::format_context& ctx) const {
        return std::formatter<std::string>::format(
            std::format("QLine({}, {}, {}, {})", l.x1(), l.y1(), l.x2(), l.y2()), ctx);
    }
};

/**
 * @brief  std::formatter specialization for QLineF.
 *
 * Formats a QLineF as "QLineF(x1, y1, x2, y2)".
 *
 * @ingroup ui
 */
template <> struct std::formatter<QLineF> : std::formatter<std::string> {
    auto format(const QLineF& l, std::format_context& ctx) const {
        return std::formatter<std::string>::format(
            std::format("QLineF({}, {}, {}, {})", l.x1(), l.y1(), l.x2(), l.y2()), ctx);
    }
};

// ============================================================================
// Margin types
// ============================================================================

/**
 * @brief  std::formatter specialization for QMargins.
 *
 * Formats a QMargins as "QMargins(left, top, right, bottom)".
 *
 * @ingroup ui
 */
template <> struct std::formatter<QMargins> : std::formatter<std::string> {
    auto format(const QMargins& m, std::format_context& ctx) const {
        return std::formatter<std::string>::format(
            std::format("QMargins({}, {}, {}, {})", m.left(), m.top(), m.right(), m.bottom()), ctx);
    }
};

/**
 * @brief  std::formatter specialization for QMarginsF.
 *
 * Formats a QMarginsF as "QMarginsF(left, top, right, bottom)".
 *
 * @ingroup ui
 */
template <> struct std::formatter<QMarginsF> : std::formatter<std::string> {
    auto format(const QMarginsF& m, std::format_context& ctx) const {
        return std::formatter<std::string>::format(
            std::format("QMarginsF({}, {}, {}, {})", m.left(), m.top(), m.right(), m.bottom()),
            ctx);
    }
};
