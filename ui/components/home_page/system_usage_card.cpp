/**
 * @file    system_usage_card.cpp
 * @brief   Implementation of the SystemUsageCard.
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup home_page
 */

#include "system_usage_card.h"

#include <QGraphicsDropShadowEffect>
#include <QLabel>
#include <QProgressBar>
#include <QTimer>
#include <QVBoxLayout>

namespace cf::desktop::desktop_component {

namespace {
/// @brief Dark gradient QSS (ported from CCIMX MemoryUsageCard / DiskUsageCard).
inline constexpr const char* kCardQss = R"(
    #SystemUsageCard {
        border-radius: 16px;
        background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
            stop:0 #5D6D7E, stop:1 #2C3E50);
    }
    #SystemUsageCard QLabel { color: #ECF0F1; }
    #title_label { font-size: 32px; font-weight: bold; }
    #value_label { font-size: 48px; font-weight: bold; }
    #detail_label { font-size: 16px; }
    QProgressBar {
        border-radius: 7px;
        background-color: #34495e;
        min-height: 18px;
        max-height: 18px;
    }
    QProgressBar::chunk {
        background-color: #1abc9c;
        border-radius: 7px;
    }
)";
} // namespace

SystemUsageCard::SystemUsageCard(const QString& title, Probe probe, int interval_ms,
                                 QWidget* parent)
    : QWidget(parent), probe_(std::move(probe)) {
    setAttribute(Qt::WA_StyledBackground); // required for QSS background on a QWidget
    setObjectName("SystemUsageCard");
    setStyleSheet(kCardQss);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 20, 24, 20);
    layout->setSpacing(8);

    title_label_ = new QLabel(title, this);
    title_label_->setObjectName("title_label");
    value_label_ = new QLabel(this);
    value_label_->setObjectName("value_label");
    bar_ = new QProgressBar(this);
    bar_->setRange(0, 100);
    bar_->setTextVisible(false);
    detail_label_ = new QLabel(this);
    detail_label_->setObjectName("detail_label");

    layout->addWidget(title_label_, 0, Qt::AlignLeft);
    layout->addStretch(1);
    layout->addWidget(value_label_, 0, Qt::AlignCenter);
    layout->addStretch(1);
    layout->addWidget(bar_);
    layout->addWidget(detail_label_, 0, Qt::AlignLeft);

    auto* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(20);
    shadow->setOffset(0, 5);
    shadow->setColor(QColor(0, 0, 0, 100));
    setGraphicsEffect(shadow);

    refresh();

    timer_ = new QTimer(this);
    timer_->setTimerType(Qt::CoarseTimer);
    connect(timer_, &QTimer::timeout, this, [this]() { refresh(); });
    timer_->start(interval_ms);
}

void SystemUsageCard::refresh() {
    if (!probe_) {
        return;
    }
    const UsageSample sample = probe_();
    int pct = sample.pct;
    if (pct < 0) {
        pct = 0;
    }
    if (pct > 100) {
        pct = 100;
    }
    bar_->setValue(pct);
    value_label_->setText(QStringLiteral("%1%").arg(pct));
    detail_label_->setText(sample.detail);
}

} // namespace cf::desktop::desktop_component
