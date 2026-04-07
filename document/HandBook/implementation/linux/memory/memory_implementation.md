# Linux 平台实现细节

Linux 下的内存信息查询主要通过解析 `/proc/meminfo` 和 `/proc/self/status` 实现。这套方案不需要额外的库依赖，但需要理解 Linux 的内存模型——特别是缓存内存这部分，和 Windows 的概念差异还挺大的。

## /proc/meminfo 解析

`/proc/meminfo` 是 Linux 内核导出的内存统计信息伪文件，格式是每行一个 `key: value kB` 的记录。解析它只需要逐行匹配字段名然后提取数值即可。

```bash
# /proc/meminfo 典型内容
MemTotal:       16384000 kB
MemFree:          256000 kB
MemAvailable:    8192000 kB
Buffers:          128000 kB
Cached:          5120000 kB
SwapTotal:       8388608 kB
SwapFree:        8388608 kB
```

解析逻辑很简单：跳过字段名后的冒号和空格，用 `strtoul` 提取数字，然后乘以 1024 转换成字节。

```cpp
bool parseMemInfoLine(const char* line, const char* fieldName, uint64_t& outKb) {
    size_t fieldNameLen = strlen(fieldName);
    if (strncmp(line, fieldName, fieldNameLen) != 0) {
        return false;
    }

    // Skip to the number after the colon
    const char* p = line + fieldNameLen;
    while (*p == ':' || *p == ' ' || *p == '\t') {
        p++;
    }

    if (*p == '\0') {
        return false;
    }

    // Parse the number
    char* end;
    unsigned long value = strtoul(p, &end, 10);
    if (end == p) {
        return false;
    }

    outKb = static_cast<uint64_t>(value);
    return true;
}
```

这个解析函数在所有基于 `/proc/meminfo` 的查询里都是共用的，避免了重复代码。只要找到需要的字段就返回，还能提前终止循环节省时间。

## 物理内存

物理内存查询读取 `MemTotal`、`MemAvailable` 和 `MemFree` 三个字段。`MemTotal` 是系统总物理内存，`MemFree` 是完全未使用的内存，`MemAvailable` 是内核认为可用于分配的总量（包含可回收的缓存）。

```cpp
void queryPhysicalMemory(PhysicalMemory& physical) {
    FILE* fp = fopen("/proc/meminfo", "r");
    if (!fp) {
        // Set to zero on failure
        physical.total_bytes = 0;
        physical.available_bytes = 0;
        physical.free_bytes = 0;
        return;
    }

    uint64_t memTotal = 0, memAvailable = 0, memFree = 0;

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        uint64_t value;
        if (parseMemInfoLine(line, "MemTotal", value)) {
            memTotal = value;
        } else if (parseMemInfoLine(line, "MemAvailable", value)) {
            memAvailable = value;
        } else if (parseMemInfoLine(line, "MemFree", value)) {
            memFree = value;
        }

        // Break early if we have all values
        if (memTotal > 0 && memAvailable > 0 && memFree > 0) {
            break;
        }
    }

    fclose(fp);

    // Convert KB to bytes
    physical.total_bytes = memTotal * 1024;
    physical.available_bytes = memAvailable * 1024;
    physical.free_bytes = memFree * 1024;
}
```

文件打开失败时所有字段归零，而不是抛异常——这在内存查询这种场景下是合理的，因为调用方通常更希望拿到"空数据"而不是崩溃。

## 缓存内存

Linux 的缓存内存是系统内存管理的重要组成部分，也是和 Windows 差异最大的地方。`CachedMemory` 结构包含四个字段：`Buffers`（块设备缓冲区）、`Cached`（页面缓存）、`Shmem`（共享内存/tmpfs）和 `Slab`（内核对象缓存）。

```cpp
void queryCachedMemory(CachedMemory& cached) {
    FILE* fp = fopen("/proc/meminfo", "r");
    // ...

    uint64_t buffers = 0, cachedMem = 0, shmem = 0, slab = 0;

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        uint64_t value;
        if (parseMemInfoLine(line, "Buffers", value)) {
            buffers = value;
        } else if (parseMemInfoLine(line, "Cached", value)) {
            cachedMem = value;
        } else if (parseMemInfoLine(line, "Shmem", value)) {
            shmem = value;
        } else if (parseMemInfoLine(line, "Slab", value)) {
            slab = value;
        }

        if (buffers > 0 && cachedMem > 0 && shmem > 0 && slab > 0) {
            break;
        }
    }

    // Convert KB to bytes
    cached.buffers_bytes = buffers * 1024;
    cached.cached_bytes = cachedMem * 1024;
    cached.shared_bytes = shmem * 1024;
    cached.slab_bytes = slab * 1024;
}
```

这些缓存内存都是可以被回收的，所以 `MemAvailable` 已经考虑了它们。如果你的程序只关心"还能分配多少内存"，看 `MemAvailable` 就够了；如果需要分析内存占用细节（比如排查内存去哪了），这些字段会很有用。

注意 `Cached` 字段旁边还有一个 `SReclaimable`，它是 slab 可回收部分，和我们当前获取的 `Slab` 有重叠。目前实现里只取了主要的 `Cached` 值，简化了逻辑。

## 交换空间

交换空间查询读取 `SwapTotal` 和 `SwapFree`，逻辑和物理内存类似。

```cpp
void querySwapMemory(SwapMemory& swap) {
    FILE* fp = fopen("/proc/meminfo", "r");
    // ...

    uint64_t swapTotal = 0, swapFree = 0;

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        uint64_t value;
        if (parseMemInfoLine(line, "SwapTotal", value)) {
            swapTotal = value;
        } else if (parseMemInfoLine(line, "SwapFree", value)) {
            swapFree = value;
        }

        if (swapTotal > 0 && swapFree > 0) {
            break;
        }
    }

    swap.total_bytes = swapTotal * 1024;
    swap.free_bytes = swapFree * 1024;
}
```

如果系统没有配置交换空间，`SwapTotal` 会是 0，这种情况在服务器上挺常见的。

## 进程内存

当前进程的内存信息从 `/proc/self/status` 读取，格式和 `/proc/meminfo` 类似。我们关注三个字段：`VmRSS`（驻留集大小，实际占用的物理内存）、`VmSize`（虚拟内存总大小）和 `VmPeak`（虚拟内存历史峰值）。

```cpp
void queryProcessMemory(ProcessMemory& process) {
    FILE* fp = fopen("/proc/self/status", "r");
    // ...

    uint64_t vmRSS = 0, vmSize = 0, vmPeak = 0;

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        uint64_t value;
        if (parseStatusLine(line, "VmRSS", value)) {
            vmRSS = value;
        } else if (parseStatusLine(line, "VmSize", value)) {
            vmSize = value;
        } else if (parseStatusLine(line, "VmPeak", value)) {
            vmPeak = value;
        }

        if (vmRSS > 0 && vmSize > 0 && vmPeak > 0) {
            break;
        }
    }

    // Convert KB to bytes
    process.vm_rss_bytes = vmRSS * 1024;
    process.vm_size_bytes = vmSize * 1024;
    process.vm_peak_bytes = vmPeak * 1024;
}
```

`/proc/self` 是指向当前进程 `/proc/<pid>` 的符号链接，这样就不需要先获取自己的 PID 了。

`VmSize` 通常远大于 `VmRSS`，因为它包含了所有已分配的虚拟地址空间——包括尚未物理占用的部分（比如用 `mmap` 预留的、按需分配的）。关注进程实际内存占用时应该看 `VmRSS`。

## DIMM 信息

物理内存条的详细信息是最难获取的。完整的信息需要 `dmidecode` 命令，但它需要 root 权限。我们的实现会先尝试 `dmidecode`，失败后回退到 `/sys/class/dmi/id/`。

```cpp
void queryDimmInfo(std::vector<DimmInfo>& dimms) {
    dimms.clear();

    // Try dmidecode first (requires root)
    if (!queryDimmViaDmidecode(dimms)) {
        // Fall back to /sys/class/dmi/id/
        queryDimmViaSysFs(dimms);
    }
}
```

### dmidecode 解析

`dmidecode -t memory` 输出的格式是文本块，每个 "Memory Device" 是一块内存。解析逻辑是状态机式的：扫描 "Memory Device" 标记开始新条目，然后解析缩进的字段，遇到空行或下一个设备时保存当前条目。

```cpp
bool parseDmidecode(const char* output, std::vector<DimmInfo>& dimms) {
    const char* p = output;
    DimmInfo currentDimm{};
    bool inDevice = false;
    bool hasSize = false;

    while (*p != '\0') {
        // Check for "Memory Device" marker
        if (strncmp(p, "Memory Device", 13) == 0) {
            // Save previous device if valid
            if (inDevice && hasSize && currentDimm.capacity_bytes > 0) {
                dimms.push_back(currentDimm);
            }

            // Start new device
            currentDimm = DimmInfo{};
            inDevice = true;
            hasSize = false;
            // ...
        }

        if (inDevice) {
            // Parse fields like "Size: 16384 MB"
            auto extractField = [&p](const char* fieldName) -> char* {
                // Implementation extracts value after "FieldName:"
                // ...
            };

            std::unique_ptr<char[]> field;

            if ((field.reset(extractField("Size")), field)) {
                currentDimm.capacity_bytes = parseMemorySize(field.get());
                hasSize = true;
            } else if ((field.reset(extractField("Type")), field)) {
                currentDimm.type = parseMemoryType(field.get());
            } else if ((field.reset(extractField("Speed")), field)) {
                currentDimm.frequency_mhz = parseFrequency(field.get());
            }
            // ... 其他字段
        }
    }

    // Don't forget last device
    if (inDevice && hasSize && currentDimm.capacity_bytes > 0) {
        dimms.push_back(currentDimm);
    }

    return !dimms.empty();
}
```

内存类型字符串的匹配是大小写不敏感的，支持 DDR2/3/4/5、LPDDR3/4/4X/5 以及 SDRAM。如果遇到未知类型会返回 `UNKNOWN`。

大小和频率的字符串格式比较统一——"16384 MB"、"3200 MT/s" 之类的——用 `sscanf` 配合单位判断就能解析。

```cpp
uint64_t parseMemorySize(const char* sizeStr) {
    if (sizeStr == nullptr || strcmp(sizeStr, "No Module Installed") == 0) {
        return 0;
    }

    unsigned long value = 0;
    char unit[16] = {0};

    if (sscanf(sizeStr, "%lu %15s", &value, unit) == 2) {
        if (strcasecmp(unit, "MB") == 0 || strcasecmp(unit, "M") == 0) {
            return value * 1024 * 1024;
        } else if (strcasecmp(unit, "GB") == 0 || strcasecmp(unit, "G") == 0) {
            return value * 1024 * 1024 * 1024;
        }
        // ...
    }

    return 0;
}
```

插槽编号从 `Locator` 字段提取，但这个字段格式不统一——可能是 "DIMM0"、"ChannelA-DIMM0"、"Slot 1" 之类的。我们用一个简单的启发式规则：找到最后一个数字，把它当作插槽号。这不是 100% 准确，但大多数情况下能工作。

### /sys 回退方案

`/sys/class/dmi/id/` 提供的是系统级别的信息，不是每条内存的详情。这里的回退实现只能拿到板卡厂商、序列号这种信息，无法获取单条内存的容量、类型、频率。

```cpp
bool queryDimmViaSysFs(std::vector<DimmInfo>& dimms) {
    std::string manufacturer = readFile("/sys/class/dmi/id/board_vendor");
    std::string serial = readFile("/sys/class/dmi/id/board_serial");
    std::string product = readFile("/sys/class/dmi/id/board_name");

    if (manufacturer.empty() && serial.empty() && product.empty()) {
        return false;
    }

    // Create a minimal DIMM entry
    DimmInfo dimm{};
    dimm.manufacturer = manufacturer;
    dimm.serial_number = serial;
    dimm.part_number = product;
    dimm.type = MemoryType::UNKNOWN;
    dimm.capacity_bytes = 0;  // Can't get individual DIMM size from /sys
    dimm.slot = 0;

    dimms.push_back(dimm);
    return true;
}
```

这个回退方案至少能保证不会返回空列表，但 `capacity_bytes` 会是 0，调用方需要处理这种情况。

## 实际文件示例

下面是一个典型的 `/proc/meminfo` 内容，可以看到我们解析的字段在其中：

```
MemTotal:       16384000 kB
MemFree:          256000 kB
MemAvailable:    8192000 kB
Buffers:          128000 kB
Cached:          5120000 kB
SwapCached:            0 kB
Active:          6144000 kB
Inactive:        3072000 kB
Active(anon):    2560000 kB
Inactive(anon):   512000 kB
Active(file):    3584000 kB
Inactive(file):  2560000 kB
Unevictable:      25600 kB
Mlocked:              0 kB
SwapTotal:       8388608 kB
SwapFree:        8388608 kB
Dirty:               64 kB
Writeback:            0 kB
AnonPages:       2560000 kB
Mapped:           256000 kB
Shmem:            128000 kB
Slab:             512000 kB
SReclaimable:     256000 kB
SUnreclaim:       256000 kB
KernelStack:       12800 kB
PageTables:        25600 kB
NFS_Unstable:          0 kB
Bounce:                0 kB
WritebackTmp:          0 kB
CommitLimit:    16588544 kB
Committed_AS:    6144000 kB
VmallocTotal:   34359738367 kB
VmallocUsed:      256000 kB
VmallocChunk:   34359478528 kB
HardwareCorrupted:     0 kB
AnonHugePages:         0 kB
ShmemHugePages:        0 kB
ShmemPmdMapped:        0 kB
HugePages_Total:       0
HugePages_Free:        0
HugePages_Rsvd:        0
HugePages_Surp:        0
DirectMap4k:      256000 kB
DirectMap2M:     5120000 kB
DirectMap1G:    10485760 kB
```

而 `/proc/self/status` 是这样：

```
Name:   myprogram
State:  S (sleeping)
Tgid:   12345
Pid:    12345
PPid:   10000
TracerPid:      0
Uid:    1000    1000    1000    1000
Gid:    1000    1000    1000    1000
VmSize:     123456 kB
VmRSS:       67890 kB
VmPeak:     234567 kB
...
```

## 注意事项

1. **权限问题**：`dmidecode` 需要权限，普通用户运行时 DIMM 信息会回退到 `/sys` 数据，`capacity_bytes` 会是 0。

2. **MemAvailable 兼容性**：`MemAvailable` 是较新的内核（3.14+）才有的字段。老内核上需要用 `MemFree + Buffers + Cached` 估算，但我们的实现目前直接读取这个字段——如果内核不支持，返回值会是 0。如果你的程序需要支持老内核，需要做兼容处理。

3. **文件打开失败**：所有实现在文件打开失败时会返回零值而不是报错，这是有意的设计。内存查询失败通常不是致命错误，返回"空数据"让调用方决定怎么处理更合适。

## 相关文档

- [Windows 平台实现](../../windows/memory/memory_implementation.md)
- [Memory 信息 API](../../../../api/system/memory/memory_info/)
- [proc_parser 工具](../../../../base/linux/proc_parser/)
