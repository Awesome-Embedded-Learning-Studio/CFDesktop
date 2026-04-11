/**
 * @file    base/include/system/network/network.h
 * @brief   Provides network interface and configuration querying.
 *
 * Defines types and functions for enumerating network interfaces, retrieving IP
 * addresses, interface flags, and general network status information.
 *
 * @author  <git:author-name or "N/A">
 * @date    <git:last-commit-date or "N/A">
 * @version <git:last-tag-or-commit or "N/A">
 * @since   N/A
 * @ingroup none
 */
#pragma once
#include "base/expected/expected.hpp"
#include "base/export.h"
#include <array>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace cf {

/**
 * @brief  Interface for platform-specific network information extensions.
 *
 * Provides an extension point for platform-specific network data that does not
 * fit into the common cross-platform structures.
 *
 * @ingroup none
 */
/* different from platforms */
struct INetworkInfoExtention {
    virtual ~INetworkInfoExtention() = default;
};

/**
 * @brief  IP protocol version identifiers.
 *
 * Distinguishes between IPv4, IPv6, and unknown protocol types.
 *
 * @ingroup none
 */
enum class IpProtocol : uint8_t {
    Unknown = 0, ///< Protocol type is unknown or unspecified.
    IPv4 = 4,    ///< Internet Protocol version 4.
    IPv6 = 6,    ///< Internet Protocol version 6.
};

/**
 * @brief  Represents an IP address with protocol type and raw bytes.
 *
 * Stores either an IPv4 or IPv6 address in a 16-byte array along with the
 * protocol type indicator.
 *
 * @ingroup none
 */
struct IpAddress {
    IpProtocol protocol = IpProtocol::Unknown; ///< IP protocol version.
    std::array<uint8_t, 16> bytes{};           ///< Raw address bytes (16-byte array).

    /**
     * @brief  Checks whether the address is null (unspecified).
     * @return True if protocol is Unknown, false otherwise.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup none
     */
    bool isNull() const noexcept { return protocol == IpProtocol::Unknown; }

    /**
     * @brief  Checks whether the address is an IPv4 address.
     * @return True if protocol is IPv4, false otherwise.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup none
     */
    bool isIPv4() const noexcept { return protocol == IpProtocol::IPv4; }

    /**
     * @brief  Checks whether the address is an IPv6 address.
     * @return True if protocol is IPv6, false otherwise.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup none
     */
    bool isIPv6() const noexcept { return protocol == IpProtocol::IPv6; }

    /**
     * @brief  Converts the IP address to a string representation.
     * @return String containing the formatted IP address.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup none
     */
    CF_BASE_EXPORT std::string toString() const;
};

/**
 * @brief  Network interface type identifiers.
 *
 * Categorizes the physical or virtual type of a network interface.
 *
 * @ingroup none
 */
enum class InterfaceType : uint16_t {
    Unknown = 0,    ///< Interface type is unknown.
    Loopback = 1,   ///< Loopback (local) interface.
    Ethernet = 2,   ///< Wired Ethernet interface.
    Wifi = 3,       ///< Wireless Wi-Fi interface.
    Ppp = 4,        ///< Point-to-Point protocol interface.
    Slip = 5,       ///< Serial Line IP interface.
    CanBus = 6,     ///< Controller Area Network bus interface.
    Phonet = 7,     ///< Phonet protocol interface.
    Ieee802154 = 8, ///< IEEE 802.15.4 wireless personal area network.
    SixLoWPAN = 9,  ///< IPv6 over Low-Power Wireless Personal Area Networks.
    Ieee80216 = 10, ///< IEEE 802.16 Worldwide Interoperability for Microwave Access.
    Ieee1394 = 11,  ///< IEEE 1394 FireWire interface.
    Virtual = 12,   ///< Virtual or emulated interface.
};

/**
 * @brief  Network interface operational flags.
 *
 * Bit flags representing the operational state and capabilities of a network
 * interface.
 *
 * @ingroup none
 */
struct InterfaceFlags {
    uint32_t value = 0; ///< Bit flag value combining all interface flags.

    static constexpr uint32_t kIsUp = 1u << 0;           ///< Interface is administratively up.
    static constexpr uint32_t kIsRunning = 1u << 1;      ///< Interface is operational.
    static constexpr uint32_t kCanBroadcast = 1u << 2;   ///< Interface supports broadcast.
    static constexpr uint32_t kIsLoopBack = 1u << 3;     ///< Interface is a loopback device.
    static constexpr uint32_t kIsPointToPoint = 1u << 4; ///< Interface is point-to-point.
    static constexpr uint32_t kCanMulticast = 1u << 5;   ///< Interface supports multicast.

    /**
     * @brief  Checks whether the interface is administratively up.
     * @return True if the interface is up, false otherwise.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup none
     */
    bool isUp() const noexcept { return (value & kIsUp) != 0; }

    /**
     * @brief  Checks whether the interface is operational (running).
     * @return True if the interface is running, false otherwise.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup none
     */
    bool isRunning() const noexcept { return (value & kIsRunning) != 0; }

    /**
     * @brief  Checks whether the interface supports broadcast.
     * @return True if broadcast is supported, false otherwise.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup none
     */
    bool canBroadcast() const noexcept { return (value & kCanBroadcast) != 0; }

    /**
     * @brief  Checks whether the interface is a loopback device.
     * @return True if this is a loopback interface, false otherwise.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup none
     */
    bool isLoopback() const noexcept { return (value & kIsLoopBack) != 0; }

    /**
     * @brief  Checks whether the interface is point-to-point.
     * @return True if the interface is point-to-point, false otherwise.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup none
     */
    bool isPointToPoint() const noexcept { return (value & kIsPointToPoint) != 0; }

    /**
     * @brief  Checks whether the interface supports multicast.
     * @return True if multicast is supported, false otherwise.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup none
     */
    bool canMulticast() const noexcept { return (value & kCanMulticast) != 0; }
};

// ─────────────────────────────────────────────
//  DNS 可达性
// ─────────────────────────────────────────────

/**
 * @brief  DNS eligibility status for an address.
 *
 * Indicates whether an address is eligible for DNS registration or queries.
 *
 * @ingroup none
 */
enum class DnsEligibility : uint8_t {
    Unknown = 0,    ///< DNS eligibility is unknown.
    Eligible = 1,   ///< Address is eligible for DNS.
    Ineligible = 2, ///< Address is not eligible for DNS.
};

// ─────────────────────────────────────────────
//  单个地址条目（一块网卡可有多个 IP）
// ─────────────────────────────────────────────

/**
 * @brief  Represents a single IP address entry on a network interface.
 *
 * Contains an IP address along with its associated network mask, broadcast
 * address, prefix length, and DNS eligibility.
 *
 * @ingroup none
 */
struct AddressEntry {
    IpAddress ip;                 ///< The IP address.
    IpAddress netmask;            ///< The network subnet mask.
    IpAddress broadcast;          ///< Broadcast address (null for IPv6).
    int prefixLength = 0;         ///< CIDR prefix length in bits.
    bool isPermanent = false;     ///< True if static, false if DHCP/temporary.
    bool isLifetimeKnown = false; ///< True if address lifetime is known.
    DnsEligibility dnsEligibility = DnsEligibility::Unknown; ///< DNS eligibility.
};

// ─────────────────────────────────────────────
//  单张网卡
// ─────────────────────────────────────────────

/**
 * @brief  Information about a single network interface.
 *
 * Contains the name, MAC address, MTU, type, flags, and a list of IP
 * addresses assigned to this interface.
 *
 * @ingroup none
 */
struct InterfaceInfo {
    std::string name;                            ///< Interface name (e.g., "eth0").
    std::string mac;                             ///< MAC address in string form.
    int mtu = 0;                                 ///< Maximum Transmission Unit in bytes.
    InterfaceType type = InterfaceType::Unknown; ///< Interface hardware type.
    InterfaceFlags flags;                        ///< Interface operational flags.
    std::vector<AddressEntry> addresses;         ///< List of IP addresses on this interface.

    /**
     * @brief  Retrieves the first IPv4 address on this interface.
     * @return Optional containing the first IPv4 address, or empty if none.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup none
     */
    CF_BASE_EXPORT std::optional<IpAddress> firstIPv4() const noexcept;

    /**
     * @brief  Retrieves the first IPv6 address on this interface.
     * @return Optional containing the first IPv6 address, or empty if none.
     * @throws None
     * @note None
     * @warning None
     * @since N/A
     * @ingroup none
     */
    CF_BASE_EXPORT std::optional<IpAddress> firstIPv6() const noexcept;
};

/**
 * @brief  Network reachability status.
 *
 * Indicates the current level of network connectivity available.
 *
 * @ingroup none
 */
enum class Reachability : uint8_t {
    Unknown = 0,      ///< Reachability status is unknown.
    Disconnected = 1, ///< No network connectivity.
    Local = 2,        ///< Only local loopback connectivity.
    Site = 3,         ///< Local network/site connectivity.
    Online = 4,       ///< Full internet connectivity.
};

/**
 * @brief  Transport medium type.
 *
 * Identifies the physical or virtual transport medium for network traffic.
 *
 * @ingroup none
 */
enum class TransportMedium : uint8_t {
    Unknown = 0,   ///< Transport medium is unknown.
    Ethernet = 1,  ///< Wired Ethernet connection.
    Cellular = 2,  ///< Cellular/mobile data connection.
    WiFi = 3,      ///< Wireless Wi-Fi connection.
    Bluetooth = 4, ///< Bluetooth connection.
};

/**
 * @brief  Overall network status information.
 *
 * Contains the current network backend availability, reachability status,
 * transport medium, metered connection status, and captive portal state.
 *
 * @ingroup none
 */
struct NetworkStatus {
    bool backendAvailable = false;                     ///< True if network backend is available.
    Reachability reachability = Reachability::Unknown; ///< Current network reachability.
    TransportMedium transportMedium = TransportMedium::Unknown; ///< Active transport medium.
    bool isMetered = false;                                     ///< True if connection is metered.
    bool isBehindCaptivePortal = false;                         ///< True if behind captive portal.
};

/**
 * @brief  Complete network information snapshot.
 *
 * Contains all network interfaces, platform-specific extensions, and overall
 * network status.
 *
 * @ingroup none
 */
struct NetworkInfo {
    std::unique_ptr<INetworkInfoExtention> extensions; ///< Platform-specific extensions.
    std::vector<InterfaceInfo> interfaces;             ///< 所有网卡（含 loopback）
    NetworkStatus status;                              ///< Overall network status.
};

/**
 * @brief  Network query result codes.
 *
 * Indicates success or failure of network information queries.
 *
 * @ingroup none
 */
enum class NetworkQueryError { OK };

/**
 * @brief  Retrieves current network information.
 *
 *  Get the screenshots of the NetworkInfo
 *
 * @return Expected containing NetworkInfo on success, or NetworkQueryError on failure.
 * @throws None
 * @note None
 * @warning None
 * @since N/A
 * @ingroup none
 */
cf::expected<NetworkInfo, NetworkQueryError> CF_BASE_EXPORT getNetworkInfo() noexcept;

/**
 * @brief  Returns the human-readable name for an interface type.
 *
 * returns type names convinients
 *
 * @param[in] t The interface type enum value.
 * @return      Pointer to a null-terminated string containing the type name.
 * @throws      None
 * @note        None
 * @warning     None
 * @since       N/A
 * @ingroup     none
 */
CF_BASE_EXPORT const char* interfaceTypeName(InterfaceType t);

} // namespace cf
