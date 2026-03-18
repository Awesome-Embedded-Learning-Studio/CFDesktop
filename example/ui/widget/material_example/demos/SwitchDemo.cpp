/**
 * @file SwitchDemo.cpp
 * @brief Material Design 3 Switch Demo Implementation
 */

#include "SwitchDemo.h"

#include "ui/widget/material/widget/switch/switch.h"

#include <QApplication>
#include <QFont>
#include <QHBoxLayout>
#include <QScrollArea>

using namespace cf::ui::widget::material;

namespace cf::ui::example {

static QLabel* statusLabel = nullptr;

SwitchDemo::SwitchDemo(QWidget* parent)
    : QWidget(parent), scrollContent_(nullptr), layout_(nullptr) {
    setupUI();
    createBasicStatesSection();
    createWithLabelsSection();
    createDisabledSection();
    createInteractiveDemoSection();
}

void SwitchDemo::setupUI() {
    layout_ = new QVBoxLayout(this);
    layout_->setContentsMargins(16, 16, 16, 16);
    layout_->setSpacing(16);

    // Create scroll area
    auto* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Create scroll content
    scrollContent_ = new QWidget();
    auto* scrollLayout = new QVBoxLayout(scrollContent_);
    scrollLayout->setSpacing(24);
    scrollLayout->setContentsMargins(8, 8, 8, 8);
    scrollLayout->addStretch();

    scrollArea->setWidget(scrollContent_);
    layout_->addWidget(scrollArea);
}

void SwitchDemo::createBasicStatesSection() {
    QGroupBox* groupBox = createGroupBox("Basic States (基本状态)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // Unchecked
    QHBoxLayout* uncheckedLayout = new QHBoxLayout();
    Switch* uncheckedSwitch = new Switch(this);
    uncheckedSwitch->setChecked(false);
    QLabel* uncheckedLabel = new QLabel("Off", this);
    uncheckedLayout->addWidget(uncheckedSwitch);
    uncheckedLayout->addWidget(uncheckedLabel);
    uncheckedLayout->addStretch();
    layout->addLayout(uncheckedLayout);

    // Checked
    QHBoxLayout* checkedLayout = new QHBoxLayout();
    Switch* checkedSwitch = new Switch(this);
    checkedSwitch->setChecked(true);
    QLabel* checkedLabel = new QLabel("On", this);
    checkedLayout->addWidget(checkedSwitch);
    checkedLayout->addWidget(checkedLabel);
    checkedLayout->addStretch();
    layout->addLayout(checkedLayout);

    // Insert before stretch
    auto* scrollLayout = static_cast<QVBoxLayout*>(scrollContent_->layout());
    scrollLayout->insertWidget(scrollLayout->count() - 1, groupBox);
}

void SwitchDemo::createWithLabelsSection() {
    QGroupBox* groupBox = createGroupBox("Switches with Labels (带标签)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // Wi-Fi example
    QHBoxLayout* wifiLayout = new QHBoxLayout();
    Switch* wifiSwitch = new Switch("Wi-Fi", this);
    wifiSwitch->setChecked(true);
    wifiLayout->addWidget(wifiSwitch);
    wifiLayout->addStretch();
    layout->addLayout(wifiLayout);

    // Bluetooth example
    QHBoxLayout* btLayout = new QHBoxLayout();
    Switch* btSwitch = new Switch("Bluetooth", this);
    btSwitch->setChecked(false);
    btLayout->addWidget(btSwitch);
    btLayout->addStretch();
    layout->addLayout(btLayout);

    // Notifications example
    QHBoxLayout* notifLayout = new QHBoxLayout();
    Switch* notifSwitch = new Switch("Notifications", this);
    notifSwitch->setChecked(true);
    notifLayout->addWidget(notifSwitch);
    notifLayout->addStretch();
    layout->addLayout(notifLayout);

    // Insert before stretch
    auto* scrollLayout = static_cast<QVBoxLayout*>(scrollContent_->layout());
    scrollLayout->insertWidget(scrollLayout->count() - 1, groupBox);
}

void SwitchDemo::createDisabledSection() {
    QGroupBox* groupBox = createGroupBox("Disabled States (禁用状态)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // Disabled unchecked
    QHBoxLayout* disabledUncheckedLayout = new QHBoxLayout();
    Switch* disabledUncheckedSwitch = new Switch("Disabled (Off)", this);
    disabledUncheckedSwitch->setChecked(false);
    disabledUncheckedSwitch->setEnabled(false);
    disabledUncheckedLayout->addWidget(disabledUncheckedSwitch);
    disabledUncheckedLayout->addStretch();
    layout->addLayout(disabledUncheckedLayout);

    // Disabled checked
    QHBoxLayout* disabledCheckedLayout = new QHBoxLayout();
    Switch* disabledCheckedSwitch = new Switch("Disabled (On)", this);
    disabledCheckedSwitch->setChecked(true);
    disabledCheckedSwitch->setEnabled(false);
    disabledCheckedLayout->addWidget(disabledCheckedSwitch);
    disabledCheckedLayout->addStretch();
    layout->addLayout(disabledCheckedLayout);

    // Insert before stretch
    auto* scrollLayout = static_cast<QVBoxLayout*>(scrollContent_->layout());
    scrollLayout->insertWidget(scrollLayout->count() - 1, groupBox);
}

void SwitchDemo::createInteractiveDemoSection() {
    QGroupBox* groupBox = createGroupBox("Interactive Demo (交互演示)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    // Interactive switch
    QHBoxLayout* demoLayout = new QHBoxLayout();
    Switch* demoSwitch = new Switch("Enable Feature", this);
    connect(demoSwitch, &Switch::toggled, this, &SwitchDemo::onSwitchToggled);
    demoLayout->addWidget(demoSwitch);
    demoLayout->addStretch();
    layout->addLayout(demoLayout);

    // Status label
    statusLabel = new QLabel("Status: Off", this);
    QFont statusFont("Segoe UI", 10, QFont::Bold);
    statusLabel->setFont(statusFont);
    QHBoxLayout* statusLayout = new QHBoxLayout();
    statusLayout->addSpacing(8); // Align with switch
    statusLayout->addWidget(statusLabel);
    statusLayout->addStretch();
    layout->addLayout(statusLayout);

    // Insert before stretch
    auto* scrollLayout = static_cast<QVBoxLayout*>(scrollContent_->layout());
    scrollLayout->insertWidget(scrollLayout->count() - 1, groupBox);
}

void SwitchDemo::onSwitchToggled(bool checked) {
    if (statusLabel) {
        if (checked) {
            statusLabel->setText("Status: On");
            statusLabel->setStyleSheet("color: green;");
        } else {
            statusLabel->setText("Status: Off");
            statusLabel->setStyleSheet("color: gray;");
        }
    }
}

QGroupBox* SwitchDemo::createGroupBox(const QString& title) {
    QGroupBox* groupBox = new QGroupBox(title, this);
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->setSpacing(12);
    layout->setContentsMargins(16, 24, 16, 16);
    return groupBox;
}

} // namespace cf::ui::example
