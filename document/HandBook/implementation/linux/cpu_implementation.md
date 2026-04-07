# Linux 平台实现细节

Linux 下的 CPU 信息查询主要通过解析 `/proc` 和 `/sys` 伪文件系统实现。这套方案的优势是不需要额外的库依赖，缺点是文件格式比较繁琐——所以专门写了一套 `proc_parser` 工具来处理。

## 基础信息

CPU 型号、厂商这些静态信息从 `/proc/cpuinfo` 读取。这个文件的格式是固定的 `key: value` 结构，但不同架构下字段名不一样。x86 用 `vendor_id`，ARM 用 `CPU implementer`（是个十六进制 ID，需要转换）。

```cpp
cf::expected<void, cf::CPUInfoErrorType> query_cpu_basic_info(cf::CPUInfoHost& hostInfo) {
    std::ifstream cpuinfo("/proc/cpuinfo");
    std::string line;

    while (std::getline(cpuinfo, line)) {
        std::string_view line_sv = line;

        // 解析 model name 字段
        const std::string_view model = cf::parse_cpuinfo_field(line_sv, "model name");
        if (!model.empty()) {
            hostInfo.model = model;
        }

        // ARM 架构需要特殊处理
        const std::string_view implementer = cf::parse_cpuinfo_field(line_sv, "CPU implementer");
        if (auto impl_val = cf::parse_hex_uint32(implementer)) {
            hostInfo.manufest = cf::arm_implementer_to_vendor(*impl_val);
        }
    }

    // 架构信息用 uname() 获取更可靠
    struct utsname unameInfo;
    if (uname(&unameInfo) == 0) {
        hostInfo.arch = unameInfo.machine;
    }

    return {};
}
```

架构信息用 `uname()` 而不是读文件，是因为某些嵌入式板子的 `/proc/cpuinfo` 可能不包含完整的架构字段。

## 性能信息

核心数量和频率信息散落在不同地方：`/proc/cpuinfo` 有核心数，`/sys/devices/system/cpu/cpu0/cpufreq/` 里有频率。使用率需要读 `/proc/stat`，计算两次采样之间的时间差。

```cpp
// /proc/stat 第一行格式：
// cpu  user  nice  system  idle  iowait  ...
// 例如：cpu  2255 34 2290 22625563 6290 127 456

float calculate_cpu_usage() {
    static uint64_t last_total = 0, last_idle = 0;

    uint64_t user, nice, system, idle;
    FILE* stat = fopen("/proc/stat", "r");
    fscanf(stat, "cpu  %lu %lu %lu %lu", &user, &nice, &system, &idle);
    fclose(stat);

    uint64_t total = user + nice + system + idle;
    uint64_t total_delta = total - last_total;
    uint64_t idle_delta = idle - last_idle;

    last_total = total;
    last_idle = idle;

    if (total_delta == 0) return 0.0f;
    return 100.0f * (1.0f - static_cast<float>(idle_delta) / total_delta);
}
```

⚠️ 首次调用会返回不准确的数据，因为需要上次采样的值作为基准。

## 扩展信息

CPU 特性标志在 `flags`（x86）或 `Features`（ARM）字段里，是个空格分隔的列表。缓存信息从 `/sys/devices/system/cpu/cpu0/cache/` 读取，每个缓存级别一个目录。

```cpp
// /sys/devices/system/cpu/cpu0/cache/
// ├── index0/ -> L1 数据缓存
// ├── index1/ -> L1 指令缓存
// ├── index2/ -> L2 统一缓存
// └── index3/ -> L3 统一缓存

// 读取缓存大小
auto l1_size = cf::read_uint32_file("/sys/devices/system/cpu/cpu0/cache/index0/size");
// 输出通常是 "32K"，parse_cache_size() 会处理单位转换
```

温度信息从 `/sys/class/thermal/thermal_zone*/temp` 读取，但不是所有设备都有温度传感器。返回值通常是 millidegree，需要除以 1000 转成摄氏度。

## big.LITTLE 检测

ARM 的大小核架构检测是通过比较不同 CPU 核心的最大频率来实现的。如果发现不同的频率值，就假定存在大小核，然后按频率分组统计核心数。

```cpp
bool detect_big_little(cf::CPUBonusInfoHost& host) {
    std::vector<uint32_t> max_frequencies;

    // 收集每个核心的最大频率
    for (int cpu = 0; cpu < logical_cores; ++cpu) {
        std::string path = "/sys/devices/system/cpu/cpu" + std::to_string(cpu) +
                          "/cpufreq/cpuinfo_max_freq";
        auto freq = cf::read_uint32_file(path.c_str());
        if (freq.has_value()) {
            max_frequencies.push_back(*freq);
        }
    }

    // 检查是否有不同的频率
    auto min_freq = *std::min_element(max_frequencies.begin(), max_frequencies.end());
    auto max_freq = *std::max_element(max_frequencies.begin(), max_frequencies.end());

    if (min_freq != max_freq) {
        host.has_big_little = true;
        host.big_core_count = std::count(max_frequencies.begin(), max_frequencies.end(), max_freq);
        host.little_core_count = std::count(max_frequencies.begin(), max_frequencies.end(), min_freq);
        return true;
    }

    return false;
}
```

这个方法不是百分之百可靠——有些同频 CPU 也可能被误判为大小核——但在我们支持的设备上效果还可以。

## 平台差异

ARM 和 x86 在 `cpuinfo` 格式上有很多不同。ARM 的 `CPU implementer` 是个十六进制 ID，需要映射到厂商名称；x86 直接用 `vendor_id` 字符串。特性标志的字段名也不一样，x86 用 `flags`，ARM 用 `Features`。

```cpp
std::string_view arm_implementer_to_vendor(uint32_t impl_val) {
    switch (impl_val) {
        case 0x41: return "ARM";
        case 0x42: return "Broadcom";
        case 0x43: return "Cavium";
        case 0x44: return "DEC";
        case 0x4E: return "NVIDIA";
        case 0x51: return "Qualcomm";
        case 0x69: return "Intel";
        default:   return "Unknown";
    }
}
```

## 相关文档

- [Windows 平台实现](../windows/cpu_implementation.md)
- [proc_parser 工具](../../../base/linux/proc_parser/)
- [CPU 模块概述](../../../api/system/cpu/overview/)
