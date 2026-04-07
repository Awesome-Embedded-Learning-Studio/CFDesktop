# Windows 平台内存模块实现

Windows 下的内存信息获取相对直接——大部分数据可以从单个 API 调用拿到。我们用 `GlobalMemoryStatusEx` 获取物理内存和交换空间，用 `GetProcessMemoryInfo` 获取进程内存，DIMM 信息则需要解析 SMBIOS 表。整体来说比 Linux 少踩坑，但也有一些平台特定的细节需要注意。

## 物理内存与交换空间

物理内存和交换空间都用 `GlobalMemoryStatusEx` 查询，这是 Windows 提供的一站式内存状态 API。只需要正确初始化结构体大小即可：

```cpp
#include <Windows.h>

void queryPhysicalMemory(PhysicalMemory& physical) {
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);  // 必须设置，否则 API 会失败
    GlobalMemoryStatusEx(&status);

    physical.total_bytes = status.ullTotalPhys;
    physical.available_bytes = status.ullAvailPhys;
    physical.free_bytes = status.ullAvailPhys;  // Windows: AvailPhys ~= Free
}
```

交换空间的获取方式类似：

```cpp
void querySwapMemory(SwapMemory& swap) {
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    GlobalMemoryStatusEx(&status);

    swap.total_bytes = status.ullTotalPageFile;
    swap.free_bytes = status.ullAvailPageFile;
}
```

⚠️ `dwLength` 字段必须设置为 `sizeof(MEMORYSTATUSEX)`，否则 `GlobalMemoryStatusEx` 会返回失败。这个设计很古老，但一直保留到现在。

Windows 上的 `ullAvailPhys` 和 Linux 的 `available` 概念接近，都是系统认为可用于分配的内存量。不过 Windows 没有类似 Linux 的页面缓存回收机制，所以 `AvailPhys` 基本等于真正空闲的物理内存，我们在实现里把 `free_bytes` 和 `available_bytes` 设成了相同值。

## 进程内存

进程内存使用 `GetProcessMemoryInfo` API，需要链接 `psapi.lib`。我们使用 `PROCESS_MEMORY_COUNTERS_EX` 结构来获取更详细的信息：

```cpp
#include <Windows.h>
#include <Psapi.h>

#pragma comment(lib, "psapi.lib")

void queryProcessMemory(ProcessMemory& process) {
    PROCESS_MEMORY_COUNTERS_EX pmc;
    pmc.cb = sizeof(pmc);

    if (GetProcessMemoryInfo(GetCurrentProcess(),
                             reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc),
                             sizeof(pmc))) {
        process.vm_rss_bytes = pmc.WorkingSetSize;
        process.vm_size_bytes = pmc.PagefileUsage;
        process.vm_peak_bytes = pmc.PeakPagefileUsage;
    } else {
        // 查询失败时返回零值
        process.vm_rss_bytes = 0;
        process.vm_size_bytes = 0;
        process.vm_peak_bytes = 0;
    }
}
```

字段映射关系：
| Windows 字段 | 我们的字段 | 含义 |
|-------------|-----------|------|
| `WorkingSetSize` | `vm_rss_bytes` | 驻留集大小，实际占用的物理内存 |
| `PagefileUsage` | `vm_size_bytes` | 页面文件使用量，类似虚拟内存大小 |
| `PeakPagefileUsage` | `vm_peak_bytes` | 页面文件使用的历史峰值 |

⚠️ 需要注意的是 `WorkingSetSize` 包含了进程占用的所有物理内存，包括共享库和内存映射文件。如果只想看进程私有的内存，应该看 `PrivateUsage` 字段（需要 `PROCESS_MEMORY_COUNTERS_EX`）。

## DIMM 信息

内存条信息是整个模块里最复杂的部分。Windows 通过 SMBIOS（System Management BIOS）表暴露硬件信息，我们需要用 `GetSystemFirmwareTable` API 读取原始数据，然后手动解析。

### SMBIOS 数据获取

SMBIOS 签名是 `'RSMB'`（Raw SMBIOS），第一次调用获取缓冲区大小，第二次调用获取实际数据：

```cpp
const DWORD signature = 'RSMB';  // 'RSMB' = Raw SMBIOS

// 获取缓冲区大小
DWORD bufferSize = GetSystemFirmwareTable(signature, 0, nullptr, 0);
if (bufferSize == 0) {
    return;  // SMBIOS 不可用
}

// 分配缓冲区并读取数据
std::vector<uint8_t> buffer(bufferSize);
DWORD result = GetSystemFirmwareTable(signature, 0, buffer.data(), bufferSize);
if (result != bufferSize) {
    return;  // 读取失败
}

const RawSMBIOSData* smbios = reinterpret_cast<const RawSMBIOSData*>(buffer.data());
parseSmbiosMemoryDevices(smbios->SMBIOSTableData, smbios->Length, dimms);
```

### SMBIOS 结构解析

SMBIOS 表由一系列结构组成，每个结构以类型号开头。内存设备是 Type 17，我们需要遍历整个表，找出所有 Type 17 的结构：

```cpp
#pragma pack(push, 1)

struct SMBIOSHeader {
    uint8_t  Type;
    uint8_t  Length;
    uint16_t Handle;
};

struct MemoryDevice {
    SMBIOSHeader Header;
    uint16_t PhysMemArrayHandle;
    uint16_t MemErrorInfoHandle;
    uint16_t TotalWidth;
    uint16_t DataWidth;
    uint16_t Size;              // 容量（MB），最高位表示扩展大小
    uint8_t  FormFactor;
    uint8_t  DeviceSet;
    uint8_t  DeviceLocator;     // 字符串索引
    uint8_t  BankLocator;
    uint8_t  MemoryType;        // 内存类型枚举值
    uint16_t TypeDetail;
    uint16_t Speed;             // 频率（MHz）
    uint8_t  Manufacturer;      // 字符串索引
    uint8_t  SerialNumber;      // 字符串索引
    uint8_t  AssetTag;
    uint8_t  PartNumber;        // 字符串索引
    // SMBIOS 2.8+ 还有更多字段...
};

#pragma pack(pop)
```

SMBIOS 结构后面跟着一个字符串表，字符串索引从 1 开始。0 表示没有字符串。我们需要跳过格式化段（`Length` 字节），然后读取字符串直到双 null 字节：

```cpp
const char* getSmbiosString(const uint8_t* structStart, uint8_t stringIndex,
                            const uint8_t* tableEnd) {
    if (stringIndex == 0)
        return "";

    const uint8_t* fmtEnd = structStart + structStart[1];  // Length
    const uint8_t* strStart = fmtEnd;

    // 跳到目标字符串（索引从 1 开始）
    for (uint8_t i = 1; i < stringIndex; ++i) {
        while (*strStart != 0 && strStart < tableEnd) {
            ++strStart;
        }
        if (*strStart == 0)
            ++strStart;  // 跳过 null 终止符
    }

    if (strStart >= tableEnd)
        return "";

    return reinterpret_cast<const char*>(strStart);
}
```

⚠️ SMBIOS 字符串索引是 1-based 的，这一点很容易忘。索引 0 表示没有字符串，不是第一个字符串。

### 内存类型映射

SMBIOS 定义的内存类型值很多，我们只映射常用的几种：

```cpp
enum class SMBiosMemoryType : uint8_t {
    Other = 0x01,
    Unknown = 0x02,
    // ...
    DDR2 = 0x11,
    DDR3 = 0x13,
    DDR4 = 0x14,
    LPDDR3 = 0x17,
    LPDDR4 = 0x18,
    LPDDR4_X = 0x19,
    LPDDR5 = 0x1A,
    DDR5 = 0x1B,
    // ...
};

MemoryType smbiosToMemoryType(uint8_t smbType) {
    switch (static_cast<SMBiosMemoryType>(smbType)) {
        case SMBiosMemoryType::DDR2: return MemoryType::DDR2;
        case SMBiosMemoryType::DDR3: return MemoryType::DDR3;
        case SMBiosMemoryType::DDR4: return MemoryType::DDR4;
        case SMBiosMemoryType::DDR5: return MemoryType::DDR5;
        case SMBiosMemoryType::LPDDR3: return MemoryType::LPDDR3;
        case SMBiosMemoryType::LPDDR4: return MemoryType::LPDDR4;
        case SMBiosMemoryType::LPDDR4_X: return MemoryType::LPDDR4X;
        case SMBiosMemoryType::LPDDR5: return MemoryType::LPDDR5;
        case SMBiosMemoryType::SDRAM: return MemoryType::SDRAM;
        default: return MemoryType::UNKNOWN;
    }
}
```

### 容量解析

内存容量的解析有点坑。`Size` 字段如果是 0 表示插槽为空，如果最高位是 1（0x8000）表示使用扩展大小字段：

```cpp
uint16_t sizeValue = memDev->Size;
if (sizeValue == 0 || (sizeValue & 0x8000) != 0) {
    // 插槽为空或使用扩展大小
    if (sizeValue != 0 && hdr->Length >= 0x23) {
        // 读取扩展大小（SMBIOS 3.2+）
        uint32_t extSize = *reinterpret_cast<const uint32_t*>(p + 0x1F);
        if (extSize > 0) {
            dimm.capacity_bytes = static_cast<uint64_t>(extSize) * 1024 * 1024;
        }
    }
} else {
    // 正常大小，单位是 MB
    dimm.capacity_bytes = static_cast<uint64_t>(sizeValue) * 1024 * 1024;
}
```

⚠️ 32GB 以上的内存条必须用扩展大小字段，因为 16 位的 `Size` 字段只能表示到 32767 MB（约 32GB）。

### 遍历 SMBIOS 表

完整的遍历逻辑如下：

```cpp
void parseSmbiosMemoryDevices(const uint8_t* data, uint32_t length,
                              std::vector<DimmInfo>& dimms) {
    const uint8_t* p = data;
    const uint8_t* end = data + length;

    while (p + sizeof(SMBIOSHeader) <= end) {
        const SMBIOSHeader* hdr = reinterpret_cast<const SMBIOSHeader*>(p);

        // 结束标记：Type 127, Length 4
        if (hdr->Type == 127 && hdr->Length == 4) {
            break;
        }

        // 找到下一个结构（格式化段 + 字符串段）
        const uint8_t* next = p + hdr->Length;
        while (next + 1 < end && !(next[0] == 0 && next[1] == 0)) {
            ++next;
        }
        next += 2;  // 跳过双 null 终止符

        // 处理 Type 17（Memory Device）
        if (hdr->Type == 17 && hdr->Length >= 0x15) {
            const MemoryDevice* memDev = reinterpret_cast<const MemoryDevice*>(p);

            DimmInfo dimm{};
            dimm.type = smbiosToMemoryType(memDev->MemoryType);

            // 容量、频率、字符串等解析...

            dimms.push_back(dimm);
        }

        p = next;
    }
}
```

## 平台限制

1. **SMBIOS 可用性**：某些虚拟机或精简版 Windows 可能不提供 SMBIOS 数据，这种情况下 `dimms` 列表会为空。

2. **权限要求**：`GetSystemFirmwareTable` 通常不需要管理员权限，但某些 OEM 特定的 SMBIOS 扩展可能需要。

3. **32 位进程**：在 32 位进程上，`GetSystemFirmwareTable` 可能无法访问完整的 SMBIOS 表（取决于系统配置）。

4. **Slot/Channel 信息**：Windows SMBIOS 中的 `DeviceLocator` 和 `BankLocator` 字符串格式由 OEM 决定，没有统一标准。我们目前的实现只做简单计数，没有尝试解析字符串。如果需要准确的通道/插槽信息，需要针对不同 OEM 做字符串解析。

## 相关文档

- [Memory 信息 API](../../../../api/system/memory/memory_info/)
- [Linux 平台内存实现](../../linux/memory/memory_implementation.md)
