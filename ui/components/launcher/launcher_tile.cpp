/**
 * @file launcher_tile.cpp
 * @brief Single application tile implementation for the launcher grid.
 *
 * Renders one launchable application as a rounded glyph tile bearing its
 * initial, with the display name elided beneath it. The glyph zooms on hover
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

#include "launcher_tile.h"

#include "app_icon_resolver.h"

#include "core/theme_manager.h"
#include "core/token/material_scheme/cfmaterial_token_literals.h"
#include "core/token/typography/cfmaterial_typography_token_literals.h"

#include <QEasingCurve>
#include <QEnterEvent>
#include <QFontMetrics>
#include <QLineF>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QTimer>
#include <QVariantAnimation>

#include <cmath>

namespace cf::desktop::desktop_component {

using namespace qw::core::token::literals;

namespace {
constexpr int kCellSize = 96;          ///< Tile widget edge length (px).
constexpr qreal kIconBase = 48.0;      ///< Resting glyph square edge (px).
constexpr qreal kHoverScale = 1.12;    ///< Glyph scale factor when hovered.
constexpr qreal kIconRadius = 12.0;    ///< Glyph corner radius (px).
constexpr qreal kIconCenterY = 38.0;   ///< Glyph vertical center (px from top).
constexpr int kLabelMargin = 10;       ///< Horizontal caption margin (px).
constexpr int kLabelTop = 64;          ///< Caption top y (px).
constexpr int kLabelHeight = 26;       ///< Caption band height (px).
constexpr int kGlyphPixelSize = 22;    ///< Initial letter font size (px).
constexpr int kLabelPixelSize = 13;    ///< Caption font size (px).
constexpr int kHoverDurationMs = 150;  ///< Hover zoom duration (ms).
constexpr int kRippleDurationMs = 350; ///< Ripple expansion duration (ms).
constexpr int kRippleAlpha = 90;       ///< Peak ripple overlay alpha.
constexpr int kHoverOverlayAlpha = 24; ///< Hover state-layer alpha.
/// Long-press delay before a desktop tile enters drag mode. Resistive touch
/// (i.MX6ULL tslib) needs a longer hold so a real tap never trips it.
constexpr int kLongPressMs = 500;
/// Move distance (px) from the press point that cancels a pending long-press.
/// Resistive panels jitter several px when held still, so this is generous.
constexpr qreal kDragThresholdPx = 10.0;
/// Tile opacity while being dragged (the floating ghost carries the full look).
constexpr qreal kDragDimOpacity = 0.45;
} // namespace

LauncherTile::LauncherTile(AppEntry entry, QWidget* parent)
    : QWidget(parent), entry_(std::move(entry)) {
    setFixedSize(kCellSize, kCellSize);
    setCursor(Qt::PointingHandCursor);
    setAutoFillBackground(false);
    setupAnimations();
    applyTheme();
    refreshIcon();

    long_press_timer_ = new QTimer(this);
    long_press_timer_->setSingleShot(true);
    long_press_timer_->setInterval(kLongPressMs);
    connect(long_press_timer_, &QTimer::timeout, this, [this]() {
        if (drag_state_ != DragState::Pressed) {
            return;
        }
        if (context_ == TileContext::Launcher) {
            // Launcher long-press pins the app to the desktop; release is a no-op.
            drag_state_ = DragState::Cancelled;
            emit addToDesktopRequested(entry_.app_id);
        } else {
            // Desktop long-press enters drag mode. Kill the ripple so it does
            // not expand under the finger for the whole drag.
            if (rippling_) {
                ripple_anim_->stop();
                rippling_ = false;
            }
            drag_state_ = DragState::DragReady;
            emit longPressed(entry_.app_id);
        }
        update();
    });
}

LauncherTile::~LauncherTile() = default;

void LauncherTile::setEntry(const AppEntry& entry) {
    entry_ = entry;
    refreshIcon();
    update();
}

QSize LauncherTile::sizeHint() const {
    return {kCellSize, kCellSize};
}

// -- Painting --------------------------------------------------------------
void LauncherTile::paintEvent(QPaintEvent* /*event*/) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    // Dim the tile while it is being dragged; the floating ghost (owned by
    // DesktopIconLayer) carries the full-opacity look under the finger.
    if (drag_state_ == DragState::DragReady || drag_state_ == DragState::Dragging) {
        p.setOpacity(kDragDimOpacity);
    }

    const QRectF cell = rect();
    const qreal edge = kIconBase * hover_scale_;
    const qreal cx = cell.center().x();
    const QRectF glyph(cx - edge / 2.0, kIconCenterY - edge / 2.0, edge, edge);

    p.setPen(Qt::NoPen);

    // Glyph tile body.
    p.setBrush(tile_color_);
    p.drawRoundedRect(glyph, kIconRadius, kIconRadius);

    // Hover state overlay (MD3 state layer), shown only while zoomed in.
    if (hover_scale_ > 1.001) {
        p.setBrush(QColor(foreground_color_.red(), foreground_color_.green(),
                          foreground_color_.blue(), kHoverOverlayAlpha));
        p.drawRoundedRect(glyph, kIconRadius, kIconRadius);
    }

    // Press ripple: an expanding circle that fades out across the whole tile.
    if (rippling_) {
        const qreal maxRadius = std::hypot(cell.width(), cell.height()) / 2.0;
        const qreal radius = ripple_progress_ * maxRadius;
        const int alpha = static_cast<int>((1.0 - ripple_progress_) * kRippleAlpha);
        p.setBrush(QColor(foreground_color_.red(), foreground_color_.green(),
                          foreground_color_.blue(), alpha));
        p.drawEllipse(ripple_center_, radius, radius);
    }

    // Real icon image when one resolves (manifest path or .desktop theme name);
    // otherwise the application's initial letter, centered on the glyph tile.
    if (!cached_icon_.isNull()) {
        p.setRenderHint(QPainter::SmoothPixmapTransform, true);
        p.drawPixmap(glyph, cached_icon_,
                     QRectF(0, 0, cached_icon_.width(), cached_icon_.height()));
    } else {
        p.setPen(foreground_color_);
        p.setFont(glyph_font_);
        const QString letter = entry_.display_name.isEmpty()
                                   ? QStringLiteral("?")
                                   : QString(entry_.display_name.at(0)).toUpper();
        p.drawText(glyph, Qt::AlignCenter, letter);
    }

    // Caption beneath the glyph, elided to the available width.
    p.setPen(label_color_);
    p.setFont(label_font_);
    const QRectF label_rect(kLabelMargin, kLabelTop, cell.width() - 2 * kLabelMargin, kLabelHeight);
    const QString caption =
        QFontMetrics(label_font_)
            .elidedText(entry_.display_name, Qt::ElideRight, label_rect.width());
    p.drawText(label_rect, Qt::AlignHCenter | Qt::AlignVCenter, caption);
}

// -- Interaction -----------------------------------------------------------
void LauncherTile::enterEvent(QEnterEvent* /*event*/) {
    startHover(true);
}

void LauncherTile::leaveEvent(QEvent* /*event*/) {
    startHover(false);
    if (rippling_) {
        ripple_anim_->stop();
        rippling_ = false;
    }
    update();
}

void LauncherTile::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        startRipple(event->position());
        // Arm the long-press timer in both contexts: launcher long-press pins
        // to desktop, desktop long-press starts a drag. A quick tap (release
        // before the timer fires) still emits clicked() in mouseReleaseEvent.
        press_pos_ = event->position();
        drag_state_ = DragState::Pressed;
        long_press_timer_->start();
    }
    QWidget::mousePressEvent(event);
}

void LauncherTile::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        long_press_timer_->stop();
        const DragState state = drag_state_;
        drag_state_ = DragState::Idle;
        if (state == DragState::Dragging || state == DragState::DragReady) {
            // Drag end: the layer computes the drop cell and commits.
            emit dragEnded(event->globalPosition().toPoint());
        } else if (state == DragState::Pressed && rect().contains(event->position().toPoint())) {
            // Quick tap (no long-press, no big move): launch.
            emit clicked(entry_.app_id);
        }
        // Cancelled (or launcher add-to-desktop already fired): no-op.
        update();
    }
    QWidget::mouseReleaseEvent(event);
}

void LauncherTile::mouseMoveEvent(QMouseEvent* event) {
    if (event->buttons() & Qt::LeftButton) {
        if (drag_state_ == DragState::Pressed) {
            // Moved beyond threshold before the long-press fired: scrap. Do
            // not click either — a deliberate sweep is not a tap. Resistive
            // touch jitters, so only cancel past the threshold.
            if (QLineF(press_pos_, event->position()).length() >= kDragThresholdPx) {
                long_press_timer_->stop();
                drag_state_ = DragState::Cancelled;
            }
        } else if (drag_state_ == DragState::DragReady || drag_state_ == DragState::Dragging) {
            drag_state_ = DragState::Dragging;
            emit dragMoved(event->globalPosition().toPoint());
        }
    }
    QWidget::mouseMoveEvent(event);
}

// -- Internal --------------------------------------------------------------
void LauncherTile::applyTheme() {
    try {
        auto& tm = qw::core::ThemeManager::instance();
        const auto& theme = tm.theme(tm.currentThemeName());
        auto& cs = theme.color_scheme();
        tile_color_ = cs.queryColor(SURFACE_VARIANT);
        foreground_color_ = cs.queryColor(ON_SURFACE);
        label_color_ = cs.queryColor(ON_SURFACE_VARIANT);
        glyph_font_ = theme.font_type().queryTargetFont(TYPOGRAPHY_TITLE_MEDIUM);
        glyph_font_.setPixelSize(kGlyphPixelSize);
        label_font_ = theme.font_type().queryTargetFont(TYPOGRAPHY_LABEL_MEDIUM);
        label_font_.setPixelSize(kLabelPixelSize);
    } catch (...) {
        // Fallback palette when no theme is registered yet.
        tile_color_ = QColor(0xE7, 0xE0, 0xEC);
        foreground_color_ = QColor(0x1C, 0x1B, 0x1F);
        label_color_ = QColor(0x49, 0x45, 0x4E);
        glyph_font_ = font();
        glyph_font_.setPixelSize(kGlyphPixelSize);
        label_font_ = font();
        label_font_.setPixelSize(kLabelPixelSize);
    }
    update();
}

void LauncherTile::startHover(bool entering) {
    hover_anim_->stop();
    hover_anim_->setStartValue(hover_scale_);
    hover_anim_->setEndValue(entering ? qreal(kHoverScale) : qreal(1.0));
    hover_anim_->start();
}

void LauncherTile::startRipple(const QPointF& center) {
    ripple_center_ = center;
    ripple_progress_ = 0.0;
    rippling_ = true;
    ripple_anim_->stop();
    ripple_anim_->setStartValue(qreal(0.0));
    ripple_anim_->setEndValue(qreal(1.0));
    ripple_anim_->start();
}

void LauncherTile::setupAnimations() {
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

void LauncherTile::refreshIcon() {
    // Cache at 2x the resting glyph edge (kIconBase = 48) so the hover zoom
    // (kHoverScale = 1.12) stays crisp instead of upscaling a 48px source.
    cached_icon_ = resolve_app_icon(entry_, QSize(96, 96));
}

} // namespace cf::desktop::desktop_component
