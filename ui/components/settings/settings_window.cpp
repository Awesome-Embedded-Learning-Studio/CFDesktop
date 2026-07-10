/**
 * @file    settings_window.cpp
 * @brief   Implementation of SettingsWindow.
 *
 * @author  CFDesktop Team
 * @date    2026-07-10
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup settings
 */

#include "settings_window.h"

#include "cfconfig.hpp"
#include "cflog.h"

#include "components/material/cfmaterial_fade_animation.h"
#include "components/material/cfmaterial_slide_animation.h"
#include "core/theme_manager.h"
#include "core/token/material_scheme/cfmaterial_token_literals.h"
#include "ui/widget/material/widget/button/button.h"
#include "ui/widget/material/widget/slider/slider.h"
#include "ui/widget/material/widget/switch/switch.h"
#include "ui/widget/material/widget/tabview/tabview.h"

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
constexpr int kWindowWidth = 720;  ///< Window width (px).
constexpr int kWindowHeight = 520; ///< Window height (px).
constexpr qreal kRadius = 16.0;    ///< Corner radius (px).
constexpr int kEnterSlidePx = 24;  ///< Enter/exit slide distance (px).
constexpr const char* kLogTag = "SettingsWindow";

/// @brief Writes one wallpaper config key (User layer, immediate notify).
void setWallpaperKey(const char* key, auto&& value) {
    namespace cfg = cf::config;
    cf::config::ConfigStore::instance()
        .domain("wallpaper")
        .set(cfg::KeyView{.group = "wallpaper", .key = key}, value, cfg::Layer::User,
             cfg::NotifyPolicy::Immediate);
    cf::config::ConfigStore::instance().sync();
}
} // namespace

SettingsWindow::SettingsWindow(QWidget* parent) : QWidget(parent) {
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

SettingsWindow::~SettingsWindow() = default;

void SettingsWindow::setupUi() {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    auto* tabs = new qw::widget::material::TabView(this);
    tabs->addTab(buildWallpaperTab(), QStringLiteral("Wallpaper"));
    tabs->addTab(buildThemeTab(), QStringLiteral("Theme"));
    tabs->addTab(buildAboutTab(), QStringLiteral("About"));
    root->addWidget(tabs);
}

QWidget* SettingsWindow::buildWallpaperTab() {
    auto* tab = new QWidget(this);
    auto* layout = new QVBoxLayout(tab);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(12);

    namespace cfg = cf::config;
    auto domain = cf::config::ConfigStore::instance().domain("wallpaper");

    // Animation enabled = NOT disable_animation. Live wallpaper reload is the
    // engine's job; settings only persist. Values apply on next engine start.
    auto* anim = new qw::widget::material::Switch(QStringLiteral("Animation enabled"), tab);
    anim->setChecked(
        !domain.query<bool>(cfg::KeyView{.group = "wallpaper", .key = "disable_animation"}, false));
    layout->addWidget(anim);
    connect(anim, &QCheckBox::toggled, tab, [](bool on) {
        setWallpaperKey("disable_animation", !on);
        cf::log::infoftag(kLogTag, "wallpaper disable_animation -> {} (applies next start)", !on);
    });

    // Switch interval slider (2-60 s).
    auto* irow = new QWidget(tab);
    auto* ih = new QHBoxLayout(irow);
    ih->setContentsMargins(0, 0, 0, 0);
    auto* ilab = new QLabel(QStringLiteral("Switch interval"), irow);
    ilab->setMinimumWidth(130);
    auto* isld = new qw::widget::material::Slider(Qt::Horizontal, irow);
    isld->setRange(2000, 60000);
    isld->setSingleStep(1000);
    isld->setValue(
        domain.query<int>(cfg::KeyView{.group = "wallpaper", .key = "switch_interval_ms"}, 20000));
    ih->addWidget(ilab);
    ih->addWidget(isld, 1);
    layout->addWidget(irow);
    connect(isld, &QSlider::valueChanged, tab,
            [](int v) { setWallpaperKey("switch_interval_ms", v); });

    // Transition duration slider (200-5000 ms).
    auto* drow = new QWidget(tab);
    auto* dh = new QHBoxLayout(drow);
    dh->setContentsMargins(0, 0, 0, 0);
    auto* dlab = new QLabel(QStringLiteral("Transition duration"), drow);
    dlab->setMinimumWidth(130);
    auto* dsld = new qw::widget::material::Slider(Qt::Horizontal, drow);
    dsld->setRange(200, 5000);
    dsld->setSingleStep(100);
    dsld->setValue(domain.query<int>(
        cfg::KeyView{.group = "wallpaper", .key = "animation_duration_ms"}, 2000));
    dh->addWidget(dlab);
    dh->addWidget(dsld, 1);
    layout->addWidget(drow);
    connect(dsld, &QSlider::valueChanged, tab,
            [](int v) { setWallpaperKey("animation_duration_ms", v); });

    layout->addStretch(1);
    return tab;
}

QWidget* SettingsWindow::buildThemeTab() {
    auto* tab = new QWidget(this);
    auto* layout = new QVBoxLayout(tab);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(12);

    using qw::widget::material::Button;
    auto* light_btn = new Button(QStringLiteral("Light"), Button::ButtonVariant::Tonal, tab);
    auto* dark_btn = new Button(QStringLiteral("Dark"), Button::ButtonVariant::Tonal, tab);
    layout->addWidget(light_btn);
    layout->addWidget(dark_btn);

    // Live: ThemeManager broadcasts themeChanged, so every panel re-themes.
    connect(light_btn, &QPushButton::clicked, tab,
            []() { qw::core::ThemeManager::instance().setThemeTo("light"); });
    connect(dark_btn, &QPushButton::clicked, tab,
            []() { qw::core::ThemeManager::instance().setThemeTo("dark"); });

    layout->addStretch(1);
    return tab;
}

QWidget* SettingsWindow::buildAboutTab() {
    auto* tab = new QWidget(this);
    auto* layout = new QVBoxLayout(tab);
    layout->setContentsMargins(16, 16, 16, 16);
    auto* label = new QLabel(QStringLiteral("CFDesktop v0.19.0\n\n"
                                            "Cross-platform Desktop Environment Framework\n"
                                            "C++23 / Qt 6.8 / Material Design 3\n\n"
                                            "github.com/Charliechen114514/CFDesktop"),
                             tab);
    label->setWordWrap(true);
    layout->addWidget(label);
    layout->addStretch(1);
    return tab;
}

void SettingsWindow::popup(const QRect& available) {
    QRect avail = available;
    if (!avail.isValid() || avail.width() <= 0 || avail.height() <= 0) {
        if (const auto* screen = QGuiApplication::primaryScreen()) {
            avail = screen->availableGeometry();
        }
    }
    const int w = kWindowWidth;
    const int h = kWindowHeight;
    setGeometry(avail.center().x() - w / 2, avail.center().y() - h / 2, w, h);

    enter_slide_->start();
    enter_fade_->start();
    show();
    raise();
}

void SettingsWindow::hidePanel() {
    if (!isVisible()) {
        hide();
        return;
    }
    exit_slide_->start();
    exit_fade_->start();
}

bool SettingsWindow::isShowing() const noexcept {
    return isVisible();
}

void SettingsWindow::paintEvent(QPaintEvent* /*event*/) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    QPainterPath surface;
    surface.addRoundedRect(QRectF(rect()), kRadius, kRadius);
    p.fillPath(surface, surface_color_);
}

void SettingsWindow::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape) {
        hidePanel();
        return;
    }
    QWidget::keyPressEvent(event);
}

void SettingsWindow::applyTheme() {
    try {
        auto& tm = qw::core::ThemeManager::instance();
        surface_color_ = tm.theme(tm.currentThemeName()).color_scheme().queryColor(SURFACE);
    } catch (...) {
        surface_color_ = QColor(0xF7, 0xF5, 0xF3);
    }
    update();
}

void SettingsWindow::setupAnimations() {
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
    exit_slide_->setMotionToken("shortExit");
    exit_slide_->setTargetWidget(this);
}

} // namespace cf::desktop::desktop_component
