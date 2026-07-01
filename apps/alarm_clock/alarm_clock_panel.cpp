/**
 * @file    apps/alarm_clock/alarm_clock_panel.cpp
 * @brief   Implementation of the Alarm Clock panel.
 *
 * Ported from CCIMXDesktop's AlarmyClock. The original AlarmyNotifier polling
 * loop (1 s QTimer + per-second QTime match) is preserved verbatim in @ref
 * onTick; the broadcaster/processor event bus is collapsed into the direct
 * @ref fireAlarm call, and the QPainter analog dial + QMainWindow/.ui shell
 * are replaced by a QuarkWidgets MD3 panel built in code.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-07-01
 * @version 0.1
 * @since   0.20
 * @ingroup alarm_clock
 */

#include "alarm_clock_panel.h"

#include "ui/widget/material/widget/button/button.h"
#include "ui/widget/material/widget/label/label.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QPainter>
#include <QPainterPath>
#include <QSpinBox>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>

namespace cf::desktop::desktop_component {

namespace {
using qw::widget::material::Button;
using qw::widget::material::Label;
using qw::widget::material::TypographyStyle;
using Variant = Button::ButtonVariant;

constexpr qreal kCornerRadius = 16.0; ///< Card corner radius (px).

/// @brief Qt item-data role key for the stored AlarmEntry.
constexpr int kAlarmRole = Qt::UserRole + 1;

/// @brief Renders an alarm entry as "hh:mm  —  note".
QString formatEntry(const AlarmEntry& e) {
    return e.time.toString(QStringLiteral("hh:mm")) + QStringLiteral("  —  ") +
           (e.note.isEmpty() ? QStringLiteral("(no note)") : e.note);
}
} // namespace

AlarmClockPanel::AlarmClockPanel(QWidget* parent) : QWidget(parent) {
    setupUi();

    // Poll the wall clock once per second (mirrors AlarmyNotifier::check_time).
    ticker_ = new QTimer(this);
    ticker_->setInterval(1000);
    connect(ticker_, &QTimer::timeout, this, &AlarmClockPanel::onTick);
    ticker_->start();
    onTick();
}

AlarmClockPanel::~AlarmClockPanel() = default;

void AlarmClockPanel::setupUi() {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(12);

    // Live clock readout.
    clock_label_ = new Label("--:--:--", TypographyStyle::DisplayLarge, this);
    clock_label_->setAlignment(Qt::AlignCenter);
    layout->addWidget(clock_label_);

    // --- Time editor row: [- hour +] : [- minute +] ---
    auto* editor_row = new QHBoxLayout;
    editor_row->setSpacing(6);

    auto make_spin = [this](int max) {
        auto* spin = new QSpinBox(this);
        spin->setRange(0, max);
        spin->setFixedHeight(40);
        spin->setButtonSymbols(QSpinBox::PlusMinus);
        return spin;
    };
    hour_spin_ = make_spin(23);
    hour_spin_->setValue(QTime::currentTime().hour());
    minute_spin_ = make_spin(59);
    minute_spin_->setValue(QTime::currentTime().minute());

    auto make_step = [this](const QString& glyph, Variant v) {
        auto* btn = new Button(glyph, v, this);
        btn->setFixedSize(40, 40);
        return btn;
    };
    auto* h_minus = make_step(QStringLiteral("-"), Variant::Tonal);
    auto* h_plus = make_step(QStringLiteral("+"), Variant::Tonal);
    auto* m_minus = make_step(QStringLiteral("-"), Variant::Tonal);
    auto* m_plus = make_step(QStringLiteral("+"), Variant::Tonal);

    connect(h_minus, &QPushButton::clicked, this,
            [this] { hour_spin_->setValue(hour_spin_->value() - 1); });
    connect(h_plus, &QPushButton::clicked, this,
            [this] { hour_spin_->setValue(hour_spin_->value() + 1); });
    connect(m_minus, &QPushButton::clicked, this,
            [this] { minute_spin_->setValue(minute_spin_->value() - 1); });
    connect(m_plus, &QPushButton::clicked, this,
            [this] { minute_spin_->setValue(minute_spin_->value() + 1); });

    editor_row->addWidget(h_minus);
    editor_row->addWidget(hour_spin_);
    editor_row->addWidget(h_plus);
    auto* colon = new QLabel(QStringLiteral(":"), this);
    colon->setAlignment(Qt::AlignCenter);
    QFont colon_font = colon->font();
    colon_font.setPointSize(20);
    colon->setFont(colon_font);
    editor_row->addWidget(colon);
    editor_row->addWidget(m_minus);
    editor_row->addWidget(minute_spin_);
    editor_row->addWidget(m_plus);
    editor_row->addStretch();
    layout->addLayout(editor_row);

    // Note editor.
    note_edit_ = new QTextEdit(this);
    note_edit_->setPlaceholderText(QStringLiteral("Reminder note (optional)"));
    note_edit_->setFixedHeight(60);
    layout->addWidget(note_edit_);

    // Add / Remove control row.
    auto* control_row = new QHBoxLayout;
    control_row->setSpacing(8);
    auto* add_btn = new Button(QStringLiteral("Add Alarm"), Variant::Filled, this);
    auto* remove_btn = new Button(QStringLiteral("Remove Selected"), Variant::Outlined, this);
    control_row->addWidget(add_btn);
    control_row->addWidget(remove_btn);
    control_row->addStretch();
    layout->addLayout(control_row);

    // Armed alarms list.
    alarm_list_ = new QListWidget(this);
    layout->addWidget(alarm_list_, /*stretch=*/1);

    connect(add_btn, &QPushButton::clicked, this, &AlarmClockPanel::onAddAlarm);
    connect(remove_btn, &QPushButton::clicked, this, &AlarmClockPanel::onRemoveAlarm);
}

void AlarmClockPanel::onTick() {
    const QTime now = QTime::currentTime();
    clock_label_->setText(now.toString(QStringLiteral("hh:mm:ss")));

    // Match hour+minute+second against every armed alarm (the original
    // AlarmyNotifier matches all three fields; entries are stored with second
    // forced to 0, so an alarm fires in the first second of its minute).
    for (int i = 0; i < alarm_list_->count(); ++i) {
        auto* item = alarm_list_->item(i);
        const auto entry = item->data(kAlarmRole).value<AlarmEntry>();
        if (entry.time.hour() == now.hour() && entry.time.minute() == now.minute() &&
            entry.time.second() == now.second()) {
            fireAlarm(item);
        }
    }
}

void AlarmClockPanel::onAddAlarm() {
    AlarmEntry entry;
    entry.time = QTime(hour_spin_->value(), minute_spin_->value(), 0);
    entry.note = note_edit_->toPlainText().trimmed();

    // Reject duplicates at the same minute to avoid a double-fire.
    for (int i = 0; i < alarm_list_->count(); ++i) {
        const auto existing = alarm_list_->item(i)->data(kAlarmRole).value<AlarmEntry>();
        if (existing.time.hour() == entry.time.hour() &&
            existing.time.minute() == entry.time.minute()) {
            return;
        }
    }

    auto* item = new QListWidgetItem(formatEntry(entry), alarm_list_);
    item->setData(kAlarmRole, QVariant::fromValue(entry));
    alarm_list_->addItem(item);

    note_edit_->clear();
}

void AlarmClockPanel::onRemoveAlarm() {
    auto selected = alarm_list_->selectedItems();
    for (auto* item : selected) {
        delete item;
    }
}

void AlarmClockPanel::fireAlarm(QListWidgetItem* item) {
    const auto entry = item->data(kAlarmRole).value<AlarmEntry>();

    // TODO(alarm_clock): add audible ringing. The CCIMX original had no audio
    // asset either (it popped a QMessageBox). Wire QSoundEffect with a bundled
    // .wav (requires Qt6::Multimedia + a qrc asset) or a platform system sound
    // here. For now the visual alert mirrors the original DefaultProcessor.
    QMessageBox::information(
        this, QStringLiteral("Alarm"),
        QStringLiteral("⏰ %1\n\n%2")
            .arg(entry.time.toString(QStringLiteral("hh:mm")),
                 entry.note.isEmpty() ? QStringLiteral("Time's up!") : entry.note));

    // One-shot: disarm after firing.
    delete item;
}

void AlarmClockPanel::paintEvent(QPaintEvent* /*event*/) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    const QColor surface_color(0xF7, 0xF5, 0xF3);
    QPainterPath card;
    card.addRoundedRect(QRectF(rect()), kCornerRadius, kCornerRadius);
    p.fillPath(card, surface_color);
}

} // namespace cf::desktop::desktop_component
