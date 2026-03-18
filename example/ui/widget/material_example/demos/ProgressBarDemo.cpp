/**
 * @file ProgressBarDemo.cpp
 * @brief Material Design 3 ProgressBar Demo - Implementation
 */

#include "ProgressBarDemo.h"

#include "ui/widget/material/widget/progressbar/progressbar.h"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QPushButton>

using namespace cf::ui::widget::material;

namespace cf::ui::example {

ProgressBarDemo::ProgressBarDemo(QWidget* parent) : QWidget(parent) {

    setupUI();
    createDeterminateSection();
    createIndeterminateSection();
    createStatesSection();
    createInteractiveDemoSection();

    // Add stretch at the end
    layout_->addStretch();
}

ProgressBarDemo::~ProgressBarDemo() {
    if (progressTimer_) {
        progressTimer_->stop();
    }
}

void ProgressBarDemo::setupUI() {
    // Scroll area for content
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Apply margins
    scrollArea->setContentsMargins(24, 24, 24, 24);

    scrollContent_ = new QWidget();
    layout_ = new QVBoxLayout(scrollContent_);
    layout_->setSpacing(24);
    layout_->setContentsMargins(0, 0, 0, 0);

    scrollArea->setWidget(scrollContent_);

    // Main layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(scrollArea);
}

void ProgressBarDemo::createDeterminateSection() {
    QGroupBox* groupBox = createGroupBox("Determinate Progress Bars (确定进度条)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // 0%
    QHBoxLayout* layout0 = new QHBoxLayout();
    QLabel* label0 = new QLabel("0%:", scrollContent_);
    label0->setMinimumWidth(80);
    ProgressBar* pb0 = new ProgressBar(scrollContent_);
    pb0->setValue(0);
    pb0->setTextVisible(true);
    layout0->addWidget(label0);
    layout0->addWidget(pb0);
    layout->addLayout(layout0);

    // 25%
    QHBoxLayout* layout25 = new QHBoxLayout();
    QLabel* label25 = new QLabel("25%:", scrollContent_);
    label25->setMinimumWidth(80);
    ProgressBar* pb25 = new ProgressBar(scrollContent_);
    pb25->setValue(25);
    pb25->setTextVisible(true);
    layout25->addWidget(label25);
    layout25->addWidget(pb25);
    layout->addLayout(layout25);

    // 50%
    QHBoxLayout* layout50 = new QHBoxLayout();
    QLabel* label50 = new QLabel("50%:", scrollContent_);
    label50->setMinimumWidth(80);
    ProgressBar* pb50 = new ProgressBar(scrollContent_);
    pb50->setValue(50);
    pb50->setTextVisible(true);
    layout50->addWidget(label50);
    layout50->addWidget(pb50);
    layout->addLayout(layout50);

    // 75%
    QHBoxLayout* layout75 = new QHBoxLayout();
    QLabel* label75 = new QLabel("75%:", scrollContent_);
    label75->setMinimumWidth(80);
    ProgressBar* pb75 = new ProgressBar(scrollContent_);
    pb75->setValue(75);
    pb75->setTextVisible(true);
    layout75->addWidget(label75);
    layout75->addWidget(pb75);
    layout->addLayout(layout75);

    // 100%
    QHBoxLayout* layout100 = new QHBoxLayout();
    QLabel* label100 = new QLabel("100%:", scrollContent_);
    label100->setMinimumWidth(80);
    ProgressBar* pb100 = new ProgressBar(scrollContent_);
    pb100->setValue(100);
    pb100->setTextVisible(true);
    layout100->addWidget(label100);
    layout100->addWidget(pb100);
    layout->addLayout(layout100);

    layout_->addWidget(groupBox);
}

void ProgressBarDemo::createIndeterminateSection() {
    QGroupBox* groupBox = createGroupBox("Indeterminate Progress Bar (不确定进度条)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    QHBoxLayout* hLayout = new QHBoxLayout();
    QLabel* label = new QLabel("Loading...", scrollContent_);
    label->setMinimumWidth(80);

    // Indeterminate: min=0, max=0
    ProgressBar* pb = new ProgressBar(scrollContent_);
    pb->setRange(0, 0);
    pb->setTextVisible(false);

    hLayout->addWidget(label);
    hLayout->addWidget(pb);
    layout->addLayout(hLayout);

    layout_->addWidget(groupBox);
}

void ProgressBarDemo::createStatesSection() {
    QGroupBox* groupBox = createGroupBox("Progress Bar States (进度条状态)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // Normal (enabled)
    QHBoxLayout* normalLayout = new QHBoxLayout();
    QLabel* normalLabel = new QLabel("Normal:", scrollContent_);
    normalLabel->setMinimumWidth(80);
    ProgressBar* normalPb = new ProgressBar(scrollContent_);
    normalPb->setValue(60);
    normalLayout->addWidget(normalLabel);
    normalLayout->addWidget(normalPb);
    layout->addLayout(normalLayout);

    // Disabled
    QHBoxLayout* disabledLayout = new QHBoxLayout();
    QLabel* disabledLabel = new QLabel("Disabled:", scrollContent_);
    disabledLabel->setMinimumWidth(80);
    ProgressBar* disabledPb = new ProgressBar(scrollContent_);
    disabledPb->setValue(60);
    disabledPb->setEnabled(false);
    disabledLayout->addWidget(disabledLabel);
    disabledLayout->addWidget(disabledPb);
    layout->addLayout(disabledLayout);

    // Without text
    QHBoxLayout* noTextLayout = new QHBoxLayout();
    QLabel* noTextLabel = new QLabel("No Text:", scrollContent_);
    noTextLabel->setMinimumWidth(80);
    ProgressBar* noTextPb = new ProgressBar(scrollContent_);
    noTextPb->setValue(80);
    noTextPb->setTextVisible(false);
    noTextLayout->addWidget(noTextLabel);
    noTextLayout->addWidget(noTextPb);
    layout->addLayout(noTextLayout);

    layout_->addWidget(groupBox);
}

void ProgressBarDemo::createInteractiveDemoSection() {
    QGroupBox* groupBox = createGroupBox("Interactive Demo (交互演示)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // Progress bar
    demoProgressBar_ = new ProgressBar(scrollContent_);
    demoProgressBar_->setValue(0);
    demoProgressBar_->setTextVisible(true);
    layout->addWidget(demoProgressBar_);

    // Control buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();

    QPushButton* decrementBtn = new QPushButton("-10", scrollContent_);
    QPushButton* incrementBtn = new QPushButton("+10", scrollContent_);
    QPushButton* resetBtn = new QPushButton("Reset", scrollContent_);

    connect(decrementBtn, &QPushButton::clicked, this, &ProgressBarDemo::onDecrementClicked);
    connect(incrementBtn, &QPushButton::clicked, this, &ProgressBarDemo::onIncrementClicked);
    connect(resetBtn, &QPushButton::clicked, this, &ProgressBarDemo::onResetClicked);

    buttonLayout->addStretch();
    buttonLayout->addWidget(decrementBtn);
    buttonLayout->addWidget(incrementBtn);
    buttonLayout->addWidget(resetBtn);
    buttonLayout->addStretch();

    layout->addLayout(buttonLayout);

    // Auto-progress button and timer
    QHBoxLayout* autoLayout = new QHBoxLayout();
    QPushButton* autoBtn = new QPushButton("Start Auto Progress", scrollContent_);
    connect(autoBtn, &QPushButton::clicked, this, [this, autoBtn]() {
        if (!progressTimer_) {
            progressTimer_ = new QTimer(this);
            connect(progressTimer_, &QTimer::timeout, this, &ProgressBarDemo::updateProgress);
        }

        if (progressTimer_->isActive()) {
            progressTimer_->stop();
            autoBtn->setText("Start Auto Progress");
        } else {
            progressTimer_->start(50); // Update every 50ms
            autoBtn->setText("Stop Auto Progress");
        }
    });

    autoLayout->addStretch();
    autoLayout->addWidget(autoBtn);
    autoLayout->addStretch();
    layout->addLayout(autoLayout);

    layout_->addWidget(groupBox);
}

void ProgressBarDemo::onIncrementClicked() {
    demoValue_ = qMin(demoValue_ + 10, 100);
    demoProgressBar_->setValue(demoValue_);
}

void ProgressBarDemo::onDecrementClicked() {
    demoValue_ = qMax(demoValue_ - 10, 0);
    demoProgressBar_->setValue(demoValue_);
}

void ProgressBarDemo::onResetClicked() {
    demoValue_ = 0;
    demoProgressBar_->setValue(0);
    if (progressTimer_) {
        progressTimer_->stop();
    }
}

void ProgressBarDemo::updateProgress() {
    demoValue_ += 1;
    if (demoValue_ > 100) {
        demoValue_ = 0;
    }
    demoProgressBar_->setValue(demoValue_);
}

QGroupBox* ProgressBarDemo::createGroupBox(const QString& title) {
    QGroupBox* groupBox = new QGroupBox(title, scrollContent_);
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->setSpacing(12);
    layout->setContentsMargins(16, 24, 16, 16);
    return groupBox;
}

} // namespace cf::ui::example
