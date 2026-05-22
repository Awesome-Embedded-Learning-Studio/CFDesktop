---
title: "procparser - Linux 文件解析工具"
description: Linux 下的硬件信息大多藏在  和  伪文件系统里。这些文件格式固定但处理起来很繁琐，而且容易出
---

# proc_parser - Linux 文件解析工具

Linux 下的硬件信息大多藏在 `/proc` 和 `/sys` 伪文件系统里。这些文件格式固定但处理起来很繁琐，而且容易出错——比如字段分隔符可能是冒号加空格，也可能是制表符。`proc_parser` 提供了一套专门处理这些格式的工具，用 `string_view` 避免拷贝，而且不抛异常。

## 解析 cpuinfo

`/proc/cpuinfo` 是 CPU 信息的核心来源，每行格式是 `key: value`。`parse_cpuinfo_field()` 提取指定字段的值：

```cpp
#include "base/linux/proc_parser.h"

std::string line = "model name\t: Intel(R) Core(TM) i7-9750H";
auto model = cf::parse_cpuinfo_field(line, "model name");
// model == "Intel(R) Core(TM) i7-9750H"

// 字段不存在时返回空视图
auto missing = cf::parse_cpuinfo_field(line, "vendor_id");
// missing.data() == nullptr
```text

实际使用时通常是逐行读取：

```cpp
std::ifstream cpuinfo("/proc/cpuinfo");
std::string line;

while (std::getline(cpuinfo, line)) {
    auto model = cf::parse_cpuinfo_field(line, "model name");
    if (!model.empty()) {
        std::cout << "CPU: " << model << std::endl;
        break;  // 找到就行，不需要继续
    }

    auto vendor = cf::parse_cpuinfo_field(line, "vendor_id");
    if (!vendor.empty()) {
        std::cout << "厂商: " << vendor << std::endl;
    }
}
```text

## 字符串去空格

这些文件里经常有多余的空格，`trim_whitespace()` 系列函数可以清理：

```cpp
std::string_view sv = "  hello world  ";
auto trimmed = cf::trim_whitespace(sv);      // "hello world"
auto left_trimmed = cf::ltrim_whitespace(sv); // "hello world  "
auto right_trimmed = cf::rtrim_whitespace(sv);// "  hello world"
```text

## 解析缓存大小

缓存大小用的是人类可读的格式——`32K`、`1M`、`2G`。`parse_cache_size()` 统一转换成 KB：

```cpp
auto size1 = cf::parse_cache_size("32K");   // 32
auto size2 = cf::parse_cache_size("1M");    // 1024
auto size3 = cf::parse_cache_size("2G");    // 2097152
auto invalid = cf::parse_cache_size("xyz"); // std::nullopt
```text

这在读取 `/sys/devices/system/cpu/cpu0/cache/index*/size` 时特别有用：

```cpp
auto l1_size = cf::read_uint32_file("/sys/devices/system/cpu/cpu0/cache/index0/size");
auto size_kb = cf::parse_cache_size(l1_size);
if (size_kb) {
    std::cout << "L1 缓存: " << *size_kb << " KB" << std::endl;
}
```text

## 解析数字

`parse_uint32()` 和 `parse_hex_uint32()` 把字符串转成数字，失败时返回 `std::nullopt` 而不是抛异常：

```cpp
auto value = cf::parse_uint32("4096");      // 4096
auto hex1 = cf::parse_hex_uint32("0x41");   // 65
auto hex2 = cf::parse_hex_uint32("FF");     // 255
auto invalid = cf::parse_uint32("abc");     // std::nullopt
```text

十六进制解析在处理 ARM 的 `CPU implementer` 字段时很常见：

```cpp
auto impl = cf::parse_cpuinfo_field(line, "CPU implementer");
if (auto impl_val = cf::parse_hex_uint32(impl)) {
    auto vendor = cf::arm_implementer_to_vendor(*impl_val);
    // impl_val = 0x41 -> vendor = "ARM"
}
```text

## 读取文件

`read_uint32_file()` 直接读取单个数字值的文件，这在 `/sys` 下很常见——很多文件就只有一个数字：

```cpp
auto max_freq = cf::read_uint32_file(
    "/sys/devices/system/cpu/cpu0/cpufreq/max_freq"
);
if (max_freq.has_value()) {
    std::cout << "最大频率: " << *max_freq << " kHz" << std::endl;
}
```text

⚠️ 这个函数会执行文件 I/O，可能因为权限或文件不存在而失败。返回 `std::nullopt` 时可以判断是文件问题还是解析失败。

## ARM 厂商映射

ARM 的 `CPU implementer` 是个十六进制 ID，需要映射到厂商名称：

```cpp
auto vendor = cf::arm_implementer_to_vendor(0x41);  // "ARM"
auto vendor2 = cf::arm_implementer_to_vendor(0x51); // "Qualcomm"
auto vendor3 = cf::arm_implementer_to_vendor(0x69); // "Intel"
auto unknown = cf::arm_implementer_to_vendor(0xFF); // "Unknown"
```text

支持的厂商 ID 包括 ARM (0x41)、Broadcom (0x42)、Qualcomm (0x51)、NVIDIA (0x4E) 等。

## 生命周期注意

所有返回 `string_view` 的函数，结果都依赖于输入数据的生命周期。不要这样做：

```cpp
std::string_view bad() {
    std::string line = "model name: Intel";
    return cf::parse_cpuinfo_field(line, "model name");  // 危险！line 会被销毁
}

std::string_view good(const std::string& line) {
    return cf::parse_cpuinfo_field(line, "model name");  // OK，调用方保证 line 有效
}
```text

## 相关文档

- [基础工具类概述](../overview.md)
- [Linux 平台实现](../../implementation/linux/cpu_implementation.md)
- [CPU 模块概述](../../api/system/cpu/overview.md)
