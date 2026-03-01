/**
 * @file ColorSchemePage.cpp
 * @brief Color Scheme page - Implementation
 *
 * @author CFDesktop Team
 * @date 2026-02-28
 * @version 0.1
 */

#include "ColorSchemePage.h"
#include "ToastWidget.h"
#include "ui/core/material/cfmaterial_scheme.h"
#include "ui/core/material/material_factory.hpp"
#include "ui/core/theme.h"
#include "ui/core/token/material_scheme/cfmaterial_token_literals.h"

#include <QClipboard>
#include <QFont>
#include <QGuiApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QTimer>

// Global namespace alias for token literals
namespace token_literals = ::cf::ui::core::token::literals;

namespace cf::ui::gallery {

// =============================================================================
// Static Token Lists
// =============================================================================

const QStringList ColorSchemePage::PRIMARY_TOKENS = {
    token_literals::PRIMARY, token_literals::ON_PRIMARY, token_literals::PRIMARY_CONTAINER,
    token_literals::ON_PRIMARY_CONTAINER};

const QStringList ColorSchemePage::SECONDARY_TOKENS = {
    token_literals::SECONDARY, token_literals::ON_SECONDARY, token_literals::SECONDARY_CONTAINER,
    token_literals::ON_SECONDARY_CONTAINER};

const QStringList ColorSchemePage::TERTIARY_TOKENS = {
    token_literals::TERTIARY, token_literals::ON_TERTIARY, token_literals::TERTIARY_CONTAINER,
    token_literals::ON_TERTIARY_CONTAINER};

const QStringList ColorSchemePage::ERROR_TOKENS = {token_literals::ERROR, token_literals::ON_ERROR,
                                                   token_literals::ERROR_CONTAINER,
                                                   token_literals::ON_ERROR_CONTAINER};

const QStringList ColorSchemePage::SURFACE_TOKENS = {
    token_literals::BACKGROUND, token_literals::ON_BACKGROUND,   token_literals::SURFACE,
    token_literals::ON_SURFACE, token_literals::SURFACE_VARIANT, token_literals::ON_SURFACE_VARIANT,
    token_literals::OUTLINE,    token_literals::OUTLINE_VARIANT};

const QStringList ColorSchemePage::UTILITY_TOKENS = {
    token_literals::SHADOW, token_literals::SCRIM, token_literals::INVERSE_SURFACE,
    token_literals::INVERSE_ON_SURFACE, token_literals::INVERSE_PRIMARY};

// =============================================================================
// ColorCardWidget Implementation
// =============================================================================

ColorCardWidget::ColorCardWidget(const QString& tokenName, const QColor& color,
                                 const QString& contrastInfo, QWidget* parent)
    : QWidget(parent), tokenName_(tokenName), color_(color), contrastInfo_(contrastInfo) {
    hexValue_ = color.name().toUpper();
    setMinimumSize(140, 160);
    setMaximumSize(180, 200);
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

    // Card background with elevation
    QPainterPath path;
    path.addRoundedRect(r, radius, radius);

    // Shadow for elevation effect
    if (isHovered_) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(0, 0, 0, 20));
        painter.drawPath(path.translated(0, 2));
    }

    painter.setPen(QPen(QColor(200, 200, 200), 1));
    painter.setBrush(QColor(250, 250, 250));
    painter.drawPath(path);

    // Color swatch
    qreal swatchSize = std::min(r.width(), r.height() * 0.35);
    QRectF swatchRect(r.center().x() - swatchSize / 2, r.top() + 12, swatchSize, swatchSize);

    QPainterPath swatchPath;
    swatchPath.addRoundedRect(swatchRect, 8, 8);
    painter.setPen(Qt::NoPen);
    painter.setBrush(color_);
    painter.drawPath(swatchPath);

    // Token name
    QFont nameFont("Segoe UI", 8, QFont::Medium);
    painter.setFont(nameFont);
    painter.setPen(QColor(60, 60, 60));
    QRectF nameRect = r.adjusted(8, swatchRect.bottom() + 8, -8, 0);
    painter.drawText(nameRect, Qt::AlignTop | Qt::AlignHCenter, tokenName_);

    // Separator line
    qreal lineY = nameRect.top() + QFontMetrics(nameFont).height() + 6;
    QPen linePen(QColor(200, 200, 200), 1);
    painter.setPen(linePen);
    painter.drawLine(QPointF(r.left() + 16, lineY), QPointF(r.right() - 16, lineY));

    // HEX value
    QFont hexFont("Consolas", 9);
    painter.setFont(hexFont);
    painter.setPen(QColor(40, 40, 40));
    QRectF hexRect = r.adjusted(8, lineY + 4, -8, 0);
    painter.drawText(hexRect, Qt::AlignTop | Qt::AlignHCenter, hexValue_);

    // Contrast info with color coding
    QFont contrastFont("Segoe UI", 7);
    painter.setFont(contrastFont);

    // Parse contrast value
    float contrast = contrastInfo_.split(": ").last().toFloat();
    QColor contrastColor;
    if (contrast < 3.0f) {
        contrastColor = QColor(220, 50, 50);
    } else if (contrast < 4.5f) {
        contrastColor = QColor(220, 150, 50);
    } else {
        contrastColor = QColor(50, 180, 80);
    }

    painter.setPen(contrastColor);
    QRectF contrastRect = r.adjusted(8, hexRect.top() + QFontMetrics(hexFont).height() + 2, -8, -8);
    painter.drawText(contrastRect, Qt::AlignTop | Qt::AlignHCenter, contrastInfo_);

    // Click hint on hover
    if (isHovered_) {
        QFont hintFont("Segoe UI", 7);
        painter.setFont(hintFont);
        painter.setPen(QColor(100, 100, 100));
        painter.drawText(r.adjusted(8, 0, -8, -4), Qt::AlignBottom | Qt::AlignHCenter,
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
// ColorSchemePage Implementation
// =============================================================================

ColorSchemePage::ColorSchemePage(QWidget* parent) : ThemePageWidget(parent) {

    // Default card colors (light theme)
    cardBgColor_ = QColor(250, 250, 250);
    cardBorderColor_ = QColor(220, 220, 220);

    setupUI();
    // Note: createColorGroups will be called after theme is set
}

void ColorSchemePage::setupUI() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Scroll area for color cards
    scrollArea_ = new QScrollArea(this);
    scrollArea_->setWidgetResizable(true);
    scrollArea_->setFrameShape(QFrame::NoFrame);
    scrollArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    scrollContent_ = new QWidget();
    scrollLayout_ = new QVBoxLayout(scrollContent_);
    scrollLayout_->setSpacing(24);
    scrollLayout_->setContentsMargins(24, 24, 24, 24);

    colorGridLayout_ = new QGridLayout();
    colorGridLayout_->setSpacing(12);
    colorGridLayout_->setContentsMargins(0, 0, 0, 0);
    scrollLayout_->addLayout(colorGridLayout_);
    scrollLayout_->addStretch();

    scrollArea_->setWidget(scrollContent_);
    mainLayout->addWidget(scrollArea_);

    // Toast
    toast_ = new ToastWidget("", this);
    toast_->hide();
}

void ColorSchemePage::createColorGroups() {
    int row = 0;

    createColorGroup("Primary Colors 主色", colorGridLayout_, row, PRIMARY_TOKENS);
    createColorGroup("Secondary Colors 副色", colorGridLayout_, row, SECONDARY_TOKENS);
    createColorGroup("Tertiary Colors 第三色", colorGridLayout_, row, TERTIARY_TOKENS);
    createColorGroup("Error Colors 错误色", colorGridLayout_, row, ERROR_TOKENS);
    createColorGroup("Surface Colors 表面色", colorGridLayout_, row, SURFACE_TOKENS);
    createColorGroup("Utility Colors 工具色", colorGridLayout_, row, UTILITY_TOKENS);
}

void ColorSchemePage::createColorGroup(const QString& title, QGridLayout* layout, int& row,
                                       const QStringList& tokens) {
    // Get color scheme from theme
    if (!theme_)
        return;
    auto& colorScheme = const_cast<cf::ui::core::MaterialColorScheme&>(
        static_cast<const cf::ui::core::MaterialColorScheme&>(theme_->color_scheme()));

    // Group title
    QLabel* titleLabel = new QLabel(title, scrollContent_);
    QFont titleFont("Segoe UI", 13, QFont::Bold);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("QLabel { color: #49454F; padding: 4px 0px; }");
    layout->addWidget(titleLabel, row++, 0, 1, -1, Qt::AlignLeft);

    // Calculate column count
    int col = 0;
    int maxCols = 4;
    layout->setRowStretch(row, 0);

    for (const QString& token : tokens) {
        QColor color = colorScheme.queryColor(token.toStdString().c_str());

        // Calculate contrast
        QString pairToken = getContrastPair(token);
        QColor pairColor = colorScheme.queryColor(pairToken.toStdString().c_str());
        float contrast = calculateContrastRatio(color, pairColor);
        QString contrastInfo = QString("⚡ %1").arg(contrast, 0, 'f', 2);

        auto* card = new ColorCardWidget(token, color, contrastInfo, scrollContent_);
        layout->addWidget(card, row, col++);

        ColorCardInfo info{card, token};
        colorCards_.append(info);

        connect(card, &ColorCardWidget::clicked, this, &ColorSchemePage::onColorCardClicked);

        if (col >= maxCols) {
            col = 0;
            row++;
        }
    }

    if (col > 0) {
        row++;
    }

    // Add spacing after group
    row++;
}

QString ColorSchemePage::getContrastPair(const QString& token) const {
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

float ColorSchemePage::calculateContrastRatio(const QColor& fg, const QColor& bg) const {
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

void ColorSchemePage::updateAllColors() {
    if (!theme_)
        return;
    auto& colorScheme = const_cast<cf::ui::core::MaterialColorScheme&>(
        static_cast<const cf::ui::core::MaterialColorScheme&>(theme_->color_scheme()));

    for (auto& info : colorCards_) {
        QColor color = colorScheme.queryColor(info.token.toStdString().c_str());
        QString pairToken = getContrastPair(info.token);
        QColor pairColor = colorScheme.queryColor(pairToken.toStdString().c_str());
        float contrast = calculateContrastRatio(color, pairColor);
        QString contrastInfo = QString("⚡ %1").arg(contrast, 0, 'f', 2);
        info.widget->updateColor(color, contrastInfo);
    }
}

void ColorSchemePage::updateWindowTheme() {
    if (!theme_)
        return;
    auto& colorScheme = const_cast<cf::ui::core::MaterialColorScheme&>(
        static_cast<const cf::ui::core::MaterialColorScheme&>(theme_->color_scheme()));

    // Update background colors
    QColor bg = colorScheme.queryColor("md.background");
    QColor onSurface = colorScheme.queryColor("md.onSurface");

    scrollContent_->setAutoFillBackground(true);
    QPalette pal = scrollContent_->palette();
    pal.setColor(QPalette::Window, bg);
    scrollContent_->setPalette(pal);
}

void ColorSchemePage::applyTheme(const cf::ui::core::ICFTheme& theme) {
    // Store theme pointer
    theme_ = &theme;

    // Clear existing cards and recreate with new theme
    colorCards_.clear();

    // Clear layout items - this also deletes the widgets (Qt takes ownership)
    while (colorGridLayout_->count()) {
        QLayoutItem* item = colorGridLayout_->takeAt(0);
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
    createColorGroups();

    updateAllColors();
    updateWindowTheme();
}

void ColorSchemePage::onColorCardClicked(const QString& hexValue) {
    QClipboard* clipboard = QGuiApplication::clipboard();
    clipboard->setText(hexValue);
    showToast(QString("已复制: %1").arg(hexValue));
}

void ColorSchemePage::showToast(const QString& message) {
    delete toast_;
    toast_ = new ToastWidget(message, this);
    toast_->show();
}

} // namespace cf::ui::gallery
