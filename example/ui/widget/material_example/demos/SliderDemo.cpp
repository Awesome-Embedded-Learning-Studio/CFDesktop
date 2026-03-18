/**
 * @file SliderDemo.cpp
 * @brief Material Design 3 Slider Demo - Implementation
 */

#include "SliderDemo.h"

#include "ui/widget/material/widget/slider/slider.h"

#include <QApplication>
#include <QFont>
#include <QHBoxLayout>
#include <QScrollArea>

using namespace cf::ui::widget::material;

namespace cf::ui::example {

SliderDemo::SliderDemo(QWidget* parent) : QWidget(parent) {
    setupUI();
    createHorizontalSection();
    createVerticalSection();
    createStatesSection();
    createTickMarksSection();
    createInteractiveDemoSection();
}

void SliderDemo::setupUI() {
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

void SliderDemo::createHorizontalSection() {
    QGroupBox* groupBox = createGroupBox("Horizontal Sliders (水平滑块)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    QHBoxLayout* layout1 = new QHBoxLayout();
    QLabel* label1 = new QLabel("0 - 100:", this);
    label1->setMinimumWidth(80);
    Slider* slider1 = new Slider(Qt::Horizontal, this);
    slider1->setRange(0, 100);
    slider1->setValue(50);
    layout1->addWidget(label1);
    layout1->addWidget(slider1);
    layout->addLayout(layout1);

    QHBoxLayout* layout2 = new QHBoxLayout();
    QLabel* label2 = new QLabel("0 - 255:", this);
    label2->setMinimumWidth(80);
    Slider* slider2 = new Slider(Qt::Horizontal, this);
    slider2->setRange(0, 255);
    slider2->setValue(128);
    layout2->addWidget(label2);
    layout2->addWidget(slider2);
    layout->addLayout(layout2);

    QHBoxLayout* layout3 = new QHBoxLayout();
    QLabel* label3 = new QLabel("Volume:", this);
    label3->setMinimumWidth(80);
    Slider* slider3 = new Slider(Qt::Horizontal, this);
    slider3->setRange(0, 100);
    slider3->setValue(70);
    layout3->addWidget(label3);
    layout3->addWidget(slider3);
    layout->addLayout(layout3);

    scrollContent_->layout()->addWidget(groupBox);
}

void SliderDemo::createVerticalSection() {
    QGroupBox* groupBox = createGroupBox("Vertical Sliders (垂直滑块)");
    QHBoxLayout* layout = static_cast<QHBoxLayout*>(groupBox->layout());

    QVBoxLayout* vLayout1 = new QVBoxLayout();
    Slider* vSlider1 = new Slider(Qt::Vertical, this);
    vSlider1->setRange(0, 100);
    vSlider1->setValue(30);
    vSlider1->setMinimumSize(60, 200);
    QLabel* vLabel1 = new QLabel("30", this);
    vLabel1->setAlignment(Qt::AlignCenter);
    vLayout1->addWidget(vSlider1);
    vLayout1->addWidget(vLabel1);
    layout->addLayout(vLayout1);

    QVBoxLayout* vLayout2 = new QVBoxLayout();
    Slider* vSlider2 = new Slider(Qt::Vertical, this);
    vSlider2->setRange(0, 100);
    vSlider2->setValue(70);
    vSlider2->setMinimumSize(60, 200);
    QLabel* vLabel2 = new QLabel("70", this);
    vLabel2->setAlignment(Qt::AlignCenter);
    vLayout2->addWidget(vSlider2);
    vLayout2->addWidget(vLabel2);
    layout->addLayout(vLayout2);

    QVBoxLayout* vLayout3 = new QVBoxLayout();
    Slider* vSlider3 = new Slider(Qt::Vertical, this);
    vSlider3->setRange(0, 100);
    vSlider3->setValue(50);
    vSlider3->setMinimumSize(60, 200);
    QLabel* vLabel3 = new QLabel("50", this);
    vLabel3->setAlignment(Qt::AlignCenter);
    vLayout3->addWidget(vSlider3);
    vLayout3->addWidget(vLabel3);
    layout->addLayout(vLayout3);

    layout->addStretch();

    scrollContent_->layout()->addWidget(groupBox);
}

void SliderDemo::createStatesSection() {
    QGroupBox* groupBox = createGroupBox("Slider States (滑块状态)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    QHBoxLayout* normalLayout = new QHBoxLayout();
    QLabel* normalLabel = new QLabel("Normal:", this);
    normalLabel->setMinimumWidth(80);
    Slider* normalSlider = new Slider(Qt::Horizontal, this);
    normalSlider->setRange(0, 100);
    normalSlider->setValue(60);
    normalLayout->addWidget(normalLabel);
    normalLayout->addWidget(normalSlider);
    layout->addLayout(normalLayout);

    QHBoxLayout* disabledLayout = new QHBoxLayout();
    QLabel* disabledLabel = new QLabel("Disabled:", this);
    disabledLabel->setMinimumWidth(80);
    Slider* disabledSlider = new Slider(Qt::Horizontal, this);
    disabledSlider->setRange(0, 100);
    disabledSlider->setValue(60);
    disabledSlider->setEnabled(false);
    disabledLayout->addWidget(disabledLabel);
    disabledLayout->addWidget(disabledSlider);
    layout->addLayout(disabledLayout);

    scrollContent_->layout()->addWidget(groupBox);
}

void SliderDemo::createTickMarksSection() {
    QGroupBox* groupBox = createGroupBox("Tick Marks (刻度)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    QHBoxLayout* ticksBelowLayout = new QHBoxLayout();
    QLabel* ticksBelowLabel = new QLabel("Ticks Below:", this);
    ticksBelowLabel->setMinimumWidth(100);
    Slider* ticksBelowSlider = new Slider(Qt::Horizontal, this);
    ticksBelowSlider->setRange(0, 100);
    ticksBelowSlider->setValue(25);
    ticksBelowSlider->setTickPosition(QSlider::TicksBelow);
    ticksBelowSlider->setTickInterval(10);
    ticksBelowLayout->addWidget(ticksBelowLabel);
    ticksBelowLayout->addWidget(ticksBelowSlider);
    layout->addLayout(ticksBelowLayout);

    QHBoxLayout* ticksAboveLayout = new QHBoxLayout();
    QLabel* ticksAboveLabel = new QLabel("Ticks Above:", this);
    ticksAboveLabel->setMinimumWidth(100);
    Slider* ticksAboveSlider = new Slider(Qt::Horizontal, this);
    ticksAboveSlider->setRange(0, 100);
    ticksAboveSlider->setValue(75);
    ticksAboveSlider->setTickPosition(QSlider::TicksAbove);
    ticksAboveSlider->setTickInterval(25);
    ticksAboveLayout->addWidget(ticksAboveLabel);
    ticksAboveLayout->addWidget(ticksAboveSlider);
    layout->addLayout(ticksAboveLayout);

    QHBoxLayout* bothLayout = new QHBoxLayout();
    QLabel* bothLabel = new QLabel("Both Sides:", this);
    bothLabel->setMinimumWidth(100);
    Slider* bothSlider = new Slider(Qt::Horizontal, this);
    bothSlider->setRange(0, 10);
    bothSlider->setValue(5);
    bothSlider->setTickPosition(QSlider::TicksBothSides);
    bothSlider->setTickInterval(1);
    bothLayout->addWidget(bothLabel);
    bothLayout->addWidget(bothSlider);
    layout->addLayout(bothLayout);

    scrollContent_->layout()->addWidget(groupBox);
}

void SliderDemo::createInteractiveDemoSection() {
    QGroupBox* groupBox = createGroupBox("Interactive Demo (交互演示)");
    QVBoxLayout* layout = static_cast<QVBoxLayout*>(groupBox->layout());

    QHBoxLayout* hSliderLayout = new QHBoxLayout();
    QLabel* hLabel = new QLabel("Horizontal:", this);
    hLabel->setMinimumWidth(80);
    Slider* hSlider = new Slider(Qt::Horizontal, this);
    hSlider->setRange(0, 100);
    hSlider->setValue(50);
    connect(hSlider, &Slider::valueChanged, this, &SliderDemo::onHorizontalSliderChanged);
    horizontalValueLabel_ = new QLabel("Value: 50", this);
    hSliderLayout->addWidget(hLabel);
    hSliderLayout->addWidget(hSlider);
    hSliderLayout->addWidget(horizontalValueLabel_);
    layout->addLayout(hSliderLayout);

    QHBoxLayout* vSliderLayout = new QHBoxLayout();
    QLabel* vLabel = new QLabel("Vertical:", this);
    vLabel->setMinimumWidth(80);
    Slider* vSlider = new Slider(Qt::Vertical, this);
    vSlider->setRange(0, 100);
    vSlider->setValue(30);
    vSlider->setMinimumSize(60, 150);
    connect(vSlider, &Slider::valueChanged, this, &SliderDemo::onVerticalSliderChanged);
    verticalValueLabel_ = new QLabel("Value: 30", this);
    vSliderLayout->addWidget(vLabel);
    vSliderLayout->addWidget(vSlider);
    vSliderLayout->addWidget(verticalValueLabel_);
    vSliderLayout->addStretch();
    layout->addLayout(vSliderLayout);

    scrollContent_->layout()->addWidget(groupBox);

    static_cast<QVBoxLayout*>(scrollContent_->layout())->addStretch();
}

void SliderDemo::onHorizontalSliderChanged(int value) {
    horizontalValueLabel_->setText(QString("Value: %1").arg(value));
}

void SliderDemo::onVerticalSliderChanged(int value) {
    verticalValueLabel_->setText(QString("Value: %1").arg(value));
}

QGroupBox* SliderDemo::createGroupBox(const QString& title) {
    QGroupBox* groupBox = new QGroupBox(title, this);
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->setSpacing(12);
    layout->setContentsMargins(16, 24, 16, 16);
    return groupBox;
}

} // namespace cf::ui::example
