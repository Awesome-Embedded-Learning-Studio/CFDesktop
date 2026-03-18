/**
 * @file MaterialGalleryWindow.cpp
 * @brief Material Design 3 Widget Gallery Main Window Implementation
 */

#include "MaterialGalleryWindow.h"

// Demo includes
#include "demos/ButtonDemo.h"
#include "demos/CheckBoxDemo.h"
#include "demos/ComboBoxDemo.h"
#include "demos/GroupBoxDemo.h"
#include "demos/LabelDemo.h"
#include "demos/ProgressBarDemo.h"
#include "demos/RadioButtonDemo.h"
#include "demos/SliderDemo.h"
#include "demos/SwitchDemo.h"
#include "demos/TextAreaDemo.h"
#include "demos/TextFieldDemo.h"

#include <QHeaderView>

namespace cf::ui::example {

MaterialGalleryWindow::MaterialGalleryWindow(QWidget* parent)
    : QMainWindow(parent), centralWidget_(nullptr), mainLayout_(nullptr), navList_(nullptr),
      contentStack_(nullptr), splitter_(nullptr) {
    setupUI();
    createNavigation();
    registerDemos();

    setWindowTitle("Material Design 3 Widget Gallery");
    resize(1200, 800);
}

MaterialGalleryWindow::~MaterialGalleryWindow() = default;

void MaterialGalleryWindow::setupUI() {
    // Create central widget
    centralWidget_ = new QWidget(this);
    setCentralWidget(centralWidget_);

    // Create main layout
    mainLayout_ = new QHBoxLayout(centralWidget_);
    mainLayout_->setContentsMargins(0, 0, 0, 0);
    mainLayout_->setSpacing(0);

    // Create splitter for resizable navigation
    splitter_ = new QSplitter(Qt::Horizontal, centralWidget_);
    mainLayout_->addWidget(splitter_);
}

void MaterialGalleryWindow::createNavigation() {
    // Create left navigation list
    navList_ = new QListWidget(splitter_);
    navList_->setMinimumWidth(180);
    navList_->setMaximumWidth(250);
    navList_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    // Create right content stack
    contentStack_ = new QStackedWidget(splitter_);

    // Add widgets to splitter
    splitter_->addWidget(navList_);
    splitter_->addWidget(contentStack_);

    // Set initial splitter sizes (nav:content = 200:1000)
    splitter_->setSizes({200, 1000});
    splitter_->setStretchFactor(0, 0);
    splitter_->setStretchFactor(1, 1);

    // Connect navigation to content stack
    connect(navList_, &QListWidget::currentRowChanged, contentStack_,
            &QStackedWidget::setCurrentIndex);
}

void MaterialGalleryWindow::registerDemos() {
    // 1. Button
    auto* buttonDemo = new ButtonDemo(contentStack_);
    navList_->addItem(buttonDemo->title());
    contentStack_->addWidget(buttonDemo);

    // 2. CheckBox
    auto* checkBoxDemo = new CheckBoxDemo(contentStack_);
    navList_->addItem(checkBoxDemo->title());
    contentStack_->addWidget(checkBoxDemo);

    // 3. ComboBox
    auto* comboBoxDemo = new ComboBoxDemo(contentStack_);
    navList_->addItem(comboBoxDemo->title());
    contentStack_->addWidget(comboBoxDemo);

    // 4. GroupBox
    auto* groupBoxDemo = new GroupBoxDemo(contentStack_);
    navList_->addItem(groupBoxDemo->title());
    contentStack_->addWidget(groupBoxDemo);

    // 5. Label
    auto* labelDemo = new LabelDemo(contentStack_);
    navList_->addItem(labelDemo->title());
    contentStack_->addWidget(labelDemo);

    // 6. ProgressBar
    auto* progressBarDemo = new ProgressBarDemo(contentStack_);
    navList_->addItem(progressBarDemo->title());
    contentStack_->addWidget(progressBarDemo);

    // 7. RadioButton
    auto* radioButtonDemo = new RadioButtonDemo(contentStack_);
    navList_->addItem(radioButtonDemo->title());
    contentStack_->addWidget(radioButtonDemo);

    // 8. Slider
    auto* sliderDemo = new SliderDemo(contentStack_);
    navList_->addItem(sliderDemo->title());
    contentStack_->addWidget(sliderDemo);

    // 9. Switch
    auto* switchDemo = new SwitchDemo(contentStack_);
    navList_->addItem(switchDemo->title());
    contentStack_->addWidget(switchDemo);

    // 10. TextArea
    auto* textAreaDemo = new TextAreaDemo(contentStack_);
    navList_->addItem(textAreaDemo->title());
    contentStack_->addWidget(textAreaDemo);

    // 11. TextField
    auto* textFieldDemo = new TextFieldDemo(contentStack_);
    navList_->addItem(textFieldDemo->title());
    contentStack_->addWidget(textFieldDemo);

    // Select first item
    navList_->setCurrentRow(0);
}

} // namespace cf::ui::example
