# CPU 基础信息 API

## 简介

`cfcpu.h` 提供了查询 CPU 基础信息的接口，包括 CPU 型号名称、架构和制造商。

## 头文件

```cpp
#include "system/cpu/cfcpu.h"
```

## 数据结构

### CPUInfoErrorType

CPU 信息查询的错误类型枚举。

```cpp
enum class CPUInfoErrorType {
    CPU_QUERY_NOERROR,         // 查询成功
    CPU_QUERY_GENERAL_FAILED   // 查询失败
};
```

### CPUInfoView

CPU 基础信息的只读视图结构。

| 字段 | 类型 | 说明 |
|------|------|------|
| model | std::string_view | CPU 型号名称（如 "Intel Core i7"） |
| arch | std::string_view | CPU 架构（如 "x86_64", "aarch64"） |
| manufacturer | std::string_view | CPU 制造商名称 |

## 函数

### getCPUInfo

```cpp
expected<CPUInfoView, CPUInfoErrorType> getCPUInfo(bool force_refresh = false);
```

获取 CPU 基础信息。

**参数：**
- `force_refresh`：是否强制重新查询。默认为 false，使用缓存数据。

**返回值：**
- 成功时返回 `CPUInfoView`，包含 CPU 的基础信息
- 失败时返回错误类型 `CPUInfoErrorType`

**使用示例：**

```cpp
auto result = cf::getCPUInfo();

if (!result.has_value()) {
    std::cerr << "查询失败，错误码: " << (int)result.error() << std::endl;
    return;
}

auto info = result.value();
std::cout << "型号: " << std::string(info.model) << std::endl;
std::cout << "架构: " << std::string(info.arch) << std::endl;
std::cout << "制造商: " << std::string(info.manufacturer) << std::endl;
```

## 注意事项

1. **生命周期**：返回的 `string_view` 仅在内部缓存未刷新时有效。不要长期存储这些视图。

2. **线程安全**：首次调用时会进行线程安全的初始化。

3. **平台依赖**：
   - Windows：需要 WMI 服务可用
   - Linux：需要可读取 `/proc/cpuinfo` 文件

4. **强制刷新**：设置 `force_refresh = true` 会重新查询信息并更新缓存。

## 完整示例

```cpp
#include "system/cpu/cfcpu.h"
#include <iostream>

int main() {
    // 首次查询
    auto cpuInfo = cf::getCPUInfo();
    if (cpuInfo.has_value()) {
        std::cout << "=== CPU 基础信息 ===" << std::endl;
        std::cout << "型号: " << std::string(cpuInfo->model) << std::endl;
        std::cout << "架构: " << std::string(cpuInfo->arch) << std::endl;
        std::cout << "制造商: " << std::string(cpuInfo->manufacturer) << std::endl;
    } else {
        std::cerr << "无法获取 CPU 信息" << std::endl;
        return 1;
    }

    // 强制刷新查询
    auto refreshedInfo = cf::getCPUInfo(true);
    // ...

    return 0;
}
```

## 相关文档

- [性能信息 API](./cfcpu_profile.md)
- [扩展信息 API](./cfcpu_bonus.md)
- [使用示例](../../examples/cpu_info_example.md)
