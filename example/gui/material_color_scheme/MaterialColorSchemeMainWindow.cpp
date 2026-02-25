/**
 * @file MaterialColorSchemeMainWindow.cpp
 * @brief Material Design 3 Color Scheme Gallery - Implementation
 */

#include "MaterialColorSchemeMainWindow.h"
#include "material/material_factory.hpp"
#include "ui/core/token/material_scheme/cfmaterial_token_literals.h"

#include <QClipboard>
#include <QDebug>
#include <QFont>
#include <QGuiApplication>
#include <QLinearGradient>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

// Global namespace alias for token literals
namespace token_literals = ::cf::ui::core::token::literals;

namespace cf::ui::gallery {

// =============================================================================
// Static Token Lists
// =============================================================================

const QStringList MaterialColorSchemeMainWindow::PRIMARY_TOKENS = {
    token_literals::PRIMARY, token_literals::ON_PRIMARY, token_literals::PRIMARY_CONTAINER,
    token_literals::ON_PRIMARY_CONTAINER};

const QStringList MaterialColorSchemeMainWindow::SECONDARY_TOKENS = {
    token_literals::SECONDARY, token_literals::ON_SECONDARY, token_literals::SECONDARY_CONTAINER,
    token_literals::ON_SECONDARY_CONTAINER};

const QStringList MaterialColorSchemeMainWindow::TERTIARY_TOKENS = {
    token_literals::TERTIARY, token_literals::ON_TERTIARY, token_literals::TERTIARY_CONTAINER,
    token_literals::ON_TERTIARY_CONTAINER};

const QStringList MaterialColorSchemeMainWindow::ERROR_TOKENS = {
    token_literals::ERROR, token_literals::ON_ERROR, token_literals::ERROR_CONTAINER,
    token_literals::ON_ERROR_CONTAINER};

const QStringList MaterialColorSchemeMainWindow::SURFACE_TOKENS = {
    token_literals::BACKGROUND, token_literals::ON_BACKGROUND,   token_literals::SURFACE,
    token_literals::ON_SURFACE, token_literals::SURFACE_VARIANT, token_literals::ON_SURFACE_VARIANT,
    token_literals::OUTLINE,    token_literals::OUTLINE_VARIANT};

const QStringList MaterialColorSchemeMainWindow::UTILITY_TOKENS = {
    token_literals::SHADOW, token_literals::SCRIM, token_literals::INVERSE_SURFACE,
    token_literals::INVERSE_ON_SURFACE, token_literals::INVERSE_PRIMARY};

// =============================================================================
// Contrast Pair Mapping
// =============================================================================

QString MaterialColorSchemeMainWindow::getContrastPair(const QString& token) const {
    static const QMap<QString, QString> pairs = {
        {"md.primary", "md.onPrimary"},
        {"md.onPrimary", "md.primary"},
        {"md.primaryContainer", "md.onPrimaryContainer"},
        {"md.onPrimaryContainer", "md.primaryContainer"},
        {"md.secondary", "md.onSecondary"},
        {"md.onSecondary", "md.secondary"},
        {"md.secondaryContainer", "md.onSecondaryContainer"},
        {"md.onSecondaryContainer", "md.secondaryContainer"},
        {"md.tertiary", "md.onTertiary"},
        {"md.onTertiary", "md.tertiary"},
        {"md.tertiaryContainer", "md.onTertiaryContainer"},
        {"md.onTertiaryContainer", "md.tertiaryContainer"},
        {"md.error", "md.onError"},
        {"md.onError", "md.error"},
        {"md.errorContainer", "md.onErrorContainer"},
        {"md.onErrorContainer", "md.errorContainer"},
        {"md.background", "md.onBackground"},
        {"md.onBackground", "md.background"},
        {"md.surface", "md.onSurface"},
        {"md.onSurface", "md.surface"},
        {"md.surfaceVariant", "md.onSurfaceVariant"},
        {"md.onSurfaceVariant", "md.surfaceVariant"},
        {"md.outline", "md.surface"},
        {"md.outlineVariant", "md.surface"},
        {"md.shadow", "md.surface"},
        {"md.scrim", "md.surface"},
        {"md.inverseSurface", "md.inverseOnSurface"},
        {"md.inverseOnSurface", "md.inverseSurface"},
        {"md.inversePrimary", "md.surface"}};
    return pairs.value(token, "md.surface");
}

float MaterialColorSchemeMainWindow::calculateContrastRatio(const QColor& fg,
                                                            const QColor& bg) const {
    // WCAG 2.1 relative luminance calculation
    auto luminance = [](const QColor& c) -> float {
        auto toLinear = [](float v) -> float {
            v /= 255.0f;
            return v <= 0.03928f ? v / 12.92f : std::pow((v + 0.055f) / 1.055f, 2.4f);
        };
        float r = toLinear(c.red());
        float g = toLinear(c.green());
        float b = toLinear(c.blue());
        return 0.2126f * r + 0.7152f * g + 0.0722f * b;
    };

    float l1 = luminance(fg);
    float l2 = luminance(bg);
    float lighter = std::max(l1, l2);
    float darker = std::min(l1, l2);
    return (lighter + 0.05f) / (darker + 0.05f);
}

// =============================================================================
// ToastWidget Implementation
// =============================================================================

ToastWidget::ToastWidget(const QString& message, QWidget* parent)
    : QWidget(parent), message_(message) {
    setAttribute(Qt::WA_TransparentForMouseEvents, false);
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
}

void ToastWidget::show(int durationMs) {
    // Calculate size and position
    QFont font("Segoe UI", 10);
    QFontMetrics fm(font);
    int padding = 20;
    int textWidth = fm.horizontalAdvance(message_);
    int width = textWidth + padding * 2;
    int height = fm.height() + padding * 2;

    setFixedSize(width, height);

    // Center at bottom of parent
    if (parentWidget()) {
        QPoint pos = parentWidget()->mapToGlobal(parentWidget()->rect().bottomLeft());
        int x = pos.x() + (parentWidget()->width() - width) / 2;
        int y = pos.y() - height - 20;
        move(x, y);
    }

    QWidget::show();

    // Auto hide after duration
    QTimer::singleShot(durationMs, this, [this]() {
        // Fade out animation
        QPropertyAnimation* anim = new QPropertyAnimation(this, "windowOpacity");
        anim->setDuration(300);
        anim->setStartValue(1.0);
        anim->setEndValue(0.0);
        connect(anim, &QPropertyAnimation::finished, this, &QWidget::hide);
        connect(anim, &QPropertyAnimation::finished, anim, &QObject::deleteLater);
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    });
}

void ToastWidget::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Semi-transparent black background
    QPainterPath path;
    path.addRoundedRect(rect(), 12, 12);
    painter.fillPath(path, QColor(0, 0, 0, 200));

    // White text
    painter.setPen(Qt::white);
    QFont font("Segoe UI", 10);
    painter.setFont(font);
    painter.drawText(rect(), Qt::AlignCenter, message_);
}

// =============================================================================
// ColorCardWidget Implementation
// =============================================================================

ColorCardWidget::ColorCardWidget(const QString& tokenName, const QColor& color,
                                 const QString& contrastInfo, QWidget* parent)
    : QWidget(parent), tokenName_(tokenName), color_(color), contrastInfo_(contrastInfo) {
    hexValue_ = color.name().toUpper();
    setMinimumSize(160, 180);
    setMaximumSize(200, 220);
    setCursor(Qt::PointingHandCursor);
}

void ColorCardWidget::updateColor(const QColor& color, const QString& contrastInfo) {
    color_ = color;
    hexValue_ = color.name().toUpper();
    contrastInfo_ = contrastInfo;
    update();
}

void ColorCardWidget::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRectF r = rect().adjusted(2, 2, -2, -2);
    qreal radius = 12;

    // Get current theme colors for card background
    cf::ui::core::MaterialColorScheme* scheme = nullptr;
    // We'll get this from parent during update
    QColor bgColor = isHovered_ ? QColor(250, 250, 250) : QColor(245, 245, 245);
    QColor borderColor = QColor(220, 220, 220);

    // Card background with elevation
    QPainterPath path;
    path.addRoundedRect(r, radius, radius);

    // Shadow for elevation effect
    if (isHovered_) {
        // Higher elevation when hovered
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(0, 0, 0, 20));
        painter.drawPath(path.translated(0, 2));
    }

    painter.setPen(QPen(borderColor, 1));
    painter.setBrush(bgColor);
    painter.drawPath(path);

    // Color swatch
    qreal swatchSize = std::min(r.width(), r.height() * 0.35);
    QRectF swatchRect(r.center().x() - swatchSize / 2, r.top() + 15, swatchSize, swatchSize);

    QPainterPath swatchPath;
    swatchPath.addRoundedRect(swatchRect, 8, 8);
    painter.setPen(Qt::NoPen);
    painter.setBrush(color_);
    painter.drawPath(swatchPath);

    // Token name
    QFont nameFont("Segoe UI", 9, QFont::Medium);
    painter.setFont(nameFont);
    painter.setPen(QColor(60, 60, 60));
    QRectF nameRect = r.adjusted(10, swatchRect.bottom() + 10, -10, 0);
    painter.drawText(nameRect, Qt::AlignTop | Qt::AlignHCenter, tokenName_);

    // Separator line
    qreal lineY = nameRect.top() + QFontMetrics(nameFont).height() + 8;
    QPen linePen(QColor(200, 200, 200), 1);
    painter.setPen(linePen);
    painter.drawLine(QPointF(r.left() + 20, lineY), QPointF(r.right() - 20, lineY));

    // HEX value
    QFont hexFont("Consolas", 10);
    painter.setFont(hexFont);
    painter.setPen(QColor(40, 40, 40));
    QRectF hexRect = r.adjusted(10, lineY + 5, -10, 0);
    painter.drawText(hexRect, Qt::AlignTop | Qt::AlignHCenter, hexValue_);

    // Contrast info with color coding
    QFont contrastFont("Segoe UI", 8);
    painter.setFont(contrastFont);

    // Parse contrast value
    float contrast = contrastInfo_.split(": ").last().toFloat();
    QColor contrastColor;
    if (contrast < 3.0f) {
        contrastColor = QColor(220, 50, 50); // Red - fail
    } else if (contrast < 4.5f) {
        contrastColor = QColor(220, 150, 50); // Yellow - large text only
    } else {
        contrastColor = QColor(50, 180, 80); // Green - pass
    }

    painter.setPen(contrastColor);
    QRectF contrastRect =
        r.adjusted(10, hexRect.top() + QFontMetrics(hexFont).height() + 3, -10, -10);
    painter.drawText(contrastRect, Qt::AlignTop | Qt::AlignHCenter, contrastInfo_);

    // Click hint on hover
    if (isHovered_) {
        QFont hintFont("Segoe UI", 8);
        painter.setFont(hintFont);
        painter.setPen(QColor(100, 100, 100));
        painter.drawText(r.adjusted(10, 0, -10, -5), Qt::AlignBottom | Qt::AlignHCenter,
                         "Click to copy");
    }
}

void ColorCardWidget::enterEvent(QEnterEvent*) {
    isHovered_ = true;
    update();
}

void ColorCardWidget::leaveEvent(QEvent*) {
    isHovered_ = false;
    update();
}

void ColorCardWidget::mousePressEvent(QMouseEvent*) {
    emit clicked(hexValue_);
}

// =============================================================================
// ThemeSwitch Implementation
// =============================================================================

ThemeSwitch::ThemeSwitch(QWidget* parent) : QWidget(parent) {
    setFixedSize(60, 32);

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
    QFont iconFont("Segoe UI Emoji", 12);
    painter.setFont(iconFont);
    QString icon = isDark_ ? "🌙" : "☀️";
    painter.drawText(knobRect, Qt::AlignCenter, icon);
}

void ThemeSwitch::mousePressEvent(QMouseEvent*) {
    setDark(!isDark_);
}

// =============================================================================
// MaterialColorSchemeMainWindow Implementation
// =============================================================================

MaterialColorSchemeMainWindow::MaterialColorSchemeMainWindow(QWidget* parent)
    : QMainWindow(parent) {

    // Create color schemes
    lightScheme_ = cf::ui::core::material::light();
    darkScheme_ = cf::ui::core::material::dark();

    setupUI();
    createHeader();
    createColorGroups();
    updateWindowTheme();
}

MaterialColorSchemeMainWindow::~MaterialColorSchemeMainWindow() = default;

void MaterialColorSchemeMainWindow::setupUI() {
    // Window setup
    setWindowTitle("Material Color Scheme Gallery");
    resize(1200, 800);
    setMinimumSize(800, 600);

    // Central widget
    centralWidget_ = new QWidget(this);
    setCentralWidget(centralWidget_);

    mainLayout_ = new QVBoxLayout(centralWidget_);
    mainLayout_->setContentsMargins(20, 20, 20, 20);
    mainLayout_->setSpacing(20);

    // Header
    headerLayout_ = new QHBoxLayout();
    headerLayout_->setSpacing(16);
    mainLayout_->addLayout(headerLayout_);

    // Scroll area for color cards
    scrollArea_ = new QScrollArea(this);
    scrollArea_->setWidgetResizable(true);
    scrollArea_->setFrameShape(QFrame::NoFrame);
    scrollArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    scrollContent_ = new QWidget();
    scrollLayout_ = new QVBoxLayout(scrollContent_);
    scrollLayout_->setSpacing(30);

    colorGridLayout_ = new QGridLayout();
    colorGridLayout_->setSpacing(16);
    colorGridLayout_->setContentsMargins(0, 0, 0, 0);
    scrollLayout_->addLayout(colorGridLayout_);
    scrollLayout_->addStretch();

    scrollArea_->setWidget(scrollContent_);
    mainLayout_->addWidget(scrollArea_);

    // Toast
    toast_ = new ToastWidget("", this);
    toast_->hide();
}

void MaterialColorSchemeMainWindow::createHeader() {
    // Title
    titleLabel_ = new QLabel("Material Color Scheme Gallery", this);
    QFont titleFont("Segoe UI", 18, QFont::Bold);
    titleLabel_->setFont(titleFont);
    headerLayout_->addWidget(titleLabel_);
    headerLayout_->addStretch();

    // Theme label
    themeLabel_ = new QLabel("☀️ Light", this);
    QFont themeFont("Segoe UI", 11);
    themeLabel_->setFont(themeFont);
    headerLayout_->addWidget(themeLabel_);

    // Theme switch
    themeSwitch_ = new ThemeSwitch(this);
    headerLayout_->addWidget(themeSwitch_);

    connect(themeSwitch_, &ThemeSwitch::themeChanged, this,
            &MaterialColorSchemeMainWindow::onThemeChanged);
}

void MaterialColorSchemeMainWindow::createColorGroups() {
    int row = 0;

    createColorGroup("Primary Colors", colorGridLayout_, row, PRIMARY_TOKENS);
    createColorGroup("Secondary Colors", colorGridLayout_, row, SECONDARY_TOKENS);
    createColorGroup("Tertiary Colors", colorGridLayout_, row, TERTIARY_TOKENS);
    createColorGroup("Error Colors", colorGridLayout_, row, ERROR_TOKENS);
    createColorGroup("Surface Colors", colorGridLayout_, row, SURFACE_TOKENS);
    createColorGroup("Utility Colors", colorGridLayout_, row, UTILITY_TOKENS);
}

void MaterialColorSchemeMainWindow::createColorGroup(const QString& title, QGridLayout* layout,
                                                     int& row, const QStringList& tokens) {
    // Group title
    QLabel* titleLabel = new QLabel(title, scrollContent_);
    QFont titleFont("Segoe UI", 14, QFont::Bold);
    titleLabel->setFont(titleFont);
    layout->addWidget(titleLabel, row++, 0, 1, -1, Qt::AlignLeft);

    // Calculate column count and start new row for cards
    int col = 0;
    layout->setRowStretch(row, 0);

    for (const QString& token : tokens) {
        QColor color = lightScheme_.queryColor(token.toStdString().c_str());

        // Calculate contrast
        QString pairToken = getContrastPair(token);
        QColor pairColor = lightScheme_.queryColor(pairToken.toStdString().c_str());
        float contrast = calculateContrastRatio(color, pairColor);
        QString contrastInfo = QString("⚡ 对比度: %1").arg(contrast, 0, 'f', 2);

        auto* card = new ColorCardWidget(token, color, contrastInfo, scrollContent_);
        layout->addWidget(card, row, col++);

        ColorCardInfo info{card, token};
        colorCards_.append(info);

        connect(card, &ColorCardWidget::clicked, this,
                &MaterialColorSchemeMainWindow::onColorCardClicked);
    }

    row++;
}

void MaterialColorSchemeMainWindow::updateAllColors() {
    auto& scheme = isDarkTheme_ ? darkScheme_ : lightScheme_;

    for (auto& info : colorCards_) {
        QColor color = scheme.queryColor(info.token.toStdString().c_str());
        QString pairToken = getContrastPair(info.token);
        QColor pairColor = scheme.queryColor(pairToken.toStdString().c_str());
        float contrast = calculateContrastRatio(color, pairColor);
        QString contrastInfo = QString("⚡ 对比度: %1").arg(contrast, 0, 'f', 2);
        info.widget->updateColor(color, contrastInfo);
    }
}

void MaterialColorSchemeMainWindow::updateWindowTheme() {
    auto& scheme = isDarkTheme_ ? darkScheme_ : lightScheme_;

    // Update background colors
    QColor bg = scheme.queryColor("md.background");
    QColor surface = scheme.queryColor("md.surface");
    QColor onSurface = scheme.queryColor("md.onSurface");

    centralWidget_->setAutoFillBackground(true);
    QPalette pal = centralWidget_->palette();
    pal.setColor(QPalette::Window, bg);
    centralWidget_->setPalette(pal);

    scrollContent_->setAutoFillBackground(true);
    pal = scrollContent_->palette();
    pal.setColor(QPalette::Window, bg);
    scrollContent_->setPalette(pal);

    // Update text colors
    titleLabel_->setStyleSheet(QString("color: %1").arg(onSurface.name()));
    themeLabel_->setStyleSheet(QString("color: %1").arg(onSurface.name()));

    // Update theme label text
    themeLabel_->setText(isDarkTheme_ ? "🌙 Dark" : "☀️ Light");

    // Update color cards
    updateAllColors();
}

int MaterialColorSchemeMainWindow::calculateColumnCount() const {
    int width = scrollArea_->width();
    if (width < 800)
        return 2;
    if (width < 1200)
        return 3;
    return 4;
}

void MaterialColorSchemeMainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    // Could trigger column count recalculation here
}

void MaterialColorSchemeMainWindow::onThemeChanged(bool isDark) {
    isDarkTheme_ = isDark;
    updateWindowTheme();
}

void MaterialColorSchemeMainWindow::onColorCardClicked(const QString& hexValue) {
    QClipboard* clipboard = QGuiApplication::clipboard();
    clipboard->setText(hexValue);
    showToast(QString("已复制: %1").arg(hexValue));
}

void MaterialColorSchemeMainWindow::showToast(const QString& message) {
    delete toast_;
    toast_ = new ToastWidget(message, this);
    toast_->show();
}

} // namespace cf::ui::gallery
