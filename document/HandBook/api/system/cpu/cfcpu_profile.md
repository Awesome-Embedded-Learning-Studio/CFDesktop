# CPU 性能信息 API

## 简介

`cfcpu_profile.h` 提供了查询 CPU 性能相关信息的接口，包括核心数量、频率和使用率。

## 头文件

```cpp
#include "system/cpu/cfcpu_profile.h"
```

## 数据结构

### CPUProfileInfoError

CPU 性能信息查询的错误类型枚举。

```cpp
enum class CPUProfileInfoError {
    CPUProfileInfoGeneralError  // 查询失败
};
```

### CPUProfileInfo

CPU 性能信息的结构。

| 字段 | 类型 | 说明 |
|------|------|------|
| logical_cnt | uint16_t | 逻辑 CPU 线程数 |
| physical_cnt | uint16_t | 物理 CPU 核心数 |
| current_frequecy | uint32_t | 当前 CPU 频率（MHz） |
| max_frequency | uint32_t | 最大 CPU 频率（MHz） |
| cpu_usage_percentage | float | CPU 使用率（0-100） |

## 函数

### getCPUProfileInfo

```cpp
expected<CPUProfileInfo, CPUProfileInfoError> getCPUProfileInfo();
```

获取 CPU 性能信息。此函数每次调用都会实时查询，不使用缓存。

**返回值：**
- 成功时返回 `CPUProfileInfo`，包含性能相关数据
- 失败时返回错误类型 `CPUProfileInfoError`

**使用示例：**

```cpp
auto result = cf::getCPUProfileInfo();

if (!result.has_value()) {
    std::cerr << "查询失败" << std::endl;
    return;
}

auto info = result.value();
std::cout << "逻辑核心: " << info.logical_cnt << std::endl;
std::cout << "物理核心: " << info.physical_cnt << std::endl;
std::cout << "当前频率: " << info.current_frequecy << " MHz" << std::endl;
std::cout << "最大频率: " << info.max_frequency << " MHz" << std::endl;
std::cout << "CPU 使用率: " << info.cpu_usage_percentage << "%" << std::endl;
```

## 注意事项

1. **实时查询**：与基础信息不同，此函数每次调用都会重新查询，不使用缓存。

2. **频率值为 0**：在某些平台上，频率信息可能不可用，此时频率字段会返回 0。

3. **CPU 使用率**：
   - 首次调用可能返回不准确的数据
   - 基于两次查询之间的时间差计算

4. **平台差异**：
   - Windows：使用 WMI 和性能计数器
   - Linux：读取 `/proc/stat` 和 `/sys/devices/system/cpu/`

## 完整示例

```cpp
#include "system/cpu/cfcpu_profile.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    auto profile = cf::getCPUProfileInfo();
    if (profile.has_value()) {
        std::cout << "=== CPU 性能信息 ===" << std::endl;
        std::cout << "逻辑线程数: " << profile->logical_cnt << std::endl;
        std::cout << "物理核心数: " << profile->physical_cnt << std::endl;

        if (profile->current_frequecy > 0) {
            std::cout << "当前频率: " << profile->current_frequecy << " MHz" << std::endl;
        }
        if (profile->max_frequency > 0) {
            std::cout << "最大频率: " << profile->max_frequency << " MHz" << std::endl;
        }

        std::cout << "CPU 使用率: " << profile->cpu_usage_percentage << "%" << std::endl;
    }

    return 0;
}
```

## 相关文档

- [基础信息 API](./cfcpu.md)
- [扩展信息 API](./cfcpu_bonus.md)
- [使用示例](../../examples/cpu_info_example.md)
