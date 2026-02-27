/**
 * @file MaterialGalleryMainWindow.cpp
 * @brief Main window for the Material Design 3 Gallery - Implementation
 *
 * @author CFDesktop Team
 * @date 2026-02-28
 * @version 0.1
 */

#include "MaterialGalleryMainWindow.h"
#include "ColorSchemePage.h"
#include "MotionSpecPage.h"
#include "RadiusScalePage.h"
#include "ThemePageWidget.h"
#include "ThemeSidebar.h"
#include "TypographyPage.h"

#include "ui/core/material/cfmaterial_scheme.h"
#include "ui/core/material/material_factory.hpp"
#include "ui/core/material/material_factory_class.h"
#include "ui/core/theme_factory.h"
#include "ui/core/theme_manager.h"
#include "ui/core/token/theme_name/material_theme_name.h"

#include <QMouseEvent>
#include <QPainter>

// Global namespace alias for token literals
namespace token_literals = ::cf::ui::core::token::literals;

namespace cf::ui::gallery {

// =============================================================================
// ThemeSwitch Implementation
// =============================================================================

ThemeSwitch::ThemeSwitch(QWidget* parent) : QWidget(parent) {
    setFixedSize(56, 28);

    animation_ = new QPropertyAnimation(this, "knobPosition", this);
    animation_->setDuration(200);
    animation_->setEasingCurve(QEasingCurve::OutCubic);
}

void ThemeSwitch::setDark(bool dark) {
    if (isDark_ != dark) {
        isDark_ = dark;
        float targetPos = dark ? 1.0f : 0.0f;
        animation_->stop();
        animation_->setStartValue(knobPosition_);
        animation_->setEndValue(targetPos);
        animation_->start();
        emit themeChanged(isDark_);
    }
}

void ThemeSwitch::setKnobPosition(float pos) {
    knobPosition_ = qBound(0.0f, pos, 1.0f);
    update();
    emit knobPositionChanged();
}

void ThemeSwitch::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRectF r = rect();
    qreal radius = r.height() / 2;

    // Track background
    QColor trackColor = isDark_ ? QColor(103, 80, 164) : QColor(180, 180, 180);
    QPainterPath trackPath;
    trackPath.addRoundedRect(r, radius, radius);
    painter.setPen(Qt::NoPen);
    painter.setBrush(trackColor);
    painter.drawPath(trackPath);

    // Knob
    qreal knobDiameter = r.height() - 6;
    qreal knobX = 3 + knobPosition_ * (r.width() - knobDiameter - 6);
    QRectF knobRect(knobX, 3, knobDiameter, knobDiameter);

    QPainterPath knobPath;
    knobPath.addEllipse(knobRect);

    // Knob shadow
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 0, 0, 30));
    painter.drawEllipse(knobRect.translated(0, 1));

    // Knob
    painter.setBrush(Qt::white);
    painter.drawPath(knobPath);

    // Icon on knob (sun or moon)
    painter.setPen(isDark_ ? QColor(60, 60, 60) : QColor(255, 200, 50));
    QFont iconFont("Segoe UI Emoji", 11);
    painter.setFont(iconFont);
    QString icon = isDark_ ? "🌙" : "☀️";
    painter.drawText(knobRect, Qt::AlignCenter, icon);
}

void ThemeSwitch::mousePressEvent(QMouseEvent*) {
    setDark(!isDark_);
}

// =============================================================================
// MaterialGalleryMainWindow Implementation
// =============================================================================

MaterialGalleryMainWindow::MaterialGalleryMainWindow(QWidget* parent) : QMainWindow(parent) {

    setupThemeManager();
    setupUI();
    createHeader();

    // Create pages and add to stacked widget
    colorSchemePage_ = new ColorSchemePage(this);
    motionSpecPage_ = new MotionSpecPage(this);
    radiusScalePage_ = new RadiusScalePage(this);
    typographyPage_ = new TypographyPage(this);

    contentStack_->addWidget(colorSchemePage_);
    contentStack_->addWidget(motionSpecPage_);
    contentStack_->addWidget(radiusScalePage_);
    contentStack_->addWidget(typographyPage_);

    // Set initial page
    contentStack_->setCurrentIndex(0);

    themeManager_->setThemeTo(token_literals::MATERIAL_THEME_LIGHT);

    // Apply initial theme
    if (themeManager_) {
        auto& theme = themeManager_->theme(token_literals::MATERIAL_THEME_LIGHT);
        applyThemeToAllPages(theme);
        sidebar_->applyTheme(theme);
    }
}

MaterialGalleryMainWindow::~MaterialGalleryMainWindow() {
    // Cleanup is handled by Qt's parent-child system
}

void MaterialGalleryMainWindow::setupThemeManager() {
    themeManager_ = &core::ThemeManager::instance();

    // Register MaterialFactory for both light and dark themes
    auto installMaterialTheme = []() { return std::make_unique<core::MaterialFactory>(); };
    // Register both theme names - the same factory handles both
    themeManager_->insert_one(token_literals::MATERIAL_THEME_LIGHT, installMaterialTheme);
    themeManager_->insert_one(token_literals::MATERIAL_THEME_DARK, installMaterialTheme);

    // Install main window for theme updates
    themeManager_->install_widget(this);

    // Connect themeChanged signal
    connect(themeManager_, &core::ThemeManager::themeChanged, this,
            &MaterialGalleryMainWindow::onThemeManagerChanged);
}

void MaterialGalleryMainWindow::setupUI() {
    // Window setup
    setWindowTitle("Material Design 3 Gallery");
    resize(1200, 800);
    setMinimumSize(900, 600);

    // Central widget
    centralWidget_ = new QWidget(this);
    setCentralWidget(centralWidget_);

    // Main horizontal layout (sidebar + content)
    mainLayout_ = new QHBoxLayout(centralWidget_);
    mainLayout_->setContentsMargins(0, 0, 0, 0);
    mainLayout_->setSpacing(0);

    // Sidebar
    sidebar_ = new ThemeSidebar(this);
    mainLayout_->addWidget(sidebar_);

    // Connect sidebar signal
    connect(sidebar_, &ThemeSidebar::tabClicked, this, &MaterialGalleryMainWindow::onTabClicked);

    // Content area (vertical layout with header + stacked widget)
    QWidget* contentWidget = new QWidget(this);
    contentLayout_ = new QVBoxLayout(contentWidget);
    contentLayout_->setContentsMargins(0, 0, 0, 0);
    contentLayout_->setSpacing(0);

    mainLayout_->addWidget(contentWidget, 1); // Stretch factor 1

    // Content stack for page switching
    contentStack_ = new QStackedWidget(this);
    contentLayout_->addWidget(contentStack_, 1); // Stretch factor 1
}

void MaterialGalleryMainWindow::createHeader() {
    // Header layout
    headerLayout_ = new QHBoxLayout();
    headerLayout_->setContentsMargins(20, 16, 20, 16);
    headerLayout_->setSpacing(16);
    contentLayout_->addLayout(headerLayout_);

    // Title
    titleLabel_ = new QLabel("Material Design 3 Gallery", this);
    QFont titleFont("Segoe UI", 16, QFont::Bold);
    titleLabel_->setFont(titleFont);
    titleLabel_->setStyleSheet("QLabel { color: #1C1B1F; }");
    headerLayout_->addWidget(titleLabel_);
    headerLayout_->addStretch();

    // Theme label
    themeLabel_ = new QLabel("☀️ Light", this);
    QFont themeFont("Segoe UI", 10);
    themeLabel_->setFont(themeFont);
    themeLabel_->setStyleSheet("QLabel { color: #49454F; }");
    headerLayout_->addWidget(themeLabel_);

    // Theme switch
    themeSwitch_ = new ThemeSwitch(this);
    headerLayout_->addWidget(themeSwitch_);

    connect(themeSwitch_, &ThemeSwitch::themeChanged, this,
            &MaterialGalleryMainWindow::onThemeChanged);
}

void MaterialGalleryMainWindow::applyThemeToAllPages(const core::ICFTheme& theme) {
    colorSchemePage_->applyTheme(theme);
    motionSpecPage_->applyTheme(theme);
    radiusScalePage_->applyTheme(theme);
    typographyPage_->applyTheme(theme);
    sidebar_->applyTheme(theme);

    // Update header colors
    auto& colorScheme = static_cast<const core::MaterialColorScheme&>(theme.color_scheme());
    QColor onSurface = colorScheme.queryColor("md.onSurface");
    QColor bg = colorScheme.queryColor("md.background");

    titleLabel_->setStyleSheet(QString("QLabel { color: %1; }").arg(onSurface.name()));
    themeLabel_->setStyleSheet(QString("QLabel { color: %1; }").arg(onSurface.name()));

    // Update central widget background
    centralWidget_->setAutoFillBackground(true);
    QPalette pal = centralWidget_->palette();
    pal.setColor(QPalette::Window, bg);
    centralWidget_->setPalette(pal);
}

void MaterialGalleryMainWindow::onTabClicked(int index) {
    contentStack_->setCurrentIndex(index);
}

void MaterialGalleryMainWindow::onThemeChanged(bool isDark) {
    isDarkTheme_ = isDark;

    // Update theme label
    themeLabel_->setText(isDark ? "🌙 Dark" : "☀️ Light");

    // Switch theme via ThemeManager
    const char* themeName =
        isDark ? token_literals::MATERIAL_THEME_DARK : token_literals::MATERIAL_THEME_LIGHT;
    themeManager_->setThemeTo(themeName);
}

void MaterialGalleryMainWindow::onThemeManagerChanged(const core::ICFTheme& theme) {
    applyThemeToAllPages(theme);
}

void MaterialGalleryMainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    // Handle responsive layout if needed
}

} // namespace cf::ui::gallery
