/**
 * @file    notification_banner.cpp
 * @brief   Implementation of NotificationBanner.
 *
 * @author  CFDesktop Team
 * @date    2026-07-10
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup notification
 */

#include "notification_banner.h"

#include "notification_card_widget.h"

#include "components/material/cfmaterial_fade_animation.h"
#include "components/material/cfmaterial_slide_animation.h"
#include "core/theme_manager.h"

#include <QKeyEvent>
#include <QScreen>
#include <QVBoxLayout>

namespace cf::desktop::desktop_component {

namespace {
constexpr int kAutoHideMs = 5000; ///< Auto-hide delay (ms).
constexpr int kEnterSlidePx = 24; ///< Enter/exit vertical slide distance (px).
constexpr int kMargin = 16;       ///< Gap from the screen edge (px).
} // namespace

NotificationBanner::NotificationBanner(QWidget* parent) : QWidget(parent) {
    // Frameless translucent child: the card paints its own rounded surface,
    // the area outside stays transparent. Not Qt::Popup so it does not fight
    // the window manager on windowless targets (same rationale as AppLauncher).
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    setAutoFillBackground(false);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    card_ = new NotificationCardWidget(this);
    layout->addWidget(card_);
    connect(card_, &NotificationCardWidget::dismissRequested, this,
            &NotificationBanner::hideBanner);

    auto_hide_timer_ = new QTimer(this);
    auto_hide_timer_->setSingleShot(true);
    connect(auto_hide_timer_, &QTimer::timeout, this, &NotificationBanner::hideBanner);

    setupAnimations();
    hide();
}

NotificationBanner::~NotificationBanner() = default;

void NotificationBanner::showFor(const Notification& notification, const QRect& available) {
    card_->setNotification(notification);
    adjustSize();

    QRect avail = available;
    if (!avail.isValid() || avail.width() <= 0 || avail.height() <= 0) {
        if (const auto* screen = QWidget::window()->screen()) {
            avail = screen->availableGeometry();
        }
    }

    // Anchor to the top-center of the available area, in desktop-local coords.
    const int w = card_->sizeHint().width();
    const int h = card_->sizeHint().height();
    const int x = avail.center().x() - w / 2;
    const int y = avail.top() + kMargin;
    setGeometry(x, y, w, h);

    // Start the enter pair before show() so the first frame is the animated
    // (transparent + offset) state, never a fully opaque flash.
    enter_slide_->start();
    enter_fade_->start();
    show();
    raise();
    auto_hide_timer_->start(kAutoHideMs);
}

void NotificationBanner::hideBanner() {
    if (!isVisible()) {
        hide();
        return;
    }
    auto_hide_timer_->stop();
    exit_slide_->start();
    exit_fade_->start();
}

bool NotificationBanner::isShowing() const noexcept {
    return isVisible();
}

void NotificationBanner::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape) {
        hideBanner();
        return;
    }
    QWidget::keyPressEvent(event);
}

void NotificationBanner::setupAnimations() {
    qw::core::IMotionSpec* spec = nullptr;
    try {
        auto& tm = qw::core::ThemeManager::instance();
        spec = &tm.theme(tm.currentThemeName()).motion_spec();
    } catch (...) {
        // No theme registered yet; animations fall back to default timing.
    }

    using qw::components::material::CFMaterialFadeAnimation;
    using qw::components::material::CFMaterialSlideAnimation;
    using qw::components::material::SlideDirection;

    enter_fade_ = new CFMaterialFadeAnimation(spec, this);
    enter_fade_->setRange(0.0f, 1.0f);
    enter_fade_->setMotionToken("mediumEnter");
    enter_fade_->setTargetWidget(this);

    enter_slide_ = new CFMaterialSlideAnimation(spec, SlideDirection::Up, this);
    enter_slide_->setRange(static_cast<float>(-kEnterSlidePx), 0.0f);
    enter_slide_->setMotionToken("longEnter");
    enter_slide_->setTargetWidget(this);

    exit_fade_ = new CFMaterialFadeAnimation(spec, this);
    exit_fade_->setRange(1.0f, 0.0f);
    exit_fade_->setMotionToken("shortExit");
    exit_fade_->setTargetWidget(this);
    connect(exit_fade_, &qw::components::ICFAbstractAnimation::finished, this,
            [this]() { hide(); });

    exit_slide_ = new CFMaterialSlideAnimation(spec, SlideDirection::Down, this);
    exit_slide_->setRange(0.0f, static_cast<float>(kEnterSlidePx));
    exit_slide_->setMotionToken("mediumExit");
    exit_slide_->setTargetWidget(this);
}

} // namespace cf::desktop::desktop_component
