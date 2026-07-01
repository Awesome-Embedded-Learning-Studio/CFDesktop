/**
 * @file    apps/noter/noter_panel.cpp
 * @brief   Implementation of the Noter panel.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-07-01
 * @version 0.1
 * @since   0.20
 * @ingroup noter
 */

#include "noter_panel.h"

#include "ui/widget/material/widget/button/button.h"

#include <QFile>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QSlider>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextEdit>
#include <QTextStream>
#include <QVBoxLayout>

namespace cf::desktop::desktop_component {

using qw::widget::material::Button;
using Variant = Button::ButtonVariant;

namespace {
constexpr qreal kCornerRadius = 16.0; ///< Card corner radius (px).
constexpr int kMinFontSize = 8;       ///< Minimum font size (pt).
constexpr int kMaxFontSize = 40;      ///< Maximum font size (pt).
constexpr int kDefaultFontSize = 15;  ///< Initial font size (pt).
} // namespace

NoterPanel::NoterPanel(QWidget* parent) : QWidget(parent) {
    setupUi();
}

NoterPanel::~NoterPanel() = default;

void NoterPanel::setupUi() {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(8);

    // Toolbar: Open | Save | B | I | size slider | size readout.
    auto* toolbar = new QHBoxLayout;
    toolbar->setSpacing(8);

    auto* open_btn = new Button(QStringLiteral("Open"), Variant::Outlined, this);
    auto* save_btn = new Button(QStringLiteral("Save"), Variant::Outlined, this);
    auto* bold_btn = new Button(QStringLiteral("B"), Variant::Tonal, this);
    bold_btn->setCheckable(true);
    auto* italic_btn = new Button(QStringLiteral("I"), Variant::Tonal, this);
    italic_btn->setCheckable(true);

    font_slider_ = new QSlider(Qt::Horizontal, this);
    font_slider_->setMinimum(kMinFontSize);
    font_slider_->setMaximum(kMaxFontSize);
    font_slider_->setValue(kDefaultFontSize);
    font_slider_->setFixedWidth(120);

    size_label_ = new QLabel(QString::number(kDefaultFontSize), this);
    size_label_->setFixedWidth(28);

    toolbar->addWidget(open_btn);
    toolbar->addWidget(save_btn);
    toolbar->addWidget(bold_btn);
    toolbar->addWidget(italic_btn);
    toolbar->addWidget(font_slider_);
    toolbar->addWidget(size_label_);
    toolbar->addStretch();
    layout->addLayout(toolbar);

    // Editor.
    editor_ = new QTextEdit(this);
    editor_->setFontPointSize(kDefaultFontSize);
    layout->addWidget(editor_);

    connect(open_btn, &QPushButton::clicked, this, &NoterPanel::onOpen);
    connect(save_btn, &QPushButton::clicked, this, &NoterPanel::onSave);
    connect(font_slider_, &QSlider::valueChanged, this, &NoterPanel::onFontSizeChanged);
    connect(bold_btn, &QPushButton::toggled, this, &NoterPanel::onBoldToggled);
    connect(italic_btn, &QPushButton::toggled, this, &NoterPanel::onItalicToggled);
}

void NoterPanel::applyCharFormat(const QTextCharFormat& format) {
    QTextCursor cursor = editor_->textCursor();
    if (!cursor.hasSelection()) {
        cursor.select(QTextCursor::WordUnderCursor);
    }
    cursor.mergeCharFormat(format);
    editor_->mergeCurrentCharFormat(format);
}

void NoterPanel::onOpen() {
    const QString file_name = QFileDialog::getOpenFileName(this, tr("Open File"));
    if (file_name.isEmpty()) {
        return;
    }
    QFile file(file_name);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        editor_->setPlainText(in.readAll());
    }
}

void NoterPanel::onSave() {
    const QString file_name = QFileDialog::getSaveFileName(this, tr("Save File"));
    if (file_name.isEmpty()) {
        return;
    }
    QFile file(file_name);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << editor_->toPlainText();
    }
}

void NoterPanel::onFontSizeChanged(int size) {
    size_label_->setText(QString::number(size));
    QTextCharFormat format;
    format.setFontPointSize(size);
    applyCharFormat(format);
}

void NoterPanel::onBoldToggled(bool checked) {
    QTextCharFormat format;
    format.setFontWeight(checked ? QFont::Bold : QFont::Normal);
    applyCharFormat(format);
}

void NoterPanel::onItalicToggled(bool checked) {
    QTextCharFormat format;
    format.setFontItalic(checked);
    applyCharFormat(format);
}

void NoterPanel::paintEvent(QPaintEvent* /*event*/) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    const QColor surface_color(0xF7, 0xF5, 0xF3);
    QPainterPath card;
    card.addRoundedRect(QRectF(rect()), kCornerRadius, kCornerRadius);
    p.fillPath(card, surface_color);
}

} // namespace cf::desktop::desktop_component
