/**
 * @file    home_page.cpp
 * @brief   Implementation of the HomePage screen.
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup home_page
 */

#include "home_page.h"

#include "analog_clock_widget.h"
#include "card_stack_widget.h"
#include "date_card.h"
#include "digital_time_widget.h"
#include "disk_gauge_card.h"
#include "home_card_manager.h"
#include "local_temp_card.h"
#include "modern_calendar_widget.h"
#include "net_status_card.h"
#include "system/cpu/cfcpu_profile.h"
#include "system/memory/memory_info.h"
#include "system_usage_card.h"
#include "user_info_card.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>

#include <filesystem>

namespace cf::desktop::desktop_component {

namespace {
/// @brief Bytes per gigabyte.
inline constexpr double kBytesPerGB = 1024.0 * 1024.0 * 1024.0;

/// @brief Memory usage sample (cfbase getSystemMemoryInfo).
UsageSample memorySample() {
    cf::MemoryInfo info{};
    cf::getSystemMemoryInfo(info);
    const quint64 total = info.physical.total_bytes;
    const quint64 avail = info.physical.available_bytes;
    const quint64 used = (total > avail) ? total - avail : 0;
    const int pct = (total > 0) ? static_cast<int>((used * 100U) / total) : 0;
    const QString detail = QStringLiteral("%1 / %2 GB")
                               .arg(static_cast<double>(used) / kBytesPerGB, 0, 'f', 1)
                               .arg(static_cast<double>(total) / kBytesPerGB, 0, 'f', 1);
    return {pct, detail};
}

/// @brief CPU usage sample (cfbase getCPUProfileInfo).
UsageSample cpuSample() {
    const auto result = cf::getCPUProfileInfo();
    if (!result.has_value()) {
        return {0, QStringLiteral("--")};
    }
    const auto& p = result.value();
    const int pct = static_cast<int>(p.cpu_usage_percentage);
    return {pct, QStringLiteral("%1 cores · %2 MHz").arg(p.logical_cnt).arg(p.current_frequecy)};
}

/// @brief Disk usage sample (std::filesystem::space of "/").
UsageSample diskSample() {
    std::error_code ec;
    const auto sp = std::filesystem::space("/", ec);
    if (ec) {
        return {0, QStringLiteral("--")};
    }
    const quint64 total = sp.capacity;
    const quint64 avail = sp.available;
    const quint64 used = (total > avail) ? total - avail : 0;
    const int pct = (total > 0) ? static_cast<int>((used * 100U) / total) : 0;
    const QString detail = QStringLiteral("%1 / %2 GB")
                               .arg(static_cast<double>(used) / kBytesPerGB, 0, 'f', 0)
                               .arg(static_cast<double>(total) / kBytesPerGB, 0, 'f', 0);
    return {pct, detail};
}
} // namespace

HomePage::HomePage(QWidget* parent) : QWidget(parent) {
    setAttribute(Qt::WA_TranslucentBackground);

    auto* outer = new QHBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->setSpacing(0);

    // Left column: analog clock (5) above digital time (2).
    auto* left = new QWidget(this);
    auto* left_layout = new QVBoxLayout(left);
    left_layout->setContentsMargins(0, 0, 0, 0);
    left_layout->setSpacing(0);
    analog_clock_ = new AnalogClockWidget(left);
    digital_time_ = new DigitalTimeWidget(left);
    left_layout->addWidget(analog_clock_, 5);
    left_layout->addWidget(digital_time_, 2);

    // Right column: card stack (3) above a gadget grid (1) — net status + local
    // temp, mirroring CCIMX's right-bottom NetCardGadget / LocalWeatherCard.
    auto* right = new QWidget(this);
    auto* right_layout = new QVBoxLayout(right);
    right_layout->setContentsMargins(0, 0, 0, 0);
    right_layout->setSpacing(0);
    card_stack_ = new CardStackWidget(right);
    auto* bottom_grid = new QWidget(right);
    auto* grid_layout = new QGridLayout(bottom_grid);
    grid_layout->setContentsMargins(2, 2, 2, 2);
    grid_layout->setSpacing(3);
    grid_layout->addWidget(new NetStatusCard(bottom_grid), 0, 0);
    grid_layout->addWidget(new LocalTempCard(bottom_grid), 0, 1);
    right_layout->addWidget(card_stack_, 3);
    right_layout->addWidget(bottom_grid, 1);

    outer->addWidget(left);
    outer->addWidget(right);
    outer->setStretchFactor(left, 1);
    outer->setStretchFactor(right, 1);

    // Card-stack order mirrors CCIMX (UserInfo, Calendar, Date, Disk, Memory);
    // CPU is an extra card reusing the SystemUsageCard renderer.
    card_manager_ = std::make_unique<HomeCardManager>(card_stack_);
    card_manager_->installCard(new UserInfoCard(card_stack_));
    card_manager_->installCard(new ModernCalendarWidget(card_stack_));
    card_manager_->installCard(new DateCard(card_stack_));
    card_manager_->installCard(new DiskGaugeCard("Disk", diskSample, 10000, card_stack_));
    card_manager_->installCard(new SystemUsageCard("Memory", memorySample, 2000, card_stack_));
    card_manager_->installCard(new SystemUsageCard("CPU", cpuSample, 5000, card_stack_));
}

HomeCardManager* HomePage::cardManager() const {
    return card_manager_.get();
}

} // namespace cf::desktop::desktop_component
