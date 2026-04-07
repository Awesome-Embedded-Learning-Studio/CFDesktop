# CPU 扩展信息 API

`cfcpu_bonus.h` 提供了 CPU 扩展信息的查询接口——特性标志、缓存大小、big.LITTLE 架构检测，还有温度信息。这些信息不是所有平台都能获取，所以返回值大多是 `optional` 或 `span`，需要调用方检查可用性。

## 基本用法

```cpp
#include "system/cpu/cfcpu_bonus.h"

auto result = cf::getCPUBonusInfo();
if (!result.has_value()) {
    std::cerr << "查询失败" << std::endl;
    return;
}

auto info = result.value();
```

## 数据结构

`CPUBonusInfoView` 包含以下字段：

| 字段 | 类型 | 说明 |
|------|------|------|
| `features` | `span<const string_view>` | CPU 特性列表，如 "avx2"、"sse4.2" |
| `cache_size` | `span<const uint32_t>` | 缓存大小（KB），顺序为 L1、L2、L3 |
| `has_big_little` | `bool` | 是否为大小核架构 |
| `big_core_count` | `uint32_t` | 大核（性能核心）数量 |
| `little_core_count` | `uint32_t` | 小核（能效核心）数量 |
| `temperature` | `optional<uint16_t>` | CPU 温度（°C），可能不可用 |

## 特性标志

CPU 特性标志是空格分隔的字符串列表，不同架构的表示方式不同。x86 通常是 "sse"、"avx2" 这种，ARM 则是 "neon"、"aes"。

```cpp
auto bonus = cf::getCPUBonusInfo();
if (bonus.has_value()) {
    std::cout << "支持的特性:" << std::endl;
    for (auto feature : bonus->features) {
        std::cout << "  " << std::string(feature) << std::endl;
    }
}
```

⚠️ `features` 返回的是 `span<string_view>`，生命周期依赖内部缓存。如果调用 `getCPUBonusInfo(true)` 强制刷新，之前的 `span` 就会失效。

## 缓存信息

缓存大小按 L1、L2、L3 的顺序返回，单位是 KB：

```cpp
const char* levels[] = {"L1", "L2", "L3"};
for (size_t i = 0; i < bonus->cache_size.size(); ++i) {
    std::cout << levels[i] << ": " << bonus->cache_size[i] << " KB" << std::endl;
}
```

不是所有系统都能获取完整的缓存信息。某些嵌入式板子可能只有 L1 缓存数据，`cache_size` 的长度会小于 3。

## big.LITTLE 架构

ARM 的大小核架构检测主要针对移动平台。x86 平台的 hybrid 架构（如 Intel 的 P-core/E-core）目前也会返回 `true`，但核心数统计可能不准确。

```cpp
if (bonus->has_big_little) {
    std::cout << "大小核架构:" << std::endl;
    std::cout << "  性能核心: " << bonus->big_core_count << std::endl;
    std::cout << "  能效核心: " << bonus->little_core_count << std::endl;
}
```

检测原理是比较不同 CPU 核心的最大频率——频率不同的核心会被归类到不同组。这个方法不是百分之百可靠，但对大多数设备够用。

## 温度信息

CPU 温度在很多设备上都不可用。Windows 上大部分 PC 不提供温度传感器，Linux 下也需要 thermal zone 支持。

```cpp
if (bonus->temperature.has_value()) {
    std::cout << "温度: " << *bonus->temperature << "°C" << std::endl;
} else {
    std::cout << "温度: 不可用" << std::endl;
}
```

如果返回 `std::nullopt`，通常不是代码问题，而是硬件或系统限制。

## 强制刷新

默认情况下，`getCPUBonusInfo()` 使用内部缓存。传入 `true` 可以强制重新查询：

```cpp
// 使用缓存（默认）
auto info1 = cf::getCPUBonusInfo();

// 强制重新查询
auto info2 = cf::getCPUBonusInfo(true);
```

缓存策略是因为扩展信息查询相对昂贵——特别是缓存信息需要遍历 `/sys` 目录下的多个文件。

## 完整示例

```cpp
#include "system/cpu/cfcpu_bonus.h"
#include <iostream>

int main() {
    auto bonus = cf::getCPUBonusInfo();
    if (!bonus.has_value()) {
        std::cerr << "无法获取 CPU 扩展信息" << std::endl;
        return 1;
    }

    std::cout << "=== CPU 扩展信息 ===" << std::endl;

    // 特性列表
    std::cout << "\n支持的特性:" << std::endl;
    for (auto feature : bonus->features) {
        std::cout << "  - " << std::string(feature) << std::endl;
    }

    // 缓存信息
    std::cout << "\n缓存大小:" << std::endl;
    const char* levels[] = {"L1", "L2", "L3"};
    for (size_t i = 0; i < bonus->cache_size.size(); ++i) {
        std::cout << "  " << levels[i] << ": " << bonus->cache_size[i] << " KB" << std::endl;
    }

    // 大小核信息
    if (bonus->has_big_little) {
        std::cout << "\n大小核架构:" << std::endl;
        std::cout << "  性能核心: " << bonus->big_core_count << std::endl;
        std::cout << "  能效核心: " << bonus->little_core_count << std::endl;
    }

    // 温度
    std::cout << "\n温度: ";
    if (bonus->temperature.has_value()) {
        std::cout << *bonus->temperature << "°C" << std::endl;
    } else {
        std::cout << "不可用（硬件不支持）" << std::endl;
    }

    return 0;
}
```

## 相关文档

- [基础信息 API](./cfcpu.md)
- [性能信息 API](./cfcpu_profile.md)
- [使用示例](../../../examples/cpu_info_example/)
