/**
 * @file TextFieldDemo.cpp
 * @brief Material Design 3 TextField Demo - Implementation
 */

#include "TextFieldDemo.h"

#include "ui/widget/material/widget/textfield/textfield.h"

#include <QApplication>
#include <QFont>
#include <QStyle>
#include <QFrame>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>

using namespace cf::ui::widget::material;
using TextFieldVariant = cf::ui::widget::material::TextField::TextFieldVariant;

namespace cf::ui::example {

TextFieldDemo::TextFieldDemo(QWidget* parent) : QWidget(parent) {
    setupUI();
    createTextFieldVariantsSection();
    createTextFieldStatesSection();
    createTextFieldWithIconsSection();
    createTextFieldWithErrorSection();
    createTextFieldAdvancedSection();

    // Add stretch at the end
    layout_->addStretch();
}

void TextFieldDemo::setupUI() {
    // Main layout
    layout_ = new QVBoxLayout(this);
    layout_->setContentsMargins(16, 16, 16, 16);
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

    // Use scrollLayout for adding content
    layout_ = scrollLayout;
}

void TextFieldDemo::createTextFieldVariantsSection() {
    QGroupBox* groupBox = new QGroupBox("TextField Variants (文本框变体)", this);
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->setSpacing(16);
    layout->setContentsMargins(16, 24, 16, 16);

    // Filled TextField
    QHBoxLayout* filledLayout = new QHBoxLayout();
    QLabel* filledLabel = new QLabel("Filled:", this);
    filledLabel->setMinimumWidth(120);
    TextField* filledField = new TextField("Filled text", TextFieldVariant::Filled, this);
    filledField->setLabel("Label");
    connect(filledField, &QLineEdit::textChanged, this, &TextFieldDemo::onTextChanged);
    filledLayout->addWidget(filledLabel);
    filledLayout->addWidget(filledField);
    filledLayout->addStretch();
    layout->addLayout(filledLayout);

    // Outlined TextField
    QHBoxLayout* outlinedLayout = new QHBoxLayout();
    QLabel* outlinedLabel = new QLabel("Outlined:", this);
    outlinedLabel->setMinimumWidth(120);
    TextField* outlinedField = new TextField("Outlined text", TextFieldVariant::Outlined, this);
    outlinedField->setLabel("Label");
    connect(outlinedField, &QLineEdit::textChanged, this, &TextFieldDemo::onTextChanged);
    outlinedLayout->addWidget(outlinedLabel);
    outlinedLayout->addWidget(outlinedField);
    outlinedLayout->addStretch();
    layout->addLayout(outlinedLayout);

    layout_->addWidget(groupBox);
}

void TextFieldDemo::createTextFieldStatesSection() {
    QGroupBox* groupBox = createGroupBox("TextField States (文本框状态)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // Normal (enabled) text fields
    QHBoxLayout* normalLayout = new QHBoxLayout();
    QLabel* normalLabel = new QLabel("Normal:", this);
    normalLabel->setMinimumWidth(120);
    TextField* normalField = new TextField(TextFieldVariant::Filled, this);
    normalField->setLabel("Enter text");
    normalLayout->addWidget(normalLabel);
    normalLayout->addWidget(normalField);
    normalLayout->addStretch();
    layout->addLayout(normalLayout);

    // Disabled text fields
    QHBoxLayout* disabledLayout = new QHBoxLayout();
    QLabel* disabledLabel = new QLabel("Disabled:", this);
    disabledLabel->setMinimumWidth(120);
    TextField* disabledFilled = new TextField("Disabled text", TextFieldVariant::Filled, this);
    disabledFilled->setLabel("Label");
    disabledFilled->setEnabled(false);
    TextField* disabledOutlined = new TextField("Disabled text", TextFieldVariant::Outlined, this);
    disabledOutlined->setLabel("Label");
    disabledOutlined->setEnabled(false);
    disabledLayout->addWidget(disabledLabel);
    disabledLayout->addWidget(disabledFilled);
    disabledLayout->addWidget(disabledOutlined);
    disabledLayout->addStretch();
    layout->addLayout(disabledLayout);

    // Read-only text fields
    QHBoxLayout* readonlyLayout = new QHBoxLayout();
    QLabel* readonlyLabel = new QLabel("Read-only:", this);
    readonlyLabel->setMinimumWidth(120);
    TextField* readonlyField = new TextField("Read-only text", TextFieldVariant::Filled, this);
    readonlyField->setLabel("Label");
    readonlyField->setReadOnly(true);
    readonlyLayout->addWidget(readonlyLabel);
    readonlyLayout->addWidget(readonlyField);
    readonlyLayout->addStretch();
    layout->addLayout(readonlyLayout);

    layout_->addWidget(groupBox);
}

void TextFieldDemo::createTextFieldWithIconsSection() {
    QGroupBox* groupBox = createGroupBox("Text Fields with Icons (带图标的文本框)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // Filled with prefix icon
    QHBoxLayout* filledPrefixLayout = new QHBoxLayout();
    QLabel* filledPrefixLabel = new QLabel("Filled + Prefix:", this);
    filledPrefixLabel->setMinimumWidth(120);
    TextField* filledPrefixField = new TextField(TextFieldVariant::Filled, this);
    filledPrefixField->setLabel("Search");
    filledPrefixField->setPrefixIcon(QApplication::style()->standardIcon(QStyle::SP_FileIcon));
    filledPrefixLayout->addWidget(filledPrefixLabel);
    filledPrefixLayout->addWidget(filledPrefixField);
    filledPrefixLayout->addStretch();
    layout->addLayout(filledPrefixLayout);

    // Outlined with suffix icon
    QHBoxLayout* outlinedSuffixLayout = new QHBoxLayout();
    QLabel* outlinedSuffixLabel = new QLabel("Outlined + Suffix:", this);
    outlinedSuffixLabel->setMinimumWidth(120);
    TextField* outlinedSuffixField = new TextField(TextFieldVariant::Outlined, this);
    outlinedSuffixField->setLabel("Email");
    outlinedSuffixField->setSuffixIcon(QApplication::style()->standardIcon(QStyle::SP_DialogOkButton));
    outlinedSuffixLayout->addWidget(outlinedSuffixLabel);
    outlinedSuffixLayout->addWidget(outlinedSuffixField);
    outlinedSuffixLayout->addStretch();
    layout->addLayout(outlinedSuffixLayout);

    // Password field
    QHBoxLayout* passwordLayout = new QHBoxLayout();
    QLabel* passwordLabel = new QLabel("Password:", this);
    passwordLabel->setMinimumWidth(120);
    TextField* passwordField = new TextField(TextFieldVariant::Filled, this);
    passwordField->setLabel("Password");
    passwordField->setEchoMode(TextField::Password);
    passwordField->setPrefixIcon(QApplication::style()->standardIcon(QStyle::SP_DialogCancelButton));
    passwordLayout->addWidget(passwordLabel);
    passwordLayout->addWidget(passwordField);
    passwordLayout->addStretch();
    layout->addLayout(passwordLayout);

    layout_->addWidget(groupBox);
}

void TextFieldDemo::createTextFieldWithErrorSection() {
    QGroupBox* groupBox = createGroupBox("Helper Text & Error (辅助文本与错误)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // With helper text
    QHBoxLayout* helperLayout = new QHBoxLayout();
    QLabel* helperLabel = new QLabel("Helper text:", this);
    helperLabel->setMinimumWidth(120);
    TextField* helperField = new TextField(TextFieldVariant::Filled, this);
    helperField->setLabel("Username");
    helperField->setHelperText("Enter your username");
    helperLayout->addWidget(helperLabel);
    helperLayout->addWidget(helperField);
    helperLayout->addStretch();
    layout->addLayout(helperLayout);

    // With error text
    QHBoxLayout* errorLayout = new QHBoxLayout();
    QLabel* errorLabel = new QLabel("Error state:", this);
    errorLabel->setMinimumWidth(120);
    TextField* errorField = new TextField("invalid-email", TextFieldVariant::Outlined, this);
    errorField->setLabel("Email");
    errorField->setErrorText("Please enter a valid email address");
    errorLayout->addWidget(errorLabel);
    errorLayout->addWidget(errorField);
    errorLayout->addStretch();
    layout->addLayout(errorLayout);

    layout_->addWidget(groupBox);
}

void TextFieldDemo::createTextFieldAdvancedSection() {
    QGroupBox* groupBox = createGroupBox("Advanced Features (高级功能)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // Character counter
    QHBoxLayout* counterLayout = new QHBoxLayout();
    QLabel* counterLabel = new QLabel("Character limit:", this);
    counterLabel->setMinimumWidth(120);
    TextField* counterField = new TextField(TextFieldVariant::Outlined, this);
    counterField->setLabel("Bio");
    counterField->setHelperText("Tell us about yourself");
    counterField->setMaxLength(50);
    counterField->setShowCharacterCounter(true);
    counterLayout->addWidget(counterLabel);
    counterLayout->addWidget(counterField);
    counterLayout->addStretch();
    layout->addLayout(counterLayout);

    // Multi-line simulation (text area hint)
    QHBoxLayout* multilineLayout = new QHBoxLayout();
    QLabel* multilineLabel = new QLabel("TextArea hint:", this);
    multilineLabel->setMinimumWidth(120);
    TextField* multilineField = new TextField("This is a single-line TextField. For multi-line input, use TextArea.", TextFieldVariant::Filled, this);
    multilineField->setLabel("Description");
    multilineField->setHelperText("Enter a description (multi-line not supported in TextField)");
    multilineLayout->addWidget(multilineLabel);
    multilineLayout->addWidget(multilineField);
    multilineLayout->addStretch();
    layout->addLayout(multilineLayout);

    layout_->addWidget(groupBox);
}

void TextFieldDemo::onTextChanged(const QString& text) {
    // Example handler for text changes
    Q_UNUSED(text)
    // Could update UI based on text content
}

QGroupBox* TextFieldDemo::createGroupBox(const QString& title) {
    QGroupBox* groupBox = new QGroupBox(title, this);
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->setSpacing(12);
    layout->setContentsMargins(16, 24, 16, 16);
    return groupBox;
}

} // namespace cf::ui::example
