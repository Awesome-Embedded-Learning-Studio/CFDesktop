/**
 * @file RadioButtonDemo.cpp
 * @brief Material Design 3 RadioButton Demo - Implementation
 */

#include "RadioButtonDemo.h"

#include "ui/widget/material/widget/radiobutton/radiobutton.h"

#include <QApplication>
#include <QButtonGroup>
#include <QFont>
#include <QStyle>
#include <QHBoxLayout>
#include <QScrollArea>

using namespace cf::ui::widget::material;

namespace cf::ui::example {

RadioButtonDemo::RadioButtonDemo(QWidget* parent) : QWidget(parent) {
    setupUI();
    createBasicRadioSection();
    createRadioStatesSection();
    createButtonGroupSection();
    createErrorStateSection();
    createInteractionDemoSection();
}

void RadioButtonDemo::setupUI() {
    layout_ = new QVBoxLayout(this);
    layout_->setContentsMargins(16, 16, 16, 16);
    layout_->setSpacing(16);

    scrollContent_ = new QWidget();
    QVBoxLayout* scrollLayout = new QVBoxLayout(scrollContent_);
    scrollLayout->setSpacing(24);
    scrollLayout->setContentsMargins(0, 0, 0, 0);

    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setWidget(scrollContent_);

    layout_->addWidget(scrollArea);
}

void RadioButtonDemo::createBasicRadioSection() {
    QGroupBox* groupBox = new QGroupBox("Basic Radio Buttons (基本单选按钮)", this);
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->setSpacing(12);
    layout->setContentsMargins(16, 24, 16, 16);

    QButtonGroup* buttonGroup = new QButtonGroup(this);

    RadioButton* option1 = new RadioButton("Option 1 - First choice", this);
    option1->setChecked(true);
    buttonGroup->addButton(option1, 1);
    layout->addWidget(option1);

    RadioButton* option2 = new RadioButton("Option 2 - Second choice", this);
    buttonGroup->addButton(option2, 2);
    layout->addWidget(option2);

    RadioButton* option3 = new RadioButton("Option 3 - Third choice", this);
    buttonGroup->addButton(option3, 3);
    layout->addWidget(option3);

    scrollContent_->layout()->addWidget(groupBox);
}

void RadioButtonDemo::createRadioStatesSection() {
    QGroupBox* groupBox = createGroupBox("Radio Button States (按钮状态)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    QHBoxLayout* normalLayout = new QHBoxLayout();
    QLabel* normalLabel = new QLabel("Normal:", this);
    normalLabel->setMinimumWidth(120);

    QButtonGroup* normalGroup = new QButtonGroup(this);
    RadioButton* normalRadio1 = new RadioButton("Enabled A", this);
    RadioButton* normalRadio2 = new RadioButton("Enabled B", this);
    normalRadio1->setChecked(true);
    normalGroup->addButton(normalRadio1);
    normalGroup->addButton(normalRadio2);

    normalLayout->addWidget(normalLabel);
    normalLayout->addWidget(normalRadio1);
    normalLayout->addWidget(normalRadio2);
    normalLayout->addStretch();
    layout->addLayout(normalLayout);

    QHBoxLayout* disabledLayout = new QHBoxLayout();
    QLabel* disabledLabel = new QLabel("Disabled:", this);
    disabledLabel->setMinimumWidth(120);

    QButtonGroup* disabledGroup = new QButtonGroup(this);
    RadioButton* disabledRadio1 = new RadioButton("Disabled Option 1", this);
    RadioButton* disabledRadio2 = new RadioButton("Disabled Option 2", this);
    disabledRadio1->setChecked(true);
    disabledRadio1->setEnabled(false);
    disabledRadio2->setEnabled(false);
    disabledGroup->addButton(disabledRadio1);
    disabledGroup->addButton(disabledRadio2);

    disabledLayout->addWidget(disabledLabel);
    disabledLayout->addWidget(disabledRadio1);
    disabledLayout->addWidget(disabledRadio2);
    disabledLayout->addStretch();
    layout->addLayout(disabledLayout);

    scrollContent_->layout()->addWidget(groupBox);
}

void RadioButtonDemo::createButtonGroupSection() {
    QGroupBox* groupBox = createGroupBox("Independent Button Groups (独立按钮组)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    QLabel* group1Label = new QLabel("Select Color:", this);
    layout->addWidget(group1Label);

    QHBoxLayout* group1Layout = new QHBoxLayout();
    QButtonGroup* colorGroup = new QButtonGroup(this);
    RadioButton* redRadio = new RadioButton("Red", this);
    RadioButton* greenRadio = new RadioButton("Green", this);
    RadioButton* blueRadio = new RadioButton("Blue", this);
    redRadio->setChecked(true);
    colorGroup->addButton(redRadio);
    colorGroup->addButton(greenRadio);
    colorGroup->addButton(blueRadio);
    group1Layout->addWidget(redRadio);
    group1Layout->addWidget(greenRadio);
    group1Layout->addWidget(blueRadio);
    group1Layout->addStretch();
    layout->addLayout(group1Layout);

    layout->addSpacing(16);

    QLabel* group2Label = new QLabel("Select Size:", this);
    layout->addWidget(group2Label);

    QHBoxLayout* group2Layout = new QHBoxLayout();
    QButtonGroup* sizeGroup = new QButtonGroup(this);
    RadioButton* smallRadio = new RadioButton("Small", this);
    RadioButton* mediumRadio = new RadioButton("Medium", this);
    RadioButton* largeRadio = new RadioButton("Large", this);
    mediumRadio->setChecked(true);
    sizeGroup->addButton(smallRadio);
    sizeGroup->addButton(mediumRadio);
    sizeGroup->addButton(largeRadio);
    group2Layout->addWidget(smallRadio);
    group2Layout->addWidget(mediumRadio);
    group2Layout->addWidget(largeRadio);
    group2Layout->addStretch();
    layout->addLayout(group2Layout);

    scrollContent_->layout()->addWidget(groupBox);
}

void RadioButtonDemo::createErrorStateSection() {
    QGroupBox* groupBox = createGroupBox("Error State (错误状态)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    QHBoxLayout* errorLayout = new QHBoxLayout();
    QLabel* errorLabel = new QLabel("Error State:", this);
    errorLabel->setMinimumWidth(120);

    QButtonGroup* errorGroup = new QButtonGroup(this);
    RadioButton* errorRadio1 = new RadioButton("Invalid Choice", this);
    RadioButton* errorRadio2 = new RadioButton("Another Invalid", this);
    errorRadio1->setError(true);
    errorRadio2->setError(true);
    errorRadio1->setChecked(true);
    errorGroup->addButton(errorRadio1);
    errorGroup->addButton(errorRadio2);

    errorLayout->addWidget(errorLabel);
    errorLayout->addWidget(errorRadio1);
    errorLayout->addWidget(errorRadio2);
    errorLayout->addStretch();
    layout->addLayout(errorLayout);

    QHBoxLayout* toggleLayout = new QHBoxLayout();
    QLabel* toggleLabel = new QLabel("Toggle Error:", this);
    toggleLabel->setMinimumWidth(120);

    QButtonGroup* toggleGroup = new QButtonGroup(this);
    RadioButton* toggleRadio1 = new RadioButton("Valid Option", this);
    RadioButton* toggleRadio2 = new RadioButton("Error Option", this);
    toggleRadio1->setChecked(true);
    toggleGroup->addButton(toggleRadio1, 1);
    toggleGroup->addButton(toggleRadio2, 2);

    toggleLayout->addWidget(toggleLabel);
    toggleLayout->addWidget(toggleRadio1);
    toggleLayout->addWidget(toggleRadio2);
    toggleLayout->addStretch();
    layout->addLayout(toggleLayout);

    connect(toggleGroup, QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked),
            this, [toggleRadio1, toggleRadio2](QAbstractButton* button) {
                bool isError = (button == toggleRadio2);
                toggleRadio1->setError(isError);
                toggleRadio2->setError(isError);
            });

    scrollContent_->layout()->addWidget(groupBox);
}

void RadioButtonDemo::createInteractionDemoSection() {
    QGroupBox* groupBox = createGroupBox("Interaction Demo (交互演示)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    selectionLabel_ = new QLabel("Selected: Option 1", this);
    selectionLabel_->setMinimumWidth(200);
    QFont labelFont = selectionLabel_->font();
    labelFont.setBold(true);
    selectionLabel_->setFont(labelFont);
    layout->addWidget(selectionLabel_);

    QButtonGroup* demoGroup = new QButtonGroup(this);

    RadioButton* demoOption1 = new RadioButton("Option 1", this);
    RadioButton* demoOption2 = new RadioButton("Option 2", this);
    RadioButton* demoOption3 = new RadioButton("Option 3", this);
    demoOption1->setChecked(true);

    demoGroup->addButton(demoOption1, 1);
    demoGroup->addButton(demoOption2, 2);
    demoGroup->addButton(demoOption3, 3);

    layout->addWidget(demoOption1);
    layout->addWidget(demoOption2);
    layout->addWidget(demoOption3);

    connect(demoGroup, QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked),
            this, [this](QAbstractButton* button) {
                selectionLabel_->setText(QString("Selected: %1").arg(button->text()));
            });

    scrollContent_->layout()->addWidget(groupBox);

    static_cast<QVBoxLayout*>(scrollContent_->layout())->addStretch();
}

QGroupBox* RadioButtonDemo::createGroupBox(const QString& title) {
    QGroupBox* groupBox = new QGroupBox(title, this);
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->setSpacing(12);
    layout->setContentsMargins(16, 24, 16, 16);
    return groupBox;
}

} // namespace cf::ui::example
