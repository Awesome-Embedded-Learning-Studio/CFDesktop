/**
 * @file gpu_info.cpp
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief GPU and Display Information Query Implementation for Linux
 * @version 0.1
 * @date 2026-03-21
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "gpu_info.h"
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <fstream>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

// libdrm headers (optional)
#if __has_include(<xf86drm.h>)
#    include <xf86drm.h>
#    define HAS_LIBDRM 1
#else
#    define HAS_LIBDRM 0
#endif

// ─────────────────────────────────────────────────────────
//  WSL2 /dev/dxg 结构体（精确匹配 WSL 内核 d3dkmthk.h）
// ─────────────────────────────────────────────────────────
namespace wsl_dxg {

struct WinLuid {
    uint32_t low_part;
    int32_t high_part;
};

// d3dkmt_adapterinfo: handle(4) + luid(8) + num_sources(4) + move_regions(4) = 20
struct AdapterInfo {
    uint32_t adapter_handle;
    WinLuid adapter_luid;
    uint32_t num_sources;
    uint32_t present_move_regions_preferred;
};
static_assert(sizeof(AdapterInfo) == 20, "AdapterInfo size mismatch");

// d3dkmt_enumadapters2: num(4) + pad(4) + ptr_as_u64(8) = 16
struct EnumAdapters2 {
    uint32_t num_adapters;
    uint32_t _reserved;
    uint64_t adapters_ptr; // pointer 存为 u64，匹配内核 __u64
};
static_assert(sizeof(EnumAdapters2) == 16, "EnumAdapters2 size mismatch");

// d3dkmt_openadapterfromluid: luid(8) + handle(4) = 12
struct OpenAdapterFromLuid {
    WinLuid adapter_luid;
    uint32_t adapter_handle;
};
static_assert(sizeof(OpenAdapterFromLuid) == 12, "OpenAdapterFromLuid size mismatch");

// d3dkmt_queryadapterinfo: handle(4)+type(4)+ptr(8)+size(4)+pad(4) = 24
struct QueryAdapterInfo {
    uint32_t adapter_handle;
    uint32_t type;
    uint64_t private_data_ptr; // void __user* 存为 u64
    uint32_t private_data_size;
    uint32_t _pad;
};
static_assert(sizeof(QueryAdapterInfo) == 24, "QueryAdapterInfo size mismatch");

// KMTQAITYPE_ADAPTERREGISTRYINFO = 1
static constexpr int kMaxPath = 260;
struct AdapterRegistryInfo {
    uint16_t adapter_string[kMaxPath];
    uint16_t bios_string[kMaxPath];
    uint16_t dac_type[kMaxPath];
    uint16_t chip_type[kMaxPath];
};
static_assert(sizeof(AdapterRegistryInfo) == kMaxPath * 2 * 4, "AdapterRegistryInfo size mismatch");

// KMTQAITYPE_ADAPTERTYPE = 15
struct AdapterType {
    union {
        struct {
            uint32_t render_supported : 1;
            uint32_t display_supported : 1;
            uint32_t software_device : 1;
            uint32_t post_device : 1;
            uint32_t hybrid_discrete : 1;
            uint32_t hybrid_integrated : 1;
            uint32_t indirect_display : 1;
            uint32_t paravirtualized : 1;
            uint32_t acg_supported : 1;
            uint32_t _reserved : 23;
        };
        uint32_t value;
    };
};
static_assert(sizeof(AdapterType) == 4, "AdapterType size mismatch");

#define DXG_BASE 0x47
#define IOCTL_ENUM_ADAPTERS2 _IOWR(DXG_BASE, 0x14, EnumAdapters2)
#define IOCTL_OPEN_FROM_LUID _IOWR(DXG_BASE, 0x0e, OpenAdapterFromLuid)
#define IOCTL_QUERY_ADAPTER _IOWR(DXG_BASE, 0x09, QueryAdapterInfo)

static inline std::string u16ToAscii(const uint16_t* src, int maxLen) {
    std::string out;
    for (int i = 0; i < maxLen && src[i]; ++i)
        out += (src[i] < 0x80) ? static_cast<char>(src[i]) : '?';
    return out;
}

} // namespace wsl_dxg

namespace {

std::string readFile(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open())
        return {};
    std::string line;
    std::getline(f, line);
    while (!line.empty() && (line.back() == '\n' || line.back() == '\r' || line.back() == ' '))
        line.pop_back();
    return line;
}

uint32_t hexToU32(const std::string& s) {
    if (s.empty())
        return 0;
    try {
        return static_cast<uint32_t>(std::stoul(s, nullptr, 16));
    } catch (...) {
        return 0;
    }
}

static bool detectWSL() {
    std::ifstream f("/proc/version");
    if (!f.is_open())
        return false;
    std::string line;
    std::getline(f, line);
    for (auto& c : line)
        c = (char)tolower((unsigned char)c);
    return line.find("microsoft") != std::string::npos || line.find("wsl") != std::string::npos;
}

// ── 从 GPU 名称字符串推断 vendorId ───────────────────────
static uint32_t inferVendorFromName(const std::string& name) {
    std::string lower = name;
    for (auto& c : lower)
        c = (char)tolower((unsigned char)c);
    if (lower.find("nvidia") != std::string::npos)
        return 0x10DE;
    if (lower.find("amd") != std::string::npos || lower.find("radeon") != std::string::npos)
        return 0x1002;
    if (lower.find("intel") != std::string::npos)
        return 0x8086;
    return 0;
}

// ── WSL GPU 探测主函数 ────────────────────────────────────
static cf::GPUInfoHost probeWSLGPU() {
    using namespace wsl_dxg;
    cf::GPUInfoHost info;
    info.backend = "D3DKMT/dxg";
    info.isWSL = true;

    int fd = open("/dev/dxg", O_RDWR);
    if (fd < 0) {
        info.name = "None (WSL GPU passthrough not enabled)";
        info.backend = "None";
        info.hasSoftware = true;
        return info;
    }

    // ① 枚举适配器
    static AdapterInfo adapterBuf[16];
    std::memset(adapterBuf, 0, sizeof(adapterBuf));
    EnumAdapters2 enumReq = {};
    enumReq.num_adapters = 16;
    enumReq.adapters_ptr = reinterpret_cast<uint64_t>(adapterBuf);

    if (ioctl(fd, IOCTL_ENUM_ADAPTERS2, &enumReq) < 0 || enumReq.num_adapters == 0) {
        close(fd);
        info.name = "None (dxg enum failed)";
        info.backend = "None";
        info.hasSoftware = true;
        return info;
    }

    AdapterInfo& ai = adapterBuf[0];

    // ② 获取可用 handle
    //    优先走 IOCTL_OPEN_FROM_LUID；
    //    失败时直接使用 EnumAdapters2 已填入的 adapter_handle。
    //    WSL 内核在 enum 阶段就已经分配了 handle，
    //    OPEN_FROM_LUID 只是重新打开，失败不意味着 handle 无效。
    uint32_t hAdapter = 0;
    {
        OpenAdapterFromLuid openReq = {};
        openReq.adapter_luid = ai.adapter_luid;
        if (ioctl(fd, IOCTL_OPEN_FROM_LUID, &openReq) == 0) {
            hAdapter = openReq.adapter_handle;
        } else {
            // OPEN_FROM_LUID 失败 → 捡起 EnumAdapters2 已给的 handle
            hAdapter = ai.adapter_handle;
        }
    }

    if (hAdapter == 0) {
        // 两条路都拿不到 handle，走纯名称降级
        close(fd);
        info.name = "Windows GPU via WSL2";
        info.isDiscrete = true;
        if (const char* distro = getenv("WSL_DISTRO_NAME"))
            info.name += std::string(" [") + distro + "]";
        return info;
    }

    // ③ 查询适配器类型（用拿到的 handle，无论来源）
    {
        AdapterType adType = {};
        QueryAdapterInfo q = {};
        q.adapter_handle = hAdapter;
        q.type = 15; // KMTQAITYPE_ADAPTERTYPE
        q.private_data_ptr = reinterpret_cast<uint64_t>(&adType);
        q.private_data_size = sizeof(adType);
        if (ioctl(fd, IOCTL_QUERY_ADAPTER, &q) == 0) {
            info.hasSoftware = adType.software_device;
            info.isDiscrete =
                adType.hybrid_discrete || (!adType.hybrid_integrated && !adType.software_device);
            info.isIntegrated = adType.hybrid_integrated;
        } else {
            info.isDiscrete = true; // 保守视为独显
        }
    }

    // ④ 查询 GPU 名称
    {
        AdapterRegistryInfo regInfo = {};
        QueryAdapterInfo q = {};
        q.adapter_handle = hAdapter;
        q.type = 1; // KMTQAITYPE_ADAPTERREGISTRYINFO
        q.private_data_ptr = reinterpret_cast<uint64_t>(&regInfo);
        q.private_data_size = sizeof(regInfo);
        if (ioctl(fd, IOCTL_QUERY_ADAPTER, &q) == 0) {
            std::string name = u16ToAscii(regInfo.adapter_string, kMaxPath);
            info.name = name.empty() ? "Windows GPU (WSL2)" : name;
        } else {
            // 名称查询失败：尝试从 dxgkrnl DRM 节点读 driver description
            // /sys/class/drm/card0/device/label 或 /sys/class/drm/card0/name
            std::string label = readFile("/sys/class/drm/card0/name");
            info.name = label.empty() ? "Windows GPU (WSL2)" : label;
        }
    }

    // ⑤ 从名称推断 vendorId（WSL 不暴露 PCI ID）
    info.vendorId = inferVendorFromName(info.name);

    close(fd);
    return info;
}

std::vector<std::string> drmCardNodes() {
    std::vector<std::string> nodes;
    DIR* dir = opendir("/dev/dri");
    if (!dir)
        return nodes;
    dirent* ent;
    while ((ent = readdir(dir)) != nullptr) {
        std::string name = ent->d_name;
        if (name.rfind("card", 0) == 0)
            nodes.push_back("/dev/dri/" + name);
    }
    closedir(dir);
    std::sort(nodes.begin(), nodes.end());
    return nodes;
}

bool sysfsReadPCI(const std::string& cardNode, uint32_t& vendorId, uint32_t& deviceId,
                  std::string& driverName) {
    std::string cardName = cardNode.substr(cardNode.rfind('/') + 1);
    std::string base = "/sys/class/drm/" + cardName + "/device/";
    std::string vStr = readFile(base + "vendor");
    if (vStr.empty())
        return false;
    vendorId = hexToU32(vStr);
    deviceId = hexToU32(readFile(base + "device"));
    char buf[256] = {};
    ssize_t len = readlink((base + "driver").c_str(), buf, sizeof(buf) - 1);
    if (len > 0) {
        std::string l(buf, len);
        driverName = l.substr(l.rfind('/') + 1);
    }
    return true;
}

bool isDiscreteByDriver(const std::string& drv, const std::string& cardSysfs) {
    if (drv == "nouveau" || drv == "nvidia")
        return true;
    if (drv == "amdgpu" || drv == "radeon") {
        std::string vram = readFile(cardSysfs + "device/mem_info_vram_total");
        if (!vram.empty()) {
            try {
                return std::stoull(vram) > 256ULL * 1024 * 1024;
            } catch (...) {
            }
        }
    }
    return false;
}

std::vector<std::string> readDeviceTreeCompatible() {
    std::vector<std::string> result;
    std::ifstream f("/proc/device-tree/compatible", std::ios::binary);
    if (!f.is_open())
        return result;
    std::string buf((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    std::string cur;
    for (unsigned char c : buf) {
        if (c == '\0') {
            if (!cur.empty()) {
                result.push_back(cur);
                cur.clear();
            }
        } else
            cur += (char)c;
    }
    if (!cur.empty())
        result.push_back(cur);
    return result;
}

struct SoCGPUEntry {
    const char* dtPrefix;
    const char* gpuName;
    uint32_t vendorId;
    bool hasGPU;
};

static const SoCGPUEntry kSoCDB[] = {
    {"rockchip,rk3588", "Mali-G610 MC4 (RK3588)", 0xC0CA, true},
    {"rockchip,rk3399", "Mali-T860 MP4 (RK3399)", 0xC0CA, true},
    {"rockchip,rk3568", "Mali-G52 (RK3568)", 0xC0CA, true},
    {"rockchip,rk3566", "Mali-G52 (RK3566)", 0xC0CA, true},
    {"rockchip,rk3288", "Mali-T760 (RK3288)", 0xC0CA, true},
    {"fsl,imx8mp", "Vivante GC7000UL (i.MX8MP)", 0x1957, true},
    {"fsl,imx8mq", "Vivante GC7000L (i.MX8MQ)", 0x1957, true},
    {"fsl,imx6q", "Vivante GC2000 (i.MX6Q)", 0x1957, true},
    {"fsl,imx6dl", "Vivante GC880 (i.MX6DL)", 0x1957, true},
    {"fsl,imx6ull", "None (i.MX6ULL)", 0, false},
    {"fsl,imx6ul", "None (i.MX6UL)", 0, false},
    {"raspberrypi,4", "VideoCore VI (RPi4)", 0xBCDE, true},
    {"raspberrypi,5", "VideoCore VII (RPi5)", 0xBCDE, true},
    {"allwinner,sun50i", "Mali-400 MP2 (Allwinner H6)", 0xA1EC, true},
    {nullptr, nullptr, 0, false}};

const SoCGPUEntry* lookupSoCGPU(const std::vector<std::string>& compat) {
    for (const auto& entry : compat)
        for (const auto* db = kSoCDB; db->dtPrefix; ++db)
            if (entry.rfind(db->dtPrefix, 0) == 0)
                return db;
    return nullptr;
}

} // anonymous namespace

namespace cf {

GPUInfoHost query_gpu_info_linux() {
    GPUInfoHost info;

    if (detectWSL())
        return probeWSLGPU();

    auto drmNodes = drmCardNodes();
    for (const auto& node : drmNodes) {
        uint32_t vid = 0, did = 0;
        std::string drv;
        if (!sysfsReadPCI(node, vid, did, drv) || vid == 0)
            continue;

        std::string cardName = node.substr(node.rfind('/') + 1);
        std::string sysfs = "/sys/class/drm/" + cardName + "/";
        info.vendorId = vid;
        info.deviceId = did;
        info.backend = "DRM+sysfs";

#if HAS_LIBDRM
        int fd = open(node.c_str(), O_RDWR);
        if (fd >= 0) {
            drmVersion* ver = drmGetVersion(fd);
            if (ver) {
                info.driverVersion =
                    std::to_string(ver->version_major) + "." + std::to_string(ver->version_minor);
                drmFreeVersion(ver);
            }
            close(fd);
        }
#endif
        std::string vName;
        switch (vid) {
            case 0x10DE:
                vName = "NVIDIA";
                break;
            case 0x1002:
                vName = "AMD";
                break;
            case 0x8086:
                vName = "Intel";
                break;
            default:
                vName = "Unknown Vendor";
        }
        info.name = vName + " GPU (" + drv + ")";
        info.isDiscrete = isDiscreteByDriver(drv, sysfs);
        info.isIntegrated = !info.isDiscrete;
        info.hasSoftware = false;
        return info;
    }

    auto dtCompat = readDeviceTreeCompatible();
    if (!dtCompat.empty()) {
        const auto* soc = lookupSoCGPU(dtCompat);
        if (soc) {
            info.name = soc->gpuName;
            info.vendorId = soc->vendorId;
            info.backend = "DeviceTree";
            if (!soc->hasGPU)
                info.hasSoftware = true;
            else
                info.isIntegrated = true;
            return info;
        }
        info.name = "Unknown Embedded GPU";
        info.backend = "DeviceTree";
        info.isIntegrated = true;
        return info;
    }

    info.name = "None";
    info.backend = "None";
    info.hasSoftware = true;
    return info;
}

DisplayInfoHost query_display_info_linux() {
    DisplayInfoHost d;

    if (detectWSL()) {
        d.isVirtual = true;
        d.refreshRate = 60.0;
        d.dpi = 96.0;

        struct stat wslgSt;
        bool wslgRunning = (stat("/mnt/wslg/.X11-unix", &wslgSt) == 0);

        // 方案1：fb0 virtual_size（格式 "1920,1080"）
        std::string vsize = readFile("/sys/class/graphics/fb0/virtual_size");
        if (!vsize.empty()) {
            auto comma = vsize.find(',');
            if (comma != std::string::npos) {
                try {
                    d.width = std::stoi(vsize.substr(0, comma));
                    d.height = std::stoi(vsize.substr(comma + 1));
                } catch (...) {
                }
            }
        }

        // 方案2：WSLg Virtual connector modes
        if (d.width == 0) {
            DIR* dir = opendir("/sys/class/drm");
            if (dir) {
                dirent* ent;
                while ((ent = readdir(dir)) != nullptr) {
                    std::string name = ent->d_name;
                    if (name.rfind("card", 0) != 0 || name.find("Virtual") == std::string::npos)
                        continue;
                    std::string line = readFile("/sys/class/drm/" + name + "/modes");
                    if (line.empty())
                        continue;
                    auto x = line.find('x');
                    if (x == std::string::npos)
                        continue;
                    try {
                        d.width = std::stoi(line.substr(0, x));
                        d.height = std::stoi(line.substr(x + 1));
                        if (d.width > 0 && d.height > 0)
                            break;
                    } catch (...) {
                    }
                }
                closedir(dir);
            }
        }

        // 方案3：WSLg 在运行但读不到分辨率 → 给保守默认
        if (d.width == 0 && wslgRunning) {
            d.width = 1920;
            d.height = 1080;
        }

        return d;
    }

    // 标准 Linux：DRM connector modes
    {
        DIR* dir = opendir("/sys/class/drm");
        if (dir) {
            dirent* ent;
            while ((ent = readdir(dir)) != nullptr) {
                std::string name = ent->d_name;
                if (name.rfind("card", 0) != 0 || name.find('-') == std::string::npos)
                    continue;
                std::string line = readFile("/sys/class/drm/" + name + "/modes");
                if (line.empty())
                    continue;
                auto x = line.find('x');
                if (x == std::string::npos)
                    continue;
                try {
                    d.width = std::stoi(line.substr(0, x));
                    d.height = std::stoi(line.substr(x + 1));
                    if (d.width > 0 && d.height > 0)
                        break;
                } catch (...) {
                    continue;
                }
            }
            closedir(dir);
        }
    }

    if (d.refreshRate == 0.0) {
        std::string fbMode = readFile("/sys/class/graphics/fb0/modes");
        auto p = fbMode.find('p');
        if (p != std::string::npos) {
            try {
                d.refreshRate = std::stod(fbMode.substr(p + 1));
            } catch (...) {
            }
        }
        if (d.refreshRate == 0.0)
            d.refreshRate = 60.0;
    }

    d.dpi = 96.0;
    d.devicePixelRatio = 1.0;
    return d;
}

} // namespace cf
