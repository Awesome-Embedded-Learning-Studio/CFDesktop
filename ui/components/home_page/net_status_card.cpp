/**
 * @file    net_status_card.cpp
 * @brief   Implementation of NetStatusCard (CCIMX NetCardGadget style).
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup home_page
 */

#include "net_status_card.h"

#include "system/network/network.h"

#include <QGraphicsDropShadowEffect>
#include <QLabel>
#include <QTimer>
#include <QVBoxLayout>

namespace cf::desktop::desktop_component {

namespace {
/// @brief Teal gradient QSS (ported from CCIMX NetCardGadget).
inline constexpr const char* kCardQss = R"(
    #NetStatusCard {
        border-radius: 20px;
        background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
            stop:0 #00B894, stop:1 #00695C);
        padding: 12px;
    }
    #NetStatusCard QLabel { color: white; }
    #status_label { font-size: 36px; font-weight: bold; }
    #detail_label { font-size: 16px; }
)";

/// @brief Maps a cfbase Reachability to a short display word.
QString reachabilityLabel(cf::Reachability r) {
    switch (r) {
        case cf::Reachability::Online:
            return QStringLiteral("Online");
        case cf::Reachability::Site:
            return QStringLiteral("LAN");
        case cf::Reachability::Local:
            return QStringLiteral("Local only");
        case cf::Reachability::Disconnected:
            return QStringLiteral("Offline");
        case cf::Reachability::Unknown:
            break;
    }
    return QStringLiteral("Unknown");
}

/// @brief Maps a cfbase TransportMedium to a short display word.
QString transportLabel(cf::TransportMedium m) {
    switch (m) {
        case cf::TransportMedium::WiFi:
            return QStringLiteral("Wi-Fi");
        case cf::TransportMedium::Ethernet:
            return QStringLiteral("Ethernet");
        case cf::TransportMedium::Cellular:
            return QStringLiteral("Cellular");
        case cf::TransportMedium::Bluetooth:
            return QStringLiteral("Bluetooth");
        case cf::TransportMedium::Unknown:
            break;
    }
    return QStringLiteral("Unknown link");
}

/// @brief Finds the first non-loopback, up interface with an IPv4 address.
QString firstIPv4Detail(const cf::NetworkInfo& info) {
    for (const cf::InterfaceInfo& iface : info.interfaces) {
        const bool is_loopback = (iface.flags.value & cf::InterfaceFlags::kIsLoopBack) != 0;
        const bool is_up = (iface.flags.value & cf::InterfaceFlags::kIsUp) != 0;
        if (is_loopback || !is_up) {
            continue;
        }
        const auto ip = iface.firstIPv4();
        if (ip.has_value()) {
            return QStringLiteral("%1 · %2")
                .arg(QString::fromStdString(iface.name))
                .arg(QString::fromStdString(ip->toString()));
        }
    }
    return QStringLiteral("No IPv4 address");
}
} // namespace

NetStatusCard::NetStatusCard(QWidget* parent) : QWidget(parent) {
    setAttribute(Qt::WA_StyledBackground);
    setObjectName("NetStatusCard");
    setStyleSheet(kCardQss);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 16, 20, 16);
    layout->setSpacing(6);

    status_label_ = new QLabel(this);
    status_label_->setObjectName("status_label");
    detail_label_ = new QLabel(this);
    detail_label_->setObjectName("detail_label");
    detail_label_->setWordWrap(true);

    layout->addWidget(status_label_, 0, Qt::AlignLeft);
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

void NetStatusCard::refresh() {
    const auto result = cf::getNetworkInfo();
    if (!result.has_value()) {
        status_label_->setText(reachabilityLabel(cf::Reachability::Unknown));
        detail_label_->setText(QStringLiteral("Network backend unavailable"));
        return;
    }
    const cf::NetworkInfo& info = result.value();
    status_label_->setText(reachabilityLabel(info.status.reachability));
    detail_label_->setText(QStringLiteral("%1\n%2")
                               .arg(transportLabel(info.status.transportMedium))
                               .arg(firstIPv4Detail(info)));
}

} // namespace cf::desktop::desktop_component
