/**
 * @file ComboBoxDemo.cpp
 * @brief Material Design 3 ComboBox Demo - Implementation
 */

#include "ComboBoxDemo.h"

#include "ui/widget/material/widget/comboBox/combobox.h"

#include <QApplication>
#include <QFont>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>

using namespace cf::ui::widget::material;

namespace cf::ui::example {

ComboBoxDemo::ComboBoxDemo(QWidget* parent) : QWidget(parent) {

    setupUI();
    createVariantsSection();
    createItemsSection();
    createStatesSection();
    createInteractiveDemoSection();

    layout_->addStretch();
}

void ComboBoxDemo::setupUI() {
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

void ComboBoxDemo::createVariantsSection() {
    QGroupBox* groupBox = new QGroupBox("ComboBox Variants (变体)", this);
    QGridLayout* gridLayout = new QGridLayout(groupBox);
    gridLayout->setSpacing(16);
    gridLayout->setContentsMargins(16, 24, 16, 16);

    // Filled
    QLabel* filledLabel = new QLabel("Filled:", this);
    ComboBox* filledCombo = new ComboBox(this);
    filledCombo->addItem("Option 1");
    filledCombo->addItem("Option 2");
    filledCombo->addItem("Option 3");
    filledCombo->setCurrentIndex(0);
    gridLayout->addWidget(filledLabel, 0, 0);
    gridLayout->addWidget(filledCombo, 0, 1);

    // Outlined
    QLabel* outlinedLabel = new QLabel("Outlined:", this);
    ComboBox* outlinedCombo = new ComboBox(this);
    outlinedCombo->setVariant(ComboBox::ComboBoxVariant::Outlined);
    outlinedCombo->addItem("Option A");
    outlinedCombo->addItem("Option B");
    outlinedCombo->addItem("Option C");
    outlinedCombo->setCurrentIndex(0);
    gridLayout->addWidget(outlinedLabel, 1, 0);
    gridLayout->addWidget(outlinedCombo, 1, 1);

    gridLayout->setRowStretch(2, 1);
    scrollContent_->layout()->addWidget(groupBox);
}

void ComboBoxDemo::createItemsSection() {
    QGroupBox* groupBox = createGroupBox("Different Item Counts (不同项目数)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // Single item
    QHBoxLayout* singleLayout = new QHBoxLayout();
    QLabel* singleLabel = new QLabel("Single:", this);
    singleLabel->setMinimumWidth(100);
    ComboBox* singleCombo = new ComboBox(this);
    singleCombo->addItem("Only Option");
    singleLayout->addWidget(singleLabel);
    singleLayout->addWidget(singleCombo);
    singleLayout->addStretch();
    layout->addLayout(singleLayout);

    // Few items
    QHBoxLayout* fewLayout = new QHBoxLayout();
    QLabel* fewLabel = new QLabel("Few items:", this);
    fewLabel->setMinimumWidth(100);
    ComboBox* fewCombo = new ComboBox(this);
    fewCombo->addItem("Apple");
    fewCombo->addItem("Banana");
    fewCombo->addItem("Cherry");
    fewLayout->addWidget(fewLabel);
    fewLayout->addWidget(fewCombo);
    fewLayout->addStretch();
    layout->addLayout(fewLayout);

    // Many items
    QHBoxLayout* manyLayout = new QHBoxLayout();
    QLabel* manyLabel = new QLabel("Many items:", this);
    manyLabel->setMinimumWidth(100);
    ComboBox* manyCombo = new ComboBox(this);
    for (int i = 1; i <= 20; ++i) {
        manyCombo->addItem(QString("Item %1").arg(i));
    }
    manyCombo->setCurrentIndex(5);
    manyLayout->addWidget(manyLabel);
    manyLayout->addWidget(manyCombo);
    manyLayout->addStretch();
    layout->addLayout(manyLayout);

    scrollContent_->layout()->addWidget(groupBox);
}

void ComboBoxDemo::createStatesSection() {
    QGroupBox* groupBox = createGroupBox("ComboBox States (状态)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // Normal
    QHBoxLayout* normalLayout = new QHBoxLayout();
    QLabel* normalLabel = new QLabel("Normal:", this);
    normalLabel->setMinimumWidth(100);
    ComboBox* normalCombo = new ComboBox(this);
    normalCombo->addItem("Enabled");
    normalCombo->addItem("Option 2");
    normalLayout->addWidget(normalLabel);
    normalLayout->addWidget(normalCombo);
    normalLayout->addStretch();
    layout->addLayout(normalLayout);

    // Disabled
    QHBoxLayout* disabledLayout = new QHBoxLayout();
    QLabel* disabledLabel = new QLabel("Disabled:", this);
    disabledLabel->setMinimumWidth(100);
    ComboBox* disabledCombo = new ComboBox(this);
    disabledCombo->addItem("Disabled");
    disabledCombo->addItem("Option 2");
    disabledCombo->setEnabled(false);
    disabledLayout->addWidget(disabledLabel);
    disabledLayout->addWidget(disabledCombo);
    disabledLayout->addStretch();
    layout->addLayout(disabledLayout);

    scrollContent_->layout()->addWidget(groupBox);
}

void ComboBoxDemo::createInteractiveDemoSection() {
    QGroupBox* groupBox = createGroupBox("Interactive Demo (交互演示)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    QHBoxLayout* demoLayout = new QHBoxLayout();
    QLabel* demoLabel = new QLabel("Choose a fruit:", this);
    demoLabel->setMinimumWidth(100);

    ComboBox* demoCombo = new ComboBox(this);
    demoCombo->addItem("Apple");
    demoCombo->addItem("Banana");
    demoCombo->addItem("Cherry");
    demoCombo->addItem("Date");
    demoCombo->addItem("Elderberry");
    connect(demoCombo, QOverload<int>::of(&ComboBox::currentIndexChanged), this,
            &ComboBoxDemo::onDemoComboBoxChanged);

    demoLayout->addWidget(demoLabel);
    demoLayout->addWidget(demoCombo);
    demoLayout->addStretch();
    layout->addLayout(demoLayout);

    // Selection display
    selectionLabel_ = new QLabel("Selected: Apple", this);
    QFont selectionFont("Segoe UI", 10, QFont::Bold);
    selectionLabel_->setFont(selectionFont);
    QHBoxLayout* selectionLayout = new QHBoxLayout();
    selectionLayout->addSpacing(100);
    selectionLayout->addWidget(selectionLabel_);
    selectionLayout->addStretch();
    layout->addLayout(selectionLayout);

    scrollContent_->layout()->addWidget(groupBox);
}

void ComboBoxDemo::onComboBoxChanged(int index) {
    Q_UNUSED(index)
    // Could add more interactive feedback here
}

void ComboBoxDemo::onDemoComboBoxChanged(int index) {
    ComboBox* combo = qobject_cast<ComboBox*>(sender());
    if (combo && index >= 0) {
        selectionLabel_->setText(QString("Selected: %1").arg(combo->currentText()));
    }
}

QGroupBox* ComboBoxDemo::createGroupBox(const QString& title) {
    QGroupBox* groupBox = new QGroupBox(title, this);
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->setSpacing(12);
    layout->setContentsMargins(16, 24, 16, 16);
    return groupBox;
}

} // namespace cf::ui::example
