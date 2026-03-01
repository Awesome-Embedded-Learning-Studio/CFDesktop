/**
 * @file MaterialTypographyMainWindow.cpp
 * @brief Material Design 3 Typography Gallery - Implementation
 */

#include "MaterialTypographyMainWindow.h"
#include "material/material_factory.hpp"
#include "ui/core/token/typography/cfmaterial_typography_token_literals.h"

#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QSize>

// Global namespace alias for token literals
namespace token_literals = ::cf::ui::core::token::literals;

namespace cf::ui::gallery {

// =============================================================================
// Static Token Lists
// =============================================================================

const QStringList MaterialTypographyMainWindow::DISPLAY_TOKENS = {
    token_literals::TYPOGRAPHY_DISPLAY_LARGE, token_literals::TYPOGRAPHY_DISPLAY_MEDIUM,
    token_literals::TYPOGRAPHY_DISPLAY_SMALL};

const QStringList MaterialTypographyMainWindow::HEADLINE_TOKENS = {
    token_literals::TYPOGRAPHY_HEADLINE_LARGE, token_literals::TYPOGRAPHY_HEADLINE_MEDIUM,
    token_literals::TYPOGRAPHY_HEADLINE_SMALL};

const QStringList MaterialTypographyMainWindow::TITLE_TOKENS = {
    token_literals::TYPOGRAPHY_TITLE_LARGE, token_literals::TYPOGRAPHY_TITLE_MEDIUM,
    token_literals::TYPOGRAPHY_TITLE_SMALL};

const QStringList MaterialTypographyMainWindow::BODY_TOKENS = {
    token_literals::TYPOGRAPHY_BODY_LARGE, token_literals::TYPOGRAPHY_BODY_MEDIUM,
    token_literals::TYPOGRAPHY_BODY_SMALL};

const QStringList MaterialTypographyMainWindow::LABEL_TOKENS = {
    token_literals::TYPOGRAPHY_LABEL_LARGE, token_literals::TYPOGRAPHY_LABEL_MEDIUM,
    token_literals::TYPOGRAPHY_LABEL_SMALL};

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
        connect(anim, &QPropertyAnimation::finished, this, [this]() {
            hide();
            deleteLater();
        });
        anim->start(QPropertyAnimation::DeleteWhenStopped);
    });
}

void ToastWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Rounded rectangle background
    QRectF rect = this->rect().adjusted(2, 2, -2, -2);
    QPainterPath path;
    path.addRoundedRect(rect, 8, 8);

    // Semi-transparent black background
    QColor bgColor(0, 0, 0, 200);
    painter.fillPath(path, bgColor);

    // White text
    painter.setPen(Qt::white);
    QFont font("Segoe UI", 10);
    painter.setFont(font);

    QRect textRect = rect.toRect();
    painter.drawText(textRect, Qt::AlignCenter, message_);
}

// =============================================================================
// FontCardWidget Implementation
// =============================================================================

FontCardWidget::FontCardWidget(const QString& tokenName, const QFont& font, float lineHeight,
                               const QString& previewText, QWidget* parent)
    : QWidget(parent), tokenName_(tokenName), font_(font), lineHeight_(lineHeight),
      previewText_(previewText) {
    cssStyle_ = generateCssStyle();
    setCursor(Qt::PointingHandCursor);
    // Calculate minimum height based on font size
    QFontMetrics fm(font_);
    int minHeight = qMax(120, fm.height() * 3 + 60); // Enough space for preview + props
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
        case QFont::Thin:
            weightStr = "100";
            break;
        case QFont::ExtraLight:
            weightStr = "200";
            break;
        case QFont::Light:
            weightStr = "300";
            break;
        case QFont::Normal:
            weightStr = "400";
            break;
        case QFont::Medium:
            weightStr = "500";
            break;
        case QFont::DemiBold:
            weightStr = "600";
            break;
        case QFont::Bold:
            weightStr = "700";
            break;
        case QFont::ExtraBold:
            weightStr = "800";
            break;
        case QFont::Black:
            weightStr = "900";
            break;
        default:
            weightStr = "400";
            break;
    }

    return QString("font-family: '%1'; font-size: %2sp; font-weight: %3; line-height: %4sp;")
        .arg(font_.family())
        .arg(font_.pointSizeF())
        .arg(weightStr)
        .arg(lineHeight_);
}

QSize FontCardWidget::sizeHint() const {
    QFontMetrics fm(font_);
    QFont tokenFont("Segoe UI", 24);
    QFontMetrics tokenFm(tokenFont);

    int tokenHeight = tokenFm.height();
    int propsHeight = fm.height() + 8;
    int padding = 16 * 2; // Both sides
    int spacing = 8;      // Between elements

    int height = tokenHeight + spacing + fm.height() * 2 + spacing + propsHeight + padding;
    int width = 200; // Minimum width

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

        // Shadow
        QPainterPath shadowPath = path.translated(0, 2);
        QColor shadowColor(0, 0, 0, 20);
        painter.fillPath(shadowPath, shadowColor);

        // Border
        QPen borderPen(QColor(100, 100, 255), 2);
        painter.setPen(borderPen);
        painter.drawPath(path);
    }

    // Background fill
    QPainterPath path;
    path.addRoundedRect(cardRect, 12, 12);
    painter.fillPath(path, QColor(250, 250, 250));

    // Content padding
    const int padding = 16;
    QRectF contentRect = cardRect.adjusted(padding, padding, -padding, -padding);

    // Calculate heights dynamically
    QFont tokenFont("Segoe UI", 24);
    QFontMetrics tokenFm(tokenFont);
    int tokenHeight = tokenFm.height();
    int tokenY = contentRect.top();

    // Token name (small, gray)
    painter.setPen(QColor(120, 120, 120));
    painter.setFont(tokenFont);
    painter.drawText(contentRect.adjusted(0, tokenY, 0, 0), Qt::AlignLeft, tokenName_);

    // Font preview - calculate available space
    QFontMetrics fm(font_);
    int propsHeight = fm.height() + 8; // Props at bottom
    int availableHeight = static_cast<int>(contentRect.height()) - tokenHeight - propsHeight - 20;
    int previewHeight = qMax(fm.height() * 2, availableHeight);

    QRectF previewRect(contentRect.left(), tokenY + tokenHeight + 8, contentRect.width(),
                       static_cast<qreal>(previewHeight));

    painter.setFont(font_);
    painter.setPen(QColor(30, 30, 30));

    // Draw preview text with elision
    QString wrappedText =
        fm.elidedText(previewText_, Qt::ElideRight, static_cast<int>(previewRect.width()));
    painter.drawText(previewRect, Qt::AlignLeft | Qt::AlignVCenter, wrappedText);

    // Font properties - fixed distance below preview
    painter.setPen(QColor(100, 100, 100));
    QFont propsFont("Segoe UI", 9);
    painter.setFont(propsFont);

    QString props =
        QString("%1sp / %2sp / W%3").arg(font_.pointSizeF()).arg(lineHeight_).arg(font_.weight());

    QRectF propsRect = contentRect;
    propsRect.setBottom(cardRect.bottom() - 9);
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
// MaterialTypographyMainWindow Implementation
// =============================================================================

MaterialTypographyMainWindow::MaterialTypographyMainWindow(QWidget* parent) : QMainWindow(parent) {
    // Create typography
    typography_ = cf::ui::core::material::defaultTypography();

    setupUI();
    createFontGroups();
}

MaterialTypographyMainWindow::~MaterialTypographyMainWindow() = default;

void MaterialTypographyMainWindow::setupUI() {
    // Central widget
    centralWidget_ = new QWidget(this);
    setCentralWidget(centralWidget_);

    // Main layout
    mainLayout_ = new QVBoxLayout(centralWidget_);
    mainLayout_->setContentsMargins(20, 20, 20, 20);
    mainLayout_->setSpacing(16);

    // Header
    createHeader();

    // Scroll area for font cards
    scrollArea_ = new QScrollArea(this);
    scrollArea_->setWidgetResizable(true);
    scrollArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea_->setFrameShape(QFrame::NoFrame);

    scrollContent_ = new QWidget();
    scrollLayout_ = new QVBoxLayout(scrollContent_);
    scrollLayout_->setContentsMargins(0, 0, 0, 0);
    scrollLayout_->setSpacing(24);

    // Grid layout for font cards
    fontGridLayout_ = new QGridLayout();
    fontGridLayout_->setSpacing(16);
    scrollLayout_->addLayout(fontGridLayout_);

    // Add stretch at bottom
    scrollLayout_->addStretch();

    scrollArea_->setWidget(scrollContent_);
    mainLayout_->addWidget(scrollArea_, 1);

    // Background color
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(245, 245, 245));
    setPalette(pal);
}

void MaterialTypographyMainWindow::createHeader() {
    headerLayout_ = new QHBoxLayout();
    headerLayout_->setSpacing(16);

    // Title
    titleLabel_ = new QLabel("Material Design 3 Type Scale", this);
    QFont titleFont("Segoe UI", 24, QFont::Bold);
    titleLabel_->setFont(titleFont);
    titleLabel_->setStyleSheet("QLabel { color: #1C1B1F; }");

    headerLayout_->addWidget(titleLabel_);
    headerLayout_->addStretch();

    mainLayout_->addLayout(headerLayout_);
}

QString MaterialTypographyMainWindow::getPreviewText(const QString& token) const {
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

void MaterialTypographyMainWindow::createFontGroups() {
    int row = 0;
    createFontGroup("Display Styles", fontGridLayout_, row, DISPLAY_TOKENS);
    createFontGroup("Headline Styles", fontGridLayout_, row, HEADLINE_TOKENS);
    createFontGroup("Title Styles", fontGridLayout_, row, TITLE_TOKENS);
    createFontGroup("Body Styles", fontGridLayout_, row, BODY_TOKENS);
    createFontGroup("Label Styles", fontGridLayout_, row, LABEL_TOKENS);
}

void MaterialTypographyMainWindow::createFontGroup(const QString& title, QGridLayout* layout,
                                                   int& row, const QStringList& tokens) {
    // Section title label
    QLabel* titleLabel = new QLabel(title);
    QFont titleFont("Segoe UI", 14, QFont::DemiBold);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("QLabel { color: #49454F; padding: 8px 0px; }");

    layout->addWidget(titleLabel, row++, 0, 1, -1);

    // Calculate column count
    int cols = calculateColumnCount();

    // Create font cards
    int col = 0;
    for (const QString& token : tokens) {
        QFont font = typography_.queryTargetFont(token.toUtf8().constData());
        float lineHeight = typography_.getLineHeight(token.toUtf8().constData());
        QString previewText = getPreviewText(token);

        FontCardWidget* card = new FontCardWidget(token, font, lineHeight, previewText);

        // Connect click signal
        connect(card, &FontCardWidget::clicked, this,
                &MaterialTypographyMainWindow::onFontCardClicked);

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

int MaterialTypographyMainWindow::calculateColumnCount() const {
    int width = scrollArea_->width();
    if (width < 800)
        return 2;
    if (width < 1200)
        return 3;
    return 4;
}

void MaterialTypographyMainWindow::showToast(const QString& message) {
    ToastWidget* toast = new ToastWidget(message, this);
    toast->show();
}

void MaterialTypographyMainWindow::onFontCardClicked(const QString& cssStyle) {
    // Copy to clipboard
    QApplication::clipboard()->setText(cssStyle);
    showToast("CSS 样式已复制到剪贴板");
}

} // namespace cf::ui::gallery
