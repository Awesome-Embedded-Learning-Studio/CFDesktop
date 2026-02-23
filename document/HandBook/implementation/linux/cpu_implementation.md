# Linux 平台 CPU 信息查询实现

## 简介

本文档描述了 CFDesktop CPU 模块在 Linux 平台上的实现细节。Linux 实现主要通过解析 `/proc` 和 `/sys` 伪文件系统来获取 CPU 信息。

## 实现文件

```
base/system/cpu/private/linux_impl/
├── cpu_info.h/cpp      # 基础信息实现
├── cpu_profile.h/cpp   # 性能信息实现
├── cpu_bonus.h/cpp     # 扩展信息实现
└── cpu_features.h/cpp  # 特性检测实现
```

## 基础信息实现 (cpu_info.cpp)

### 数据来源

| 信息 | 文件路径 | 解析方式 |
|------|----------|----------|
| 型号 | /proc/cpuinfo | model name 字段 |
| 制造商 | /proc/cpuinfo | vendor_id / CPU implementer 字段 |
| 架构 | uname() | utsname.machine |

### 实现要点

```cpp
cf::expected<void, cf::CPUInfoErrorType> query_cpu_basic_info(cf::CPUInfoHost& hostInfo) {
    // 1. 打开 /proc/cpuinfo
    std::ifstream cpuinfo("/proc/cpuinfo");

    // 2. 逐行解析
    while (std::getline(cpuinfo, line)) {
        // 解析 model name
        const std::string_view model = cf::parse_cpuinfo_field(line_sv, "model name");
        if (!model.empty()) {
            hostInfo.model = model;
        }

        // 解析 vendor_id (x86)
        const std::string_view vendor = cf::parse_cpuinfo_field(line_sv, "vendor_id");

        // 解析 CPU implementer (ARM)
        const std::string_view implementer = cf::parse_cpuinfo_field(line_sv, "CPU implementer");
        if (auto impl_val = cf::parse_hex_uint32(implementer)) {
            const std::string_view vendor = cf::arm_implementer_to_vendor(*impl_val);
            hostInfo.manufest = vendor;
        }
    }

    // 3. 使用 uname 获取架构
    struct utsname unameInfo;
    if (uname(&unameInfo) == 0) {
        hostInfo.arch = unameInfo.machine;
    }

    return {};
}
```

## 性能信息实现 (cpu_profile.cpp)

### 数据来源

| 信息 | 文件路径 | 解析方式 |
|------|----------|----------|
| 逻辑核心 | /proc/cpuinfo | processors 数量 |
| 物理核心 | /proc/cpuinfo | cpu cores 字段 |
| 当前频率 | /sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq | 文件内容 |
| 最大频率 | /sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq | 文件内容 |
| 使用率 | /proc/stat | 计算两次采样的差值 |

### CPU 使用率计算

```cpp
// 读取 /proc/stat 的第一行
cpu  2255 34 2290 22625563 6290 127 456

// 各字段含义
// user:    用户态时间
// nice:    低优先级用户态时间
// system:  内核态时间
// idle:    空闲时间
// iowait:  I/O 等待时间
// ...

// 使用率计算
uint64_t total_time = user + nice + system + idle + ...
uint64_t idle_time = idle + iowait
float usage = 100.0 * (1.0 - (idle_time - last_idle) / (total_time - last_total))
```

## 扩展信息实现 (cpu_bonus.cpp)

### 数据来源

| 信息 | 文件路径 | 解析方式 |
|------|----------|----------|
| 特性 | /proc/cpuinfo | flags / Features 字段 |
| 缓存 | /sys/devices/system/cpu/cpu0/cache/ | 各级 cache 目录 |
| 温度 | /sys/class/thermal/thermal_zone*/temp | 传感器文件 |

### 缓存信息解析

```cpp
// /sys/devices/system/cpu/cpu0/cache/
// ├── index0/ -> L1 数据缓存
// ├── index1/ -> L1 指令缓存
// ├── index2/ -> L2 统一缓存
// └── index3/ -> L3 统一缓存

// 读取 size 文件
// /sys/devices/system/cpu/cpu0/cache/index0/size
// 输出: "32K"
```

### 温度信息解析

```cpp
// 遍历 thermal_zone 目录
for (int i = 0; i < 10; ++i) {
    std::string path = "/sys/class/thermal/thermal_zone" + std::to_string(i) + "/temp";

    auto temp = cf::read_uint32_file(path.c_str());
    if (temp.has_value()) {
        // 温度值通常是 millidegree，需要除以 1000
        uint16_t celsius = *temp / 1000;
        return celsius;
    }
}
```

## big.LITTLE 架构检测

```cpp
bool detect_big_little(cf::CPUBonusInfoHost& host) {
    // 方法1：检查 CPU 频率策略
    // 在 /sys/devices/system/cpu/ 目录下查找不同的频率范围

    // 方法2：解析 /proc/cpuinfo 中的 CPU 实现者
    // 检查是否有不同的实现者 ID

    // 简化实现：检查不同 CPU 核心的最大频率
    std::vector<uint32_t> max_frequencies;
    for (int cpu = 0; cpu < logical_cores; ++cpu) {
        std::string path = "/sys/devices/system/cpu/cpu" + std::to_string(cpu) +
                          "/cpufreq/cpuinfo_max_freq";
        auto freq = cf::read_uint32_file(path.c_str());
        if (freq.has_value()) {
            max_frequencies.push_back(*freq);
        }
    }

    // 如果有不同的最大频率，可能存在大小核
    if (has_different_values(max_frequencies)) {
        host.has_big_little = true;
        host.big_core_count = count_by_frequency(max_frequencies, /*max*/);
        host.little_core_count = count_by_frequency(max_frequencies, /*min*/);
    }
}
```

## 平台差异处理

### ARM vs x86

```cpp
// x86: 使用 "flags" 字段
// ARM: 使用 "Features" 字段

if (arch_is_arm) {
    features = parse_cpuinfo_field(line, "Features");
} else {
    features = parse_cpuinfo_field(line, "flags");
}
```

### ARM 实现者 ID 转换

```cpp
// ARM 实现者 ID 到厂商名称的映射
std::string_view arm_implementer_to_vendor(uint32_t impl_val) {
    switch (impl_val) {
        case 0x41: return "ARM";
        case 0x42: return "Broadcom";
        case 0x43: return "Cavium";
        case 0x44: return "DEC";
        case 0x49: return "Infineon";
        case 0x4D: return "Motorola/Freescale";
        case 0x4E: return "NVIDIA";
        case 0x50: return "APM";
        case 0x51: return "Qualcomm";
        case 0x56: return "Marvell";
        case 0x69: return "Intel";
        default:   return "Unknown";
    }
}
```

## 相关文档

- [Windows 平台实现](../windows/cpu_implementation.md)
- [proc_parser 工具](../../base/linux/proc_parser.md)
- [CPU 模块概述](../../api/system/cpu/overview.md)
