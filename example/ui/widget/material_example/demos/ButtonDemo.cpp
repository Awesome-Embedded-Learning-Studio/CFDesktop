/**
 * @file ButtonDemo.cpp
 * @brief Material Design 3 Button Demo - Implementation
 */

#include "ButtonDemo.h"

#include "ui/widget/material/widget/button/button.h"

#include <QApplication>
#include <QFont>
#include <QStyle>
#include <QGroupBox>
#include <QLabel>
#include <QGridLayout>
#include <QHBoxLayout>

using namespace cf::ui::widget::material;
using ButtonVariant = cf::ui::widget::material::Button::ButtonVariant;

namespace cf::ui::example {

ButtonDemo::ButtonDemo(QWidget* parent) : QWidget(parent) {

    setupUI();
    createButtonVariantsSection();
    createButtonStatesSection();
    createButtonWithIconSection();
    createElevationSection();
    createInteractionDemoSection();

    layout_->addStretch();
}

void ButtonDemo::setupUI() {
    layout_ = new QVBoxLayout(this);
    layout_->setContentsMargins(24, 24, 24, 24);
    layout_->setSpacing(16);

    // Scroll area for content
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    scrollContent_ = new QWidget();
    QVBoxLayout* scrollLayout = new QVBoxLayout(scrollContent_);
    scrollLayout->setSpacing(24);
    scrollLayout->setContentsMargins(0, 0, 0, 0);

    scrollArea->setWidget(scrollContent_);
    layout_->addWidget(scrollArea);
}

void ButtonDemo::createButtonVariantsSection() {
    QGroupBox* groupBox = new QGroupBox("Button Variants (按钮变体)", this);
    QGridLayout* gridLayout = new QGridLayout(groupBox);
    gridLayout->setSpacing(16);
    gridLayout->setContentsMargins(16, 24, 16, 16);

    // Create 5 button variants in 2x3 grid layout
    // Row 1, Col 0: Filled Button
    QLabel* filledLabel = new QLabel("Filled", this);
    filledLabel->setAlignment(Qt::AlignCenter);
    Button* filledButton = new Button("Filled", ButtonVariant::Filled, this);
    gridLayout->addWidget(filledLabel, 0, 0);
    gridLayout->addWidget(filledButton, 1, 0);

    // Row 1, Col 1: Tonal Button
    QLabel* tonalLabel = new QLabel("Tonal", this);
    tonalLabel->setAlignment(Qt::AlignCenter);
    Button* tonalButton = new Button("Tonal", ButtonVariant::Tonal, this);
    gridLayout->addWidget(tonalLabel, 0, 1);
    gridLayout->addWidget(tonalButton, 1, 1);

    // Row 1, Col 2: Outlined Button
    QLabel* outlinedLabel = new QLabel("Outlined", this);
    outlinedLabel->setAlignment(Qt::AlignCenter);
    Button* outlinedButton = new Button("Outlined", ButtonVariant::Outlined, this);
    gridLayout->addWidget(outlinedLabel, 0, 2);
    gridLayout->addWidget(outlinedButton, 1, 2);

    // Row 2, Col 0: Text Button
    QLabel* textLabel = new QLabel("Text", this);
    textLabel->setAlignment(Qt::AlignCenter);
    Button* textButton = new Button("Text", ButtonVariant::Text, this);
    gridLayout->addWidget(textLabel, 2, 0);
    gridLayout->addWidget(textButton, 3, 0);

    // Row 2, Col 1: Elevated Button
    QLabel* elevatedLabel = new QLabel("Elevated", this);
    elevatedLabel->setAlignment(Qt::AlignCenter);
    Button* elevatedButton = new Button("Elevated", ButtonVariant::Elevated, this);
    gridLayout->addWidget(elevatedLabel, 2, 1);
    gridLayout->addWidget(elevatedButton, 3, 1);

    // Add stretch to empty cell (Row 2, Col 2)
    gridLayout->setRowStretch(4, 1);
    gridLayout->setColumnStretch(0, 1);
    gridLayout->setColumnStretch(1, 1);
    gridLayout->setColumnStretch(2, 1);

    scrollContent_->layout()->addWidget(groupBox);
}

void ButtonDemo::createButtonStatesSection() {
    QGroupBox* groupBox = createGroupBox("Button States (按钮状态)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // Normal (enabled) buttons
    QHBoxLayout* normalLayout = new QHBoxLayout();
    QLabel* normalLabel = new QLabel("Normal:", this);
    normalLabel->setMinimumWidth(120);
    Button* normalButton = new Button("Enabled Button", ButtonVariant::Filled, this);
    normalLayout->addWidget(normalLabel);
    normalLayout->addWidget(normalButton);
    normalLayout->addStretch();
    layout->addLayout(normalLayout);

    // Disabled buttons
    QHBoxLayout* disabledLayout = new QHBoxLayout();
    QLabel* disabledLabel = new QLabel("Disabled:", this);
    disabledLabel->setMinimumWidth(120);
    Button* disabledFilled = new Button("Disabled Filled", ButtonVariant::Filled, this);
    disabledFilled->setEnabled(false);
    Button* disabledOutlined = new Button("Disabled Outlined", ButtonVariant::Outlined, this);
    disabledOutlined->setEnabled(false);
    Button* disabledText = new Button("Disabled Text", ButtonVariant::Text, this);
    disabledText->setEnabled(false);
    disabledLayout->addWidget(disabledLabel);
    disabledLayout->addWidget(disabledFilled);
    disabledLayout->addWidget(disabledOutlined);
    disabledLayout->addWidget(disabledText);
    disabledLayout->addStretch();
    layout->addLayout(disabledLayout);

    scrollContent_->layout()->addWidget(groupBox);
}

void ButtonDemo::createButtonWithIconSection() {
    QGroupBox* groupBox = createGroupBox("Buttons with Icons (带图标的按钮)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // Filled with icon
    QHBoxLayout* filledIconLayout = new QHBoxLayout();
    QLabel* filledIconLabel = new QLabel("Filled + Icon:", this);
    filledIconLabel->setMinimumWidth(120);
    Button* filledIconButton = new Button("Save", ButtonVariant::Filled, this);
    // Use standard Qt icon as example
    filledIconButton->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton));
    filledIconLayout->addWidget(filledIconLabel);
    filledIconLayout->addWidget(filledIconButton);
    filledIconLayout->addStretch();
    layout->addLayout(filledIconLayout);

    // Outlined with icon
    QHBoxLayout* outlinedIconLayout = new QHBoxLayout();
    QLabel* outlinedIconLabel = new QLabel("Outlined + Icon:", this);
    outlinedIconLabel->setMinimumWidth(120);
    Button* outlinedIconButton = new Button("Open", ButtonVariant::Outlined, this);
    outlinedIconButton->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogOpenButton));
    outlinedIconLayout->addWidget(outlinedIconLabel);
    outlinedIconLayout->addWidget(outlinedIconButton);
    outlinedIconLayout->addStretch();
    layout->addLayout(outlinedIconLayout);

    // Tonal with icon
    QHBoxLayout* tonalIconLayout = new QHBoxLayout();
    QLabel* tonalIconLabel = new QLabel("Tonal + Icon:", this);
    tonalIconLabel->setMinimumWidth(120);
    Button* tonalIconButton = new Button("Delete", ButtonVariant::Tonal, this);
    tonalIconButton->setIcon(QApplication::style()->standardIcon(QStyle::SP_TrashIcon));
    tonalIconLayout->addWidget(tonalIconLabel);
    tonalIconLayout->addWidget(tonalIconButton);
    tonalIconLayout->addStretch();
    layout->addLayout(tonalIconLayout);

    scrollContent_->layout()->addWidget(groupBox);
}

void ButtonDemo::createElevationSection() {
    QGroupBox* groupBox = createGroupBox("Elevation Levels (阴影级别)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // Elevation 0 (default)
    QHBoxLayout* e0Layout = new QHBoxLayout();
    QLabel* e0Label = new QLabel("Elevation 0:", this);
    e0Label->setMinimumWidth(120);
    Button* e0Button = new Button("Level 0", ButtonVariant::Elevated, this);
    e0Button->setElevation(0);
    e0Layout->addWidget(e0Label);
    e0Layout->addWidget(e0Button);
    e0Layout->addStretch();
    layout->addLayout(e0Layout);

    // Elevation 1
    QHBoxLayout* e1Layout = new QHBoxLayout();
    QLabel* e1Label = new QLabel("Elevation 1:", this);
    e1Label->setMinimumWidth(120);
    Button* e1Button = new Button("Level 1", ButtonVariant::Elevated, this);
    e1Button->setElevation(1);
    e1Layout->addWidget(e1Label);
    e1Layout->addWidget(e1Button);
    e1Layout->addStretch();
    layout->addLayout(e1Layout);

    // Elevation 2
    QHBoxLayout* e2Layout = new QHBoxLayout();
    QLabel* e2Label = new QLabel("Elevation 2:", this);
    e2Label->setMinimumWidth(120);
    Button* e2Button = new Button("Level 2", ButtonVariant::Elevated, this);
    e2Button->setElevation(2);
    e2Layout->addWidget(e2Label);
    e2Layout->addWidget(e2Button);
    e2Layout->addStretch();
    layout->addLayout(e2Layout);

    // Elevation 3
    QHBoxLayout* e3Layout = new QHBoxLayout();
    QLabel* e3Label = new QLabel("Elevation 3:", this);
    e3Label->setMinimumWidth(120);
    Button* e3Button = new Button("Level 3", ButtonVariant::Elevated, this);
    e3Button->setElevation(3);
    e3Layout->addWidget(e3Label);
    e3Layout->addWidget(e3Button);
    e3Layout->addStretch();
    layout->addLayout(e3Layout);

    // Elevation 4
    QHBoxLayout* e4Layout = new QHBoxLayout();
    QLabel* e4Label = new QLabel("Elevation 4:", this);
    e4Label->setMinimumWidth(120);
    Button* e4Button = new Button("Level 4", ButtonVariant::Elevated, this);
    e4Button->setElevation(4);
    e4Layout->addWidget(e4Label);
    e4Layout->addWidget(e4Button);
    e4Layout->addStretch();
    layout->addLayout(e4Layout);

    scrollContent_->layout()->addWidget(groupBox);
}

void ButtonDemo::createInteractionDemoSection() {
    QGroupBox* groupBox = createGroupBox("Button Interaction (按钮交互)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // Click counter demo
    QHBoxLayout* counterLayout = new QHBoxLayout();
    QLabel* hintLabel = new QLabel("Click the button to test interaction:", this);
    hintLabel->setMinimumWidth(200);
    Button* demoButton = new Button("Click me (0)", ButtonVariant::Filled, this);
    connect(demoButton, &Button::clicked, this, &ButtonDemo::onDemoButtonClicked);
    counterLayout->addWidget(hintLabel);
    counterLayout->addWidget(demoButton);
    counterLayout->addStretch();
    layout->addLayout(counterLayout);

    scrollContent_->layout()->addWidget(groupBox);
}

void ButtonDemo::onDemoButtonClicked() {
    clickCount_++;
    Button* button = qobject_cast<Button*>(sender());
    if (button) {
        button->setText(QString("Click me (%1)").arg(clickCount_));
    }
}

QGroupBox* ButtonDemo::createGroupBox(const QString& title) {
    QGroupBox* groupBox = new QGroupBox(title, this);
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->setSpacing(12);
    layout->setContentsMargins(16, 24, 16, 16);
    return groupBox;
}

} // namespace cf::ui::example
