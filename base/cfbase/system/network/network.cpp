#include "system/network/network.h"
#include <QHostAddress>
#include <QNetworkAddressEntry>
#include <QNetworkInformation>
#include <QNetworkInterface>

#include <cstdio>
#include <cstring>

namespace {
using namespace cf;
static IpAddress fromQHostAddress(const QHostAddress& qa) {
    IpAddress addr;
    if (qa.isNull())
        return addr;

    if (qa.protocol() == QAbstractSocket::IPv4Protocol) {
        addr.protocol = IpProtocol::IPv4;
        const uint32_t v4 = qa.toIPv4Address(); // 主机字节序
        addr.bytes[0] = static_cast<uint8_t>((v4 >> 24) & 0xFF);
        addr.bytes[1] = static_cast<uint8_t>((v4 >> 16) & 0xFF);
        addr.bytes[2] = static_cast<uint8_t>((v4 >> 8) & 0xFF);
        addr.bytes[3] = static_cast<uint8_t>(v4 & 0xFF);
    } else if (qa.protocol() == QAbstractSocket::IPv6Protocol) {
        addr.protocol = IpProtocol::IPv6;
        Q_IPV6ADDR v6 = qa.toIPv6Address();
        std::memcpy(addr.bytes.data(), &v6, 16);
    }
    return addr;
}

// QNetworkInterface::InterfaceType → InterfaceType
static InterfaceType mapType(QNetworkInterface::InterfaceType qt) {
    switch (qt) {
        case QNetworkInterface::Loopback:
            return InterfaceType::Loopback;
        case QNetworkInterface::Ethernet:
            return InterfaceType::Ethernet;
        case QNetworkInterface::Wifi:
            return InterfaceType::Wifi;
        case QNetworkInterface::Ppp:
            return InterfaceType::Ppp;
        case QNetworkInterface::Slip:
            return InterfaceType::Slip;
        case QNetworkInterface::CanBus:
            return InterfaceType::CanBus;
        case QNetworkInterface::Phonet:
            return InterfaceType::Phonet;
        case QNetworkInterface::Ieee802154:
            return InterfaceType::Ieee802154;
        case QNetworkInterface::SixLoWPAN:
            return InterfaceType::SixLoWPAN;
        case QNetworkInterface::Ieee80216:
            return InterfaceType::Ieee80216;
        case QNetworkInterface::Ieee1394:
            return InterfaceType::Ieee1394;
        case QNetworkInterface::Virtual:
            return InterfaceType::Virtual;
        default:
            return InterfaceType::Unknown;
    }
}
} // namespace

namespace cf {

const char* interfaceTypeName(InterfaceType t) {
    switch (t) {
        case InterfaceType::Loopback:
            return "Loopback";
        case InterfaceType::Ethernet:
            return "Ethernet";
        case InterfaceType::Wifi:
            return "WiFi";
        case InterfaceType::Ppp:
            return "PPP";
        case InterfaceType::Slip:
            return "SLIP";
        case InterfaceType::CanBus:
            return "CAN Bus";
        case InterfaceType::Phonet:
            return "Phonet";
        case InterfaceType::Ieee802154:
            return "IEEE 802.15.4";
        case InterfaceType::SixLoWPAN:
            return "6LoWPAN";
        case InterfaceType::Ieee80216:
            return "IEEE 802.16 (WiMAX)";
        case InterfaceType::Ieee1394:
            return "IEEE 1394 (FireWire)";
        case InterfaceType::Virtual:
            return "Virtual";
        default:
            return "Unknown";
    }
}

std::string IpAddress::toString() const {
    using namespace cf;
    if (protocol == IpProtocol::IPv4) {
        char buf[16];
        std::snprintf(buf, sizeof(buf), "%u.%u.%u.%u", bytes[0], bytes[1], bytes[2], bytes[3]);
        return buf;
    }
    if (protocol == IpProtocol::IPv6) {
        // 完整展开（不压缩 ::），方便嵌入式场景日志解析
        char buf[40];
        std::snprintf(buf, sizeof(buf),
                      "%02x%02x:%02x%02x:%02x%02x:%02x%02x:"
                      "%02x%02x:%02x%02x:%02x%02x:%02x%02x",
                      bytes[0], bytes[1], bytes[2], bytes[3], bytes[4], bytes[5], bytes[6],
                      bytes[7], bytes[8], bytes[9], bytes[10], bytes[11], bytes[12], bytes[13],
                      bytes[14], bytes[15]);
        return buf;
    }
    return "";
}

std::optional<IpAddress> InterfaceInfo::firstIPv4() const noexcept {
    for (const auto& e : addresses)
        if (e.ip.isIPv4())
            return e.ip;
    return std::nullopt;
}

std::optional<IpAddress> InterfaceInfo::firstIPv6() const noexcept {
    for (const auto& e : addresses)
        if (e.ip.isIPv6())
            return e.ip;
    return std::nullopt;
}

// ════════════════════════════════════════════
//  内部 builder 函数（全部 static，不暴露）
// ════════════════════════════════════════════

static AddressEntry buildAddressEntry(const QNetworkAddressEntry& src) {
    AddressEntry e;
    e.ip = fromQHostAddress(src.ip());
    e.netmask = fromQHostAddress(src.netmask());
    e.broadcast = fromQHostAddress(src.broadcast());
    e.prefixLength = src.prefixLength();
    e.isPermanent = src.isPermanent();
    e.isLifetimeKnown = src.isLifetimeKnown();

    switch (src.dnsEligibility()) {
        case QNetworkAddressEntry::DnsEligible:
            e.dnsEligibility = DnsEligibility::Eligible;
            break;
        case QNetworkAddressEntry::DnsIneligible:
            e.dnsEligibility = DnsEligibility::Ineligible;
            break;
        default:
            e.dnsEligibility = DnsEligibility::Unknown;
            break;
    }
    return e;
}

static InterfaceFlags buildFlags(QNetworkInterface::InterfaceFlags qf) {
    InterfaceFlags f;
    if (qf & QNetworkInterface::IsUp)
        f.value |= InterfaceFlags::kIsUp;
    if (qf & QNetworkInterface::IsRunning)
        f.value |= InterfaceFlags::kIsRunning;
    if (qf & QNetworkInterface::CanBroadcast)
        f.value |= InterfaceFlags::kCanBroadcast;
    if (qf & QNetworkInterface::IsLoopBack)
        f.value |= InterfaceFlags::kIsLoopBack;
    if (qf & QNetworkInterface::IsPointToPoint)
        f.value |= InterfaceFlags::kIsPointToPoint;
    if (qf & QNetworkInterface::CanMulticast)
        f.value |= InterfaceFlags::kCanMulticast;
    return f;
}

static InterfaceInfo buildInterfaceInfo(const QNetworkInterface& src) {
    InterfaceInfo info;
    info.name = src.humanReadableName().toStdString();
    info.mac = src.hardwareAddress().toStdString();
    info.mtu = src.maximumTransmissionUnit();
    info.type = mapType(src.type());
    info.flags = buildFlags(src.flags());

    const auto entries = src.addressEntries();
    info.addresses.reserve(static_cast<size_t>(entries.size()));
    for (const auto& e : entries)
        info.addresses.push_back(buildAddressEntry(e));

    return info;
}

static NetworkStatus buildNetworkStatus() {
    NetworkStatus s;

    if (!QNetworkInformation::loadDefaultBackend())
        return s;

    auto* ni = QNetworkInformation::instance();
    if (!ni)
        return s;

    s.backendAvailable = true;
    switch (ni->reachability()) {
        case QNetworkInformation::Reachability::Disconnected:
            s.reachability = Reachability::Disconnected;
            break;
        case QNetworkInformation::Reachability::Local:
            s.reachability = Reachability::Local;
            break;
        case QNetworkInformation::Reachability::Site:
            s.reachability = Reachability::Site;
            break;
        case QNetworkInformation::Reachability::Online:
            s.reachability = Reachability::Online;
            break;
        default:
            s.reachability = Reachability::Unknown;
            break;
    }
    const auto features = ni->supportedFeatures();

    if (features.testFlag(QNetworkInformation::Feature::CaptivePortal))
        s.isBehindCaptivePortal = ni->isBehindCaptivePortal();

    if (features.testFlag(QNetworkInformation::Feature::Metered))
        s.isMetered = ni->isMetered();

    if (features.testFlag(QNetworkInformation::Feature::TransportMedium)) {
        switch (ni->transportMedium()) {
            case QNetworkInformation::TransportMedium::Ethernet:
                s.transportMedium = TransportMedium::Ethernet;
                break;
            case QNetworkInformation::TransportMedium::Cellular:
                s.transportMedium = TransportMedium::Cellular;
                break;
            case QNetworkInformation::TransportMedium::WiFi:
                s.transportMedium = TransportMedium::WiFi;
                break;
            case QNetworkInformation::TransportMedium::Bluetooth:
                s.transportMedium = TransportMedium::Bluetooth;
                break;
            default:
                s.transportMedium = TransportMedium::Unknown;
                break;
        }
    }

    return s;
}

aex::expected<NetworkInfo, NetworkQueryError> getNetworkInfo() noexcept {
    NetworkInfo result;

    try {
        const auto ifaces = QNetworkInterface::allInterfaces();
        result.interfaces.reserve(static_cast<size_t>(ifaces.size()));
        for (const auto& iface : ifaces)
            result.interfaces.push_back(buildInterfaceInfo(iface));
        result.status = buildNetworkStatus();
    } catch (...) {
        // noexcepts, as if any exception happens shoud not
        // causes crashes
    }

    return result;
}

} // namespace cf
