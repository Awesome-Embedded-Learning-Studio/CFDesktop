/**
 * @file    control_center.cpp
 * @brief   Implementation of ControlCenter.
 *
 * @author  CFDesktop Team
 * @date    2026-07-10
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup control_center
 */

#include "control_center.h"

#include "notification_service.h"

#include "components/material/cfmaterial_fade_animation.h"
#include "components/material/cfmaterial_slide_animation.h"
#include "core/theme_manager.h"
#include "core/token/material_scheme/cfmaterial_token_literals.h"
#include "ui/widget/material/widget/button/button.h"
#include "ui/widget/material/widget/slider/slider.h"
#include "ui/widget/material/widget/switch/switch.h"

#include "cflog.h"

#include <QGuiApplication>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QScreen>
#include <QSlider>
#include <QVBoxLayout>

#include <algorithm>

namespace cf::desktop::desktop_component {

using namespace qw::core::token::literals;

namespace {
constexpr int kPanelWidth = 360;  ///< Fixed panel width (px).
constexpr int kPanelHeight = 360; ///< Fixed panel height (px).
constexpr qreal kRadius = 16.0;   ///< Corner radius (px).
constexpr int kSideMargin = 16;   ///< Gap from the screen edge (px).
constexpr int kEnterSlidePx = 24; ///< Enter/exit vertical slide distance (px).
constexpr const char* kLogTag = "ControlCenter";
} // namespace

ControlCenter::ControlCenter(QWidget* parent) : QWidget(parent) {
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    setAutoFillBackground(false);

    setupUi();
    applyTheme();
    setupAnimations();
    hide();

    try {
        connect(&qw::core::ThemeManager::instance(), &qw::core::ThemeManager::themeChanged, this,
                [this](const qw::core::ICFTheme&) { applyTheme(); });
    } catch (...) {
        // No theme registered yet; applyTheme() falls back below.
    }
}

ControlCenter::~ControlCenter() = default;

void ControlCenter::setupUi() {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(20, 20, 20, 20);
    root->setSpacing(14);

    using qw::widget::material::Button;
    using qw::widget::material::Slider;
    using qw::widget::material::Switch;

    // Slider row helper: label + horizontal slider.
    auto add_slider_row = [&](const QString& label, int init) -> Slider* {
        auto* row = new QWidget(this);
        auto* h = new QHBoxLayout(row);
        h->setContentsMargins(0, 0, 0, 0);
        auto* l = new QLabel(label, row);
        l->setMinimumWidth(72);
        auto* s = new Slider(Qt::Horizontal, row);
        s->setRange(0, 100);
        s->setValue(init);
        h->addWidget(l);
        h->addWidget(s, 1);
        root->addWidget(row);
        return s;
    };

    brightness_ = add_slider_row(QStringLiteral("Brightness"), 80);
    volume_ = add_slider_row(QStringLiteral("Volume"), 60);
    // UI-only this phase: log the intent, no hardware backend yet.
    connect(brightness_, &QSlider::valueChanged, this,
            [](int v) { cf::log::infoftag(kLogTag, "brightness -> {} (UI-only)", v); });
    connect(volume_, &QSlider::valueChanged, this,
            [](int v) { cf::log::infoftag(kLogTag, "volume -> {} (UI-only)", v); });

    // Toggle row: Wi-Fi, Bluetooth, DND.
    auto* switches = new QWidget(this);
    auto* sw = new QHBoxLayout(switches);
    sw->setContentsMargins(0, 0, 0, 0);
    wifi_ = new Switch(QStringLiteral("Wi-Fi"), switches);
    bluetooth_ = new Switch(QStringLiteral("Bluetooth"), switches);
    dnd_ = new Switch(QStringLiteral("DND"), switches);
    wifi_->setChecked(false);
    bluetooth_->setChecked(false);
    dnd_->setChecked(NotificationService::instance().isDndEnabled());
    sw->addWidget(wifi_);
    sw->addWidget(bluetooth_);
    sw->addWidget(dnd_);
    sw->addStretch(1);
    root->addWidget(switches);

    connect(wifi_, &QCheckBox::toggled, this,
            [](bool on) { cf::log::infoftag(kLogTag, "wi-fi={} (UI-only)", on); });
    connect(bluetooth_, &QCheckBox::toggled, this,
            [](bool on) { cf::log::infoftag(kLogTag, "bluetooth={} (UI-only)", on); });
    // DND is real: persists to ConfigStore + drives banner suppression.
    connect(dnd_, &QCheckBox::toggled, this,
            [](bool on) { NotificationService::instance().setDndEnabled(on); });

    // Button row: theme toggle + screenshot placeholder.
    auto* btns = new QWidget(this);
    auto* bh = new QHBoxLayout(btns);
    bh->setContentsMargins(0, 0, 0, 0);
    theme_btn_ = new Button(QStringLiteral("Toggle theme"), Button::ButtonVariant::Tonal, btns);
    screenshot_btn_ =
        new Button(QStringLiteral("Screenshot"), Button::ButtonVariant::Outlined, btns);
    bh->addWidget(theme_btn_);
    bh->addWidget(screenshot_btn_);
    bh->addStretch(1);
    root->addWidget(btns);

    connect(theme_btn_, &QPushButton::clicked, this, []() {
        auto& tm = qw::core::ThemeManager::instance();
        const bool dark = tm.currentThemeName() == "dark";
        tm.setThemeTo(dark ? "light" : "dark");
    });
    connect(screenshot_btn_, &QPushButton::clicked, this,
            []() { cf::log::infoftag(kLogTag, "screenshot requested (placeholder)"); });

    // Test stub: posts a "Hello World" notification so the banner + center can
    // be exercised end-to-end without an IPC client. Drop once real producers
    // arrive.
    auto* testRow = new QWidget(this);
    auto* th = new QHBoxLayout(testRow);
    th->setContentsMargins(0, 0, 0, 0);
    test_notif_btn_ = new Button(QStringLiteral("Send test notification"),
                                 Button::ButtonVariant::Filled, testRow);
    th->addWidget(test_notif_btn_);
    th->addStretch(1);
    root->addWidget(testRow);
    connect(test_notif_btn_, &QPushButton::clicked, this, []() {
        Notification n;
        n.title = "Hello World";
        n.message = "Test notification from the control center";
        n.app_id = "org.cf.control_center";
        NotificationService::instance().post(n);
    });
}

void ControlCenter::popup(const QRect& available) {
    QRect avail = available;
    if (!avail.isValid() || avail.width() <= 0 || avail.height() <= 0) {
        if (const auto* screen = QGuiApplication::primaryScreen()) {
            avail = screen->availableGeometry();
        }
    }
    const int w = kPanelWidth;
    // Height follows the layout so switches / buttons are never clipped (a
    // fixed 360 px left the bottom row cut off). Bounded to the work area.
    layout()->activate();
    const int h = std::min(layout()->sizeHint().height(), avail.height() - kSideMargin * 2);
    const int x = avail.right() - w - kSideMargin;
    const int y = avail.top() + kSideMargin;
    setGeometry(x, y, w, h);

    // Refresh the DND toggle from the store. Block signals so the programmatic
    // setChecked does not re-fire toggled -> setDndEnabled (echo write loop).
    dnd_->blockSignals(true);
    dnd_->setChecked(NotificationService::instance().isDndEnabled());
    dnd_->blockSignals(false);

    enter_slide_->start();
    enter_fade_->start();
    show();
    raise();
}

void ControlCenter::hidePanel() {
    if (!isVisible()) {
        hide();
        return;
    }
    exit_slide_->start();
    exit_fade_->start();
}

bool ControlCenter::isShowing() const noexcept {
    return isVisible();
}

void ControlCenter::paintEvent(QPaintEvent* /*event*/) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    QPainterPath surface;
    surface.addRoundedRect(QRectF(rect()), kRadius, kRadius);
    p.fillPath(surface, surface_color_);
}

void ControlCenter::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape) {
        hidePanel();
        return;
    }
    QWidget::keyPressEvent(event);
}

void ControlCenter::applyTheme() {
    try {
        auto& tm = qw::core::ThemeManager::instance();
        const auto& theme = tm.theme(tm.currentThemeName());
        surface_color_ = theme.color_scheme().queryColor(SURFACE);
    } catch (...) {
        surface_color_ = QColor(0xF7, 0xF5, 0xF3);
    }
    update();
}

void ControlCenter::setupAnimations() {
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

    enter_slide_ = new CFMaterialSlideAnimation(spec, SlideDirection::Up, this);
    enter_slide_->setRange(static_cast<float>(-kEnterSlidePx), 0.0f);
    enter_slide_->setMotionToken("mediumEnter");
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
