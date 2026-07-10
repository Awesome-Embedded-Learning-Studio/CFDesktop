/**
 * @file    notification_center_panel.cpp
 * @brief   Implementation of NotificationCenterPanel.
 *
 * @author  CFDesktop Team
 * @date    2026-07-10
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup notification
 */

#include "notification_center_panel.h"

#include "notification.h"
#include "notification_card_widget.h"
#include "notification_service.h"

#include "components/material/cfmaterial_fade_animation.h"
#include "components/material/cfmaterial_slide_animation.h"
#include "core/theme_manager.h"
#include "core/token/material_scheme/cfmaterial_token_literals.h"

#include <QGuiApplication>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QScreen>
#include <QScrollArea>
#include <QVBoxLayout>

#include <algorithm>

namespace cf::desktop::desktop_component {

using namespace qw::core::token::literals;

namespace {
constexpr int kPanelWidth = 360;   ///< Fixed panel width (px).
constexpr int kHeaderHeight = 44;  ///< Header band height (px).
constexpr int kRadius = 16.0;      ///< Corner radius (px).
constexpr int kSideMargin = 16;    ///< Gap from the screen edge (px).
constexpr int kEnterSlidePx = 24;  ///< Enter/exit horizontal slide distance (px).
constexpr int kClearAllWidth = 80; ///< Clear-all hotspot width (px).
} // namespace

NotificationCenterPanel::NotificationCenterPanel(QWidget* parent) : QWidget(parent) {
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    setAutoFillBackground(false);

    setupUi();
    applyTheme();
    setupAnimations();
    hide();

    // Live theme switches.
    try {
        connect(&qw::core::ThemeManager::instance(), &qw::core::ThemeManager::themeChanged, this,
                [this](const qw::core::ICFTheme&) { applyTheme(); });
    } catch (...) {
        // No theme registered yet; applyTheme() falls back below.
    }

    // Stay live while open: any service change triggers a rebuild.
    auto& svc = NotificationService::instance();
    connect(&svc, &NotificationService::notificationPosted, this,
            [this](const Notification&, bool) { rebuildList(); });
    connect(&svc, &NotificationService::notificationDismissed, this,
            [this](const QString&) { rebuildList(); });
    connect(&svc, &NotificationService::allCleared, this, [this]() { rebuildList(); });
}

NotificationCenterPanel::~NotificationCenterPanel() = default;

void NotificationCenterPanel::setupUi() {
    auto* outer = new QVBoxLayout(this);
    outer->setContentsMargins(0, kHeaderHeight, 0, 0);
    outer->setSpacing(0);

    scroll_ = new QScrollArea(this);
    scroll_->setWidgetResizable(true);
    scroll_->setFrameShape(QFrame::NoFrame);
    scroll_->setAttribute(Qt::WA_TranslucentBackground, true);

    list_container_ = new QWidget(scroll_);
    list_layout_ = new QVBoxLayout(list_container_);
    list_layout_->setContentsMargins(kSideMargin, 0, kSideMargin, kSideMargin);
    list_layout_->setSpacing(8);
    list_layout_->addStretch(1);
    scroll_->setWidget(list_container_);
    outer->addWidget(scroll_);

    clear_all_rect_ =
        QRect(kPanelWidth - kClearAllWidth - kSideMargin / 2, 0, kClearAllWidth, kHeaderHeight);
}

void NotificationCenterPanel::popup(const QRect& available) {
    QRect avail = available;
    if (!avail.isValid() || avail.width() <= 0 || avail.height() <= 0) {
        if (const auto* screen = QGuiApplication::primaryScreen()) {
            avail = screen->availableGeometry();
        }
    }

    const int w = kPanelWidth;
    const int h = std::clamp(avail.height() - kSideMargin * 2, 320, 720);
    const int x = avail.right() - w - kSideMargin;
    const int y = avail.top() + kSideMargin;
    setGeometry(x, y, w, h);

    rebuildList();

    enter_slide_->start();
    enter_fade_->start();
    show();
    raise();
}

void NotificationCenterPanel::hidePanel() {
    if (!isVisible()) {
        hide();
        return;
    }
    exit_slide_->start();
    exit_fade_->start();
}

bool NotificationCenterPanel::isShowing() const noexcept {
    return isVisible();
}

void NotificationCenterPanel::paintEvent(QPaintEvent* /*event*/) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    // Rounded panel surface.
    QPainterPath surface;
    surface.addRoundedRect(QRectF(rect()), kRadius, kRadius);
    p.fillPath(surface, surface_color_);

    // Header title.
    QFont title_font = font();
    title_font.setBold(true);
    title_font.setPixelSize(15);
    p.setFont(title_font);
    p.setPen(title_color_);
    p.drawText(QRect(kSideMargin, 0, width() - kSideMargin * 2, kHeaderHeight),
               Qt::AlignVCenter | Qt::AlignLeft, QStringLiteral("Notifications"));

    // Clear-all label (right-aligned inside its hotspot).
    QFont clear_font = font();
    clear_font.setPixelSize(13);
    p.setFont(clear_font);
    p.setPen(clear_color_);
    p.drawText(clear_all_rect_, Qt::AlignVCenter | Qt::AlignRight, QStringLiteral("Clear all"));

    // Empty-state hint when there is nothing to show.
    if (NotificationService::instance().count() == 0) {
        QFont hint_font = font();
        hint_font.setPixelSize(13);
        p.setFont(hint_font);
        p.setPen(hint_color_);
        p.drawText(QRect(0, kHeaderHeight, width(), height() - kHeaderHeight), Qt::AlignCenter,
                   QStringLiteral("No notifications"));
    }
}

void NotificationCenterPanel::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape) {
        hidePanel();
        return;
    }
    QWidget::keyPressEvent(event);
}

void NotificationCenterPanel::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && clear_all_rect_.contains(event->pos())) {
        pressed_on_clear_ = true;
        return;
    }
    QWidget::mousePressEvent(event);
}

void NotificationCenterPanel::mouseReleaseEvent(QMouseEvent* event) {
    if (pressed_on_clear_ && event->button() == Qt::LeftButton &&
        clear_all_rect_.contains(event->pos())) {
        pressed_on_clear_ = false;
        NotificationService::instance().clearAll();
        return;
    }
    pressed_on_clear_ = false;
    QWidget::mouseReleaseEvent(event);
}

void NotificationCenterPanel::applyTheme() {
    try {
        auto& tm = qw::core::ThemeManager::instance();
        const auto& theme = tm.theme(tm.currentThemeName());
        auto& cs = theme.color_scheme();
        surface_color_ = cs.queryColor(SURFACE);
        title_color_ = cs.queryColor(ON_SURFACE);
        clear_color_ = cs.queryColor(PRIMARY);
        hint_color_ = cs.queryColor(ON_SURFACE_VARIANT);
    } catch (...) {
        surface_color_ = QColor(0xF7, 0xF5, 0xF3);
        title_color_ = QColor(0x1C, 0x1B, 0x1F);
        clear_color_ = QColor(0x67, 0x50, 0xA4);
        hint_color_ = QColor(0x49, 0x45, 0x4E);
    }
    update();
}

void NotificationCenterPanel::rebuildList() {
    // Drop old cards but keep the trailing stretch item (count == 1 means
    // only the stretch remains). Cards are direct children of list_container_.
    while (list_layout_->count() > 1) {
        QLayoutItem* item = list_layout_->takeAt(0);
        if (item != nullptr && item->widget() != nullptr) {
            delete item->widget(); // synchronous: no dangling cards between rebuilds
        }
        delete item;
    }

    const auto all = NotificationService::instance().all();
    for (const auto& n : all) {
        auto* card = new NotificationCardWidget(list_container_);
        card->setNotification(n);
        connect(card, &NotificationCardWidget::dismissRequested, this,
                [](const QString& id) { NotificationService::instance().dismiss(id); });
        // Insert before the trailing stretch (last index).
        list_layout_->insertWidget(list_layout_->count() - 1, card);
    }
    update();
}

void NotificationCenterPanel::setupAnimations() {
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
    enter_fade_->setMotionToken("shortEnter");
    enter_fade_->setTargetWidget(this);

    // Slide in from the right edge.
    enter_slide_ = new CFMaterialSlideAnimation(spec, SlideDirection::Left, this);
    enter_slide_->setRange(static_cast<float>(-kEnterSlidePx), 0.0f);
    enter_slide_->setMotionToken("mediumEnter");
    enter_slide_->setTargetWidget(this);

    exit_fade_ = new CFMaterialFadeAnimation(spec, this);
    exit_fade_->setRange(1.0f, 0.0f);
    exit_fade_->setMotionToken("shortExit");
    exit_fade_->setTargetWidget(this);
    connect(exit_fade_, &qw::components::ICFAbstractAnimation::finished, this,
            [this]() { hide(); });

    exit_slide_ = new CFMaterialSlideAnimation(spec, SlideDirection::Right, this);
    exit_slide_->setRange(0.0f, static_cast<float>(kEnterSlidePx));
    exit_slide_->setMotionToken("mediumExit");
    exit_slide_->setTargetWidget(this);
}

} // namespace cf::desktop::desktop_component
