/**
 * @file taskbar_scroll_arrow.cpp
 * @brief Taskbar scroll-arrow button implementation.
 *
 * Renders a rounded Material surface bearing a chevron glyph ("<" or ">")
 * with a hover state-layer, dims when disabled, and emits clicked() on
 * release. QPainter-native, mirroring StartButton/TaskbarIcon so it builds
 * everywhere; stripped of ripple and scale since it is a secondary affordance.
 *
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date 2026-07-08
 * @version 0.1
 * @since 0.20
 * @ingroup components
 */

#include "taskbar_scroll_arrow.h"

#include "core/theme_manager.h"
#include "core/token/material_scheme/cfmaterial_token_literals.h"

#include <QEnterEvent>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>

namespace cf::desktop::desktop_component {

using namespace qw::core::token::literals;

namespace {
constexpr int kArrowWidth = 40;         ///< Button width (px); the full tap target.
constexpr int kArrowHeight = 56;        ///< Button height (px); matches taskbar tiles.
constexpr qreal kChevronHalf = 8.0;     ///< Half the chevron's horizontal extent (px).
constexpr qreal kRestingOpacity = 0.8;  ///< Chevron opacity when idle (no fill background).
constexpr qreal kHoveredOpacity = 1.0;  ///< Chevron opacity on hover.
constexpr qreal kDisabledOpacity = 0.3; ///< Dim factor when the scroll end is reached.
} // namespace

TaskbarScrollArrow::TaskbarScrollArrow(ScrollDirection direction, QWidget* parent)
    : QWidget(parent), direction_(direction) {
    setFixedSize(kArrowWidth, kArrowHeight);
    setCursor(Qt::PointingHandCursor);
    setAutoFillBackground(false);
    setToolTip(direction == ScrollDirection::Left ? QStringLiteral("Scroll left")
                                                  : QStringLiteral("Scroll right"));
    applyTheme();

    // Follow live theme switches (ThemeManager is the canonical source).
    connect(&qw::core::ThemeManager::instance(), &qw::core::ThemeManager::themeChanged, this,
            [this](const qw::core::ICFTheme&) { applyTheme(); });
}

TaskbarScrollArrow::~TaskbarScrollArrow() = default;

QSize TaskbarScrollArrow::sizeHint() const {
    return {kArrowWidth, kArrowHeight};
}

// -- Painting --------------------------------------------------------------
void TaskbarScrollArrow::paintEvent(QPaintEvent* /*event*/) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    // No filled background — just the chevron, so the arrow reads as a light
    // indicator beside the tiles instead of a chunky button competing with
    // them. Opacity carries the state: dim at a scroll end, full on hover.
    qreal opacity = kRestingOpacity;
    if (!isEnabled()) {
        opacity = kDisabledOpacity;
    } else if (hovered_) {
        opacity = kHoveredOpacity;
    }
    p.setOpacity(opacity);

    // Chevron as a polyline (font-independent, crisp at any size). A right
    // chevron ">" runs upper-left -> mid-right -> lower-left; left mirrors it.
    const QPointF c = rect().center();
    const qreal dx = kChevronHalf;
    const qreal dy = kChevronHalf * 1.6; // taller than wide, like a typical ">"
    QPainterPath chevron;
    if (direction_ == ScrollDirection::Right) {
        chevron.moveTo(c.x() - dx, c.y() - dy);
        chevron.lineTo(c.x() + dx, c.y());
        chevron.lineTo(c.x() - dx, c.y() + dy);
    } else {
        chevron.moveTo(c.x() + dx, c.y() - dy);
        chevron.lineTo(c.x() - dx, c.y());
        chevron.lineTo(c.x() + dx, c.y() + dy);
    }
    QPen pen(foreground_color_);
    pen.setWidthF(2.5);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    p.setPen(pen);
    p.setBrush(Qt::NoBrush);
    p.drawPath(chevron);
}

// -- Interaction -----------------------------------------------------------
void TaskbarScrollArrow::enterEvent(QEnterEvent* /*event*/) {
    hovered_ = true;
    update();
}

void TaskbarScrollArrow::leaveEvent(QEvent* /*event*/) {
    hovered_ = false;
    update();
}

void TaskbarScrollArrow::mousePressEvent(QMouseEvent* event) {
    // Accept the press so the matching release is delivered here.
    QWidget::mousePressEvent(event);
}

void TaskbarScrollArrow::mouseReleaseEvent(QMouseEvent* event) {
    // Qt already blocks delivery to a disabled widget, but guard anyway.
    if (isEnabled() && event->button() == Qt::LeftButton &&
        rect().contains(event->position().toPoint())) {
        emit clicked();
    }
    QWidget::mouseReleaseEvent(event);
}

// -- Internal --------------------------------------------------------------
void TaskbarScrollArrow::applyTheme() {
    try {
        auto& tm = qw::core::ThemeManager::instance();
        const auto& theme = tm.theme(tm.currentThemeName());
        const auto& cs = theme.color_scheme();
        foreground_color_ = cs.queryColor(ON_SURFACE);
    } catch (...) {
        // Fallback color when no theme is registered yet.
        foreground_color_ = QColor(0x1C, 0x1B, 0x1F);
    }
    update();
}

} // namespace cf::desktop::desktop_component
