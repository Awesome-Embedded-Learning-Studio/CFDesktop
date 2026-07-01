/**
 * @file    apps/calendar/calendar_panel.cpp
 * @brief   Implementation of the Calendar panel.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-07-01
 * @version 0.1
 * @since   0.20
 * @ingroup calendar
 */

#include "calendar_panel.h"

#include "ui/widget/material/widget/button/button.h"

#include <QCalendarWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QTextEdit>
#include <QVBoxLayout>

namespace cf::desktop::desktop_component {

using qw::widget::material::Button;
using Variant = Button::ButtonVariant;

namespace {
constexpr qreal kCornerRadius = 16.0; ///< Card corner radius (px).
} // namespace

CalendarPanel::CalendarPanel(QWidget* parent) : QWidget(parent) {
    setupUi();
    refreshDateDescription();
}

CalendarPanel::~CalendarPanel() = default;

void CalendarPanel::setupUi() {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(12, 12, 12, 12);
    root->setSpacing(8);

    // Top split: calendar (left) | note editor (right).
    auto* split = new QHBoxLayout;
    split->setSpacing(8);

    calendar_ = new QCalendarWidget(this);
    split->addWidget(calendar_, /*stretch=*/3);

    auto* editor_col = new QVBoxLayout;
    editor_col->setSpacing(6);

    date_label_ = new QLabel(this);
    date_label_->setWordWrap(true);
    editor_col->addWidget(date_label_);

    editor_ = new QTextEdit(this);
    editor_->setPlaceholderText(QStringLiteral("Write a note for the selected date..."));
    editor_col->addWidget(editor_);

    auto* toolbar = new QHBoxLayout;
    toolbar->setSpacing(6);
    auto* save_btn = new Button(QStringLiteral("Save"), Variant::Filled, this);
    auto* delete_btn = new Button(QStringLiteral("Delete"), Variant::Outlined, this);
    toolbar->addWidget(save_btn);
    toolbar->addWidget(delete_btn);
    toolbar->addStretch();
    editor_col->addLayout(toolbar);

    split->addLayout(editor_col, /*stretch=*/2);
    root->addLayout(split);

    connect(calendar_, &QCalendarWidget::selectionChanged, this,
            &CalendarPanel::onSelectionChanged);
    connect(save_btn, &QPushButton::clicked, this, &CalendarPanel::onSaveNote);
    connect(delete_btn, &QPushButton::clicked, this, &CalendarPanel::onDeleteNote);
}

void CalendarPanel::onSelectionChanged() {
    const QDate date = calendar_->selectedDate();
    editor_->setPlainText(notes_.value(date));
    refreshDateDescription();
}

void CalendarPanel::onSaveNote() {
    const QDate date = calendar_->selectedDate();
    const QString text = editor_->toPlainText();
    if (text.isEmpty()) {
        notes_.remove(date);
    } else {
        notes_.insert(date, text);
    }
}

void CalendarPanel::onDeleteNote() {
    const QDate date = calendar_->selectedDate();
    notes_.remove(date);
    editor_->clear();
}

void CalendarPanel::refreshDateDescription() {
    date_label_->setText(describeDate(calendar_->selectedDate()));
}

QString CalendarPanel::describeDate(const QDate& date) {
    if (!date.isValid()) {
        return QStringLiteral("Invalid date");
    }
    return QStringLiteral("%1 %2").arg(
        date.toString(Qt::TextDate),
        QStringLiteral("(day %1 of %2)").arg(date.dayOfYear()).arg(date.daysInYear()));
}

void CalendarPanel::paintEvent(QPaintEvent* /*event*/) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    const QColor surface_color(0xF7, 0xF5, 0xF3);
    QPainterPath card;
    card.addRoundedRect(QRectF(rect()), kCornerRadius, kCornerRadius);
    p.fillPath(card, surface_color);
}

} // namespace cf::desktop::desktop_component
