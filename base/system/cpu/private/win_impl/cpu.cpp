#include "cpu.h"
#include "../cpu_host.h"
#include "base/expected/expected.hpp"
#include "base/scope_guard/scope_guard.hpp"
#include "base/windows/co_helper.hpp"
#include <Wbemidl.h>
#include <Windows.h>
#include <comdef.h>
#include <cstddef>
#include <cstdint>
#include <pdh.h>
#include <powrprof.h>
#include <sstream>
#include <string>

#ifndef _PROCESSOR_POWER_INFORMATION_DEFINED
#    define _PROCESSOR_POWER_INFORMATION_DEFINED

typedef struct _PROCESSOR_POWER_INFORMATION {
    ULONG Number;
    ULONG MaxMhz;
    ULONG CurrentMhz;
    ULONG MhzLimit;
    ULONG MaxIdleState;
    ULONG CurrentIdleState;
} PROCESSOR_POWER_INFORMATION, *PPROCESSOR_POWER_INFORMATION;

#endif

namespace {
using namespace cf;
// Helper function to query a single WMI property
cf::expected<std::string, CPUInfoErrorType>
queryWMIProperty(IWbemServices* pSvc, const std::wstring& className, const std::wstring& property) {
    IEnumWbemClassObject* pEnumerator = nullptr;

    // Build WQL query
    std::wstringstream query;
    query << L"SELECT " << property << L" FROM " << className;

    HRESULT hres = pSvc->ExecQuery(_bstr_t(L"WQL"), _bstr_t(query.str().c_str()),
                                   WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, nullptr,
                                   &pEnumerator);

    if (FAILED(hres)) {
        return cf::unexpected(CPUInfoErrorType::CPU_QUERY_GENERAL_FAILED);
    }

    // Ensure enumerator is released
    cf::ScopeGuard enumeratorGuard([&pEnumerator]() {
        if (pEnumerator) {
            pEnumerator->Release();
        }
    });

    IWbemClassObject* pclsObj = nullptr;
    ULONG uReturn = 0;

    hres = pEnumerator->Next(static_cast<LONG>(WBEM_INFINITE), 1, &pclsObj, &uReturn);
    if (uReturn == 0 || FAILED(hres)) {
        return cf::unexpected(CPUInfoErrorType::CPU_QUERY_GENERAL_FAILED);
    }

    cf::ScopeGuard classObjGuard([&pclsObj]() {
        if (pclsObj) {
            pclsObj->Release();
        }
    });

    VARIANT vtProp;
    VariantInit(&vtProp);

    hres = pclsObj->Get(property.c_str(), 0, &vtProp, 0, 0);

    cf::ScopeGuard variantGuard([&vtProp]() { VariantClear(&vtProp); });

    if (SUCCEEDED(hres)) {
        // Handle different variant types
        std::string result;
        if (vtProp.vt == VT_BSTR) {
            // Convert BSTR to std::string
            int len =
                WideCharToMultiByte(CP_UTF8, 0, vtProp.bstrVal, -1, nullptr, 0, nullptr, nullptr);
            if (len > 0) {
                result.resize(static_cast<size_t>(len - 1)); // -1 to exclude null terminator
                WideCharToMultiByte(CP_UTF8, 0, vtProp.bstrVal, -1, &result[0], len, nullptr,
                                    nullptr);
            }
        } else if (vtProp.vt == VT_I4) {
            // Convert integer to string
            result = std::to_string(vtProp.lVal);
        } else if (vtProp.vt == VT_UI4) {
            result = std::to_string(vtProp.ulVal);
        }
        return result;
    }

    return cf::unexpected(CPUInfoErrorType::CPU_QUERY_GENERAL_FAILED);
}

// Convert Architecture value to string
std::string architectureToString(UINT16 archValue) {
    switch (archValue) {
        case 0:
            return "x86";
        case 1:
            return "MIPS";
        case 2:
            return "Alpha";
        case 3:
            return "PowerPC";
        case 5:
            return "ARM";
        case 6:
            return "ia64";
        case 9:
            return "x64";
        case 12:
            return "ARM64";
        default:
            return "Unknown";
    }
}

} // namespace

cf::expected<void, CPUInfoErrorType> query_cpu_info(CPUInfoHost& hostInfo) {
    using CpuInfoQueryExpected = cf::expected<void, CPUInfoErrorType>;

    return cf::COMHelper<void, CPUInfoErrorType>::RunComInterfacesMTA(
        [&hostInfo]() -> CpuInfoQueryExpected {
            // Get WMI locator
            IWbemLocator* pLoc = nullptr;
            HRESULT hres = CoCreateInstance(CLSID_WbemLocator, nullptr, CLSCTX_INPROC_SERVER,
                                            IID_IWbemLocator, reinterpret_cast<LPVOID*>(&pLoc));

            if (FAILED(hres)) {
                return cf::unexpected(CPUInfoErrorType::CPU_QUERY_GENERAL_FAILED);
            }

            cf::ScopeGuard locGuard([&pLoc]() {
                if (pLoc) {
                    pLoc->Release();
                }
            });

            // Connect to WMI namespace
            IWbemServices* pSvc = nullptr;
            hres =
                pLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), nullptr, nullptr, 0, 0, 0, 0, &pSvc);

            if (FAILED(hres)) {
                return cf::unexpected(CPUInfoErrorType::CPU_QUERY_GENERAL_FAILED);
            }

            cf::ScopeGuard svcGuard([&pSvc]() {
                if (pSvc) {
                    pSvc->Release();
                }
            });

            // Set proxy security level
            hres = CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
                                     RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL,
                                     EOAC_NONE);

            if (FAILED(hres)) {
                return cf::unexpected(CPUInfoErrorType::CPU_QUERY_GENERAL_FAILED);
            }

            // Query CPU information
            auto modelName = queryWMIProperty(pSvc, L"Win32_Processor", L"Name");
            if (modelName) {
                hostInfo.model = *modelName;
            }

            auto manufacturer = queryWMIProperty(pSvc, L"Win32_Processor", L"Manufacturer");
            if (manufacturer) {
                hostInfo.manufest = *manufacturer;
            }

            auto archValue = queryWMIProperty(pSvc, L"Win32_Processor", L"Architecture");
            if (archValue) {
                try {
                    UINT16 archNum = static_cast<UINT16>(std::stoi(*archValue));
                    hostInfo.arch = architectureToString(archNum);
                } catch (...) {
                    hostInfo.arch = "Unknown";
                }
            }

            // Success - hostInfo has been filled via reference
            return {};
        });
}

namespace {
float getCpuUsage() {
    PDH_HQUERY query;
    PDH_HCOUNTER counter;

    PdhOpenQuery(nullptr, 0, &query);
    PdhAddEnglishCounter(query, L"\\Processor(_Total)\\% Processor Time", 0, &counter);
    PdhCollectQueryData(query);

    Sleep(100);

    PdhCollectQueryData(query);

    PDH_FMT_COUNTERVALUE value;
    PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, nullptr, &value);

    PdhCloseQuery(query);

    return static_cast<float>(value.doubleValue);
}
} // namespace

cf::expected<cf::CPUProfileInfo, cf::CPUProfileInfoError> query_cpu_profile_info() {
    cf::CPUProfileInfo profile_info{};
    // logical cnt
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    profile_info.logical_cnt = static_cast<uint8_t>(sysInfo.dwNumberOfProcessors);

    // physical cnt
    DWORD len = 0;
    GetLogicalProcessorInformationEx(RelationProcessorCore, nullptr, &len);
    std::vector<char> buffer(len);
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX info =
        reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(buffer.data());

    if (!GetLogicalProcessorInformationEx(RelationProcessorCore, info, &len)) {
        profile_info.physical_cnt = 0;
    } else {
        uint8_t count = 0;
        char* ptr = buffer.data();
        while (ptr < buffer.data() + len) {
            PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX entry =
                reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(ptr);

            if (entry->Relationship == RelationProcessorCore)
                count++;

            ptr += entry->Size;
        }
        profile_info.physical_cnt = count;
    }

    PROCESSOR_POWER_INFORMATION power_info[64];

    ULONG retLen;
    CallNtPowerInformation(ProcessorInformation, nullptr, 0, power_info, sizeof(power_info));

    profile_info.current_frequecy = power_info[0].CurrentMhz * 1'000'000;
    profile_info.max_frequency = power_info[0].MaxMhz * 1'000'000;

    profile_info.cpu_usage_percentage = getCpuUsage();

    return profile_info;
}
