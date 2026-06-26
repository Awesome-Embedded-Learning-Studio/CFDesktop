/**
 * @file start_button.cpp
 * @brief Start affordance widget implementation.
 *
 * Renders the leading taskbar tile as a rounded-square bearing a tinted start
 * icon. The tile zooms on hover
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

#include "icon_mask.h"

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
} // namespace

StartButton::StartButton(QWidget* parent) : QWidget(parent) {
    setFixedSize(kCellSize, kCellSize);
    setCursor(Qt::PointingHandCursor);
    setAutoFillBackground(false);
    setupAnimations();
    applyTheme();
    setToolTip(QStringLiteral("Start"));
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

    // Start glyph: the tinted icon mask. A missing mask leaves the tile blank
    // (no silent 2x2-grid fallback) so a broken resource stays obvious.
    if (!icon_mask_.isNull()) {
        const qreal glyph = edge * 0.6;
        const QRectF glyph_rect(c.x() - glyph / 2.0, c.y() - glyph / 2.0, glyph, glyph);
        p.setRenderHint(QPainter::SmoothPixmapTransform, true);
        p.drawPixmap(glyph_rect, icon_mask_, QRectF(0, 0, icon_mask_.width(), icon_mask_.height()));
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
    refreshIcon();
    update();
}

void StartButton::refreshIcon() {
    icon_mask_ = tintedIconMask(QStringLiteral(":/cfdesktop/taskbar/start.png"), foreground_color_);
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
