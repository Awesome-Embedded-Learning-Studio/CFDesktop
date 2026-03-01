/**
 * @file RadiusScalePage.cpp
 * @brief Radius Scale page - Implementation
 *
 * @author CFDesktop Team
 * @date 2026-02-28
 * @version 0.1
 */

#include "RadiusScalePage.h"
#include "ui/core/material/cfmaterial_radius_scale.h"
#include "ui/core/material/cfmaterial_scheme.h"
#include "ui/core/material/material_factory.hpp"
#include "ui/core/theme.h"
#include "ui/core/token/radius_scale/cfmaterial_radius_scale_literals.h"

#include <QFont>
#include <QPainter>
#include <QPainterPath>

// Global namespace alias for token literals
namespace token_literals = ::cf::ui::core::token::literals;

namespace cf::ui::gallery {

// =============================================================================
// RadiusPreviewWidget Implementation
// =============================================================================

RadiusPreviewWidget::RadiusPreviewWidget(const QString& name, float radiusDp,
                                         const QColor& accentColor, QWidget* parent)
    : QWidget(parent), name_(name), radiusDp_(radiusDp), accentColor_(accentColor) {
    setMinimumSize(180, 160);
}

void RadiusPreviewWidget::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRectF r = rect().adjusted(10, 10, -10, -10);

    // Card background
    QPainterPath cardPath;
    cardPath.addRoundedRect(r, 12, 12);
    painter.setPen(QPen(QColor(200, 200, 200), 1));
    painter.setBrush(QColor(250, 250, 250));
    painter.drawPath(cardPath);

    // Radius preview area
    QRectF previewRect = r.adjusted(20, 45, -20, -45);

    // Calculate px from dp (1dp = 1px for preview)
    float radiusPx = radiusDp_;

    QPainterPath previewPath;
    previewPath.addRoundedRect(previewRect, radiusPx, radiusPx);

    // Draw rounded rectangle with accent color
    painter.setPen(Qt::NoPen);
    painter.setBrush(accentColor_);
    painter.drawPath(previewPath);

    // Title
    QFont nameFont("Segoe UI", 11, QFont::Bold);
    painter.setFont(nameFont);
    painter.setPen(QColor(60, 60, 60));
    painter.drawText(QRectF(r.left(), r.top() + 8, r.width(), 25), Qt::AlignCenter, name_);

    // dp value label
    QFont dpFont("Consolas", 10);
    painter.setFont(dpFont);
    painter.setPen(QColor(100, 100, 100));
    QString dpText = QString("%1 dp").arg(radiusDp_, 0, 'f', 0);
    painter.drawText(QRectF(previewRect.left(), previewRect.bottom() + 6, previewRect.width(), 20),
                     Qt::AlignCenter, dpText);
}

// =============================================================================
// RadiusScalePage Implementation
// =============================================================================

RadiusScalePage::RadiusScalePage(QWidget* parent) : ThemePageWidget(parent) {
    // Default colors (light theme)
    cardBgColor_ = QColor(250, 250, 250);
    cardBorderColor_ = QColor(200, 200, 200);
    textColor_ = QColor(60, 60, 60);

    setupUI();
    // Note: createRadiusCards will be called after theme is set
}

void RadiusScalePage::setupUI() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Scroll area for cards
    scrollArea_ = new QScrollArea(this);
    scrollArea_->setWidgetResizable(true);
    scrollArea_->setFrameShape(QFrame::NoFrame);
    scrollArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    scrollContent_ = new QWidget();
    auto* scrollLayout = new QVBoxLayout(scrollContent_);
    scrollLayout->setSpacing(16);
    scrollLayout->setContentsMargins(24, 24, 24, 24);

    cardsLayout_ = new QGridLayout();
    cardsLayout_->setSpacing(12);
    cardsLayout_->setContentsMargins(0, 0, 0, 0);
    scrollLayout->addLayout(cardsLayout_);
    scrollLayout->addStretch();

    scrollArea_->setWidget(scrollContent_);
    mainLayout->addWidget(scrollArea_);
}

void RadiusScalePage::createRadiusCards() {
    if (!theme_)
        return;
    auto& radiusScale = const_cast<cf::ui::core::MaterialRadiusScale&>(
        static_cast<const cf::ui::core::MaterialRadiusScale&>(theme_->radius_scale()));
    auto& colorScheme = const_cast<cf::ui::core::MaterialColorScheme&>(
        static_cast<const cf::ui::core::MaterialColorScheme&>(theme_->color_scheme()));

    struct RadiusInfo {
        const char* token;
        const char* name;
        QColor color;
    };

    static const RadiusInfo radii[] = {
        {token_literals::CORNER_NONE, "Corner None", QColor(100, 100, 100)},
        {token_literals::CORNER_EXTRA_SMALL, "Corner Extra Small", QColor(103, 80, 164)},
        {token_literals::CORNER_SMALL, "Corner Small", QColor(103, 80, 164)},
        {token_literals::CORNER_MEDIUM, "Corner Medium", QColor(103, 80, 164)},
        {token_literals::CORNER_LARGE, "Corner Large", QColor(103, 80, 164)},
        {token_literals::CORNER_EXTRA_LARGE, "Corner Extra Large", QColor(103, 80, 164)},
        {token_literals::CORNER_EXTRA_EXTRA_LARGE, "Corner Extra Extra Large",
         QColor(103, 80, 164)}};

    int row = 0, col = 0;
    int maxCols = 4;

    for (const auto& info : radii) {
        float radius = radiusScale.queryRadiusScale(info.token);
        auto* card = new RadiusPreviewWidget(info.name, radius, info.color, scrollContent_);
        cardsLayout_->addWidget(card, row, col++);

        if (col >= maxCols) {
            col = 0;
            row++;
        }
    }
}

void RadiusScalePage::applyTheme(const cf::ui::core::ICFTheme& theme) {
    // Store theme pointer
    theme_ = &theme;

    auto& colorScheme = const_cast<cf::ui::core::MaterialColorScheme&>(
        static_cast<const cf::ui::core::MaterialColorScheme&>(theme.color_scheme()));
    auto& radiusScale = const_cast<cf::ui::core::MaterialRadiusScale&>(
        static_cast<const cf::ui::core::MaterialRadiusScale&>(theme.radius_scale()));

    // Update colors
    QColor bg = colorScheme.queryColor("md.background");
    QColor onSurface = colorScheme.queryColor("md.onSurface");
    QColor primary = colorScheme.queryColor("md.primary");

    // Update background
    scrollContent_->setAutoFillBackground(true);
    QPalette pal = scrollContent_->palette();
    pal.setColor(QPalette::Window, bg);
    scrollContent_->setPalette(pal);

    // Recreate cards with new theme
    // Clear existing layout
    while (cardsLayout_->count()) {
        auto* item = cardsLayout_->takeAt(0);
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }

    // Recreate cards
    struct RadiusInfo {
        const char* token;
        const char* name;
    };

    static const RadiusInfo radii[] = {
        {token_literals::CORNER_NONE, "Corner None"},
        {token_literals::CORNER_EXTRA_SMALL, "Corner Extra Small"},
        {token_literals::CORNER_SMALL, "Corner Small"},
        {token_literals::CORNER_MEDIUM, "Corner Medium"},
        {token_literals::CORNER_LARGE, "Corner Large"},
        {token_literals::CORNER_EXTRA_LARGE, "Corner Extra Large"},
        {token_literals::CORNER_EXTRA_EXTRA_LARGE, "Corner Extra Extra Large"}};

    int row = 0, col = 0;
    int maxCols = 4;

    for (const auto& info : radii) {
        float radius = radiusScale.queryRadiusScale(info.token);
        auto* card = new RadiusPreviewWidget(info.name, radius, primary, scrollContent_);
        cardsLayout_->addWidget(card, row, col++);

        if (col >= maxCols) {
            col = 0;
            row++;
        }
    }
}

} // namespace cf::ui::gallery
