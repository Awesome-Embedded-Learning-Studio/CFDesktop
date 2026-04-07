# Windows 平台实现细节

Windows 下的 CPU 信息主要通过 WMI（Windows Management Instrumentation）和 CPUID 指令获取。WMI 能拿到大部分基础信息，但查询开销较大且需要小心处理 COM 生命周期。CPU 指令则用于特性检测，这是跨平台的一致方案。

## COM 初始化

WMI 查询必须在 COM 环境中进行，我们用 `COMHelper` 封装了初始化和清理逻辑。选择 MTA（多线程公寓）而不是 STA，是因为 CPU 查询可能在不同线程执行。

```cpp
cf::expected<void, CPUInfoErrorType> query_cpu_basic_info(cf::CPUInfoHost& hostInfo) {
    return cf::COMHelper<void, CPUInfoErrorType>::RunComInterfacesMTA(
        [&hostInfo]() -> cf::expected<void, CPUInfoErrorType> {
            // WMI 查询代码
        });
}
```

这个封装确保 COM 正确初始化，线程退出时自动调用 `CoUninitialize()`，而且异常安全。

## 基础信息

CPU 型号、厂商、架构这些信息从 `Win32_Processor` 类查询。WQL 查询语法类似 SQL，但只能用于查询系统信息。

```cpp
cf::expected<void, CPUInfoErrorType> query_cpu_basic_info(cf::CPUInfoHost& hostInfo) {
    IWbemLocator* pLoc = nullptr;
    IWbemServices* pSvc = nullptr;

    // 1. 创建 WMI 定位器
    CoCreateInstance(CLSID_WbemLocator, nullptr, CLSCTX_INPROC_SERVER,
                    IID_IWbemLocator, (LPVOID*)&pLoc);

    // 2. 连接到 WMI 服务
    pLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), nullptr, nullptr,
                       nullptr, 0, nullptr, nullptr, &pSvc);

    // 3. 设置代理安全级别（必须，否则会访问被拒）
    CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE,
                     nullptr, RPC_C_AUTHN_LEVEL_CALL,
                     RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE);

    // 4. 查询 CPU 信息
    auto modelName = queryWMIProperty(pSvc, L"Win32_Processor", L"Name");
    if (modelName) hostInfo.model = *modelName;

    auto manufacturer = queryWMIProperty(pSvc, L"Win32_Processor", L"Manufacturer");
    if (manufacturer) hostInfo.manufest = *manufacturer;

    // 清理
    pSvc->Release();
    pLoc->Release();

    return {};
}
```

⚠️ `CoSetProxyBlanket()` 调用是必须的，否则查询会返回 `E_ACCESSDENIED`。这个坑踩过一次。

## 架构值转换

WMI 返回的 `Architecture` 是个数字，需要映射到字符串。Windows 的架构值定义有点奇怪——9 是 x64，12 是 ARM64——但这些都是标准值，照着映射就行。

```cpp
std::string architectureToString(UINT16 archValue) {
    switch (archValue) {
        case 0:  return "x86";
        case 1:  return "MIPS";
        case 2:  return "Alpha";
        case 3:  return "PowerPC";
        case 5:  return "ARM";
        case 6:  return "ia64";
        case 9:  return "x64";
        case 12: return "ARM64";
        default: return "Unknown";
    }
}
```

## 性能信息

核心数和最大频率可以从 `Win32_Processor` 直接拿到，但当前频率和 CPU 使用率需要其他方式。当前频率 WMI 也提供，但不太可靠；使用率则用 PDH（Performance Data Helper）API。

```cpp
float get_cpu_usage() {
    PDH_HQUERY query;
    PDH_HCOUNTER counter;

    PdhOpenQuery(nullptr, 0, &query);
    PdhAddCounter(query, L"\\Processor(_Total)\\% Processor Time", 0, &counter);

    // 第一次调用初始化计数器
    PdhCollectQueryData(query);
    Sleep(1000);  // 必须等待，否则数据无效
    PdhCollectQueryData(query);

    PDH_FMT_COUNTERVALUE value;
    PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, nullptr, &value);

    PdhCloseQuery(query);
    return static_cast<float>(value.doubleValue);
}
```

⚠️ 两次 `PdhCollectQueryData()` 之间必须有延迟，否则返回的数据是 0 或无效值。这是因为性能计数器是基于时间差计算的。

## 特性检测

CPU 特性通过 CPUID 指令检测，这是 x86 平台的标准方式。MSVC 用 `__cpuid()` intrinsic，GCC/Clang 用内联汇编。

```cpp
void cpuid(int info[4], int function_id) {
    #ifdef _MSC_VER
        __cpuid(info, function_id);
    #else
        __asm__ (
            "cpuid"
            : "=a"(info[0]), "=b"(info[1]), "=c"(info[2]), "=d"(info[3])
            : "a"(function_id)
        );
    #endif
}

bool detect_feature(const char* feature_name) {
    int info[4];
    cpuid(info, 1);  // EAX=1 返回基本特性

    if (strcmp(feature_name, "SSE") == 0)
        return (info[3] & (1 << 25)) != 0;
    if (strcmp(feature_name, "SSE2") == 0)
        return (info[3] & (1 << 26)) != 0;
    if (strcmp(feature_name, "AVX") == 0)
        return (info[2] & (1 << 28)) != 0;
    if (strcmp(feature_name, "AVX2") == 0) {
        cpuid(info, 7);  // EAX=7 返回扩展特性
        return (info[1] & (1 << 5)) != 0;
    }
    return false;
}
```

EAX=1 返回的是基本特性，EAX=7 返回的是扩展特性。不同特性位分布在不同的寄存器里，需要查 Intel 的手册确认。

## 温度信息

Windows 上温度信息比较麻烦。`MSAcpi_ThermalZoneTemperature` WMI 类理论上可以查，但大部分 PC 不支持。注册表 `HKEY_LOCAL_MACHINE\HARDWARE\DESCRIPTION\System\ThermalInfo` 也经常是空的。

```cpp
std::optional<uint16_t> get_cpu_temperature() {
    auto temp = queryWMIProperty(L"MSAcpi_ThermalZoneTemperature", L"CurrentTemperature");
    if (temp) {
        // 返回值是摄氏度 * 10 - 273.15（开尔文转摄氏度）
        return (std::stoull(*temp) / 10.0) - 273.15;
    }
    return std::nullopt;  // 大多数情况不可用
}
```

所以我们的实现里，温度信息在 Windows 上基本总是 `std::nullopt`。这不是 bug，是 Windows 硬件生态的限制。

## 资源管理

WMI 和 COM 的资源管理很繁琐，每个接口都要手动 `Release()`。用 `ScopeGuard` 可以确保即使中途出错也不会泄漏。

```cpp
IWbemLocator* pLoc = nullptr;
IWbemServices* pSvc = nullptr;
IWbemClassObject* pclsObj = nullptr;
VARIANT vtProp;

cf::ScopeGuard locGuard([&pLoc]() { if (pLoc) pLoc->Release(); });
cf::ScopeGuard svcGuard([&pSvc]() { if (pSvc) pSvc->Release(); });
cf::ScopeGuard objGuard([&pclsObj]() { if (pclsObj) pclsObj->Release(); });
cf::ScopeGuard varGuard([&vtProp]() { VariantClear(&vtProp); });

// ... 复杂的查询逻辑，无论哪里返回，资源都会被释放
```

## 相关文档

- [Linux 平台实现](../linux/cpu_implementation.md)
- [CPU 模块概述](../../../api/system/cpu/overview/)
