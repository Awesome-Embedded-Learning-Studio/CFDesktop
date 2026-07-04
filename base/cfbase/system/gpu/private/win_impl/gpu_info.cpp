/**
 * @file gpu_info.cpp
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief GPU and Display Information Query Implementation for Windows
 * @version 0.1
 * @date 2026-03-21
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "gpu_info.h"

#ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#endif
#include <dxgi1_6.h>         // IDXGIFactory6 / DXGI_ADAPTER_DESC3
#include <shellscalingapi.h> // GetDpiForMonitor — link: Shcore.lib
#include <windows.h>
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "Shcore.lib")

namespace cf {

// ── GPU ──────────────────────────────────────────────────
GPUInfoHost query_gpu_info_win() {
    GPUInfoHost info;

    // DXGI 1.6：不需要 D3D 上下文，直接枚举 adapter
    IDXGIFactory6* factory = nullptr;
    if (FAILED(CreateDXGIFactory2(0, IID_PPV_ARGS(&factory)))) {
        info.name = "None";
        info.backend = "None";
        info.hasSoftware = true;
        return info;
    }

    IDXGIAdapter4* adapter = nullptr;
    // 优先枚举高性能 GPU（独显）
    HRESULT hr = factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
                                                     IID_PPV_ARGS(&adapter));

    if (FAILED(hr) || !adapter) {
        factory->Release();
        info.name = "None";
        info.backend = "None";
        info.hasSoftware = true;
        return info;
    }

    DXGI_ADAPTER_DESC3 desc = {};
    adapter->GetDesc3(&desc);

    // wchar_t → std::string（简单 ASCII 截断，通常厂商名都是 ASCII）
    char nameBuf[256] = {};
    WideCharToMultiByte(CP_UTF8, 0, desc.Description, -1, nameBuf, sizeof(nameBuf), nullptr,
                        nullptr);
    info.name = nameBuf;
    info.vendorId = desc.VendorId;
    info.deviceId = desc.DeviceId;
    info.backend = "DXGI";

    // DXGI_ADAPTER_FLAG3_SOFTWARE 表示 Microsoft Basic Render（软件）
    bool isSoftware = (desc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE) != 0;
    if (isSoftware) {
        info.hasSoftware = true;
    } else {
        // DedicatedVideoMemory > 0 → 独显（共享 UMA 架构为 0）
        info.isDiscrete = (desc.DedicatedVideoMemory > 0);
        info.isIntegrated = !info.isDiscrete;
    }

    // 驱动版本从注册表读（DXGI 不直接暴露）
    // HKLM\SYSTEM\CurrentControlSet\Control\Class\{4d36e968...}\0000\DriverVersion
    HKEY hKey = nullptr;
    const char* kClass = "SYSTEM\\CurrentControlSet\\Control\\Class\\"
                         "{4d36e968-e325-11ce-bfc1-08002be10318}\\0000";
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, kClass, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        char ver[64] = {};
        DWORD sz = sizeof(ver);
        RegQueryValueExA(hKey, "DriverVersion", nullptr, nullptr, (LPBYTE)ver, &sz);
        info.driverVersion = ver;
        RegCloseKey(hKey);
    }

    adapter->Release();
    factory->Release();
    return info;
}

// ── Display ───────────────────────────────────────────────
DisplayInfoHost query_display_info_win() {
    DisplayInfoHost d;
    HWND desk = GetDesktopWindow();
    HMONITOR mon = MonitorFromWindow(desk, MONITOR_DEFAULTTOPRIMARY);

    // 分辨率
    MONITORINFO mi = {sizeof(mi)};
    if (GetMonitorInfoA(mon, &mi)) {
        d.width = mi.rcMonitor.right - mi.rcMonitor.left;
        d.height = mi.rcMonitor.bottom - mi.rcMonitor.top;
    }

    // DPI（需要 Per-Monitor DPI Awareness，否则返回系统 DPI）
    UINT dpiX = 96, dpiY = 96;
    using Fn = HRESULT(WINAPI*)(HMONITOR, MONITOR_DPI_TYPE, UINT*, UINT*);
    if (HMODULE hLib = LoadLibraryA("Shcore.dll")) {
        if (auto fn = (Fn)GetProcAddress(hLib, "GetDpiForMonitor")) {
            fn(mon, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
        }
        FreeLibrary(hLib);
    }
    d.dpi = static_cast<double>(dpiX);
    d.devicePixelRatio = dpiX / 96.0;

    // 刷新率
    DEVMODEA dm = {};
    dm.dmSize = sizeof(dm);
    if (EnumDisplaySettingsA(nullptr, ENUM_CURRENT_SETTINGS, &dm)) {
        d.refreshRate = dm.dmDisplayFrequency;
    }

    return d;
}

} // namespace cf
