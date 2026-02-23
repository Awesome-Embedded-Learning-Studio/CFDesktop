# CPU 扩展信息 API

## 简介

`cfcpu_bonus.h` 提供了查询 CPU 扩展信息的接口，包括 CPU 特性标志、缓存大小、big.LITTLE 架构检测和温度信息。注意，某些字段在某些平台或硬件上可能不可用。

## 头文件

```cpp
#include "system/cpu/cfcpu_bonus.h"
```

## 数据结构

### CPUBonusInfoViewError

CPU 扩展信息查询的错误类型枚举。

```cpp
enum class CPUBonusInfoViewError {
    NoError,       // 查询成功
    GeneralError   // 查询失败
};
```

### CPUBonusInfoView

CPU 扩展信息的结构。

| 字段 | 类型 | 说明 |
|------|------|------|
| features | span<const string_view> | CPU 特性标志列表 |
| cache_size | span<const uint32_t> | 缓存大小（KB），顺序为 L1、L2、L3 |
| has_big_little | bool | 是否为 big.LITTLE/DynamIQ 架构 |
| big_core_count | uint32_t | 大核数量（性能核心） |
| little_core_count | uint32_t | 小核数量（能效核心） |
| temperature | optional<uint16_t> | CPU 温度（摄氏度），可能不可用 |

## 函数

### getCPUBonusInfo

```cpp
expected<CPUBonusInfoView, CPUBonusInfoViewError> getCPUBonusInfo(bool force_refresh = false);
```

获取 CPU 扩展信息。

**参数：**
- `force_refresh`：是否强制重新查询。默认为 false，使用缓存数据。

**返回值：**
- 成功时返回 `CPUBonusInfoView`，包含扩展信息
- 失败时返回错误类型 `CPUBonusInfoViewError`

**使用示例：**

```cpp
auto result = cf::getCPUBonusInfo();

if (!result.has_value()) {
    std::cerr << "查询失败" << std::endl;
    return;
}

auto info = result.value();

// 打印 CPU 特性
std::cout << "CPU 特性: ";
for (auto feature : info.features) {
    std::cout << std::string(feature) << " ";
}
std::cout << std::endl;

// 打印缓存大小
std::cout << "缓存: ";
for (auto cache : info.cache_size) {
    std::cout << cache << "KB ";
}
std::cout << std::endl;

// big.LITTLE 信息
if (info.has_big_little) {
    std::cout << "big.LITTLE 架构: 是" << std::endl;
    std::cout << "  大核: " << info.big_core_count << std::endl;
    std::cout << "  小核: " << info.little_core_count << std::endl;
}

// 温度信息
if (info.temperature.has_value()) {
    std::cout << "温度: " << *info.temperature << "°C" << std::endl;
} else {
    std::cout << "温度: 不可用" << std::endl;
}
```

## 注意事项

1. **生命周期**：返回的 `span` 和 `string_view` 仅在内部缓存未刷新时有效。

2. **平台差异**：
   - 特性检测：不同架构的表示方式不同（如 ARM 的 "neon"，x86 的 "avx2"）
   - 温度：并非所有平台都提供温度信息
   - 缓存：某些系统可能无法获取所有级别的缓存信息

3. **big.LITTLE 检测**：
   - 主要用于 ARM 平台
   - 需要特定硬件支持
   - x86 平台通常返回 false

4. **温度信息**：
   - 返回 `std::nullopt` 表示不可用
   - 并非所有设备都有温度传感器

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
        std::cout << "不可用" << std::endl;
    }

    return 0;
}
```

## 相关文档

- [基础信息 API](./cfcpu.md)
- [性能信息 API](./cfcpu_profile.md)
- [使用示例](../../examples/cpu_info_example.md)
