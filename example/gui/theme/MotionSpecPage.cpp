/**
 * @file MotionSpecPage.cpp
 * @brief Motion Spec page - Implementation
 *
 * @author CFDesktop Team
 * @date 2026-02-28
 * @version 0.1
 */

#include "MotionSpecPage.h"
#include "ui/core/material/cfmaterial_motion.h"
#include "ui/core/material/cfmaterial_scheme.h"
#include "ui/core/theme.h"

#include <QApplication>
#include <QComboBox>
#include <QDebug>
#include <QFont>
#include <QFontMetrics>
#include <QLinearGradient>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QTimer>

namespace {

QString easingTypeToString(cf::ui::base::Easing::Type type) {
    switch (type) {
        case cf::ui::base::Easing::Type::Emphasized:
            return "Emphasized";
        case cf::ui::base::Easing::Type::EmphasizedDecelerate:
            return "EmphasizedDecelerate";
        case cf::ui::base::Easing::Type::EmphasizedAccelerate:
            return "EmphasizedAccelerate";
        case cf::ui::base::Easing::Type::Standard:
            return "Standard";
        case cf::ui::base::Easing::Type::StandardDecelerate:
            return "StandardDecelerate";
        case cf::ui::base::Easing::Type::StandardAccelerate:
            return "StandardAccelerate";
        case cf::ui::base::Easing::Type::Linear:
            return "Linear";
        default:
            return "Unknown";
    }
}

} // namespace

namespace cf::ui::gallery {

// =============================================================================
// MotionPreviewWidget Implementation
// =============================================================================

MotionPreviewWidget::MotionPreviewWidget(const core::MotionSpec& spec, const QString& name,
                                         QWidget* parent)
    : QWidget(parent), spec_(spec), name_(name) {
    setMinimumHeight(100);
    setMaximumHeight(160);

    timer_ = new QTimer(this);
    connect(timer_, &QTimer::timeout, this, &MotionPreviewWidget::updateAnimation);

    resetAnimation();
}

void MotionPreviewWidget::setProgress(float progress) {
    progress_ = qBound(0.0f, progress, 1.0f);

    float eased = calculateEasedProgress(progress_);
    QRectF r = rect().adjusted(40, 30, -40, -30);
    ballPosition_ = QPointF(r.left() + eased * r.width(), r.center().y());

    update();
    emit progressChanged();
}

float MotionPreviewWidget::calculateEasedProgress(float linearProgress) const {
    QEasingCurve curve = spec_.toEasingCurve();
    return curve.valueForProgress(linearProgress);
}

void MotionPreviewWidget::startAnimation() {
    isAnimating_ = true;
    elapsed_ = 0.0f;
    progress_ = 0.0f;
    timer_->start(16);
}

void MotionPreviewWidget::resetAnimation() {
    isAnimating_ = false;
    timer_->stop();
    progress_ = 0.0f;
    elapsed_ = 0.0f;
    ballPosition_ = QPointF(rect().left() + 40, rect().center().y());
    update();
}

void MotionPreviewWidget::updateAnimation() {
    if (!isAnimating_)
        return;

    elapsed_ += 16.0f;

    if (elapsed_ >= spec_.durationMs) {
        elapsed_ = spec_.durationMs;
        setProgress(1.0f);
        isAnimating_ = false;
        timer_->stop();
        emit animationFinished();
    } else {
        float linearProgress = elapsed_ / spec_.durationMs;
        setProgress(linearProgress);
    }
}

void MotionPreviewWidget::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRectF r = rect().adjusted(20, 20, -20, -20);

    // Background track
    QColor trackColor = isDarkTheme_ ? QColor(60, 60, 60) : QColor(220, 220, 220);
    QPainterPath trackPath;
    trackPath.addRoundedRect(r, 8, 8);
    painter.setPen(Qt::NoPen);
    painter.setBrush(trackColor);
    painter.drawPath(trackPath);

    // Progress fill
    QEasingCurve curve = spec_.toEasingCurve();
    QColor progressColor;
    switch (spec_.easing) {
        case base::Easing::Type::Emphasized:
        case base::Easing::Type::EmphasizedDecelerate:
            progressColor = QColor(103, 80, 164);
            break;
        case base::Easing::Type::EmphasizedAccelerate:
            progressColor = QColor(155, 93, 175);
            break;
        case base::Easing::Type::Standard:
        case base::Easing::Type::StandardDecelerate:
        case base::Easing::Type::StandardAccelerate:
            progressColor = QColor(98, 91, 113);
            break;
        case base::Easing::Type::Linear:
            progressColor = QColor(98, 91, 113);
            break;
        default:
            progressColor = QColor(103, 80, 164);
            break;
    }

    float eased = calculateEasedProgress(progress_);
    float fillWidth = r.width() * eased;

    if (fillWidth > 0) {
        QRectF fillRect(r.left(), r.top(), fillWidth, r.height());
        QPainterPath fillPath;
        fillPath.addRoundedRect(fillRect, 8, 8);

        QLinearGradient gradient(r.left(), 0, r.left() + fillWidth, 0);
        gradient.setColorAt(0, progressColor);
        gradient.setColorAt(1, progressColor.lighter(120));
        painter.setBrush(gradient);
        painter.drawPath(fillPath);
    }

    // Animated ball
    float ballRadius = 10;
    QPainterPath ballPath;
    ballPath.addEllipse(ballPosition_, ballRadius, ballRadius);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 0, 0, 50));
    painter.drawEllipse(ballPosition_ + QPointF(0, 2), ballRadius, ballRadius);

    QRadialGradient ballGradient(ballPosition_, ballRadius);
    ballGradient.setColorAt(0, QColor(255, 255, 255));
    ballGradient.setColorAt(1, QColor(220, 220, 220));
    painter.setBrush(ballGradient);
    painter.drawPath(ballPath);

    // Time markers
    QFont markerFont("Segoe UI", 8);
    painter.setFont(markerFont);
    QColor markerColor = isDarkTheme_ ? QColor(180, 180, 180) : QColor(100, 100, 100);
    painter.setPen(markerColor);

    painter.drawText(QRectF(r.left(), r.bottom() + 5, 40, 20), Qt::AlignCenter, "0ms");
    painter.drawText(QRectF(r.right() - 40, r.bottom() + 5, 40, 20), Qt::AlignCenter,
                     QString("%1ms").arg(spec_.durationMs));

    // Current time label
    if (isAnimating_ || progress_ > 0) {
        int currentTime = static_cast<int>(elapsed_);
        painter.drawText(QRectF(r.center().x() - 30, r.top() - 18, 60, 20), Qt::AlignCenter,
                         QString("%1ms").arg(currentTime));
    }
}

void MotionPreviewWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    resetAnimation();
}

void MotionPreviewWidget::updateSpec(const core::MotionSpec& spec) {
    spec_ = spec;
    resetAnimation();
}

// =============================================================================
// MotionCardWidget Implementation
// =============================================================================

MotionCardWidget::MotionCardWidget(const core::MotionSpec& spec, const QString& name,
                                   const QString& description, QWidget* parent)
    : QWidget(parent), spec_(spec), name_(name), description_(description) {
    setMinimumSize(240, 180);
    setMaximumSize(280, 220);
    setCursor(Qt::PointingHandCursor);

    backgroundColor_ = QColor(250, 250, 250);
    surfaceColor_ = QColor(245, 245, 245);
    onSurfaceColor_ = QColor(60, 60, 60);

    updateCurvePath();
}

void MotionCardWidget::updateCurvePath() {
    curvePath_ = QPainterPath();
    QRectF curveRect(10, 10, 80, 40);
    curvePath_.moveTo(curveRect.bottomLeft());

    QEasingCurve curve = spec_.toEasingCurve();
    for (int i = 0; i <= 20; i++) {
        float t = i / 20.0f;
        float value = curve.valueForProgress(t);
        QPointF point(curveRect.left() + t * curveRect.width(),
                      curveRect.bottom() - value * curveRect.height());
        curvePath_.lineTo(point);
    }
}

void MotionCardWidget::setThemeColors(const QColor& background, const QColor& surface,
                                      const QColor& onSurface) {
    backgroundColor_ = background;
    surfaceColor_ = surface;
    onSurfaceColor_ = onSurface;
    update();
}

void MotionCardWidget::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRectF r = rect().adjusted(4, 4, -4, -4);
    float radius = 16;

    // Card background with elevation
    QPainterPath path;
    path.addRoundedRect(r, radius, radius);

    if (isHovered_) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(0, 0, 0, 15));
        painter.drawPath(path.translated(0, 3));
    }

    QColor bgColor = isHovered_ ? surfaceColor_.lighter(105) : surfaceColor_;
    painter.setPen(QPen(QColor(200, 200, 200), 1));
    painter.setBrush(bgColor);
    painter.drawPath(path);

    // Title
    QFont titleFont("Segoe UI", 11, QFont::Bold);
    painter.setFont(titleFont);
    painter.setPen(onSurfaceColor_);
    QRectF titleRect = r.adjusted(14, 14, -14, 0);
    painter.drawText(titleRect, Qt::AlignLeft | Qt::AlignTop, name_);

    // Description
    QFont descFont("Segoe UI", 8);
    painter.setFont(descFont);
    QColor descColor = onSurfaceColor_;
    descColor.setAlpha(180);
    painter.setPen(descColor);
    QRectF descRect = titleRect.adjusted(0, QFontMetrics(titleFont).height() + 4, 0, 0);
    painter.drawText(descRect, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, description_);

    // Spec info
    QFont infoFont("Consolas", 9);
    painter.setFont(infoFont);
    QColor infoColor = QColor(103, 80, 164);
    painter.setPen(infoColor);

    QString specText =
        QString("⏱ %1ms • %2").arg(spec_.durationMs).arg(easingTypeToString(spec_.easing));

    QRectF specRect = r.adjusted(14, 0, -14, -45);
    painter.drawText(specRect, Qt::AlignLeft | Qt::AlignBottom, specText);

    // Easing curve preview
    QRectF curveBox(r.right() - 90, r.top() + 14, 76, 44);
    QPainterPath curveBoxPath;
    curveBoxPath.addRoundedRect(curveBox, 8, 8);

    painter.setPen(Qt::NoPen);
    painter.setBrush(isDarkTheme_ ? QColor(40, 40, 40) : QColor(235, 235, 235));
    painter.drawPath(curveBoxPath);

    painter.setPen(QPen(QColor(103, 80, 164), 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawPath(curvePath_);

    // Play button hint
    if (isHovered_) {
        QFont hintFont("Segoe UI", 8);
        painter.setFont(hintFont);
        painter.setPen(onSurfaceColor_);
        painter.drawText(r.adjusted(14, 0, -14, -14), Qt::AlignBottom | Qt::AlignHCenter,
                         "▶ 点击预览动画");
    }
}

void MotionCardWidget::enterEvent(QEnterEvent*) {
    isHovered_ = true;
    update();
}

void MotionCardWidget::leaveEvent(QEvent*) {
    isHovered_ = false;
    update();
}

void MotionCardWidget::mousePressEvent(QMouseEvent*) {
    emit playRequested(spec_);
}

// =============================================================================
// MotionSpecPage Implementation
// =============================================================================

MotionSpecPage::MotionSpecPage(QWidget* parent) : ThemePageWidget(parent) {
    // Initialize motion presets
    motionPresets_ = {
        {"Short Enter", "小元素入场 (按钮、图标)", core::MotionPresets::shortEnter()},
        {"Short Exit", "小元素离场", core::MotionPresets::shortExit()},
        {"Medium Enter", "中等元素入场 (卡片、列表)", core::MotionPresets::mediumEnter()},
        {"Medium Exit", "中等元素离场", core::MotionPresets::mediumExit()},
        {"Long Enter", "大元素入场 (对话框、页面)", core::MotionPresets::longEnter()},
        {"Long Exit", "大元素离场", core::MotionPresets::longExit()},
        {"State Change", "状态层动画 (hover、focus)", core::MotionPresets::stateChange()},
        {"Ripple Expand", "涟漪扩散动画", core::MotionPresets::rippleExpand()},
        {"Ripple Fade", "涟漪淡出动画", core::MotionPresets::rippleFade()}};

    setupUI();
    createPreviewSection();
    createMotionCards();
}

void MotionSpecPage::setupUI() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(16);

    // Preview frame
    previewFrame_ = new QFrame(this);
    previewFrame_->setFrameStyle(QFrame::NoFrame);
    previewLayout_ = new QVBoxLayout(previewFrame_);
    previewLayout_->setContentsMargins(20, 16, 20, 16);
    mainLayout->addWidget(previewFrame_);

    // Scroll area for motion cards
    scrollArea_ = new QScrollArea(this);
    scrollArea_->setWidgetResizable(true);
    scrollArea_->setFrameShape(QFrame::NoFrame);
    scrollArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    scrollContent_ = new QWidget();
    scrollLayout_ = new QVBoxLayout(scrollContent_);
    scrollLayout_->setSpacing(16);
    scrollLayout_->setContentsMargins(20, 0, 20, 20);

    cardsLayout_ = new QGridLayout();
    cardsLayout_->setSpacing(12);
    cardsLayout_->setContentsMargins(0, 0, 0, 0);
    scrollLayout_->addLayout(cardsLayout_);
    scrollLayout_->addStretch();

    scrollArea_->setWidget(scrollContent_);
    mainLayout->addWidget(scrollArea_);
}

void MotionSpecPage::createPreviewSection() {
    // Preview title
    previewLabel_ = new QLabel("动画预览区 - 点击下方卡片播放", this);
    QFont previewTitleFont("Segoe UI", 13, QFont::Medium);
    previewLabel_->setFont(previewTitleFont);
    previewLabel_->setStyleSheet("QLabel { color: #49454F; }");
    previewLayout_->addWidget(previewLabel_);

    // Preview widget container
    previewContainer_ = new QHBoxLayout();

    // Preview widget
    previewWidget_ = new MotionPreviewWidget(core::MotionPresets::shortEnter(), "shortEnter", this);
    previewContainer_->addWidget(previewWidget_, 1);

    // Info panel
    QVBoxLayout* infoPanel = new QVBoxLayout();

    // Speed control
    QLabel* speedLabel = new QLabel("播放速度:", this);
    speedLabel->setStyleSheet("QLabel { color: #49454F; }");
    infoPanel->addWidget(speedLabel);

    speedCombo_ = new QComboBox(this);
    speedCombo_->addItem("1x (正常)", 1);
    speedCombo_->addItem("0.5x (慢放)", 2);
    speedCombo_->addItem("0.25x (极慢)", 4);
    speedCombo_->setCurrentIndex(0);
    connect(speedCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &MotionSpecPage::onSpeedChanged);
    infoPanel->addWidget(speedCombo_);

    infoPanel->addStretch();

    // Spec info
    previewInfoLabel_ = new QLabel(this);
    previewInfoLabel_->setWordWrap(true);
    QFont infoFont("Consolas", 9);
    previewInfoLabel_->setFont(infoFont);
    previewInfoLabel_->setStyleSheet("QLabel { color: #49454F; }");
    infoPanel->addWidget(previewInfoLabel_);

    previewContainer_->addLayout(infoPanel, 1);
    previewLayout_->addLayout(previewContainer_);

    // Update initial info
    onPlayRequested(core::MotionPresets::shortEnter());
}

void MotionSpecPage::createMotionCards() {
    int row = 0, col = 0;
    int maxCols = 3;

    for (const auto& preset : motionPresets_) {
        auto* card =
            new MotionCardWidget(preset.spec, preset.name, preset.description, scrollContent_);

        cardsLayout_->addWidget(card, row, col);
        motionCards_.append(card);

        connect(card, &MotionCardWidget::playRequested, this, &MotionSpecPage::onPlayRequested);

        col++;
        if (col >= maxCols) {
            col = 0;
            row++;
        }
    }
}

void MotionSpecPage::updateWindowTheme() {
    // Background update is handled by applyTheme
}

QString MotionSpecPage::easingTypeToString(base::Easing::Type type) const {
    return ::easingTypeToString(type);
}

void MotionSpecPage::onPlayRequested(const core::MotionSpec& spec) {
    // Find preset name
    QString presetName;
    for (const auto& preset : motionPresets_) {
        if (preset.spec.durationMs == spec.durationMs && preset.spec.easing == spec.easing) {
            presetName = preset.name;
            break;
        }
    }

    // Create a copy with adjusted duration for animation speed
    int speedMultiplier = speedCombo_->currentData().toInt();
    core::MotionSpec adjustedSpec = spec;
    adjustedSpec.durationMs = spec.durationMs * speedMultiplier;

    // Update the existing preview widget with new spec
    if (previewWidget_) {
        previewWidget_->setDarkTheme(isDarkTheme_);
        previewWidget_->updateSpec(adjustedSpec);
    }

    // Update info label
    QString infoText = QString("动画名称: %1\n"
                               "持续时间: %2ms\n"
                               "缓动类型: %3\n"
                               "延迟: %4ms")
                           .arg(presetName)
                           .arg(adjustedSpec.durationMs)
                           .arg(easingTypeToString(spec.easing))
                           .arg(spec.delayMs);

    previewInfoLabel_->setText(infoText);

    // Start animation
    previewWidget_->startAnimation();
}

void MotionSpecPage::onAnimationFinished() {
    // Could add replay functionality here
}

void MotionSpecPage::onSpeedChanged(int index) {
    Q_UNUSED(index);
    // Speed will be applied on next play
}

void MotionSpecPage::applyTheme(const core::ICFTheme& theme) {
    auto& colorScheme = static_cast<const core::MaterialColorScheme&>(theme.color_scheme());

    QColor bg = colorScheme.queryColor("md.background");
    QColor surface = colorScheme.queryColor("md.surface");
    QColor onSurface = colorScheme.queryColor("md.onSurface");

    // Update background
    scrollContent_->setAutoFillBackground(true);
    QPalette pal = scrollContent_->palette();
    pal.setColor(QPalette::Window, bg);
    scrollContent_->setPalette(pal);

    previewLabel_->setStyleSheet(QString("QLabel { color: %1; }").arg(onSurface.name()));
    previewInfoLabel_->setStyleSheet(QString("QLabel { color: %1; }").arg(onSurface.name()));

    // Update preview frame
    previewFrame_->setStyleSheet(
        QString("QFrame { background-color: %1; border-radius: 12px; border: 1px solid %2; }")
            .arg(surface.name())
            .arg(colorScheme.queryColor("md.outlineVariant").name()));

    // Update all cards
    for (auto* card : motionCards_) {
        card->setThemeColors(bg, surface, onSurface);
    }

    isDarkTheme_ = (bg.value() < 128);
    previewWidget_->setDarkTheme(isDarkTheme_);

    for (auto* card : motionCards_) {
        card->setDarkTheme(isDarkTheme_);
    }
}

} // namespace cf::ui::gallery
