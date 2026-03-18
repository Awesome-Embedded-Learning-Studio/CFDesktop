/**
 * @file CheckBoxDemo.cpp
 * @brief Material Design 3 CheckBox Demo - Implementation
 */

#include "CheckBoxDemo.h"

#include "ui/widget/material/widget/checkbox/checkbox.h"

#include <QApplication>
#include <QFont>
#include <QStyle>
#include <QGroupBox>
#include <QLabel>
#include <QGridLayout>
#include <QHBoxLayout>

using namespace cf::ui::widget::material;

namespace cf::ui::example {

CheckBoxDemo::CheckBoxDemo(QWidget* parent) : QWidget(parent) {

    setupUI();
    createCheckboxStatesSection();
    createTristateSection();
    createErrorStateSection();
    createDisabledSection();
    createInteractiveDemoSection();
    createCustomLabelsSection();

    layout_->addStretch();
}

void CheckBoxDemo::setupUI() {
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

void CheckBoxDemo::createCheckboxStatesSection() {
    QGroupBox* groupBox = new QGroupBox("Checkbox States (复选框状态)", this);
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->setSpacing(12);
    layout->setContentsMargins(16, 24, 16, 16);

    // Unchecked
    QHBoxLayout* uncheckedLayout = new QHBoxLayout();
    CheckBox* uncheckedBox = new CheckBox("Unchecked (未选中)", this);
    uncheckedLayout->addWidget(uncheckedBox);
    uncheckedLayout->addStretch();
    layout->addLayout(uncheckedLayout);

    // Checked
    QHBoxLayout* checkedLayout = new QHBoxLayout();
    CheckBox* checkedBox = new CheckBox("Checked (已选中)", this);
    checkedBox->setChecked(true);
    checkedLayout->addWidget(checkedBox);
    checkedLayout->addStretch();
    layout->addLayout(checkedLayout);

    // Programmatically set checked
    QHBoxLayout* progLayout = new QHBoxLayout();
    CheckBox* progBox = new CheckBox("Click to toggle (点击切换)", this);
    progLayout->addWidget(progBox);
    progLayout->addStretch();
    layout->addLayout(progLayout);

    scrollContent_->layout()->addWidget(groupBox);
}

void CheckBoxDemo::createTristateSection() {
    QGroupBox* groupBox = new QGroupBox("Tristate Checkbox (三态复选框)", this);
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->setSpacing(12);
    layout->setContentsMargins(16, 24, 16, 16);

    // Tristate checkbox
    QHBoxLayout* tristateLayout = new QHBoxLayout();
    CheckBox* tristateBox = new CheckBox("Tristate (三态)", this);
    tristateBox->setTristate(true);
    tristateBox->setCheckState(Qt::PartiallyChecked);
    tristateLayout->addWidget(tristateBox);
    tristateLayout->addStretch();
    layout->addLayout(tristateLayout);

    // Parent-child checkboxes example
    QLabel* hintLabel = new QLabel("Parent-Child Example (父子级联动):", this);
    layout->addWidget(hintLabel);

    QHBoxLayout* parentLayout = new QHBoxLayout();
    CheckBox* parentBox = new CheckBox("Select All (全选)", this);
    parentBox->setTristate(true);
    parentLayout->addWidget(parentBox);
    parentLayout->addStretch();
    layout->addLayout(parentLayout);

    QHBoxLayout* child1Layout = new QHBoxLayout();
    CheckBox* child1Box = new CheckBox("Option 1 (选项 1)", this);
    child1Layout->addSpacing(24);
    child1Layout->addWidget(child1Box);
    child1Layout->addStretch();
    layout->addLayout(child1Layout);

    QHBoxLayout* child2Layout = new QHBoxLayout();
    CheckBox* child2Box = new CheckBox("Option 2 (选项 2)", this);
    child2Layout->addSpacing(24);
    child2Layout->addWidget(child2Box);
    child2Layout->addStretch();
    layout->addLayout(child2Layout);

    QHBoxLayout* child3Layout = new QHBoxLayout();
    CheckBox* child3Box = new CheckBox("Option 3 (选项 3)", this);
    child3Layout->addSpacing(24);
    child3Layout->addWidget(child3Box);
    child3Layout->addStretch();
    layout->addLayout(child3Layout);

    // Connect parent-child logic
    connect(parentBox, &CheckBox::checkStateChanged, this,
            [child1Box, child2Box, child3Box](Qt::CheckState state) {
                if (state == Qt::Checked) {
                    child1Box->setChecked(true);
                    child2Box->setChecked(true);
                    child3Box->setChecked(true);
                } else if (state == Qt::Unchecked) {
                    child1Box->setChecked(false);
                    child2Box->setChecked(false);
                    child3Box->setChecked(false);
                }
            });

    auto updateParentState = [parentBox, child1Box, child2Box, child3Box]() {
        int checkedCount = 0;
        if (child1Box->isChecked())
            checkedCount++;
        if (child2Box->isChecked())
            checkedCount++;
        if (child3Box->isChecked())
            checkedCount++;

        if (checkedCount == 0) {
            parentBox->setCheckState(Qt::Unchecked);
        } else if (checkedCount == 3) {
            parentBox->setCheckState(Qt::Checked);
        } else {
            parentBox->setCheckState(Qt::PartiallyChecked);
        }
    };

    connect(child1Box, &CheckBox::checkStateChanged, parentBox,
            [updateParentState]() { updateParentState(); });
    connect(child2Box, &CheckBox::checkStateChanged, parentBox,
            [updateParentState]() { updateParentState(); });
    connect(child3Box, &CheckBox::checkStateChanged, parentBox,
            [updateParentState]() { updateParentState(); });

    scrollContent_->layout()->addWidget(groupBox);
}

void CheckBoxDemo::createErrorStateSection() {
    QGroupBox* groupBox = new QGroupBox("Error State (错误状态)", this);
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->setSpacing(12);
    layout->setContentsMargins(16, 24, 16, 16);

    // Error checkbox
    QHBoxLayout* errorLayout = new QHBoxLayout();
    CheckBox* errorBox = new CheckBox("I agree to the terms (同意条款)", this);
    errorBox->setError(true);
    errorLayout->addWidget(errorBox);
    errorLayout->addStretch();
    layout->addLayout(errorLayout);

    // Toggle error state
    QHBoxLayout* toggleLayout = new QHBoxLayout();
    CheckBox* toggleBox = new CheckBox("Toggle error state (切换错误状态)", this);
    connect(toggleBox, &CheckBox::clicked, this, &CheckBoxDemo::onErrorToggleClicked);
    toggleLayout->addWidget(toggleBox);
    toggleLayout->addStretch();
    layout->addLayout(toggleLayout);

    scrollContent_->layout()->addWidget(groupBox);
}

void CheckBoxDemo::createDisabledSection() {
    QGroupBox* groupBox = new QGroupBox("Disabled State (禁用状态)", this);
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->setSpacing(12);
    layout->setContentsMargins(16, 24, 16, 16);

    // Disabled unchecked
    QHBoxLayout* disabledUncheckedLayout = new QHBoxLayout();
    CheckBox* disabledUnchecked = new CheckBox("Disabled unchecked (禁用未选)", this);
    disabledUnchecked->setEnabled(false);
    disabledUncheckedLayout->addWidget(disabledUnchecked);
    disabledUncheckedLayout->addStretch();
    layout->addLayout(disabledUncheckedLayout);

    // Disabled checked
    QHBoxLayout* disabledCheckedLayout = new QHBoxLayout();
    CheckBox* disabledChecked = new CheckBox("Disabled checked (禁用已选)", this);
    disabledChecked->setEnabled(false);
    disabledChecked->setChecked(true);
    disabledCheckedLayout->addWidget(disabledChecked);
    disabledCheckedLayout->addStretch();
    layout->addLayout(disabledCheckedLayout);

    // Disabled indeterminate
    QHBoxLayout* disabledIndeterminateLayout = new QHBoxLayout();
    CheckBox* disabledIndeterminate = new CheckBox("Disabled indeterminate (禁用半选)", this);
    disabledIndeterminate->setEnabled(false);
    disabledIndeterminate->setTristate(true);
    disabledIndeterminate->setCheckState(Qt::PartiallyChecked);
    disabledIndeterminateLayout->addWidget(disabledIndeterminate);
    disabledIndeterminateLayout->addStretch();
    layout->addLayout(disabledIndeterminateLayout);

    scrollContent_->layout()->addWidget(groupBox);
}

void CheckBoxDemo::createInteractiveDemoSection() {
    QGroupBox* groupBox = new QGroupBox("Interactive Demo (交互演示)", this);
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->setSpacing(12);
    layout->setContentsMargins(16, 24, 16, 16);

    statusLabel_ = new QLabel("Status: No checkboxes selected", this);
    QFont statusFont("Segoe UI", 10);
    statusLabel_->setFont(statusFont);
    layout->addWidget(statusLabel_);

    QHBoxLayout* termsLayout = new QHBoxLayout();
    CheckBox* termsBox = new CheckBox("I accept the Terms of Service (接受服务条款)", this);
    connect(termsBox, &CheckBox::clicked, this, &CheckBoxDemo::onTermsCheckboxClicked);
    termsLayout->addWidget(termsBox);
    termsLayout->addStretch();
    layout->addLayout(termsLayout);

    QHBoxLayout* privacyLayout = new QHBoxLayout();
    CheckBox* privacyBox = new CheckBox("I accept the Privacy Policy (接受隐私政策)", this);
    connect(privacyBox, &CheckBox::clicked, this, &CheckBoxDemo::onTermsCheckboxClicked);
    privacyLayout->addWidget(privacyBox);
    privacyLayout->addStretch();
    layout->addLayout(privacyLayout);

    QHBoxLayout* acceptLayout = new QHBoxLayout();
    CheckBox* acceptBox = new CheckBox("Enable submit button (启用提交按钮)", this);
    connect(acceptBox, &CheckBox::clicked, this, &CheckBoxDemo::onAcceptCheckboxClicked);
    acceptLayout->addWidget(acceptBox);
    acceptLayout->addStretch();
    layout->addLayout(acceptLayout);

    scrollContent_->layout()->addWidget(groupBox);
}

void CheckBoxDemo::createCustomLabelsSection() {
    QGroupBox* groupBox = new QGroupBox("Various Labels (各种标签)", this);
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->setSpacing(12);
    layout->setContentsMargins(16, 24, 16, 16);

    // Short label
    QHBoxLayout* shortLayout = new QHBoxLayout();
    CheckBox* shortBox = new CheckBox("Short", this);
    shortLayout->addWidget(shortBox);
    shortLayout->addStretch();
    layout->addLayout(shortLayout);

    // Long label
    QHBoxLayout* longLayout = new QHBoxLayout();
    CheckBox* longBox =
        new CheckBox("This is a very long checkbox label that demonstrates text wrapping "
                     "or truncation behavior when the text is too long to fit in a single line.",
                     this);
    longLayout->addWidget(longBox);
    longLayout->addStretch();
    layout->addLayout(longLayout);

    // Checkbox without text
    QHBoxLayout* noTextLayout = new QHBoxLayout();
    CheckBox* noTextBox = new CheckBox("", this);
    QLabel* noTextLabel = new QLabel("Checkbox without text (无文本):", this);
    noTextLayout->addWidget(noTextLabel);
    noTextLayout->addWidget(noTextBox);
    noTextLayout->addStretch();
    layout->addLayout(noTextLayout);

    // Newsletter subscription example
    QHBoxLayout* newsletterLayout = new QHBoxLayout();
    CheckBox* newsletterBox = new CheckBox(
        "Subscribe to our newsletter for updates and special offers (订阅我们的新闻通讯)", this);
    connect(newsletterBox, &CheckBox::clicked, this,
            &CheckBoxDemo::onNewsletterCheckboxClicked);
    newsletterLayout->addWidget(newsletterBox);
    newsletterLayout->addStretch();
    layout->addLayout(newsletterLayout);

    scrollContent_->layout()->addWidget(groupBox);
}

void CheckBoxDemo::onAcceptCheckboxClicked(bool checked) {
    if (checked) {
        statusLabel_->setText("Status: Submit button enabled");
        statusLabel_->setStyleSheet("color: green;");
    } else {
        statusLabel_->setText("Status: Submit button disabled");
        statusLabel_->setStyleSheet("color: red;");
    }
}

void CheckBoxDemo::onTermsCheckboxClicked(bool checked) {
    Q_UNUSED(checked)
    // Check if both terms and privacy are checked
    CheckBox* termsBox = qobject_cast<CheckBox*>(sender());
    CheckBox* privacyBox = nullptr;

    // Find the privacy checkbox
    QList<CheckBox*> allCheckboxes = findChildren<CheckBox*>();
    for (CheckBox* box : allCheckboxes) {
        if (box->text().contains("Privacy Policy")) {
            privacyBox = box;
            break;
        }
    }

    if (termsBox && privacyBox) {
        if (termsBox->isChecked() && privacyBox->isChecked()) {
            statusLabel_->setText("Status: All terms accepted!");
            statusLabel_->setStyleSheet("color: green;");
        } else {
            statusLabel_->setText("Status: Please accept all terms");
            statusLabel_->setStyleSheet("color: orange;");
        }
    }
}

void CheckBoxDemo::onNewsletterCheckboxClicked(bool checked) {
    if (checked) {
        statusLabel_->setText("Status: You are now subscribed!");
        statusLabel_->setStyleSheet("color: blue;");
    } else {
        statusLabel_->setText("Status: Subscription cancelled");
        statusLabel_->setStyleSheet("");
    }
}

void CheckBoxDemo::onErrorToggleClicked(bool checked) {
    // Find and toggle error state on the agreement checkbox
    QList<CheckBox*> allCheckboxes = findChildren<CheckBox*>();
    for (CheckBox* box : allCheckboxes) {
        if (box->text().contains("I agree to the terms")) {
            box->setError(checked);
            break;
        }
    }
}

QGroupBox* CheckBoxDemo::createGroupBox(const QString& title) {
    QGroupBox* groupBox = new QGroupBox(title, this);
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->setSpacing(12);
    layout->setContentsMargins(16, 24, 16, 16);
    return groupBox;
}

} // namespace cf::ui::example
