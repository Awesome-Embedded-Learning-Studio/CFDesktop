#include "system/gpu/gpu.h"
#include "aex/expected/expected.hpp"
#include "aex/macro/system_judge.h"

// Platform-specific includes
#ifdef CFDESKTOP_OS_WINDOWS
#    include "private/win_impl/gpu_info.h"
#elif defined(CFDESKTOP_OS_LINUX)
#    include "private/linux_impl/gpu_info.h"
#endif

namespace cf {

// ═══════════════════════════════════════════════════════════
//  Internal Prober Class (Platform-Specific Implementation)
// ═══════════════════════════════════════════════════════════

class GPUEnvProber {
  public:
    static GPUInfo probeGPU();
    static DisplayInfo probeDisplay();
    static EnvironmentScore calcScore(const GPUInfo& g, const DisplayInfo& d);
};

// ═══════════════════════════════════════════════════════════
//  Platform-Specific Probes
// ═══════════════════════════════════════════════════════════

#ifdef CFDESKTOP_OS_WINDOWS

GPUInfo GPUEnvProber::probeGPU() {
    auto host = query_gpu_info_win();

    GPUInfo info;
    info.name = std::move(host.name);
    info.backend = std::move(host.backend);
    info.driverVersion = std::move(host.driverVersion);
    info.vendorId = host.vendorId;
    info.deviceId = host.deviceId;
    info.isDiscrete = host.isDiscrete;
    info.isIntegrated = host.isIntegrated;
    info.hasSoftware = host.hasSoftware;
    info.isWSL = host.isWSL;
    return info;
}

DisplayInfo GPUEnvProber::probeDisplay() {
    auto host = query_display_info_win();

    DisplayInfo info;
    info.width = host.width;
    info.height = host.height;
    info.refreshRate = host.refreshRate;
    info.dpi = host.dpi;
    info.devicePixelRatio = host.devicePixelRatio;
    info.isVirtual = host.isVirtual;
    return info;
}

#elif defined(CFDESKTOP_OS_LINUX)

GPUInfo GPUEnvProber::probeGPU() {
    auto host = query_gpu_info_linux();

    GPUInfo info;
    info.name = std::move(host.name);
    info.backend = std::move(host.backend);
    info.driverVersion = std::move(host.driverVersion);
    info.vendorId = host.vendorId;
    info.deviceId = host.deviceId;
    info.isDiscrete = host.isDiscrete;
    info.isIntegrated = host.isIntegrated;
    info.hasSoftware = host.hasSoftware;
    info.isWSL = host.isWSL;
    return info;
}

DisplayInfo GPUEnvProber::probeDisplay() {
    auto host = query_display_info_linux();

    DisplayInfo info;
    info.width = host.width;
    info.height = host.height;
    info.refreshRate = host.refreshRate;
    info.dpi = host.dpi;
    info.devicePixelRatio = host.devicePixelRatio;
    info.isVirtual = host.isVirtual;
    return info;
}

#else

// ── Fallback for unsupported platforms ────────────────────────
GPUInfo GPUEnvProber::probeGPU() {
    GPUInfo info;
    info.name = "None (unsupported platform)";
    info.backend = "None";
    info.hasSoftware = true;
    return info;
}

DisplayInfo GPUEnvProber::probeDisplay() {
    return DisplayInfo{};
}

#endif // platform detection

// ═══════════════════════════════════════════════════════════
//  Cross-Platform Common Logic (Scoring + Entry Point)
// ═══════════════════════════════════════════════════════════

EnvironmentScore GPUEnvProber::calcScore(const GPUInfo& g, const DisplayInfo& d) {
    EnvironmentScore s;

    // ── GPU 分（满分 50）──────────────────────────────────
    if (g.hasSoftware || g.backend == "None") {
        s.gpu = 0;
    } else if (g.isDiscrete) {
        s.gpu = 50;
        if (g.vendorId == 0x8086)
            s.gpu = 38; // Intel 独显偏弱
    } else {
        // 集成 / 嵌入式：按厂商/架构给基础分
        if (g.vendorId == 0x10DE || g.vendorId == 0x1002)
            s.gpu = 28; // NVIDIA/AMD 核显
        else if (g.vendorId == 0x8086)
            s.gpu = 20; // Intel 核显
        else if (g.vendorId == 0xC0CA)
            s.gpu = 22; // Rockchip Mali（RK3588 较强）
        else if (g.vendorId == 0x1957)
            s.gpu = 10; // NXP Vivante（嵌入式）
        else
            s.gpu = 15; // 其他嵌入式
    }

    // ── Display 分（满分 50）─────────────────────────────
    long long pixels = (long long)d.width * d.height;
    int resSc = (pixels >= 3840LL * 2160)   ? 25
                : (pixels >= 2560LL * 1440) ? 20
                : (pixels >= 1920LL * 1080) ? 15
                : (pixels >= 1280LL * 720)  ? 8
                                            : 3;

    int refSc = (d.refreshRate >= 120)  ? 15
                : (d.refreshRate >= 90) ? 12
                : (d.refreshRate >= 60) ? 10
                                        : 5;

    int dpiSc = (d.dpi >= 200) ? 10 : (d.dpi >= 150) ? 8 : (d.dpi >= 96) ? 5 : 2;

    s.display = resSc + refSc + dpiSc;

    s.total = s.gpu + s.display;
    s.level = (s.total >= 75) ? "High" : (s.total >= 45) ? "Medium" : "Low";
    return s;
}

aex::expected<GpuDisplayInfo, GpuDisplayInfoError> getGpuDisplayInfo() noexcept {
    GpuDisplayInfo info;
    info.gpu = GPUEnvProber::probeGPU();
    info.display = GPUEnvProber::probeDisplay();
    info.score = GPUEnvProber::calcScore(info.gpu, info.display);
    return info;
}

} // namespace cf
