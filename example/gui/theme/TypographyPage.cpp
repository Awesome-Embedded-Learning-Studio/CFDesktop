/**
 * @file TypographyPage.cpp
 * @brief Typography page - Implementation
 *
 * @author CFDesktop Team
 * @date 2026-02-28
 * @version 0.1
 */

#include "TypographyPage.h"
#include "ToastWidget.h"
#include "ui/core/theme.h"
#include "ui/core/material/cfmaterial_fonttype.h"
#include "ui/core/material/cfmaterial_scheme.h"
#include "ui/core/material/material_factory.hpp"
#include "ui/core/token/typography/cfmaterial_typography_token_literals.h"

#include <QApplication>
#include <QClipboard>
#include <QFont>
#include <QFontMetrics>
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

const QStringList TypographyPage::DISPLAY_TOKENS = {
    token_literals::TYPOGRAPHY_DISPLAY_LARGE, token_literals::TYPOGRAPHY_DISPLAY_MEDIUM,
    token_literals::TYPOGRAPHY_DISPLAY_SMALL};

const QStringList TypographyPage::HEADLINE_TOKENS = {
    token_literals::TYPOGRAPHY_HEADLINE_LARGE, token_literals::TYPOGRAPHY_HEADLINE_MEDIUM,
    token_literals::TYPOGRAPHY_HEADLINE_SMALL};

const QStringList TypographyPage::TITLE_TOKENS = {
    token_literals::TYPOGRAPHY_TITLE_LARGE, token_literals::TYPOGRAPHY_TITLE_MEDIUM,
    token_literals::TYPOGRAPHY_TITLE_SMALL};

const QStringList TypographyPage::BODY_TOKENS = {
    token_literals::TYPOGRAPHY_BODY_LARGE, token_literals::TYPOGRAPHY_BODY_MEDIUM,
    token_literals::TYPOGRAPHY_BODY_SMALL};

const QStringList TypographyPage::LABEL_TOKENS = {
    token_literals::TYPOGRAPHY_LABEL_LARGE, token_literals::TYPOGRAPHY_LABEL_MEDIUM,
    token_literals::TYPOGRAPHY_LABEL_SMALL};

// =============================================================================
// FontCardWidget Implementation
// =============================================================================

FontCardWidget::FontCardWidget(const QString& tokenName, const QFont& font, float lineHeight,
                               const QString& previewText, QWidget* parent)
    : QWidget(parent), tokenName_(tokenName), font_(font), lineHeight_(lineHeight),
      previewText_(previewText) {
    cssStyle_ = generateCssStyle();
    setCursor(Qt::PointingHandCursor);

    QFontMetrics fm(font_);
    int minHeight = qMax(100, fm.height() * 2 + 50);
    setMinimumHeight(minHeight);
}

void FontCardWidget::updateFont(const QFont& font, float lineHeight) {
    font_ = font;
    lineHeight_ = lineHeight;
    cssStyle_ = generateCssStyle();
    update();
}

QString FontCardWidget::generateCssStyle() const {
    QString weightStr;
    switch (font_.weight()) {
        case QFont::Thin: weightStr = "100"; break;
        case QFont::ExtraLight: weightStr = "200"; break;
        case QFont::Light: weightStr = "300"; break;
        case QFont::Normal: weightStr = "400"; break;
        case QFont::Medium: weightStr = "500"; break;
        case QFont::DemiBold: weightStr = "600"; break;
        case QFont::Bold: weightStr = "700"; break;
        case QFont::ExtraBold: weightStr = "800"; break;
        case QFont::Black: weightStr = "900"; break;
        default: weightStr = "400"; break;
    }

    return QString("font-family: '%1'; font-size: %2sp; font-weight: %3; line-height: %4sp;")
        .arg(font_.family())
        .arg(font_.pointSizeF())
        .arg(weightStr)
        .arg(lineHeight_);
}

QSize FontCardWidget::sizeHint() const {
    QFontMetrics fm(font_);
    QFont tokenFont("Segoe UI", 20);
    QFontMetrics tokenFm(tokenFont);

    int tokenHeight = tokenFm.height();
    int propsHeight = fm.height() + 8;
    int padding = 14 * 2;
    int spacing = 8;

    int height = tokenHeight + spacing + fm.height() * 2 + spacing + propsHeight + padding;
    int width = 180;

    return QSize(width, height);
}

void FontCardWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRectF cardRect = rect().adjusted(4, 4, -4, -4);

    // Card background with shadow effect when hovered
    if (isHovered_) {
        QPainterPath path;
        path.addRoundedRect(cardRect, 12, 12);

        QPainterPath shadowPath = path.translated(0, 2);
        QColor shadowColor(0, 0, 0, 20);
        painter.fillPath(shadowPath, shadowColor);

        QPen borderPen(QColor(100, 100, 255), 2);
        painter.setPen(borderPen);
        painter.drawPath(path);
    }

    // Background fill
    QPainterPath path;
    path.addRoundedRect(cardRect, 12, 12);
    painter.fillPath(path, QColor(250, 250, 250));

    // Content padding
    const int padding = 14;
    QRectF contentRect = cardRect.adjusted(padding, padding, -padding, -padding);

    // Calculate heights
    QFont tokenFont("Segoe UI", 20);
    QFontMetrics tokenFm(tokenFont);
    int tokenHeight = tokenFm.height();
    int tokenY = contentRect.top();

    // Token name
    painter.setPen(QColor(120, 120, 120));
    painter.setFont(tokenFont);
    painter.drawText(contentRect.adjusted(0, tokenY, 0, 0), Qt::AlignLeft, tokenName_);

    // Font preview
    QFontMetrics fm(font_);
    int propsHeight = fm.height() + 8;
    int availableHeight = static_cast<int>(contentRect.height()) - tokenHeight - propsHeight - 15;
    int previewHeight = qMax(fm.height() * 2, availableHeight);

    QRectF previewRect(contentRect.left(), tokenY + tokenHeight + 6, contentRect.width(),
                       static_cast<qreal>(previewHeight));

    painter.setFont(font_);
    painter.setPen(QColor(30, 30, 30));

    QString wrappedText =
        fm.elidedText(previewText_, Qt::ElideRight, static_cast<int>(previewRect.width()));
    painter.drawText(previewRect, Qt::AlignLeft | Qt::AlignVCenter, wrappedText);

    // Font properties
    painter.setPen(QColor(100, 100, 100));
    QFont propsFont("Segoe UI", 8);
    painter.setFont(propsFont);

    QString props =
        QString("%1sp / %2sp / W%3").arg(font_.pointSizeF()).arg(lineHeight_).arg(font_.weight());

    QRectF propsRect = contentRect;
    propsRect.setBottom(cardRect.bottom() - 7);
    painter.drawText(propsRect, Qt::AlignLeft | Qt::AlignTop, props);

    // "Click to copy" hint
    if (isHovered_) {
        painter.setPen(QColor(100, 100, 255));
        painter.drawText(propsRect, Qt::AlignRight | Qt::AlignTop, "点击复制 CSS");
    }
}

void FontCardWidget::enterEvent(QEnterEvent* event) {
    Q_UNUSED(event)
    isHovered_ = true;
    update();
}

void FontCardWidget::leaveEvent(QEvent* event) {
    Q_UNUSED(event)
    isHovered_ = false;
    update();
}

void FontCardWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit clicked(cssStyle_);
    }
}

// =============================================================================
// TypographyPage Implementation
// =============================================================================

TypographyPage::TypographyPage(QWidget* parent) : ThemePageWidget(parent) {
    // Default colors (light theme)
    cardBgColor_ = QColor(250, 250, 250);
    cardBorderColor_ = QColor(200, 200, 200);
    textColor_ = QColor(60, 60, 60);

    setupUI();
    // Note: createFontGroups will be called after theme is set
}

void TypographyPage::setupUI() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Scroll area for font cards
    scrollArea_ = new QScrollArea(this);
    scrollArea_->setWidgetResizable(true);
    scrollArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea_->setFrameShape(QFrame::NoFrame);

    scrollContent_ = new QWidget();
    scrollLayout_ = new QVBoxLayout(scrollContent_);
    scrollLayout_->setContentsMargins(0, 0, 0, 0);
    scrollLayout_->setSpacing(20);

    // Grid layout for font cards
    fontGridLayout_ = new QGridLayout();
    fontGridLayout_->setSpacing(12);
    fontGridLayout_->setContentsMargins(24, 24, 24, 24);
    scrollLayout_->addLayout(fontGridLayout_);

    // Add stretch at bottom
    scrollLayout_->addStretch();

    scrollArea_->setWidget(scrollContent_);
    mainLayout->addWidget(scrollArea_, 1);

    // Background color
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(245, 245, 245));
    setPalette(pal);

    // Toast
    toast_ = new ToastWidget("", this);
    toast_->hide();
}

QString TypographyPage::getPreviewText(const QString& token) const {
    if (token.contains("Display")) {
        return "Ag 你好世界 Display";
    } else if (token.contains("Headline")) {
        return "标题文字 Headline 大标题";
    } else if (token.contains("Title")) {
        return "标题文字 Title 标题";
    } else if (token.contains("Body")) {
        return "正文内容 The quick brown fox";
    } else if (token.contains("Label")) {
        return "标签 LABEL 按钮 Button";
    }
    return "Preview Text 预览";
}

void TypographyPage::createFontGroups() {
    int row = 0;
    createFontGroup("Display Styles 展示样式", fontGridLayout_, row, DISPLAY_TOKENS);
    createFontGroup("Headline Styles 标题样式", fontGridLayout_, row, HEADLINE_TOKENS);
    createFontGroup("Title Styles 小标题样式", fontGridLayout_, row, TITLE_TOKENS);
    createFontGroup("Body Styles 正文样式", fontGridLayout_, row, BODY_TOKENS);
    createFontGroup("Label Styles 标签样式", fontGridLayout_, row, LABEL_TOKENS);
}

void TypographyPage::createFontGroup(const QString& title, QGridLayout* layout,
                                     int& row, const QStringList& tokens) {
    // Get typography from theme
    if (!theme_) return;
    auto& typography = const_cast<cf::ui::core::MaterialTypography&>(
        static_cast<const cf::ui::core::MaterialTypography&>(theme_->font_type()));

    // Section title label
    QLabel* titleLabel = new QLabel(title);
    QFont titleFont("Segoe UI", 13, QFont::DemiBold);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("QLabel { color: #49454F; padding: 4px 0px; }");

    layout->addWidget(titleLabel, row++, 0, 1, -1);

    // Calculate column count
    int cols = calculateColumnCount();

    // Create font cards
    int col = 0;
    for (const QString& token : tokens) {
        QFont font = typography.queryTargetFont(token.toUtf8().constData());
        float lineHeight = typography.getLineHeight(token.toUtf8().constData());
        QString previewText = getPreviewText(token);

        FontCardWidget* card = new FontCardWidget(token, font, lineHeight, previewText);

        // Connect click signal
        connect(card, &FontCardWidget::clicked, this,
                &TypographyPage::onFontCardClicked);

        // Store card info
        fontCards_.append({card, token});

        layout->addWidget(card, row, col);

        col++;
        if (col >= cols) {
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

int TypographyPage::calculateColumnCount() const {
    int width = scrollArea_->width();
    if (width < 800)
        return 2;
    if (width < 1200)
        return 3;
    return 4;
}

void TypographyPage::showToast(const QString& message) {
    ToastWidget* toast = new ToastWidget(message, this);
    toast->show();
}

void TypographyPage::onFontCardClicked(const QString& cssStyle) {
    QApplication::clipboard()->setText(cssStyle);
    showToast("CSS 样式已复制到剪贴板");
}

void TypographyPage::applyTheme(const cf::ui::core::ICFTheme& theme) {
    auto& colorScheme = static_cast<const cf::ui::core::MaterialColorScheme&>(theme.color_scheme());

    // Store theme pointer
    theme_ = &theme;

    // Update colors
    QColor bg = colorScheme.queryColor("md.background");
    QColor onSurface = colorScheme.queryColor("md.onSurface");

    // Update background
    scrollContent_->setAutoFillBackground(true);
    QPalette pal = scrollContent_->palette();
    pal.setColor(QPalette::Window, bg);
    scrollContent_->setPalette(pal);

    // Clear existing cards and recreate with new theme
    for (auto& info : fontCards_) {
        delete info.widget;
    }
    fontCards_.clear();
    // Clear layout items
    while (fontGridLayout_->count()) {
        QLayoutItem* item = fontGridLayout_->takeAt(0);
        if (item->widget()) {
            item->widget()->setParent(nullptr);
        }
        delete item;
    }
    createFontGroups();

    // Update font cards with new typography
    auto& typography = const_cast<cf::ui::core::MaterialTypography&>(
        static_cast<const cf::ui::core::MaterialTypography&>(theme.font_type()));
    for (auto& info : fontCards_) {
        QFont font = typography.queryTargetFont(info.token.toUtf8().constData());
        float lineHeight = typography.getLineHeight(info.token.toUtf8().constData());
        info.widget->updateFont(font, lineHeight);
    }
}

} // namespace cf::ui::gallery
