# proc_parser - Linux /proc 文件解析工具

## 简介

`proc_parser` 提供了一系列用于解析 Linux `/proc` 和 `/sys` 伪文件系统的工具函数。这些函数采用现代 C++17 风格，避免异常和不必要的内存分配。

## 头文件

```cpp
#include "base/linux/proc_parser.h"
```

## 函数列表

### parse_cpuinfo_field

```cpp
std::string_view parse_cpuinfo_field(std::string_view line,
                                      std::string_view field) noexcept;
```

从 `/proc/cpuinfo` 格式的行中解析指定字段。

**参数：**
- `line`：要解析的行，格式为 "key: value"
- `field`：要查找的字段名

**返回值：** 字段值的字符串视图，未找到时返回空视图

**示例：**

```cpp
std::string line = "model name\t: Intel(R) Core(TM) i7-9750H";
auto model = cf::parse_cpuinfo_field(line, "model name");
// model == "Intel(R) Core(TM) i7-9750H"
```

### trim_whitespace / ltrim_whitespace / rtrim_whitespace

```cpp
std::string_view trim_whitespace(std::string_view sv) noexcept;
std::string_view ltrim_whitespace(std::string_view sv) noexcept;
std::string_view rtrim_whitespace(std::string_view sv) noexcept;
```

移除字符串视图两端的空白字符。

**示例：**

```cpp
std::string_view sv = "  hello world  ";
auto trimmed = cf::trim_whitespace(sv);  // "hello world"
auto left_trimmed = cf::ltrim_whitespace(sv);  // "hello world  "
auto right_trimmed = cf::rtrim_whitespace(sv); // "  hello world"
```

### parse_cache_size

```cpp
std::optional<uint32_t> parse_cache_size(std::string_view size_str) noexcept;
```

解析人类可读的缓存大小字符串为 KB 单位。

**支持的格式：**
- `32K` - 32 KB
- `1M` - 1024 KB
- `2G` - 2097152 KB

**示例：**

```cpp
auto size1 = cf::parse_cache_size("32K");   // 32
auto size2 = cf::parse_cache_size("1M");    // 1024
auto size3 = cf::parse_cache_size("2G");    // 2097152
auto invalid = cf::parse_cache_size("xyz"); // std::nullopt
```

### parse_uint32

```cpp
std::optional<uint32_t> parse_uint32(std::string_view str) noexcept;
```

解析十进制字符串为 32 位无符号整数。

**示例：**

```cpp
auto value = cf::parse_uint32("4096");  // 4096
auto invalid = cf::parse_uint32("abc"); // std::nullopt
```

### parse_hex_uint32

```cpp
std::optional<uint32_t> parse_hex_uint32(std::string_view str) noexcept;
```

解析十六进制字符串为 32 位无符号整数。支持 `0x` 前缀。

**示例：**

```cpp
auto value1 = cf::parse_hex_uint32("0x41");    // 65
auto value2 = cf::parse_hex_uint32("FF");      // 255
auto invalid = cf::parse_hex_uint32("xyz");    // std::nullopt
```

### read_uint32_file

```cpp
std::optional<uint32_t> read_uint32_file(const char* path) noexcept;
```

从文件中读取单个 uint32_t 值。

**示例：**

```cpp
auto max_freq = cf::read_uint32_file(
    "/sys/devices/system/cpu/cpu0/cpufreq/max_freq"
);
if (max_freq.has_value()) {
    std::cout << "最大频率: " << *max_freq << " kHz" << std::endl;
}
```

### arm_implementer_to_vendor

```cpp
std::string_view arm_implementer_to_vendor(uint32_t impl_val) noexcept;
```

将 ARM 实现者 ID 转换为厂商名称。

**已知的实现者 ID：**

| ID | 厂商 |
|----|------|
| 0x41 | ARM |
| 0x42 | Broadcom |
| 0x43 | Cavium |
| 0x44 | DEC |
| 0x49 | Infineon |
| 0x4D | Motorola/Freescale |
| 0x4E | NVIDIA |
| 0x50 | APM |
| 0x51 | Qualcomm |
| 0x56 | Marvell |
| 0x69 | Intel |

**示例：**

```cpp
auto vendor = cf::arm_implementer_to_vendor(0x41);  // "ARM"
auto vendor2 = cf::arm_implementer_to_vendor(0x51); // "Qualcomm"
auto unknown = cf::arm_implementer_to_vendor(0xFF); // "Unknown"
```

## 使用场景

### 解析 /proc/cpuinfo

```cpp
#include "base/linux/proc_parser.h"
#include <fstream>

void parse_cpuinfo() {
    std::ifstream cpuinfo("/proc/cpuinfo");
    std::string line;

    while (std::getline(cpuinfo, line)) {
        auto model = cf::parse_cpuinfo_field(line, "model name");
        if (!model.empty()) {
            std::cout << "CPU: " << model << std::endl;
        }

        auto vendor = cf::parse_cpuinfo_field(line, "vendor_id");
        if (!vendor.empty()) {
            std::cout << "厂商: " << vendor << std::endl;
        }
    }
}
```

### 读取 CPU 频率

```cpp
void read_cpu_frequency() {
    auto min_freq = cf::read_uint32_file(
        "/sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq"
    );
    auto max_freq = cf::read_uint32_file(
        "/sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq"
    );

    if (min_freq && max_freq) {
        std::cout << "频率范围: " << *min_freq << " - " << *max_freq
                  << " kHz" << std::endl;
    }
}
```

## 注意事项

1. **生命周期**：返回的 `string_view` 仅在输入数据有效时有效

2. **错误处理**：所有函数都不会抛出异常，使用 `std::optional` 或空视图表示错误

3. **文件 I/O**：`read_uint32_file` 执行文件 I/O，可能因权限或文件不存在而失败

## 相关文档

- [基础工具类概述](../overview.md)
- [CPU 模块概述](../../api/system/cpu/overview.md)
