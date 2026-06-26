/**
 * @file start_button.cpp
 * @brief Start affordance widget implementation.
 *
 * Renders the leading taskbar tile as a rounded-square bearing a fixed app-grid
 * glyph (a 2x2 block of rounded squares). The tile zooms on hover
 * (QVariantAnimation), plays a self-drawn press ripple, and emits clicked() on
 * release. All rendering is QPainter-native, mirroring TaskbarIcon so it builds
 * everywhere.
 *
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date 2026-06-26
 * @version 0.1
 * @since 0.20
 * @ingroup components
 */

#include "start_button.h"

#include "core/theme_manager.h"
#include "core/token/material_scheme/cfmaterial_token_literals.h"

#include <QEasingCurve>
#include <QEnterEvent>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QVariantAnimation>

#include <cmath>

namespace cf::desktop::desktop_component {

using namespace qw::core::token::literals;

namespace {
constexpr int kCellSize = 56;          ///< Tile widget edge length (px).
constexpr qreal kIconBase = 36.0;      ///< Resting tile square edge (px).
constexpr qreal kHoverScale = 1.2;     ///< Scale factor when hovered.
constexpr qreal kIconRadius = 10.0;    ///< Tile corner radius (px).
constexpr int kHoverDurationMs = 150;  ///< Hover zoom duration (ms).
constexpr int kRippleDurationMs = 350; ///< Ripple expansion duration (ms).
constexpr int kRippleAlpha = 90;       ///< Peak ripple overlay alpha.
constexpr int kHoverOverlayAlpha = 24; ///< Hover state-layer alpha.

// App-grid glyph geometry: a 2x2 block of small rounded squares.
constexpr qreal kGlyphCell = 9.0;   ///< Edge of one glyph square (px).
constexpr qreal kGlyphGap = 4.0;    ///< Gap between glyph squares (px).
constexpr qreal kGlyphRadius = 2.0; ///< Corner radius of a glyph square (px).
} // namespace

StartButton::StartButton(QWidget* parent) : QWidget(parent) {
    setFixedSize(kCellSize, kCellSize);
    setCursor(Qt::PointingHandCursor);
    setAutoFillBackground(false);
    setupAnimations();
    applyTheme();
}

StartButton::~StartButton() = default;

QSize StartButton::sizeHint() const {
    return {kCellSize, kCellSize};
}

// -- Painting --------------------------------------------------------------
void StartButton::paintEvent(QPaintEvent* /*event*/) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    const QRectF cell = rect();
    const qreal edge = kIconBase * hover_scale_;
    const QPointF c = cell.center();
    const QRectF tile(c.x() - edge / 2.0, c.y() - edge / 2.0, edge, edge);

    p.setPen(Qt::NoPen);

    // Tile body.
    p.setBrush(tile_color_);
    p.drawRoundedRect(tile, kIconRadius, kIconRadius);

    // Hover state overlay (MD3 state layer), shown only while zoomed in.
    if (hover_scale_ > 1.001) {
        p.setBrush(QColor(foreground_color_.red(), foreground_color_.green(),
                          foreground_color_.blue(), kHoverOverlayAlpha));
        p.drawRoundedRect(tile, kIconRadius, kIconRadius);
    }

    // Press ripple: an expanding circle that fades out.
    if (rippling_) {
        const qreal maxRadius = std::hypot(cell.width(), cell.height()) / 2.0;
        const qreal radius = ripple_progress_ * maxRadius;
        const int alpha = static_cast<int>((1.0 - ripple_progress_) * kRippleAlpha);
        p.setBrush(QColor(foreground_color_.red(), foreground_color_.green(),
                          foreground_color_.blue(), alpha));
        p.drawEllipse(ripple_center_, radius, radius);
    }

    // App-grid glyph: 2x2 of small rounded squares, centered on the tile.
    const qreal block = 2.0 * kGlyphCell + kGlyphGap;
    const QPointF origin(c.x() - block / 2.0, c.y() - block / 2.0);
    p.setBrush(foreground_color_);
    for (int row = 0; row < 2; ++row) {
        for (int col = 0; col < 2; ++col) {
            const QRectF sq(origin.x() + col * (kGlyphCell + kGlyphGap),
                            origin.y() + row * (kGlyphCell + kGlyphGap), kGlyphCell, kGlyphCell);
            p.drawRoundedRect(sq, kGlyphRadius, kGlyphRadius);
        }
    }
}

// -- Interaction -----------------------------------------------------------
void StartButton::enterEvent(QEnterEvent* /*event*/) {
    startHover(true);
}

void StartButton::leaveEvent(QEvent* /*event*/) {
    startHover(false);
    if (rippling_) {
        ripple_anim_->stop();
        rippling_ = false;
    }
    update();
}

void StartButton::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        startRipple(event->position());
    }
    QWidget::mousePressEvent(event);
}

void StartButton::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && rect().contains(event->position().toPoint())) {
        emit clicked();
    }
    QWidget::mouseReleaseEvent(event);
}

// -- Internal --------------------------------------------------------------
void StartButton::applyTheme() {
    try {
        auto& tm = qw::core::ThemeManager::instance();
        const auto& theme = tm.theme(tm.currentThemeName());
        auto& cs = theme.color_scheme();
        tile_color_ = cs.queryColor(SURFACE_VARIANT);
        foreground_color_ = cs.queryColor(ON_SURFACE);
    } catch (...) {
        // Fallback palette when no theme is registered yet.
        tile_color_ = QColor(0xE7, 0xE0, 0xEC);
        foreground_color_ = QColor(0x1C, 0x1B, 0x1F);
    }
    update();
}

void StartButton::startHover(bool entering) {
    hover_anim_->stop();
    hover_anim_->setStartValue(hover_scale_);
    hover_anim_->setEndValue(entering ? qreal(kHoverScale) : qreal(1.0));
    hover_anim_->start();
}

void StartButton::startRipple(const QPointF& center) {
    ripple_center_ = center;
    ripple_progress_ = 0.0;
    rippling_ = true;
    ripple_anim_->stop();
    ripple_anim_->setStartValue(qreal(0.0));
    ripple_anim_->setEndValue(qreal(1.0));
    ripple_anim_->start();
}

void StartButton::setupAnimations() {
    hover_anim_ = new QVariantAnimation(this);
    hover_anim_->setDuration(kHoverDurationMs);
    hover_anim_->setEasingCurve(QEasingCurve::OutCubic);
    connect(hover_anim_, &QVariantAnimation::valueChanged, this, [this](const QVariant& v) {
        hover_scale_ = v.toReal();
        update();
    });

    ripple_anim_ = new QVariantAnimation(this);
    ripple_anim_->setDuration(kRippleDurationMs);
    ripple_anim_->setEasingCurve(QEasingCurve::OutQuad);
    connect(ripple_anim_, &QVariantAnimation::valueChanged, this, [this](const QVariant& v) {
        ripple_progress_ = v.toReal();
        update();
    });
    connect(ripple_anim_, &QVariantAnimation::finished, this, [this]() {
        rippling_ = false;
        update();
    });

    // Follow live theme switches (ThemeManager is the canonical source).
    connect(&qw::core::ThemeManager::instance(), &qw::core::ThemeManager::themeChanged, this,
            [this](const qw::core::ICFTheme&) { applyTheme(); });
}

} // namespace cf::desktop::desktop_component
