/**
 * @file taskbar_icon.cpp
 * @brief Single application icon widget implementation.
 *
 * Renders one launchable application as a rounded-square tile bearing its
 * initial. The tile zooms on hover (QVariantAnimation), plays a self-drawn
 * press ripple, and shows a running-state indicator dot. All rendering is
 * QPainter-native, mirroring the status bar so it builds everywhere.
 *
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date 2026-06-16
 * @version 0.1
 * @since 0.19
 * @ingroup components
 */

#include "taskbar_icon.h"

#include "core/theme_manager.h"
#include "core/token/material_scheme/cfmaterial_token_literals.h"
#include "core/token/typography/cfmaterial_typography_token_literals.h"

#include <QEasingCurve>
#include <QEnterEvent>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QVariantAnimation>

#include <cmath>

namespace cf::desktop::desktop_component {

using namespace cf::ui::core::token::literals;

namespace {
constexpr int kCellSize = 56;          ///< Tile widget edge length (px).
constexpr qreal kIconBase = 36.0;      ///< Resting tile square edge (px).
constexpr qreal kHoverScale = 1.2;     ///< Scale factor when hovered.
constexpr qreal kIconRadius = 10.0;    ///< Tile corner radius (px).
constexpr qreal kDotRadius = 2.5;      ///< Running-indicator dot radius (px).
constexpr qreal kDotOffset = 6.0;      ///< Dot offset below the tile (px).
constexpr int kLabelPixelSize = 18;    ///< Initial letter font size (px).
constexpr int kHoverDurationMs = 150;  ///< Hover zoom duration (ms).
constexpr int kRippleDurationMs = 350; ///< Ripple expansion duration (ms).
constexpr int kRippleAlpha = 90;       ///< Peak ripple overlay alpha.
constexpr int kHoverOverlayAlpha = 24; ///< Hover state-layer alpha.
} // namespace

TaskbarIcon::TaskbarIcon(AppEntry entry, QWidget* parent)
    : QWidget(parent), entry_(std::move(entry)) {
    setFixedSize(kCellSize, kCellSize);
    setCursor(Qt::PointingHandCursor);
    setAutoFillBackground(false);
    setupAnimations();
    applyTheme();
}

TaskbarIcon::~TaskbarIcon() = default;

void TaskbarIcon::setEntry(const AppEntry& entry) {
    entry_ = entry;
    update();
}

void TaskbarIcon::setRunning(bool running) {
    if (running_ != running) {
        running_ = running;
        update();
    }
}

QSize TaskbarIcon::sizeHint() const {
    return {kCellSize, kCellSize};
}

// -- Painting --------------------------------------------------------------
void TaskbarIcon::paintEvent(QPaintEvent* /*event*/) {
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

    // Initial letter.
    p.setPen(foreground_color_);
    p.setFont(label_font_);
    const QString letter = entry_.display_name.isEmpty()
                               ? QStringLiteral("?")
                               : QString(entry_.display_name.at(0)).toUpper();
    p.drawText(tile, Qt::AlignCenter, letter);

    // Running indicator dot near the tile bottom.
    if (running_) {
        const QPointF dot(c.x(), tile.bottom() + kDotOffset);
        p.setPen(Qt::NoPen);
        p.setBrush(indicator_color_);
        p.drawEllipse(dot, kDotRadius, kDotRadius);
    }
}

// -- Interaction -----------------------------------------------------------
void TaskbarIcon::enterEvent(QEnterEvent* /*event*/) {
    startHover(true);
}

void TaskbarIcon::leaveEvent(QEvent* /*event*/) {
    startHover(false);
    if (rippling_) {
        ripple_anim_->stop();
        rippling_ = false;
    }
    update();
}

void TaskbarIcon::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        startRipple(event->position());
    }
    QWidget::mousePressEvent(event);
}

void TaskbarIcon::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && rect().contains(event->position().toPoint())) {
        emit clicked(entry_.app_id);
    }
    QWidget::mouseReleaseEvent(event);
}

// -- Internal --------------------------------------------------------------
void TaskbarIcon::applyTheme() {
    try {
        auto& tm = cf::ui::core::ThemeManager::instance();
        const auto& theme = tm.theme(tm.currentThemeName());
        auto& cs = theme.color_scheme();
        tile_color_ = cs.queryColor(SURFACE_VARIANT);
        foreground_color_ = cs.queryColor(ON_SURFACE);
        indicator_color_ = cs.queryColor(ON_SURFACE_VARIANT);
        label_font_ = theme.font_type().queryTargetFont(TYPOGRAPHY_TITLE_MEDIUM);
        label_font_.setPixelSize(kLabelPixelSize);
    } catch (...) {
        // Fallback palette when no theme is registered yet.
        tile_color_ = QColor(0xE7, 0xE0, 0xEC);
        foreground_color_ = QColor(0x1C, 0x1B, 0x1F);
        indicator_color_ = QColor(0x49, 0x45, 0x4E);
        label_font_ = font();
        label_font_.setPixelSize(kLabelPixelSize);
    }
    update();
}

void TaskbarIcon::startHover(bool entering) {
    hover_anim_->stop();
    hover_anim_->setStartValue(hover_scale_);
    hover_anim_->setEndValue(entering ? qreal(kHoverScale) : qreal(1.0));
    hover_anim_->start();
}

void TaskbarIcon::startRipple(const QPointF& center) {
    ripple_center_ = center;
    ripple_progress_ = 0.0;
    rippling_ = true;
    ripple_anim_->stop();
    ripple_anim_->setStartValue(qreal(0.0));
    ripple_anim_->setEndValue(qreal(1.0));
    ripple_anim_->start();
}

void TaskbarIcon::setupAnimations() {
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
    connect(&cf::ui::core::ThemeManager::instance(), &cf::ui::core::ThemeManager::themeChanged,
            this, [this](const cf::ui::core::ICFTheme&) { applyTheme(); });
}

} // namespace cf::desktop::desktop_component
