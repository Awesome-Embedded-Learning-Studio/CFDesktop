/**
 * @file GroupBoxDemo.cpp
 * @brief Material Design 3 GroupBox Demo - Implementation
 */

#include "GroupBoxDemo.h"

#include "ui/widget/material/widget/button/button.h"
#include "ui/widget/material/widget/groupbox/groupbox.h"

#include <QCheckBox>
#include <QFont>
#include <QLineEdit>
#include <QRadioButton>
#include <QSlider>
#include <QSpinBox>

using namespace cf::ui::widget::material;
using ButtonVariant = cf::ui::widget::material::Button::ButtonVariant;

namespace cf::ui::example {

GroupBoxDemo::GroupBoxDemo(QWidget* parent) : QWidget(parent) {

    setupUI();
    createBasicGroupBoxSection();
    createElevationSection();
    createBorderSection();
    createCornerRadiusSection();
    createContentExamplesSection();
    createNestedGroupBoxSection();

    // Add stretch at the end
    layout_->addStretch();
}

void GroupBoxDemo::setupUI() {
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

void GroupBoxDemo::createBasicGroupBoxSection() {
    GroupBox* groupBox = new GroupBox("Basic GroupBox (基本分组框)", scrollContent_);
    QVBoxLayout* gbLayout = new QVBoxLayout(groupBox);
    gbLayout->setSpacing(12);
    gbLayout->setContentsMargins(16, 32, 16, 16);

    // Description label
    QLabel* descLabel = new QLabel(
        "This is a basic Material Design 3 GroupBox with default settings.\n"
        "It features rounded corners, a subtle elevation shadow, and a border.",
        groupBox);
    descLabel->setWordWrap(true);
    gbLayout->addWidget(descLabel);

    // Example content
    QHBoxLayout* hLayout = new QHBoxLayout();
    Button* button1 = new Button("Button 1", ButtonVariant::Filled, groupBox);
    Button* button2 = new Button("Button 2", ButtonVariant::Outlined, groupBox);
    hLayout->addWidget(button1);
    hLayout->addWidget(button2);
    hLayout->addStretch();
    gbLayout->addLayout(hLayout);

    layout_->addWidget(groupBox);
}

void GroupBoxDemo::createElevationSection() {
    // Section title
    QLabel* sectionLabel = new QLabel("Elevation Levels (阴影级别)", scrollContent_);
    QFont sectionFont("Segoe UI", 14, QFont::Bold);
    sectionLabel->setFont(sectionFont);
    layout_->addWidget(sectionLabel);

    // Grid layout for different elevations
    QWidget* gridWidget = new QWidget(scrollContent_);
    gridWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    QGridLayout* gridLayout = new QGridLayout(gridWidget);
    gridLayout->setSpacing(16);

    // Add column stretch for even distribution
    gridLayout->setColumnStretch(0, 1);
    gridLayout->setColumnStretch(1, 1);

    // Create group boxes with different elevation levels
    for (int level = 0; level <= 5; ++level) {
        GroupBox* gb = new GroupBox(QString("Elevation %1").arg(level), gridWidget);
        gb->setElevation(level);

        QVBoxLayout* gbLayout = new QVBoxLayout(gb);
        QLabel* label = new QLabel(QString("Shadow level: %1").arg(level), gb);
        gbLayout->addWidget(label);

        int row = level / 2;
        int col = level % 2;
        gridLayout->addWidget(gb, row, col);
    }

    layout_->addWidget(gridWidget);
}

void GroupBoxDemo::createBorderSection() {
    // Section title
    QLabel* sectionLabel = new QLabel("Border Variants (边框样式)", scrollContent_);
    QFont sectionFont("Segoe UI", 14, QFont::Bold);
    sectionLabel->setFont(sectionFont);
    layout_->addWidget(sectionLabel);

    // Horizontal layout
    QWidget* hWidget = new QWidget(scrollContent_);
    QHBoxLayout* hLayout = new QHBoxLayout(hWidget);
    hLayout->setSpacing(16);

    // With border (default)
    GroupBox* withBorder = new GroupBox("With Border", hWidget);
    withBorder->setHasBorder(true);
    withBorder->setElevation(1);
    QVBoxLayout* wbLayout = new QVBoxLayout(withBorder);
    QLabel* wbLabel = new QLabel("This group box has a border.", withBorder);
    wbLayout->addWidget(wbLabel);
    hLayout->addWidget(withBorder);

    // Without border
    GroupBox* noBorder = new GroupBox("Without Border", hWidget);
    noBorder->setHasBorder(false);
    noBorder->setElevation(2);
    QVBoxLayout* nbLayout = new QVBoxLayout(noBorder);
    QLabel* nbLabel = new QLabel("This group box has no border,\nonly elevation shadow.", noBorder);
    nbLabel->setWordWrap(true);
    nbLayout->addWidget(nbLabel);
    hLayout->addWidget(noBorder);

    hLayout->addStretch();
    layout_->addWidget(hWidget);
}

void GroupBoxDemo::createCornerRadiusSection() {
    // Section title
    QLabel* sectionLabel = new QLabel("Corner Radius (圆角半径)", scrollContent_);
    QFont sectionFont("Segoe UI", 14, QFont::Bold);
    sectionLabel->setFont(sectionFont);
    layout_->addWidget(sectionLabel);

    // Horizontal layout
    QWidget* hWidget = new QWidget(scrollContent_);
    QHBoxLayout* hLayout = new QHBoxLayout(hWidget);
    hLayout->setSpacing(16);

    // Small corner radius
    GroupBox* smallRadius = new GroupBox("Small Radius (4dp)", hWidget);
    smallRadius->setCornerRadius(4.0f);
    smallRadius->setElevation(1);
    QVBoxLayout* srLayout = new QVBoxLayout(smallRadius);
    srLayout->addWidget(new QLabel("Subtle rounding", smallRadius));
    hLayout->addWidget(smallRadius);

    // Medium corner radius
    GroupBox* mediumRadius = new GroupBox("Medium Radius (12dp)", hWidget);
    mediumRadius->setCornerRadius(12.0f);
    mediumRadius->setElevation(1);
    QVBoxLayout* mrLayout = new QVBoxLayout(mediumRadius);
    mrLayout->addWidget(new QLabel("Medium rounding", mediumRadius));
    hLayout->addWidget(mediumRadius);

    // Large corner radius
    GroupBox* largeRadius = new GroupBox("Large Radius (20dp)", hWidget);
    largeRadius->setCornerRadius(20.0f);
    largeRadius->setElevation(1);
    QVBoxLayout* lrLayout = new QVBoxLayout(largeRadius);
    lrLayout->addWidget(new QLabel("Pronounced rounding", largeRadius));
    hLayout->addWidget(largeRadius);

    hLayout->addStretch();
    layout_->addWidget(hWidget);
}

void GroupBoxDemo::createContentExamplesSection() {
    // Section title
    QLabel* sectionLabel = new QLabel("Content Examples (内容示例)", scrollContent_);
    QFont sectionFont("Segoe UI", 14, QFont::Bold);
    sectionLabel->setFont(sectionFont);
    layout_->addWidget(sectionLabel);

    // Form controls example
    GroupBox* formGroup = new GroupBox("Form Controls (表单控件)", scrollContent_);
    formGroup->setElevation(1);
    QGridLayout* formLayout = new QGridLayout(formGroup);
    formLayout->setSpacing(12);
    formLayout->setContentsMargins(16, 32, 16, 16);

    // Text input
    formLayout->addWidget(new QLabel("Name:", formGroup), 0, 0);
    formLayout->addWidget(new QLineEdit(formGroup), 0, 1);

    // Checkboxes
    formLayout->addWidget(new QLabel("Options:", formGroup), 1, 0);
    QVBoxLayout* checkLayout = new QVBoxLayout();
    checkLayout->addWidget(new QCheckBox("Enable feature A", formGroup));
    checkLayout->addWidget(new QCheckBox("Enable feature B", formGroup));
    formLayout->addLayout(checkLayout, 1, 1);

    // Radio buttons
    formLayout->addWidget(new QLabel("Mode:", formGroup), 2, 0);
    QVBoxLayout* radioLayout = new QVBoxLayout();
    QRadioButton* simpleModeRadio = new QRadioButton("Simple mode", formGroup);
    radioLayout->addWidget(simpleModeRadio);
    radioLayout->addWidget(new QRadioButton("Advanced mode", formGroup));
    simpleModeRadio->setChecked(true); // Select first
    formLayout->addLayout(radioLayout, 2, 1);

    // Slider
    formLayout->addWidget(new QLabel("Value:", formGroup), 3, 0);
    QSlider* slider = new QSlider(Qt::Horizontal, formGroup);
    slider->setRange(0, 100);
    slider->setValue(50);
    formLayout->addWidget(slider, 3, 1);

    // Spin box
    formLayout->addWidget(new QLabel("Quantity:", formGroup), 4, 0);
    formLayout->addWidget(new QSpinBox(formGroup), 4, 1);

    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(new Button("Submit", ButtonVariant::Filled, formGroup));
    buttonLayout->addWidget(new Button("Cancel", ButtonVariant::Outlined, formGroup));
    buttonLayout->addStretch();
    formLayout->addLayout(buttonLayout, 5, 0, 1, 2);

    layout_->addWidget(formGroup);

    // Settings example
    GroupBox* settingsGroup = new GroupBox("Settings (设置)", scrollContent_);
    settingsGroup->setElevation(2);
    QVBoxLayout* settingsLayout = new QVBoxLayout(settingsGroup);
    settingsLayout->setSpacing(12);
    settingsLayout->setContentsMargins(16, 32, 16, 16);

    settingsLayout->addWidget(new QCheckBox("Show notifications", settingsGroup));
    settingsLayout->addWidget(new QCheckBox("Auto-save changes", settingsGroup));
    settingsLayout->addWidget(new QCheckBox("Enable dark mode", settingsGroup));
    settingsLayout->addWidget(new QCheckBox("Remember my preferences", settingsGroup));

    settingsLayout->addStretch();

    QHBoxLayout* actionLayout = new QHBoxLayout();
    actionLayout->addStretch();
    actionLayout->addWidget(new Button("Apply Settings", ButtonVariant::Tonal, settingsGroup));
    settingsLayout->addLayout(actionLayout);

    layout_->addWidget(settingsGroup);
}

void GroupBoxDemo::createNestedGroupBoxSection() {
    // Section title
    QLabel* sectionLabel = new QLabel("Nested GroupBoxes (嵌套分组框)", scrollContent_);
    QFont sectionFont("Segoe UI", 14, QFont::Bold);
    sectionLabel->setFont(sectionFont);
    layout_->addWidget(sectionLabel);

    // Outer group box
    GroupBox* outerGroup = new GroupBox("Outer GroupBox", scrollContent_);
    outerGroup->setElevation(1);
    QVBoxLayout* outerLayout = new QVBoxLayout(outerGroup);
    outerLayout->setSpacing(16);
    outerLayout->setContentsMargins(16, 32, 16, 16);

    QLabel* outerLabel = new QLabel("This is the outer group box containing nested group boxes:", outerGroup);
    outerLayout->addWidget(outerLabel);

    // Inner group box 1
    GroupBox* inner1 = new GroupBox("Inner Group 1", outerGroup);
    inner1->setElevation(2);
    QVBoxLayout* inner1Layout = new QVBoxLayout(inner1);
    inner1Layout->addWidget(new QLabel("Content in nested group box 1", inner1));
    outerLayout->addWidget(inner1);

    // Inner group box 2
    GroupBox* inner2 = new GroupBox("Inner Group 2", outerGroup);
    inner2->setElevation(2);
    QVBoxLayout* inner2Layout = new QVBoxLayout(inner2);
    inner2Layout->addWidget(new QLabel("Content in nested group box 2", inner2));
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(new Button("OK", ButtonVariant::Filled, inner2));
    buttonLayout->addWidget(new Button("Close", ButtonVariant::Text, inner2));
    inner2Layout->addLayout(buttonLayout);
    outerLayout->addWidget(inner2);

    layout_->addWidget(outerGroup);
}

} // namespace cf::ui::example
