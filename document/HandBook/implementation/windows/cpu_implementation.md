# Windows 平台 CPU 信息查询实现

## 简介

本文档描述了 CFDesktop CPU 模块在 Windows 平台上的实现细节。Windows 实现主要通过 WMI (Windows Management Instrumentation) 和 CPUID 指令来获取 CPU 信息。

## 实现文件

```
base/system/cpu/private/win_impl/
├── cpu_info.h/cpp      # 基础信息实现
├── cpu_profile.h/cpp   # 性能信息实现
├── cpu_bonus.h/cpp     # 扩展信息实现
└── cpu_features.h/cpp  # 特性检测实现
```

## 基础信息实现 (cpu_info.cpp)

### 数据来源

| 信息 | WMI 类 | 属性 |
|------|--------|------|
| 型号 | Win32_Processor | Name |
| 制造商 | Win32_Processor | Manufacturer |
| 架构 | Win32_Processor | Architecture |

### 实现要点

```cpp
cf::expected<void, CPUInfoErrorType> query_cpu_basic_info(cf::CPUInfoHost& hostInfo) {
    return cf::COMHelper<void, CPUInfoErrorType>::RunComInterfacesMTA(
        [&hostInfo]() -> cf::expected<void, CPUInfoErrorType> {
            // 1. 创建 WMI 定位器
            IWbemLocator* pLoc = nullptr;
            CoCreateInstance(CLSID_WbemLocator, nullptr, CLSCTX_INPROC_SERVER,
                            IID_IWbemLocator, (LPVOID*)&pLoc);

            // 2. 连接到 WMI 服务
            IWbemServices* pSvc = nullptr;
            pLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), ...);

            // 3. 设置代理安全级别
            CoSetProxyBlanket(pSvc, ...);

            // 4. 查询 CPU 信息
            auto modelName = queryWMIProperty(pSvc, L"Win32_Processor", L"Name");
            if (modelName) {
                hostInfo.model = *modelName;
            }

            auto manufacturer = queryWMIProperty(pSvc, L"Win32_Processor",
                                                L"Manufacturer");
            if (manufacturer) {
                hostInfo.manufest = *manufacturer;
            }

            auto archValue = queryWMIProperty(pSvc, L"Win32_Processor",
                                             L"Architecture");
            hostInfo.arch = architectureToString(std::stoi(*archValue));

            return {};
        });
}
```

### 架构值转换

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

## WMI 辅助函数

### queryWMIProperty

```cpp
cf::expected<std::string, CPUInfoErrorType>
queryWMIProperty(IWbemServices* pSvc,
                 const std::wstring& className,
                 const std::wstring& property) {
    // 构建 WQL 查询
    std::wstringstream query;
    query << L"SELECT " << property << L" FROM " << className;

    // 执行查询
    IEnumWbemClassObject* pEnumerator = nullptr;
    pSvc->ExecQuery(_bstr_t(L"WQL"), _bstr_t(query.str().c_str()),
                   WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
                   nullptr, &pEnumerator);

    // 获取结果
    IWbemClassObject* pclsObj = nullptr;
    pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

    // 获取属性值
    VARIANT vtProp;
    pclsObj->Get(property.c_str(), 0, &vtProp, 0, 0);

    // 转换 BSTR 到 std::string
    if (vtProp.vt == VT_BSTR) {
        int len = WideCharToMultiByte(CP_UTF8, 0, vtProp.bstrVal, -1,
                                     nullptr, 0, nullptr, nullptr);
        std::string result(len - 1, '\0');
        WideCharToMultiByte(CP_UTF8, 0, vtProp.bstrVal, -1,
                           &result[0], len, nullptr, nullptr);
        return result;
    }

    VariantClear(&vtProp);
    return cf::unexpected(CPUInfoErrorType::CPU_QUERY_GENERAL_FAILED);
}
```

## 性能信息实现 (cpu_profile.cpp)

### 数据来源

| 信息 | WMI 类 / 其他 | 属性 / 方法 |
|------|--------------|-------------|
| 逻辑核心 | Win32_Processor | NumberOfLogicalProcessors |
| 物理核心 | Win32_Processor | NumberOfCores |
| 最大频率 | Win32_Processor | MaxClockSpeed |
| 当前频率 | Win32_Processor | CurrentClockSpeed |
| 使用率 | 性能计数器 | \Processor(_Total)\% Processor Time |

### CPU 使用率实现

```cpp
float get_cpu_usage() {
    // 使用 PDH (Performance Data Helper) API
    PDH_HQUERY query;
    PDH_HCOUNTER counter;

    // 打开查询
    PdhOpenQuery(nullptr, 0, &query);

    // 添加计数器
    PdhAddCounter(query,
                 L"\\Processor(_Total)\\% Processor Time",
                 0, &counter);

    // 收集数据
    PdhCollectQueryData(query);

    // 等待一秒后再次收集
    Sleep(1000);
    PdhCollectQueryData(query);

    // 获取格式化值
    PDH_FMT_COUNTERVALUE value;
    PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, nullptr, &value);

    PdhCloseQuery(query);

    return static_cast<float>(value.doubleValue);
}
```

## 特性检测实现 (cpu_features.cpp)

### CPUID 指令

```cpp
// 使用内联汇编或intrinsics执行 CPUID 指令
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
    cpuid(info, 1);  // 基本处理器信息

    if (strcmp(feature_name, "SSE") == 0)
        return (info[3] & (1 << 25)) != 0;
    if (strcmp(feature_name, "SSE2") == 0)
        return (info[3] & (1 << 26)) != 0;
    if (strcmp(feature_name, "AVX") == 0)
        return (info[2] & (1 << 28)) != 0;
    if (strcmp(feature_name, "AVX2") == 0) {
        cpuid(info, 7);
        return (info[1] & (1 << 5)) != 0;
    }

    return false;
}
```

### 扩展特性检测

```cpp
std::vector<std::string> get_cpu_features() {
    std::vector<std::string> features;
    int info[4];

    // 基本特性 (EAX=1)
    cpuid(info, 1);
    if (info[3] & (1 << 25)) features.push_back("SSE");
    if (info[3] & (1 << 26)) features.push_back("SSE2");
    if (info[2] & (1 << 0))  features.push_back("SSE3");
    if (info[2] & (1 << 9))  features.push_back("SSSE3");
    if (info[2] & (1 << 19)) features.push_back("SSE4.1");
    if (info[2] & (1 << 20)) features.push_back("SSE4.2");
    if (info[2] & (1 << 25)) features.push_back("AES");
    if (info[2] & (1 << 28)) features.push_back("AVX");

    // 扩展特性 (EAX=7)
    cpuid(info, 7);
    if (info[1] & (1 << 5))  features.push_back("AVX2");
    if (info[1] & (1 << 14)) features.push_back("BMI1");
    if (info[1] & (1 << 16)) features.push_back("AVX512F");

    return features;
}
```

## 温度查询实现

```cpp
std::optional<uint16_t> get_cpu_temperature() {
    // Windows 上温度信息来源较少
    // 可以尝试以下方法：

    // 方法1: MSAcpi_ThermalZoneTemperature
    auto temp = queryWMIProperty(L"MSAcpi_ThermalZoneTemperature",
                                L"CurrentTemperature");
    if (temp) {
        // 温度值需要转换：返回值 = 摄氏度 * 10 - 273.15
        return (std::stoull(*temp) / 10.0) - 273.15;
    }

    // 方法2: 注册表
    // HKEY_LOCAL_MACHINE\HARDWARE\DESCRIPTION\System\ ThermalInfo

    return std::nullopt;  // 大多数情况下不可用
}
```

## COM 资源管理

### COMHelper 类

```cpp
// 使用 ScopeGuard 管理 COM 资源
cf::ScopeGuard locGuard([&pLoc]() {
    if (pLoc) pLoc->Release();
});

cf::ScopeGuard svcGuard([&pSvc]() {
    if (pSvc) pSvc->Release();
});

cf::ScopeGuard classObjGuard([&pclsObj]() {
    if (pclsObj) pclsObj->Release();
});

cf::ScopeGuard variantGuard([&vtProp]() {
    VariantClear(&vtProp);
});
```

## 相关文档

- [Linux 平台实现](../linux/cpu_implementation.md)
- [CPU 模块概述](../../api/system/cpu/overview.md)
