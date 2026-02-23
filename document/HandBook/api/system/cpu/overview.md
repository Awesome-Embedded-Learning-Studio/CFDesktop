# CPU 模块概述

## 简介

CFDesktop CPU 模块提供跨平台的 CPU 信息查询功能，支持 Windows 和 Linux 两大平台。该模块采用懒加载设计，首次调用时缓存查询结果，后续调用可直接返回缓存数据。

## 功能特性

- **基础信息查询**：获取 CPU 型号、架构、制造商
- **性能信息查询**：获取核心数、频率、使用率
- **扩展信息查询**：获取 CPU 特性、缓存、大小核架构、温度

## 模块结构

```
base/include/system/cpu/
├── cfcpu.h           # 基础信息接口
├── cfcpu_profile.h   # 性能信息接口
└── cfcpu_bonus.h     # 扩展信息接口

base/system/cpu/
├── cfcpu.cpp         # 基础信息实现
├── cfcpu_profile.cpp # 性能信息实现
├── cfcpu_bonus.cpp   # 扩展信息实现
└── private/
    ├── cpu_host.h    # 内部数据结构
    ├── win_impl/     # Windows 平台实现
    └── linux_impl/   # Linux 平台实现
```

## 设计理念

### 1. 跨平台抽象

通过编译时宏选择平台特定实现，公共 API 保持一致：

```cpp
#ifdef CFDESKTOP_OS_WINDOWS
    // Windows 实现
#elif defined(CFDESKTOP_OS_LINUX)
    // Linux 实现
#endif
```

### 2. 懒加载缓存

使用 `CallOnceInit` 模板实现线程安全的懒加载：

```cpp
class CPUHostInfoIniter : public cf::CallOnceInit<cf::CPUInfoHost> {
protected:
    bool init_resources() override {
        // 执行实际的 CPU 信息查询
    }
};
```

### 3. 类型安全的错误处理

使用 `expected<T, E>` 替代异常，提供更清晰的错误处理路径：

```cpp
auto result = getCPUInfo();
if (!result.has_value()) {
    // 处理错误
    auto error = result.error();
}
// 使用结果
auto info = result.value();
```

### 4. 零拷贝视图

使用 `string_view` 和 `span` 避免不必要的内存拷贝：

```cpp
struct CPUInfoView {
    std::string_view model;      // 视图，不拥有数据
    std::string_view arch;
    std::string_view manufacturer;
};
```

## 快速开始

```cpp
#include "system/cpu/cfcpu.h"
#include "system/cpu/cfcpu_profile.h"
#include "system/cpu/cfcpu_bonus.h"
#include <iostream>

int main() {
    // 查询基础信息
    auto cpuInfo = cf::getCPUInfo();
    if (cpuInfo.has_value()) {
        std::cout << "CPU: " << cpuInfo->model << std::endl;
        std::cout << "架构: " << cpuInfo->arch << std::endl;
    }

    // 查询性能信息
    auto profile = cf::getCPUProfileInfo();
    if (profile.has_value()) {
        std::cout << "核心数: " << profile->logical_cnt << std::endl;
        std::cout << "频率: " << profile->max_frequency << " MHz" << std::endl;
    }

    // 查询扩展信息
    auto bonus = cf::getCPUBonusInfo();
    if (bonus.has_value()) {
        std::cout << "特性: ";
        for (auto f : bonus->features) {
            std::cout << f << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}
```

## 平台差异

| 功能 | Windows | Linux |
|------|---------|-------|
| 基础信息 | WMI 查询 | /proc/cpuinfo 解析 |
| 性能信息 | WMI + 性能计数器 | /proc/stat + /sys 文件系统 |
| 特性检测 | CPUID 指令 | cpuinfo Features 字段 |
| 温度 | 注册表 | /sys/class/thermal/ |

## 相关文档

- [基础信息 API](./cfcpu.md)
- [性能信息 API](./cfcpu_profile.md)
- [扩展信息 API](./cfcpu_bonus.md)
- [使用示例](../../examples/cpu_info_example.md)
