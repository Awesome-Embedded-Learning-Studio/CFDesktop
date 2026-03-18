/**
 * @file TextAreaDemo.cpp
 * @brief Material Design 3 TextArea Demo - Implementation
 */

#include "TextAreaDemo.h"

#include "ui/widget/material/widget/textarea/textarea.h"
#include <QApplication>
#include <QFont>
#include <QFrame>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>

using namespace cf::ui::widget::material;
using TextAreaVariant = cf::ui::widget::material::TextArea::TextAreaVariant;

namespace cf::ui::example {

TextAreaDemo::TextAreaDemo(QWidget* parent) : QWidget(parent) {
    setupUI();
    createTextAreaVariantsSection();
    createTextAreaStatesSection();
    createCharacterCounterSection();
    createMinMaxLinesSection();
    createInteractionDemoSection();

    // Add stretch at the end
    layout_->addStretch();
}

void TextAreaDemo::setupUI() {
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

void TextAreaDemo::createTextAreaVariantsSection() {
    QGroupBox* groupBox = new QGroupBox("TextArea Variants (文本域变体)", this);
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->setSpacing(16);
    layout->setContentsMargins(16, 24, 16, 16);

    // Filled TextArea
    QHBoxLayout* filledLayout = new QHBoxLayout();
    QLabel* filledLabel = new QLabel("Filled:", this);
    filledLabel->setMinimumWidth(100);
    TextArea* filledTextArea = new TextArea(TextAreaVariant::Filled, this);
    filledTextArea->setLabel("Label");
    filledTextArea->setPlaceholderText("Enter text here...");
    filledTextArea->setMinLines(3);
    filledLayout->addWidget(filledLabel);
    filledLayout->addWidget(filledTextArea, 1);
    layout->addLayout(filledLayout);

    // Outlined TextArea
    QHBoxLayout* outlinedLayout = new QHBoxLayout();
    QLabel* outlinedLabel = new QLabel("Outlined:", this);
    outlinedLabel->setMinimumWidth(100);
    TextArea* outlinedTextArea = new TextArea(TextAreaVariant::Outlined, this);
    outlinedTextArea->setLabel("Label");
    outlinedTextArea->setPlaceholderText("Enter text here...");
    outlinedTextArea->setMinLines(3);
    outlinedLayout->addWidget(outlinedLabel);
    outlinedLayout->addWidget(outlinedTextArea, 1);
    layout->addLayout(outlinedLayout);

    layout_->addWidget(groupBox);
}

void TextAreaDemo::createTextAreaStatesSection() {
    QGroupBox* groupBox = createGroupBox("TextArea States (文本域状态)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // Normal (enabled) text areas
    QHBoxLayout* normalLayout = new QHBoxLayout();
    QLabel* normalLabel = new QLabel("Normal:", this);
    normalLabel->setMinimumWidth(100);
    TextArea* normalTextArea = new TextArea("Enabled text area", TextAreaVariant::Filled, this);
    normalTextArea->setLabel("Message");
    normalTextArea->setMinLines(2);
    normalLayout->addWidget(normalLabel);
    normalLayout->addWidget(normalTextArea, 1);
    layout->addLayout(normalLayout);

    // Disabled text areas
    QHBoxLayout* disabledLayout = new QHBoxLayout();
    QLabel* disabledLabel = new QLabel("Disabled:", this);
    disabledLabel->setMinimumWidth(100);
    TextArea* disabledFilled =
        new TextArea("Disabled filled text area", TextAreaVariant::Filled, this);
    disabledFilled->setLabel("Message");
    disabledFilled->setEnabled(false);
    disabledFilled->setMinLines(2);
    TextArea* disabledOutlined =
        new TextArea("Disabled outlined text area", TextAreaVariant::Outlined, this);
    disabledOutlined->setLabel("Message");
    disabledOutlined->setEnabled(false);
    disabledOutlined->setMinLines(2);
    disabledLayout->addWidget(disabledLabel);
    disabledLayout->addWidget(disabledFilled, 1);
    disabledLayout->addWidget(disabledOutlined, 1);
    layout->addLayout(disabledLayout);

    // Error state
    QHBoxLayout* errorLayout = new QHBoxLayout();
    QLabel* errorLabel = new QLabel("Error:", this);
    errorLabel->setMinimumWidth(100);
    TextArea* errorTextArea = new TextArea(TextAreaVariant::Outlined, this);
    errorTextArea->setLabel("Email");
    errorTextArea->setErrorText("Please enter a valid email address");
    errorTextArea->setMinLines(2);
    errorLayout->addWidget(errorLabel);
    errorLayout->addWidget(errorTextArea, 1);
    layout->addLayout(errorLayout);

    layout_->addWidget(groupBox);
}

void TextAreaDemo::createCharacterCounterSection() {
    QGroupBox* groupBox = createGroupBox("Character Counter (字符计数器)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // TextArea with character counter
    QHBoxLayout* counterLayout = new QHBoxLayout();
    QLabel* counterLabel = new QLabel("With Counter:", this);
    counterLabel->setMinimumWidth(100);
    TextArea* counterTextArea = new TextArea(TextAreaVariant::Filled, this);
    counterTextArea->setLabel("Bio");
    counterTextArea->setPlaceholderText("Tell us about yourself...");
    counterTextArea->setMaxLength(150);
    counterTextArea->setShowCharacterCounter(true);
    counterTextArea->setMinLines(3);
    counterLayout->addWidget(counterLabel);
    counterLayout->addWidget(counterTextArea, 1);
    layout->addLayout(counterLayout);

    layout_->addWidget(groupBox);
}

void TextAreaDemo::createMinMaxLinesSection() {
    QGroupBox* groupBox = createGroupBox("Min/Max Lines (最小/最大行数)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // Fixed 3 lines
    QHBoxLayout* fixed3Layout = new QHBoxLayout();
    QLabel* fixed3Label = new QLabel("Fixed 3 lines:", this);
    fixed3Label->setMinimumWidth(100);
    TextArea* fixed3TextArea = new TextArea(TextAreaVariant::Outlined, this);
    fixed3TextArea->setLabel("Short description");
    fixed3TextArea->setMinLines(3);
    fixed3TextArea->setMaxLines(3);
    fixed3TextArea->setPlaceholderText("Exactly 3 lines visible...");
    fixed3Layout->addWidget(fixed3Label);
    fixed3Layout->addWidget(fixed3TextArea, 1);
    layout->addLayout(fixed3Layout);

    // Auto-grow (1-5 lines)
    QHBoxLayout* autoGrowLayout = new QHBoxLayout();
    QLabel* autoGrowLabel = new QLabel("Auto-grow (1-5):", this);
    autoGrowLabel->setMinimumWidth(100);
    TextArea* autoGrowTextArea = new TextArea(TextAreaVariant::Filled, this);
    autoGrowTextArea->setLabel("Description");
    autoGrowTextArea->setMinLines(1);
    autoGrowTextArea->setMaxLines(5);
    autoGrowTextArea->setPlaceholderText("Starts at 1 line, grows up to 5 lines...");
    autoGrowLayout->addWidget(autoGrowLabel);
    autoGrowLayout->addWidget(autoGrowTextArea, 1);
    layout->addLayout(autoGrowLayout);

    // Multi-line (5 lines min)
    QHBoxLayout* multiLineLayout = new QHBoxLayout();
    QLabel* multiLineLabel = new QLabel("Multi-line (5+):", this);
    multiLineLabel->setMinimumWidth(100);
    TextArea* multiLineTextArea = new TextArea(TextAreaVariant::Outlined, this);
    multiLineTextArea->setLabel("Comments");
    multiLineTextArea->setHelperText("Share your thoughts with us");
    multiLineTextArea->setMinLines(5);
    multiLineTextArea->setPlaceholderText("Type your message here...");
    multiLineLayout->addWidget(multiLineLabel);
    multiLineLayout->addWidget(multiLineTextArea, 1);
    layout->addLayout(multiLineLayout);

    layout_->addWidget(groupBox);
}

void TextAreaDemo::createInteractionDemoSection() {
    QGroupBox* groupBox = createGroupBox("Interaction Demo (交互演示)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // Live character count demo
    QHBoxLayout* demoLayout = new QHBoxLayout();
    QLabel* demoLabel = new QLabel("Type to test:", this);
    demoLabel->setMinimumWidth(100);
    TextArea* demoTextArea = new TextArea(TextAreaVariant::Filled, this);
    demoTextArea->setLabel("Your Message");
    demoTextArea->setMaxLength(200);
    demoTextArea->setShowCharacterCounter(true);
    demoTextArea->setMinLines(4);
    demoTextArea->setPlaceholderText("Start typing to see the character counter update...");
    demoLayout->addWidget(demoLabel);
    demoLayout->addWidget(demoTextArea, 1);
    layout->addLayout(demoLayout);

    // Character count display
    QHBoxLayout* countDisplayLayout = new QHBoxLayout();
    QLabel* countLabel = new QLabel("Character count:", this);
    countLabel->setMinimumWidth(100);
    charCountLabel_ = new QLabel("0 / 200", this);
    QFont countFont("Segoe UI", 12, QFont::Bold);
    charCountLabel_->setFont(countFont);
    countDisplayLayout->addWidget(countLabel);
    countDisplayLayout->addWidget(charCountLabel_);
    countDisplayLayout->addStretch();
    layout->addLayout(countDisplayLayout);

    // Connect text changed signal
    connect(demoTextArea, &QTextEdit::textChanged, this, &TextAreaDemo::onDemoTextChanged);

    layout_->addWidget(groupBox);
}

void TextAreaDemo::onDemoTextChanged() {
    // Update character count display
    TextArea* textArea = qobject_cast<TextArea*>(sender());
    if (textArea && charCountLabel_) {
        int count = textArea->toPlainText().length();
        int max = textArea->maxLength();
        charCountLabel_->setText(QString("%1 / %2").arg(count).arg(max));
    }
}

QGroupBox* TextAreaDemo::createGroupBox(const QString& title) {
    QGroupBox* groupBox = new QGroupBox(title, this);
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->setSpacing(12);
    layout->setContentsMargins(16, 24, 16, 16);
    return groupBox;
}

} // namespace cf::ui::example
