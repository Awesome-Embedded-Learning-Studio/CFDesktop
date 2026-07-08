/**
 * @file    local_temp_card.cpp
 * @brief   Implementation of LocalTempCard (Linux thermal subsystem).
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup home_page
 */

#include "local_temp_card.h"

#include <QGraphicsDropShadowEffect>
#include <QLabel>
#include <QTimer>
#include <QVBoxLayout>

#include <filesystem>
#include <fstream>
#include <string>

namespace cf::desktop::desktop_component {

namespace {
/// @brief Warm gradient QSS (temperature card).
inline constexpr const char* kCardQss = R"(
    #LocalTempCard {
        border-radius: 20px;
        background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
            stop:0 #FFB347, stop:1 #FF6B6B);
        padding: 12px;
    }
    #LocalTempCard QLabel { color: white; }
    #value_label { font-size: 40px; font-weight: bold; }
    #detail_label { font-size: 16px; }
)";

/// @brief Reads one thermal-zone file's contents, trimmed.
std::string readZoneFile(const std::filesystem::path& path) {
    std::ifstream in(path);
    std::string line;
    if (in) {
        std::getline(in, line);
    }
    // Trim trailing whitespace.
    while (!line.empty() && (line.back() == '\n' || line.back() == '\r' || line.back() == ' ')) {
        line.pop_back();
    }
    return line;
}
} // namespace

LocalTempCard::LocalTempCard(QWidget* parent) : QWidget(parent) {
    setAttribute(Qt::WA_StyledBackground);
    setObjectName("LocalTempCard");
    setStyleSheet(kCardQss);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 16, 20, 16);
    layout->setSpacing(6);

    value_label_ = new QLabel(this);
    value_label_->setObjectName("value_label");
    detail_label_ = new QLabel(this);
    detail_label_->setObjectName("detail_label");

    layout->addWidget(value_label_, 0, Qt::AlignLeft);
    layout->addStretch(1);
    layout->addWidget(detail_label_, 0, Qt::AlignLeft);

    auto* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(10);
    shadow->setOffset(0, 4);
    shadow->setColor(QColor(0, 0, 0, 160));
    setGraphicsEffect(shadow);

    refresh();

    timer_ = new QTimer(this);
    timer_->setTimerType(Qt::CoarseTimer);
    connect(timer_, &QTimer::timeout, this, [this]() { refresh(); });
    timer_->start(5000);
}

void LocalTempCard::refresh() {
    namespace fs = std::filesystem;
    const fs::path kThermalRoot{"/sys/class/thermal"};

    std::error_code ec;
    if (!fs::exists(kThermalRoot, ec)) {
        value_label_->setText(QStringLiteral("N/A"));
        detail_label_->setText(QStringLiteral("No thermal subsystem"));
        return;
    }

    // Take the first thermal_zone* with a valid temperature reading.
    for (const auto& entry : fs::directory_iterator(kThermalRoot, ec)) {
        const std::string name = entry.path().filename().string();
        if (name.rfind("thermal_zone", 0) != 0) {
            continue;
        }
        const std::string raw = readZoneFile(entry.path() / "temp");
        if (raw.empty()) {
            continue;
        }
        try {
            const long millideg = std::stol(raw);
            value_label_->setText(QStringLiteral("%1 C").arg(millideg / 1000.0, 0, 'f', 1));
        } catch (...) {
            continue; // Malformed reading; try the next zone.
        }
        std::string zone_type = readZoneFile(entry.path() / "type");
        if (zone_type.empty()) {
            zone_type = name;
        }
        detail_label_->setText(QString::fromStdString(zone_type));
        return;
    }

    // No usable zone (common on WSL2, which lacks thermal zones).
    value_label_->setText(QStringLiteral("N/A"));
    detail_label_->setText(QStringLiteral("No thermal zone exposed"));
}

} // namespace cf::desktop::desktop_component
