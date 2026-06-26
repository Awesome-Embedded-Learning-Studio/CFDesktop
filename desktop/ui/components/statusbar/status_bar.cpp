/**
 * @file status_bar.cpp
 * @brief Concrete status bar panel implementation.
 *
 * Renders the clock and system-icon glyphs with Material Design 3 polish: a
 * tonal elevation surface, an in-band soft shadow at the bottom seam, refined
 * vector icons, and a boot fade-in. All rendering is QPainter-native so it
 * builds for the embedded target without extra Qt modules.
 *
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date 2026-06-15
 * @version 0.2
 * @since 0.19
 * @ingroup components
 */

#include "status_bar.h"

#include "base/color.h"
#include "base/device_pixel.h"
#include "cflog.h"
#include "core/theme_manager.h"
#include "core/token/material_scheme/cfmaterial_token_literals.h"
#include "core/token/typography/cfmaterial_typography_token_literals.h"

#include <QDateTime>
#include <QEasingCurve>
#include <QGuiApplication>
#include <QLinearGradient>
#include <QPaintEvent>
#include <QPainter>
#include <QResizeEvent>
#include <QTimer>
#include <QVariantAnimation>

#include <algorithm>
#include <string>

// Q_INIT_RESOURCE must run at global scope: the rcc-generated registration
// function lives in the global namespace, but the macro's extern declaration
// is emitted in the surrounding scope, so calling it from inside
// cf::desktop::desktop_component would look up a namespaced symbol that
// doesn't exist and fail to link.
static void registerStatusbarIconsResource() {
    Q_INIT_RESOURCE(statusbar_icons);
}

namespace cf::desktop::desktop_component {

using cf::desktop::PanelPosition;
using qw::base::CFColor;
using qw::base::device::CanvasUnitHelper;
using namespace qw::core::token::literals;

namespace {
// Fallback palette used when no theme is available (mirrors MD3 light).
constexpr int kBarHeight = 48;          ///< Status bar thickness in device pixels.
constexpr int kSideMarginDp = 16;       ///< Horizontal padding for clock/icons (dp).
constexpr int kIconGapDp = 12;          ///< Spacing between adjacent icons (dp).
constexpr int kIconSizeDp = 16;         ///< Icon cell edge length (dp).
constexpr int kTimeGapDp = 10;          ///< Gap between the time and the icon cluster (dp).
constexpr qreal kShadowBandDp = 5.0;    ///< In-band elevation shadow height (dp).
constexpr qreal kFrostTintAlpha = 0.60; ///< Frosted-glass tint opacity (wallpaper bleed).

// Icon kinds and their compiled-resource mask paths. Indices align with
// StatusBar::icon_masks_[]. A missing mask leaves a visible gap in the bar
// (no silent fallback) so a broken resource stays obvious.
enum class StatusIcon { Signal = 0, Battery = 1, Wifi = 2, Volume = 3 };
constexpr int kStatusIconCount = 4;
constexpr const char* const kIconMaskPaths[kStatusIconCount] = {
    ":/cfdesktop/statusbar/signal.png",  // Signal
    ":/cfdesktop/statusbar/battery.png", // Battery
    ":/cfdesktop/statusbar/wifi.png",    // Wifi
    ":/cfdesktop/statusbar/volume.png",  // Volume
};
} // namespace

StatusBar::StatusBar(QWidget* parent)
    : QWidget(parent), timer_(new QTimer(this)), cached_time_(currentTimeText()),
      cached_date_(currentDateText()) {
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAutoFillBackground(false);
    setFixedHeight(kBarHeight);
    setupUi();
    applyTheme();
    loadIconMasks();
    startFadeIn();
}

StatusBar::~StatusBar() = default;

void StatusBar::setupUi() {
    connect(timer_, &QTimer::timeout, this, &StatusBar::onTimeout);
    timer_->start(1000);

    // Coalesce rapid resizes (e.g. a window drag) into a single frosted-backdrop
    // reblur instead of rebuilding on every intermediate geometry.
    reblur_debounce_ = new QTimer(this);
    reblur_debounce_->setSingleShot(true);
    reblur_debounce_->setInterval(80);
    connect(reblur_debounce_, &QTimer::timeout, this, [this]() {
        frosted_.invalidate();
        update();
    });

    // A screen change (different monitor / device-pixel-ratio) invalidates the
    // backdrop cache so the next paint rebuilds at the new DPR. QWidget has no
    // screenChanged signal, so listen to the application-wide primary-screen
    // change; devicePixelRatioF() is also part of the cache key as a backstop.
    connect(qApp, &QGuiApplication::primaryScreenChanged, this, [this]() {
        frosted_.invalidate();
        update();
    });

    // React to theme switches (ThemeManager is the canonical source).
    connect(&qw::core::ThemeManager::instance(), &qw::core::ThemeManager::themeChanged, this,
            [this](const qw::core::ICFTheme&) { applyTheme(); });
}

void StatusBar::applyTheme() {
    try {
        auto& tm = qw::core::ThemeManager::instance();
        const auto& theme = tm.theme(tm.currentThemeName());
        auto& cs = theme.color_scheme();
        background_color_ = cs.queryColor(SURFACE);
        foreground_color_ = cs.queryColor(ON_SURFACE);
        icon_color_ = cs.queryColor(ON_SURFACE_VARIANT);
        divider_color_ = cs.queryColor(OUTLINE_VARIANT);
        clock_font_ = theme.font_type().queryTargetFont(TYPOGRAPHY_TITLE_MEDIUM);
        // MD3 elevation tonal lift: lighten the surface tone a few steps for the
        // top of the gradient, so the bar reads as raised above the shell.
        const CFColor base(background_color_);
        surface_top_color_ =
            CFColor(base.hue(), base.chroma(), std::clamp(base.tone() + 3.0f, 0.0f, 100.0f))
                .native_color();
    } catch (...) {
        // Fallback palette when no theme is registered yet.
        background_color_ = QColor(0xF7, 0xF5, 0xF3);
        surface_top_color_ = QColor(0xFF, 0xFF, 0xFF);
        foreground_color_ = QColor(0x1C, 0x1B, 0x1F);
        icon_color_ = QColor(0x49, 0x45, 0x4E);
        divider_color_ = QColor(0xCA, 0xC4, 0xD0);
        clock_font_ = font();
        clock_font_.setPixelSize(15);
    }
    // Frosted-glass tint tracks the resolved surface color (covers both the
    // themed and fallback branches above). The top highlight gives the top bar
    // its "raised glass" reading.
    frosted_params_.tint = background_color_;
    frosted_params_.tint_alpha = kFrostTintAlpha;
    frosted_params_.top_highlight = true;
    frosted_.invalidate();
    update();
}

void StatusBar::startFadeIn() {
    auto* fade = new QVariantAnimation(this);
    fade->setDuration(250);
    fade->setStartValue(qreal(0));
    fade->setEndValue(qreal(1));
    fade->setEasingCurve(QEasingCurve::OutCubic);
    connect(fade, &QVariantAnimation::valueChanged, this, [this](const QVariant& v) {
        fade_opacity_ = v.toReal();
        update();
    });
    fade->start(QAbstractAnimation::DeleteWhenStopped);
}

void StatusBar::loadIconMasks() {
    // Qt resources compiled into a STATIC library can be stripped by the linker
    // because nothing references the registration symbol. Forcing registration
    // keeps the object, so the ":/cfdesktop/statusbar/*.png" lookups succeed.
    registerStatusbarIconsResource();

    int loaded = 0;
    std::string missing;
    for (int i = 0; i < kStatusIconCount; ++i) {
        icon_masks_[i] = QPixmap(QString::fromLatin1(kIconMaskPaths[i]));
        if (icon_masks_[i].isNull()) {
            missing += missing.empty() ? "" : ", ";
            missing += kIconMaskPaths[i];
        } else {
            ++loaded;
        }
    }
    if (missing.empty()) {
        cf::log::infoftag("StatusBar", "loaded {}/{} icon masks", loaded, kStatusIconCount);
    } else {
        // Fail loud: a missing mask now leaves a visible gap in the bar rather
        // than a silent substitute, and this warning names exactly what is gone.
        cf::log::warningftag("StatusBar", "loaded {}/{} icon masks; missing: {}", loaded,
                             kStatusIconCount, missing);
    }
}

QPixmap StatusBar::tintedPixmap(const QPixmap& mask, const QColor& color) {
    if (mask.isNull()) {
        return {};
    }
    QPixmap out(mask.size());
    out.setDevicePixelRatio(mask.devicePixelRatio());
    out.fill(Qt::transparent);
    QPainter tp(&out);
    tp.drawPixmap(0, 0, mask);
    // Replace every non-transparent pixel's color with `color`, keeping the
    // mask's alpha shape. Works for any single-color source (black or white).
    tp.setCompositionMode(QPainter::CompositionMode_SourceIn);
    tp.fillRect(out.rect(), color);
    tp.end();
    return out;
}

QString StatusBar::currentTimeText() const {
    return QDateTime::currentDateTime().toString(QStringLiteral("HH:mm"));
}

QString StatusBar::currentDateText() const {
    return QDateTime::currentDateTime().toString(QStringLiteral("yyyy/MM/dd"));
}

void StatusBar::onTimeout() {
    const QString nextTime = currentTimeText();
    const QString nextDate = currentDateText();
    if (nextTime != cached_time_ || nextDate != cached_date_) {
        cached_time_ = nextTime;
        cached_date_ = nextDate;
        update();
    }
}

// -- IPanel ----------------------------------------------------------------
PanelPosition StatusBar::position() const {
    return PanelPosition::Top;
}

int StatusBar::priority() const {
    return 100;
}

int StatusBar::preferredSize() const {
    return kBarHeight;
}

QWidget* StatusBar::widget() const {
    return const_cast<StatusBar*>(this);
}

// -- IStatusBar ------------------------------------------------------------
void StatusBar::setTimeVisible(bool visible) {
    if (time_visible_ != visible) {
        time_visible_ = visible;
        update();
    }
}

bool StatusBar::isTimeVisible() const {
    return time_visible_;
}

void StatusBar::setIconsVisible(bool visible) {
    if (icons_visible_ != visible) {
        icons_visible_ = visible;
        update();
    }
}

bool StatusBar::isIconsVisible() const {
    return icons_visible_;
}

void StatusBar::setStyle(StatusBarStyle s) {
    if (style_ != s) {
        style_ = s;
        update();
    }
}

StatusBarStyle StatusBar::style() const {
    return style_;
}

// -- Backdrop --------------------------------------------------------------
void StatusBar::setBackdropSource(cf::desktop::IShellLayer* source) {
    backdrop_source_ = source;
    frosted_.invalidate();
    update();
}

void StatusBar::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    // Debounce: collapse a burst of resizes (e.g. a window drag) into one
    // frosted-backdrop reblur rather than rebuilding on every intermediate size.
    if (reblur_debounce_ != nullptr) {
        reblur_debounce_->start();
    }
}

// -- Painting --------------------------------------------------------------
void StatusBar::paintEvent(QPaintEvent* /*event*/) {
    const CanvasUnitHelper h(devicePixelRatioF());
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::SmoothPixmapTransform, true);
    // 1. Frosted-glass backdrop at FULL opacity. The boot fade must only fade
    //    the content layer (clock, icons, chrome); if the glass faded too, the
    //    raw wallpaper would bleed through during the 250ms fade-in.
    p.setOpacity(1.0);
    const QImage backdrop =
        backdrop_source_ != nullptr ? backdrop_source_->currentBackgroundImage() : QImage{};
    const QRect strip = geometry();
    if (!backdrop.isNull() && !strip.isEmpty()) {
        p.drawPixmap(0, 0, frosted_.render(backdrop, strip, devicePixelRatioF(), frosted_params_));
        backdrop_null_warned_ = false;
    } else {
        // Flat fallback (e.g. before the wallpaper loads). Fail loud, once.
        QLinearGradient surface(0, 0, 0, height());
        surface.setColorAt(0.0, surface_top_color_);
        surface.setColorAt(1.0, background_color_);
        p.fillRect(rect(), surface);
        if (!backdrop_null_warned_) {
            backdrop_null_warned_ = true;
            cf::log::warningftag("StatusBar", "backdrop image null; using flat fallback");
        }
    }

    // 2. Content layer fades in on boot.
    p.setOpacity(fade_opacity_);

    // Clock region: date on the left, time on the right just before the icons
    // (Split) or centered (Centered). The icon-cluster left edge anchors the time.
    const qreal sideMargin = h.dpToPx(kSideMarginDp);
    const qreal iconSize = h.dpToPx(kIconSizeDp);
    const qreal iconGap = h.dpToPx(kIconGapDp);
    const qreal clusterWidth = icons_visible_ ? (4 * iconSize + 3 * iconGap) : 0;
    const qreal iconClusterLeft = width() - sideMargin - clusterWidth;

    if (time_visible_) {
        p.setPen(foreground_color_);
        p.setFont(clock_font_);
        const int sm = static_cast<int>(sideMargin);

        // Date: far left.
        p.drawText(QRect(sm, 0, width(), height()), Qt::AlignVCenter | Qt::AlignLeft, cached_date_);

        // Time: right-aligned to the icon cluster (Split) or centered (Centered).
        if (style_ == StatusBarStyle::Centered) {
            p.drawText(rect(), Qt::AlignCenter, cached_time_);
        } else {
            const int timeRight = static_cast<int>(iconClusterLeft - h.dpToPx(kTimeGapDp));
            p.drawText(QRect(sm, 0, timeRight - sm, height()), Qt::AlignVCenter | Qt::AlignRight,
                       cached_time_);
        }
    }

    // System-icon cluster (right-aligned): signal, battery, wifi, volume.
    if (icons_visible_) {
        const int y = static_cast<int>((height() - iconSize) / 2.0);
        qreal x = width() - sideMargin;
        auto cell = [&](qreal rightEdge) {
            return QRectF(rightEdge - iconSize, y, iconSize, iconSize);
        };

        // PNG mask tinted to the theme. A missing mask leaves a visible gap —
        // no silent fallback, so a broken resource stays obvious.
        auto drawStatusIcon = [&](StatusIcon kind, qreal rightEdge) {
            const QPixmap& mask = icon_masks_[static_cast<int>(kind)];
            if (mask.isNull()) {
                return;
            }
            p.drawPixmap(cell(rightEdge).toRect(), tintedPixmap(mask, icon_color_));
        };

        drawStatusIcon(StatusIcon::Volume, x);
        x -= (iconSize + iconGap);
        drawStatusIcon(StatusIcon::Wifi, x);
        x -= (iconSize + iconGap);
        drawStatusIcon(StatusIcon::Battery, x);
        x -= (iconSize + iconGap);
        drawStatusIcon(StatusIcon::Signal, x);
    }

    // In-band soft elevation shadow at the bottom seam (PanelManager locks the
    // widget height to preferredSize, so the cast shadow is drawn in-band).
    const qreal shadowH = h.dpToPx(kShadowBandDp);
    QLinearGradient shadow(0, height() - shadowH, 0, height());
    shadow.setColorAt(0.0, QColor(0, 0, 0, 0));
    shadow.setColorAt(1.0, QColor(0, 0, 0, 26)); // ~10% alpha
    p.fillRect(QRectF(0, height() - shadowH, width(), shadowH), shadow);

    // Horizontally-faded hairline (less "ruled" than a hard full-width line).
    QColor lineMid = divider_color_;
    lineMid.setAlphaF(0.45);
    QColor lineEdge = divider_color_;
    lineEdge.setAlphaF(0.0);
    QLinearGradient hairline(0, 0, width(), 0);
    hairline.setColorAt(0.0, lineEdge);
    hairline.setColorAt(0.08, lineMid);
    hairline.setColorAt(0.92, lineMid);
    hairline.setColorAt(1.0, lineEdge);
    p.setPen(QPen(hairline, h.dpToPx(1)));
    p.drawLine(QPointF(0, height() - h.dpToPx(0.5)), QPointF(width(), height() - h.dpToPx(0.5)));
}

} // namespace cf::desktop::desktop_component
