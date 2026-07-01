/**
 * @file    apps/system_state/system_state_panel.cpp
 * @brief   Implementation of the System State panel.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-07-01
 * @version 0.1
 * @since   0.20
 * @ingroup system_state
 */

#include "system_state_panel.h"

#include "ui/widget/material/widget/button/button.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QScrollArea>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include "system/cpu/cfcpu.h"
#include "system/cpu/cfcpu_bonus.h"
#include "system/cpu/cfcpu_profile.h"
#include "system/memory/memory_info.h"

namespace cf::desktop::desktop_component {

using qw::widget::material::Button;
using Variant = Button::ButtonVariant;

namespace {
constexpr qreal kCornerRadius = 16.0;    ///< Card corner radius (px).
constexpr int kRefreshIntervalMs = 2000; ///< Auto-refresh interval (ms).

/// @brief Formats a byte count into a human-readable binary string.
/// @param bytes  The size in bytes.
/// @return String like "8.00 GB", "512.00 MB", or "1.23 KB".
QString formatBytes(uint64_t bytes) {
    constexpr uint64_t kb = 1024ULL;
    constexpr uint64_t mb = kb * 1024ULL;
    constexpr uint64_t gb = mb * 1024ULL;
    if (bytes < kb) {
        return QStringLiteral("%1 B").arg(static_cast<qulonglong>(bytes));
    }
    if (bytes < mb) {
        return QString::number(static_cast<double>(bytes) / kb, 'f', 2) + " KB";
    }
    if (bytes < gb) {
        return QString::number(static_cast<double>(bytes) / mb, 'f', 2) + " MB";
    }
    return QString::number(static_cast<double>(bytes) / gb, 'f', 2) + " GB";
}

/// @brief Builds a section heading label.
/// @param title  The heading text.
/// @return A styled, bold label.
QLabel* makeSectionLabel(const QString& title) {
    auto* label = new QLabel(title);
    QFont font = label->font();
    font.setBold(true);
    font.setPointSize(font.pointSize() + 1);
    label->setFont(font);
    return label;
}

/// @brief Builds a two-column key/value row.
/// @param key  The left column caption.
/// @return The value label (caller updates its text).
QLabel* makeRow(QVBoxLayout* layout, const QString& key) {
    auto* row = new QHBoxLayout;
    row->setSpacing(12);
    auto* key_label = new QLabel(key);
    key_label->setMinimumWidth(170);
    auto* value_label = new QLabel(QStringLiteral("—"));
    value_label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    value_label->setTextInteractionFlags(Qt::TextSelectableByMouse);
    row->addWidget(key_label);
    row->addWidget(value_label, 1);
    layout->addLayout(row);
    return value_label;
}
} // namespace

SystemStatePanel::SystemStatePanel(QWidget* parent) : QWidget(parent) {
    setupUi();
    loadStaticInfo();
    loadLiveInfo();

    refresh_timer_ = new QTimer(this);
    refresh_timer_->setInterval(kRefreshIntervalMs);
    connect(refresh_timer_, &QTimer::timeout, this, &SystemStatePanel::loadLiveInfo);
    refresh_timer_->start();
}

SystemStatePanel::~SystemStatePanel() = default;

void SystemStatePanel::setupUi() {
    auto* outer = new QVBoxLayout(this);
    outer->setContentsMargins(12, 12, 12, 12);
    outer->setSpacing(8);

    // Toolbar: Refresh | Auto (toggle).
    auto* toolbar = new QHBoxLayout;
    toolbar->setSpacing(8);
    auto* refresh_btn = new Button(QStringLiteral("Refresh"), Variant::Outlined, this);
    auto* auto_btn = new Button(QStringLiteral("Auto"), Variant::Tonal, this);
    auto_btn->setCheckable(true);
    auto_btn->setChecked(true);
    toolbar->addWidget(refresh_btn);
    toolbar->addWidget(auto_btn);
    toolbar->addStretch();
    outer->addLayout(toolbar);

    // Scrollable readout area.
    auto* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    auto* content = new QWidget;
    auto* layout = new QVBoxLayout(content);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(4);

    // CPU static section.
    layout->addWidget(makeSectionLabel(QStringLiteral("CPU")));
    cpu_model_label_ = makeRow(layout, QStringLiteral("Model"));
    cpu_arch_label_ = makeRow(layout, QStringLiteral("Architecture"));
    cpu_manufacturer_label_ = makeRow(layout, QStringLiteral("Manufacturer"));
    cpu_cores_label_ = makeRow(layout, QStringLiteral("Cores (logical/physical)"));
    cpu_freq_label_ = makeRow(layout, QStringLiteral("Frequency (current/max)"));
    cpu_usage_label_ = makeRow(layout, QStringLiteral("Usage"));
    cpu_temp_label_ = makeRow(layout, QStringLiteral("Temperature"));

    layout->addSpacing(8);

    // Memory section.
    layout->addWidget(makeSectionLabel(QStringLiteral("Memory")));
    mem_phys_total_label_ = makeRow(layout, QStringLiteral("Physical total"));
    mem_phys_used_label_ = makeRow(layout, QStringLiteral("Physical used"));
    mem_phys_avail_label_ = makeRow(layout, QStringLiteral("Physical available"));
    mem_swap_total_label_ = makeRow(layout, QStringLiteral("Swap total"));
    mem_swap_used_label_ = makeRow(layout, QStringLiteral("Swap used"));

    layout->addStretch();
    scroll->setWidget(content);
    outer->addWidget(scroll, 1);

    connect(refresh_btn, &QPushButton::clicked, this, &SystemStatePanel::refreshNow);
    connect(auto_btn, &QPushButton::toggled, this, &SystemStatePanel::toggleAutoRefresh);
}

void SystemStatePanel::loadStaticInfo() {
    const auto info_result = cf::getCPUInfo();
    if (info_result.has_value()) {
        const auto& info = info_result.value();
        cpu_model_label_->setText(QString::fromStdString(std::string(info.model)));
        cpu_arch_label_->setText(QString::fromStdString(std::string(info.arch)));
        cpu_manufacturer_label_->setText(QString::fromStdString(std::string(info.manufacturer)));
    } else {
        cpu_model_label_->setText(QStringLiteral("Unavailable"));
        cpu_arch_label_->setText(QStringLiteral("Unavailable"));
        cpu_manufacturer_label_->setText(QStringLiteral("Unavailable"));
    }
}

void SystemStatePanel::loadLiveInfo() {
    // CPU profile (cores / frequency / usage) — real-time each call.
    const auto profile_result = cf::getCPUProfileInfo();
    if (profile_result.has_value()) {
        const auto& p = profile_result.value();
        cpu_cores_label_->setText(QStringLiteral("%1 / %2").arg(p.logical_cnt).arg(p.physical_cnt));
        cpu_freq_label_->setText(
            QStringLiteral("%1 / %2 MHz").arg(p.current_frequecy).arg(p.max_frequency));
        cpu_usage_label_->setText(QString::number(p.cpu_usage_percentage, 'f', 1) + " %");
    } else {
        cpu_cores_label_->setText(QStringLiteral("Unavailable"));
        cpu_freq_label_->setText(QStringLiteral("Unavailable"));
        cpu_usage_label_->setText(QStringLiteral("Unavailable"));
    }

    // CPU bonus (temperature) — cached, refreshed on demand.
    const auto bonus_result = cf::getCPUBonusInfo(true);
    if (bonus_result.has_value() && bonus_result.value().temperature.has_value()) {
        cpu_temp_label_->setText(QString::number(*bonus_result.value().temperature) + " C");
    } else {
        cpu_temp_label_->setText(QStringLiteral("Unavailable"));
    }

    // Memory — real-time each call.
    cf::MemoryInfo mem{};
    cf::getSystemMemoryInfo(mem);

    const uint64_t phys_total = mem.physical.total_bytes;
    const uint64_t phys_avail = mem.physical.available_bytes;
    const uint64_t phys_used = (phys_total > phys_avail) ? (phys_total - phys_avail) : 0;
    mem_phys_total_label_->setText(formatBytes(phys_total));
    mem_phys_avail_label_->setText(formatBytes(phys_avail));
    if (phys_total > 0) {
        const double phys_pct = static_cast<double>(phys_used) / phys_total * 100.0;
        mem_phys_used_label_->setText(
            QStringLiteral("%1 (%2%)").arg(formatBytes(phys_used)).arg(phys_pct, 0, 'f', 1));
    } else {
        mem_phys_used_label_->setText(formatBytes(phys_used));
    }

    const uint64_t swap_total = mem.swap.total_bytes;
    const uint64_t swap_free = mem.swap.free_bytes;
    const uint64_t swap_used = (swap_total > swap_free) ? (swap_total - swap_free) : 0;
    mem_swap_total_label_->setText(formatBytes(swap_total));
    if (swap_total > 0) {
        const double swap_pct = static_cast<double>(swap_used) / swap_total * 100.0;
        mem_swap_used_label_->setText(
            QStringLiteral("%1 (%2%)").arg(formatBytes(swap_used)).arg(swap_pct, 0, 'f', 1));
    } else {
        mem_swap_used_label_->setText(formatBytes(swap_used));
    }
}

void SystemStatePanel::refreshNow() {
    loadStaticInfo();
    loadLiveInfo();
}

void SystemStatePanel::toggleAutoRefresh(bool checked) {
    checked ? refresh_timer_->start() : refresh_timer_->stop();
}

void SystemStatePanel::paintEvent(QPaintEvent* /*event*/) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    const QColor surface_color(0xF7, 0xF5, 0xF3);
    QPainterPath card;
    card.addRoundedRect(QRectF(rect()), kCornerRadius, kCornerRadius);
    p.fillPath(card, surface_color);
}

} // namespace cf::desktop::desktop_component
