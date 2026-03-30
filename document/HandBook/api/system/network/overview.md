# Network API - 系统网络信息

Network API 提供跨平台的网络接口和配置查询功能，包括枚举网络接口、获取 IP 地址、接口标志和总体网络状态信息。

## 头文件

```cpp
#include "base/include/system/network/network.h"
```

命名空间：`cf`

## 核心 API

### getNetworkInfo()

查询当前系统的网络信息。

```cpp
cf::expected<NetworkInfo, NetworkQueryError> CF_BASE_EXPORT getNetworkInfo() noexcept;
```

- 返回 `expected<NetworkInfo, NetworkQueryError>`
- `noexcept` 保证不抛出异常
- 查询失败时返回 `NetworkQueryError`

### interfaceTypeName()

获取接口类型的人类可读名称。

```cpp
const char* interfaceTypeName(InterfaceType t);
```

### 使用示例

```cpp
#include "base/include/system/network/network.h"

auto result = cf::getNetworkInfo();
if (result) {
    auto& info = result.value();

    // 检查网络状态
    qDebug() << "Reachability:"
             << static_cast<int>(info.status.reachability);
    qDebug() << "Backend available:"
             << info.status.backendAvailable;

    // 枚举网络接口
    for (const auto& iface : info.interfaces) {
        qDebug() << "Interface:" << iface.name.c_str();
        qDebug() << "  MAC:" << iface.mac.c_str();
        qDebug() << "  MTU:" << iface.mtu;
        qDebug() << "  Type:"
                 << cf::interfaceTypeName(iface.type);
        qDebug() << "  Up:" << iface.flags.isUp();
        qDebug() << "  Running:" << iface.flags.isRunning();

        // IP 地址
        for (const auto& addr : iface.addresses) {
            qDebug() << "  Address:"
                     << addr.ip.toString().c_str()
                     << "/" << addr.prefixLength;
        }
    }
}
```

## 数据结构

### IpProtocol

IP 协议版本标识。

| 值 | 说明 |
|------|------|
| `Unknown = 0` | 未知或未指定 |
| `IPv4 = 4` | IPv4 |
| `IPv6 = 6` | IPv6 |

### IpAddress

IP 地址表示，支持 IPv4 和 IPv6。

| 字段 | 类型 | 说明 |
|------|------|------|
| `protocol` | `IpProtocol` | IP 协议版本 |
| `bytes` | `std::array<uint8_t, 16>` | 原始地址字节 |

方法：

| 方法 | 说明 |
|------|------|
| `isNull()` | 是否为空（protocol == Unknown） |
| `isIPv4()` | 是否为 IPv4 地址 |
| `isIPv6()` | 是否为 IPv6 地址 |
| `toString()` | 转换为字符串表示 |

### InterfaceType

网络接口类型标识。

| 值 | 说明 |
|------|------|
| `Unknown = 0` | 未知类型 |
| `Loopback = 1` | 本地回环接口 |
| `Ethernet = 2` | 有线以太网 |
| `Wifi = 3` | 无线 Wi-Fi |
| `Ppp = 4` | 点对点协议 |
| `Slip = 5` | 串行线路 IP |
| `CanBus = 6` | CAN 总线 |
| `Phonet = 7` | Phonet 协议 |
| `Ieee802154 = 8` | IEEE 802.15.4 WPAN |
| `SixLoWPAN = 9` | IPv6 over Low-Power WPAN |
| `Ieee80216 = 10` | IEEE 802.16 WiMAX |
| `Ieee1394 = 11` | IEEE 1394 FireWire |
| `Virtual = 12` | 虚拟/模拟接口 |

### InterfaceFlags

网络接口操作标志（位标志）。

| 常量 | 位 | 说明 |
|------|------|------|
| `kIsUp` | 0 | 接口已启用 |
| `kIsRunning` | 1 | 接口正在运行 |
| `kCanBroadcast` | 2 | 支持广播 |
| `kIsLoopBack` | 3 | 回环设备 |
| `kIsPointToPoint` | 4 | 点对点连接 |
| `kCanMulticast` | 5 | 支持多播 |

方法：

| 方法 | 说明 |
|------|------|
| `isUp()` | 接口是否已启用 |
| `isRunning()` | 接口是否正在运行 |
| `canBroadcast()` | 是否支持广播 |
| `isLoopback()` | 是否为回环设备 |
| `isPointToPoint()` | 是否为点对点 |
| `canMulticast()` | 是否支持多播 |

### DnsEligibility

DNS 可达性状态。

| 值 | 说明 |
|------|------|
| `Unknown = 0` | 未知 |
| `Eligible = 1` | 可用于 DNS |
| `Ineligible = 2` | 不可用于 DNS |

### AddressEntry

单个 IP 地址条目。

| 字段 | 类型 | 说明 |
|------|------|------|
| `ip` | `IpAddress` | IP 地址 |
| `netmask` | `IpAddress` | 子网掩码 |
| `broadcast` | `IpAddress` | 广播地址（IPv6 为空） |
| `prefixLength` | `int` | CIDR 前缀长度（位） |
| `isPermanent` | `bool` | 是否为静态地址（非 DHCP/临时） |
| `isLifetimeKnown` | `bool` | 地址生命周期是否已知 |
| `dnsEligibility` | `DnsEligibility` | DNS 资格状态 |

### InterfaceInfo

单个网络接口信息。

| 字段 | 类型 | 说明 |
|------|------|------|
| `name` | `std::string` | 接口名称（如 "eth0"） |
| `mac` | `std::string` | MAC 地址字符串 |
| `mtu` | `int` | 最大传输单元（字节） |
| `type` | `InterfaceType` | 接口硬件类型 |
| `flags` | `InterfaceFlags` | 接口操作标志 |
| `addresses` | `std::vector<AddressEntry>` | IP 地址列表 |

方法：

| 方法 | 说明 |
|------|------|
| `firstIPv4()` | 获取第一个 IPv4 地址 |
| `firstIPv6()` | 获取第一个 IPv6 地址 |

### Reachability

网络可达性状态。

| 值 | 说明 |
|------|------|
| `Unknown = 0` | 未知 |
| `Disconnected = 1` | 无网络连接 |
| `Local = 2` | 仅本地回环 |
| `Site = 3` | 本地网络/站点 |
| `Online = 4` | 完整互联网连接 |

### TransportMedium

传输介质类型。

| 值 | 说明 |
|------|------|
| `Unknown = 0` | 未知 |
| `Ethernet = 1` | 有线以太网 |
| `Cellular = 2` | 蜂窝移动数据 |
| `WiFi = 3` | 无线 Wi-Fi |
| `Bluetooth = 4` | 蓝牙 |

### NetworkStatus

总体网络状态。

| 字段 | 类型 | 说明 |
|------|------|------|
| `backendAvailable` | `bool` | 网络后端是否可用 |
| `reachability` | `Reachability` | 当前网络可达性 |
| `transportMedium` | `TransportMedium` | 活跃传输介质 |
| `isMetered` | `bool` | 是否为计量连接 |
| `isBehindCaptivePortal` | `bool` | 是否在强制门户后面 |

### NetworkInfo

完整网络信息快照。

| 字段 | 类型 | 说明 |
|------|------|------|
| `extensions` | `unique_ptr<INetworkInfoExtention>` | 平台特定扩展 |
| `interfaces` | `std::vector<InterfaceInfo>` | 所有网络接口（含回环） |
| `status` | `NetworkStatus` | 总体网络状态 |

### INetworkInfoExtention

平台特定网络信息扩展接口。提供平台特定数据的扩展点。

### NetworkQueryError

查询结果错误码。

| 值 | 说明 |
|------|------|
| `OK` | 无错误 |

## 使用场景

### 检查网络连通性

```cpp
auto result = cf::getNetworkInfo();
if (result && result->status.reachability == cf::Reachability::Online) {
    performNetworkOperation();
} else {
    showOfflineMessage();
}
```

### 查找活跃 Wi-Fi 接口

```cpp
auto result = cf::getNetworkInfo();
if (result) {
    for (const auto& iface : result->interfaces) {
        if (iface.type == cf::InterfaceType::Wifi && iface.flags.isUp()) {
            auto ipv4 = iface.firstIPv4();
            if (ipv4) {
                qDebug() << "Wi-Fi IP:" << ipv4->toString().c_str();
            }
        }
    }
}
```

### 检查计量连接

```cpp
auto result = cf::getNetworkInfo();
if (result && result->status.isMetered) {
    reduceNetworkUsage();
}
```

### 检测强制门户

```cpp
auto result = cf::getNetworkInfo();
if (result && result->status.isBehindCaptivePortal) {
    openCaptivePortalLogin();
}
```

## 相关文件

| 文件 | 说明 |
|------|------|
| `base/include/system/network/network.h` | 公开 API 头文件 |
| `base/system/network/network.cpp` | API 实现 |
| `base/system/network/CMakeLists.txt` | 构建配置 |
