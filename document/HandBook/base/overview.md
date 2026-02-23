# 基础工具类概述

## 简介

CFDesktop 基础工具类模块提供了一系列跨平台的通用工具，用于支持主项目的功能实现。这些工具类包括错误处理、容器视图、资源管理和解析工具等。

## 工具类列表

| 工具类 | 头文件 | 功能描述 |
|--------|--------|----------|
| expected | base/expected/expected.hpp | 函数式错误处理，std::expected (C++23) 的 C++17 实现 |
| span | base/span/span.h | 零开销容器视图，std::span (C++20) 的 C++17 实现 |
| ScopeGuard | base/scope_guard/scope_guard.hpp | RAII 风格的资源管理 |
| CallOnceInit | base/helpers/once_init.hpp | 线程安全的懒加载初始化 |
| proc_parser | base/linux/proc_parser.h | Linux /proc 文件解析工具 |

## expected - 错误处理

`expected<T, E>` 是一个模板类，用于表示可能失败的操作的结果。与异常不同，`expected` 将错误处理显式化，使代码更加清晰。

### 基本用法

```cpp
#include "base/expected/expected.hpp"

enum class ErrorCode { InvalidInput, NotFound };

// 成功时返回值
cf::expected<int, ErrorCode> parseNumber(std::string str) {
    if (str.empty()) {
        return cf::unexpected(ErrorCode::InvalidInput);
    }
    return std::stoi(str);
}

// 使用
auto result = parseNumber("42");
if (result.has_value()) {
    std::cout << "值: " << result.value() << std::endl;
} else {
    std::cout << "错误: " << (int)result.error() << std::endl;
}
```

## span - 容器视图

`span<T>` 提供了对连续序列的非拥有视图，可以避免不必要的拷贝。支持 C 数组、std::vector 和 std::array。

### 基本用法

```cpp
#include "base/span/span.h"

std::vector<int> vec = {1, 2, 3, 4, 5};
cf::span<int> s = vec;

// 访问元素
int first = s[0];
int last = s.back();

// 子视图
auto first_three = s.first(3);
auto middle = s.subspan(1, 3);
```

## ScopeGuard - 资源管理

`ScopeGuard` 确保在作用域结束时执行清理操作，即使在发生异常的情况下也能保证资源被正确释放。

### 基本用法

```cpp
#include "base/scope_guard/scope_guard.hpp"

{
    FILE* f = fopen("data.txt", "r");
    cf::ScopeGuard guard([&f]() { fclose(f); });

    // 使用文件...
    // 离开作用域时自动关闭文件
}
```

## CallOnceInit - 懒加载初始化

`CallOnceInit<T>` 是一个线程安全的懒加载模板，适用于需要延迟初始化的资源。

### 基本用法

```cpp
#include "base/helpers/once_init.hpp"

class MyCache : public cf::CallOnceInit<Data> {
protected:
    bool init_resources() override {
        // 执行初始化
        resource.data = fetchData();
        return true;
    }
    bool force_do_reinit() override {
        // 重新初始化
        resource.data = fetchData();
        return true;
    }
};
```

## proc_parser - Linux 文件解析

提供了一系列用于解析 Linux /proc 和 /sys 文件系统的工具函数。

### 基本用法

```cpp
#include "base/linux/proc_parser.h"

// 解析 cpuinfo 字段
std::string_view vendor = cf::parse_cpuinfo_field(line, "vendor_id");

// 解析缓存大小
auto cache = cf::parse_cache_size("1M");  // 返回 1024

// 解析数字
auto value = cf::parse_uint32("4096");

// 读取文件
auto value = cf::read_uint32_file("/sys/devices/system/cpu/cpu0/cpufreq/max_freq");
```

## 相关文档

- [expected 详解](./expected.md)
- [span 详解](./span.md)
- [ScopeGuard 详解](./scope_guard.md)
- [CallOnceInit 详解](./once_init.md)
- [proc_parser 详解](./linux/proc_parser.md)
