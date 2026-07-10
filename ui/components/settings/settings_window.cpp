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

#include <QButtonGroup>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QRadioButton>
#include <QScreen>
#include <QScrollArea>
#include <QSlider>
#include <QVBoxLayout>

#include <algorithm>

namespace cf::desktop::desktop_component {

using namespace qw::core::token::literals;

namespace {
constexpr int kWindowWidth = 720;  ///< Window width (px).
constexpr int kWindowHeight = 560; ///< Window height (px).
constexpr qreal kRadius = 16.0;    ///< Corner radius (px).
constexpr int kEnterSlidePx = 24;  ///< Enter/exit slide distance (px).
constexpr const char* kLogTag = "SettingsWindow";

/// @brief Card QSS: a rounded gradient surface (mirrors the home-page gadget
/// cards; deliberately not bound to the live theme, matching that style).
constexpr const char* kCardQss = "QWidget { background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
                                 " stop:0 #5D6D7E, stop:1 #2C3E50); border-radius: 12px; }";
constexpr const char* kTitleQss = "color: white; font-weight: bold; font-size: 13px;"
                                  " background: transparent; border: none;";
constexpr const char* kValueQss = "color: #1abc9c; font-size: 12px;"
                                  " background: transparent; border: none;";
constexpr const char* kHintQss = "color: #bdc3c7; font-size: 11px;"
                                 " background: transparent; border: none;";
constexpr const char* kRadioQss = "color: white; background: transparent; border: none;";
constexpr const char* kTabQss =
    "QTabWidget::pane { border: none; background: transparent; top: 0px; }"
    "QTabBar::tab { background: #E5E1E8; color: #2C3E50; padding: 8px 20px;"
    "  border-top-left-radius: 8px; border-top-right-radius: 8px; margin: 0 2px; }"
    "QTabBar::tab::selected { background: #6750A4; color: white; }";

/// @brief Writes one wallpaper config key (User layer, immediate notify).
void setWallpaperKey(const char* key, auto&& value) {
    namespace cfg = cf::config;
    cf::config::ConfigStore::instance()
        .domain("wallpaper")
        .set(cfg::KeyView{.group = "wallpaper", .key = key}, value, cfg::Layer::User,
             cfg::NotifyPolicy::Immediate);
    cf::config::ConfigStore::instance().sync();
}

/// @brief Reads one wallpaper config key with a default.
template <typename T> T wallpaperKey(const char* key, T fallback) {
    return cf::config::ConfigStore::instance()
        .domain("wallpaper")
        .query<T>(cf::config::KeyView{.group = "wallpaper", .key = key}, fallback);
}

/// @brief Builds an empty titled card; caller adds child widgets to its layout.
QWidget* makeCard(const QString& title, const QString& hint, QWidget* parent) {
    auto* card = new QWidget(parent);
    card->setStyleSheet(QString::fromLatin1(kCardQss));
    auto* layout = new QVBoxLayout(card);
    layout->setContentsMargins(16, 12, 16, 12);
    layout->setSpacing(6);
    auto* t = new QLabel(title, card);
    t->setStyleSheet(QString::fromLatin1(kTitleQss));
    layout->addWidget(t);
    if (!hint.isEmpty()) {
        auto* h = new QLabel(hint, card);
        h->setStyleSheet(QString::fromLatin1(kHintQss));
        h->setWordWrap(true);
        layout->addWidget(h);
    }
    return card;
}

/// @brief Styles a value label that mirrors a slider's current value.
QLabel* makeValueLabel(QWidget* parent, int ms) {
    auto* label = new QLabel(QString::number(ms) + QStringLiteral(" ms"), parent);
    label->setStyleSheet(QString::fromLatin1(kValueQss));
    return label;
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
    root->setContentsMargins(12, 12, 12, 12);
    root->setSpacing(8);

    auto* tabs = new qw::widget::material::TabView(this);
    tabs->setStyleSheet(QString::fromLatin1(kTabQss));
    tabs->addTab(buildWallpaperTab(), QStringLiteral("Wallpaper"));
    tabs->addTab(buildThemeTab(), QStringLiteral("Theme"));
    tabs->addTab(buildAboutTab(), QStringLiteral("About"));
    root->addWidget(tabs);
}

QWidget* SettingsWindow::buildWallpaperTab() {
    auto* tab = new QWidget(this);
    auto* layout = new QVBoxLayout(tab);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(10);

    // --- Animation enabled (disable_animation) ---
    {
        auto* card = makeCard(QStringLiteral("Animation"),
                              QStringLiteral("Auto-switch the wallpaper on a timer."), tab);
        auto* row = new QWidget(card);
        auto* h = new QHBoxLayout(row);
        h->setContentsMargins(0, 0, 0, 0);
        auto* sw = new qw::widget::material::Switch(QStringLiteral("Enabled"), row);
        sw->setChecked(!wallpaperKey<bool>("disable_animation", false));
        h->addWidget(sw);
        h->addStretch(1);
        card->layout()->addWidget(row);
        connect(sw, &QCheckBox::toggled, tab, [](bool on) {
            setWallpaperKey("disable_animation", !on);
            cf::log::infoftag(kLogTag, "wallpaper disable_animation -> {} (applies next start)",
                              !on);
        });
        layout->addWidget(card);
    }

    // --- Switch interval (switch_interval_ms) ---
    {
        const int cur = wallpaperKey<int>("switch_interval_ms", 20000);
        auto* card =
            makeCard(QStringLiteral("Switch interval"),
                     QStringLiteral("How long each wallpaper stays before switching."), tab);
        auto* row = new QWidget(card);
        auto* h = new QHBoxLayout(row);
        h->setContentsMargins(0, 0, 0, 0);
        auto* value = makeValueLabel(row, cur);
        auto* slider = new qw::widget::material::Slider(Qt::Horizontal, row);
        slider->setRange(2000, 60000);
        slider->setSingleStep(1000);
        slider->setValue(cur);
        h->addWidget(value, 0);
        h->addWidget(slider, 1);
        card->layout()->addWidget(row);
        connect(slider, &QSlider::valueChanged, tab, [value](int v) {
            value->setText(QString::number(v) + QStringLiteral(" ms"));
            setWallpaperKey("switch_interval_ms", v);
        });
        layout->addWidget(card);
    }

    // --- Transition duration (animation_duration_ms) ---
    {
        const int cur = wallpaperKey<int>("animation_duration_ms", 2000);
        auto* card = makeCard(QStringLiteral("Transition duration"),
                              QStringLiteral("Fade/move length when switching wallpapers."), tab);
        auto* row = new QWidget(card);
        auto* h = new QHBoxLayout(row);
        h->setContentsMargins(0, 0, 0, 0);
        auto* value = makeValueLabel(row, cur);
        auto* slider = new qw::widget::material::Slider(Qt::Horizontal, row);
        slider->setRange(200, 5000);
        slider->setSingleStep(100);
        slider->setValue(cur);
        h->addWidget(value, 0);
        h->addWidget(slider, 1);
        card->layout()->addWidget(row);
        connect(slider, &QSlider::valueChanged, tab, [value](int v) {
            value->setText(QString::number(v) + QStringLiteral(" ms"));
            setWallpaperKey("animation_duration_ms", v);
        });
        layout->addWidget(card);
    }

    // --- Switch mode (switch_mode: fixed/gradient/movement) ---
    {
        const QString cur =
            QString::fromStdString(wallpaperKey<std::string>("switch_mode", "movement"));
        auto* card = makeCard(QStringLiteral("Switch mode"),
                              QStringLiteral("fixed = no switch; gradient = cross-fade;"
                                             " movement = pan."),
                              tab);
        auto* group = new QButtonGroup(card);
        for (const QString& m :
             {QStringLiteral("fixed"), QStringLiteral("gradient"), QStringLiteral("movement")}) {
            auto* rb = new QRadioButton(m, card);
            rb->setStyleSheet(QString::fromLatin1(kRadioQss));
            rb->setChecked(m == cur);
            group->addButton(rb);
            card->layout()->addWidget(rb);
        }
        connect(group, &QButtonGroup::idClicked, tab, [group](int) {
            auto* rb = qobject_cast<QRadioButton*>(group->checkedButton());
            if (rb != nullptr) {
                setWallpaperKey("switch_mode", rb->text().toStdString());
            }
        });
        layout->addWidget(card);
    }

    // --- Selector (switch_selector: sequential/random) ---
    {
        const QString cur =
            QString::fromStdString(wallpaperKey<std::string>("switch_selector", "sequential"));
        auto* card =
            makeCard(QStringLiteral("Order"), QStringLiteral("sequential or random."), tab);
        auto* group = new QButtonGroup(card);
        for (const QString& m : {QStringLiteral("sequential"), QStringLiteral("random")}) {
            auto* rb = new QRadioButton(m, card);
            rb->setStyleSheet(QString::fromLatin1(kRadioQss));
            rb->setChecked(m == cur);
            group->addButton(rb);
            card->layout()->addWidget(rb);
        }
        connect(group, &QButtonGroup::idClicked, tab, [group](int) {
            auto* rb = qobject_cast<QRadioButton*>(group->checkedButton());
            if (rb != nullptr) {
                setWallpaperKey("switch_selector", rb->text().toStdString());
            }
        });
        layout->addWidget(card);
    }

    // --- Easing (switch_easing: inoutcubic/outcubic/linear) ---
    {
        const QString cur =
            QString::fromStdString(wallpaperKey<std::string>("switch_easing", "inoutcubic"));
        auto* card = makeCard(QStringLiteral("Easing"),
                              QStringLiteral("inoutcubic / outcubic / linear."), tab);
        auto* group = new QButtonGroup(card);
        for (const QString& m :
             {QStringLiteral("inoutcubic"), QStringLiteral("outcubic"), QStringLiteral("linear")}) {
            auto* rb = new QRadioButton(m, card);
            rb->setStyleSheet(QString::fromLatin1(kRadioQss));
            rb->setChecked(m == cur);
            group->addButton(rb);
            card->layout()->addWidget(rb);
        }
        connect(group, &QButtonGroup::idClicked, tab, [group](int) {
            auto* rb = qobject_cast<QRadioButton*>(group->checkedButton());
            if (rb != nullptr) {
                setWallpaperKey("switch_easing", rb->text().toStdString());
            }
        });
        layout->addWidget(card);
    }

    layout->addStretch(1);

    // Wrap in a scroll area: six cards can exceed the window height.
    auto* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setWidget(tab);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setAttribute(Qt::WA_TranslucentBackground, true);
    return scroll;
}

QWidget* SettingsWindow::buildThemeTab() {
    auto* tab = new QWidget(this);
    auto* layout = new QVBoxLayout(tab);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(10);

    QString current_name;
    try {
        current_name =
            QString::fromStdString(qw::core::ThemeManager::instance().currentThemeName());
    } catch (...) {
        current_name = QStringLiteral("light");
    }
    const bool dark = current_name == QStringLiteral("dark");

    auto* card = makeCard(QStringLiteral("Theme"),
                          QStringLiteral("Switch the active Material theme. The active one is "
                                         "highlighted (filled); both stay clickable."),
                          tab);
    using qw::widget::material::Button;
    // Filled = current (highlighted), Tonal = the other. Both stay enabled —
    // disabling the current one made it unclickable, so toggling back felt
    // broken ("theme switch did nothing").
    auto* light_btn =
        new Button(QStringLiteral("Light"),
                   dark ? Button::ButtonVariant::Tonal : Button::ButtonVariant::Filled, card);
    auto* dark_btn =
        new Button(QStringLiteral("Dark"),
                   dark ? Button::ButtonVariant::Filled : Button::ButtonVariant::Tonal, card);
    auto* row = new QWidget(card);
    auto* h = new QHBoxLayout(row);
    h->setContentsMargins(0, 0, 0, 0);
    h->addWidget(light_btn);
    h->addWidget(dark_btn);
    h->addStretch(1);
    card->layout()->addWidget(row);
    layout->addWidget(card, 1);

    connect(light_btn, &QPushButton::clicked, tab, [light_btn, dark_btn]() {
        qw::core::ThemeManager::instance().setThemeTo("light");
        light_btn->setVariant(Button::ButtonVariant::Filled);
        dark_btn->setVariant(Button::ButtonVariant::Tonal);
    });
    connect(dark_btn, &QPushButton::clicked, tab, [light_btn, dark_btn]() {
        qw::core::ThemeManager::instance().setThemeTo("dark");
        dark_btn->setVariant(Button::ButtonVariant::Filled);
        light_btn->setVariant(Button::ButtonVariant::Tonal);
    });

    return tab;
}

QWidget* SettingsWindow::buildAboutTab() {
    auto* tab = new QWidget(this);
    auto* layout = new QVBoxLayout(tab);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(10);

    auto* card = makeCard(QStringLiteral("About"), QString(), tab);
    auto* rich = new QLabel(card);
    rich->setTextFormat(Qt::RichText);
    rich->setOpenExternalLinks(true);
    rich->setStyleSheet("color: white; font-size: 13px; background: transparent; border: none;");
    rich->setText(QStringLiteral(
        "<p style='font-size:18px; font-weight:600;'>CFDesktop</p>"
        "<p>Version 0.19.0</p>"
        "<p>Cross-platform Desktop Environment Framework<br>"
        "C++23 / Qt 6.8 / Material Design 3</p>"
        "<p><a style='color:#1abc9c;' href='https://github.com/Charliechen114514/CFDesktop'>"
        "github.com/Charliechen114514/CFDesktop</a></p>"));
    rich->setWordWrap(true);
    // Center the text vertically inside the card; the card itself fills the tab.
    // makeCard() builds a QVBoxLayout, so the cast is safe.
    auto* card_layout = static_cast<QVBoxLayout*>(card->layout());
    card_layout->addStretch(1);
    card_layout->addWidget(rich);
    card_layout->addStretch(2);
    layout->addWidget(card, 1);
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
