# CPU 性能信息 API

`cfcpu_profile.h` 查询 CPU 的性能相关信息——核心数量、频率和使用率。和基础信息不同，性能信息每次调用都会实时查询，不使用缓存。这是因为核心数虽然是静态的，但频率和使用率会随时间变化。

## 基本用法

```cpp
#include "system/cpu/cfcpu_profile.h"

auto result = cf::getCPUProfileInfo();
if (!result.has_value()) {
    std::cerr << "查询失败" << std::endl;
    return;
}

auto info = result.value();
std::cout << "逻辑核心: " << info.logical_cnt << std::endl;
std::cout << "物理核心: " << info.physical_cnt << std::endl;
std::cout << "CPU 使用率: " << info.cpu_usage_percentage << "%" << std::endl;
```

## 数据结构

`CPUProfileInfo` 包含以下字段：

| 字段 | 类型 | 说明 |
|------|------|------|
| `logical_cnt` | `uint16_t` | 逻辑 CPU 线程数 |
| `physical_cnt` | `uint16_t` | 物理 CPU 核心数 |
| `current_frequecy` | `uint32_t` | 当前频率（MHz），可能为 0 |
| `max_frequency` | `uint32_t` | 最大频率（MHz），可能为 0 |
| `cpu_usage_percentage` | `float` | CPU 使用率（0-100） |

⚠️ 注意字段名拼写：`current_frequecy` 是 `frequency` 的历史拼写错误，为了保持 API 兼容性没有修改。

## 核心数量

逻辑核心数是系统看到的线程总数（包含超线程），物理核心数是实际的 CPU 核心数：

```cpp
auto profile = cf::getCPUProfileInfo();
std::cout << "逻辑线程: " << profile->logical_cnt << std::endl;
std::cout << "物理核心: " << profile->physical_cnt << std::endl;

// 判断是否支持超线程
if (profile->logical_cnt > profile->physical_cnt) {
    std::cout << "支持超线程" << std::endl;
}
```

## 频率信息

CPU 频率在某些平台可能不可用，此时返回 0：

```cpp
auto profile = cf::getCPUProfileInfo();

if (profile->current_frequecy > 0) {
    std::cout << "当前频率: " << profile->current_frequecy << " MHz" << std::endl;
}

if (profile->max_frequency > 0) {
    std::cout << "最大频率: " << profile->max_frequency << " MHz" << std::endl;

    // 计算当前频率占比
    if (profile->current_frequecy > 0) {
        float ratio = 100.0f * profile->current_frequecy / profile->max_frequency;
        std::cout << "频率占比: " << ratio << "%" << std::endl;
    }
}
```

频率为 0 不一定是错误，可能是平台限制。某些虚拟机和嵌入式系统就不提供频率信息。

## CPU 使用率

CPU 使用率基于两次采样的时间差计算。首次调用可能返回不准确的数据：

```cpp
auto profile = cf::getCPUProfileInfo();
std::cout << "CPU 使用率: " << profile->cpu_usage_percentage << "%" << std::endl;
```

⚠️ 首次调用的使用率数据可能不准确，因为需要上一次采样的值作为基准。如果需要精确的使用率，可以连续调用两次，中间加个延迟：

```cpp
// 第一次调用初始化采样基准
cf::getCPUProfileInfo();
std::this_thread::sleep_for(std::chrono::milliseconds(100));

// 第二次调用返回准确的使用率
auto profile = cf::getCPUProfileInfo();
```

## 实时查询

`getCPUProfileInfo()` 每次都实时查询，不使用缓存。如果需要持续监控 CPU 使用率，建议控制查询频率：

```cpp
while (monitoring) {
    auto profile = cf::getCPUProfileInfo();
    if (profile.has_value()) {
        update_ui(profile->cpu_usage_percentage);
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
}
```

过于频繁的查询会增加系统开销，特别是在 Windows 上——WMI 和 PDH 查询都不是零成本的。

## 平台差异

不同平台的实现方式不同，但 API 保持一致：

- **Linux**：读取 `/proc/stat` 计算使用率，`/sys/devices/system/cpu/` 获取频率
- **Windows**：使用 PDH 性能计数器获取使用率，WMI 查询频率

这些差异对调用方透明，但可能影响返回值的精度和更新频率。

## 完整示例

```cpp
#include "system/cpu/cfcpu_profile.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    // 首次调用初始化采样基准
    cf::getCPUProfileInfo();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto profile = cf::getCPUProfileInfo();
    if (!profile.has_value()) {
        std::cerr << "无法获取 CPU 性能信息" << std::endl;
        return 1;
    }

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

    return 0;
}
```

## 相关文档

- [基础信息 API](./cfcpu.md)
- [扩展信息 API](./cfcpu_bonus.md)
- [使用示例](../../../examples/cpu_info_example/)
