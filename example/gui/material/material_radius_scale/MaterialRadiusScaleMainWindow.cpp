/**
 * @file MaterialRadiusScaleMainWindow.cpp
 * @brief Material Design 3 Radius Scale Gallery - Implementation
 *
 * @author CFDesktop Team
 * @date 2026-02-26
 * @version 0.1
 */

#include "MaterialRadiusScaleMainWindow.h"

#include "ui/core/material/material_factory.hpp"
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
    setMinimumSize(200, 180);
}

void RadiusPreviewWidget::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRectF r = rect().adjusted(10, 10, -10, -10);

    // 卡片背景
    QPainterPath cardPath;
    cardPath.addRoundedRect(r, 12, 12);
    painter.setPen(QPen(QColor(200, 200, 200), 1));
    painter.setBrush(QColor(250, 250, 250));
    painter.drawPath(cardPath);

    // 圆角预览区域
    QRectF previewRect = r.adjusted(20, 50, -20, -50);

    // 根据 dp 计算 px (假设 1dp = 1px 用于预览)
    float radiusPx = radiusDp_;

    QPainterPath previewPath;
    previewPath.addRoundedRect(previewRect, radiusPx, radiusPx);

    // 绘制带圆角的矩形
    painter.setPen(Qt::NoPen);
    painter.setBrush(accentColor_);
    painter.drawPath(previewPath);

    // 标题
    QFont nameFont("Segoe UI", 12, QFont::Bold);
    painter.setFont(nameFont);
    painter.setPen(QColor(60, 60, 60));
    painter.drawText(QRectF(r.left(), r.top() + 10, r.width(), 25), Qt::AlignCenter, name_);

    // dp 值标签
    QFont dpFont("Consolas", 11);
    painter.setFont(dpFont);
    painter.setPen(QColor(100, 100, 100));
    QString dpText = QString("%1 dp").arg(radiusDp_, 0, 'f', 0);
    painter.drawText(QRectF(previewRect.left(), previewRect.bottom() + 8, previewRect.width(), 20),
                     Qt::AlignCenter, dpText);
}

// =============================================================================
// MaterialRadiusScaleMainWindow Implementation
// =============================================================================

MaterialRadiusScaleMainWindow::MaterialRadiusScaleMainWindow(QWidget* parent)
    : QMainWindow(parent), radiusScale_(cf::ui::core::material::defaultRadiusScale()) {

    setupUI();
    createHeader();
    createRadiusCards();
}

void MaterialRadiusScaleMainWindow::setupUI() {
    // Window setup
    setWindowTitle("Material Radius Scale Gallery");
    resize(1000, 700);
    setMinimumSize(600, 400);

    // Central widget
    centralWidget_ = new QWidget(this);
    setCentralWidget(centralWidget_);

    mainLayout_ = new QVBoxLayout(centralWidget_);
    mainLayout_->setContentsMargins(20, 20, 20, 20);
    mainLayout_->setSpacing(16);

    // Header
    auto* headerLayout = new QHBoxLayout();
    headerLayout->setSpacing(16);
    mainLayout_->addLayout(headerLayout);

    auto* titleLabel = new QLabel("Material Radius Scale Gallery", this);
    QFont titleFont("Segoe UI", 18, QFont::Bold);
    titleLabel->setFont(titleFont);
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();

    auto* subtitleLabel = new QLabel("Material Design 3 Corner Radius Tokens", this);
    QFont subtitleFont("Segoe UI", 11);
    subtitleLabel->setFont(subtitleFont);
    subtitleLabel->setStyleSheet("color: #666;");
    headerLayout->addWidget(subtitleLabel);

    // Scroll area for cards
    scrollArea_ = new QScrollArea(this);
    scrollArea_->setWidgetResizable(true);
    scrollArea_->setFrameShape(QFrame::NoFrame);
    scrollArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    scrollContent_ = new QWidget();
    auto* scrollLayout = new QVBoxLayout(scrollContent_);
    scrollLayout->setSpacing(20);

    cardsLayout_ = new QGridLayout();
    cardsLayout_->setSpacing(16);
    cardsLayout_->setContentsMargins(0, 0, 0, 0);
    scrollLayout->addLayout(cardsLayout_);
    scrollLayout->addStretch();

    scrollArea_->setWidget(scrollContent_);
    mainLayout_->addWidget(scrollArea_);
}

void MaterialRadiusScaleMainWindow::createHeader() {
    // Header already created in setupUI
}

void MaterialRadiusScaleMainWindow::createRadiusCards() {
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
    for (const auto& info : radii) {
        float radius = radiusScale_.queryRadiusScale(info.token);
        auto* card = new RadiusPreviewWidget(info.name, radius, info.color, scrollContent_);
        cardsLayout_->addWidget(card, row, col++);

        if (col >= 4) {
            col = 0;
            row++;
        }
    }
}

} // namespace cf::ui::gallery
