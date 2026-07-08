/**
 * @file    disk_gauge_card.cpp
 * @brief   Implementation of DiskGaugeCard (CCIMX DiskUsageCardWidget style).
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup home_page
 */

#include "disk_gauge_card.h"

#include "gauge_widget.h"

#include <QFutureWatcher>
#include <QGraphicsDropShadowEffect>
#include <QLabel>
#include <QTimer>
#include <QVBoxLayout>
#include <QtConcurrent>

namespace cf::desktop::desktop_component {

namespace {
/// @brief Dark gradient QSS (ported from CCIMX DiskUsageCardWidget).
inline constexpr const char* kCardQss = R"(
    #DiskGaugeCard {
        border-radius: 16px;
        background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
            stop:0 #5D6D7E, stop:1 #2C3E50);
    }
    #DiskGaugeCard QLabel { color: #ECF0F1; }
    #title_label { font-size: 32px; font-weight: bold; }
    #detail_label { font-size: 16px; }
)";
} // namespace

DiskGaugeCard::DiskGaugeCard(const QString& title, SystemUsageCard::Probe probe, int interval_ms,
                             QWidget* parent)
    : QWidget(parent), probe_(std::move(probe)) {
    setAttribute(Qt::WA_StyledBackground);
    setObjectName("DiskGaugeCard");
    setStyleSheet(kCardQss);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(8);

    title_label_ = new QLabel(title, this);
    title_label_->setObjectName("title_label");
    layout->addWidget(title_label_, 0, Qt::AlignLeft);

    gauge_ = new GaugeWidget(this);
    gauge_->setRange(0, 100);
    gauge_->setTitle(title);
    gauge_->setUnit("%");
    layout->addWidget(gauge_, 1, Qt::AlignCenter);

    detail_label_ = new QLabel(this);
    detail_label_->setObjectName("detail_label");
    detail_label_->setAlignment(Qt::AlignCenter);
    layout->addWidget(detail_label_, 0, Qt::AlignCenter);

    auto* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(12);
    shadow->setOffset(0, 4);
    shadow->setColor(QColor(0, 0, 0, 160));
    setGraphicsEffect(shadow);

    // Same off-thread poll pattern as SystemUsageCard.
    probe_watcher_ = new QFutureWatcher<UsageSample>(this);
    connect(probe_watcher_, &QFutureWatcher<UsageSample>::finished, this, [this]() {
        applySample(probe_watcher_->result());
        in_flight_ = false;
    });

    refresh(); // first (async) sample

    auto* timer = new QTimer(this);
    timer->setTimerType(Qt::CoarseTimer);
    connect(timer, &QTimer::timeout, this, [this]() { refresh(); });
    timer->start(interval_ms);
}

void DiskGaugeCard::refresh() {
    if (!probe_ || in_flight_) {
        return;
    }
    in_flight_ = true;
    probe_watcher_->setFuture(QtConcurrent::run([probe = probe_]() { return probe(); }));
}

void DiskGaugeCard::applySample(const UsageSample& sample) {
    int pct = sample.pct;
    if (pct < 0) {
        pct = 0;
    }
    if (pct > 100) {
        pct = 100;
    }
    gauge_->update_value(pct);
    detail_label_->setText(sample.detail);
}

} // namespace cf::desktop::desktop_component
