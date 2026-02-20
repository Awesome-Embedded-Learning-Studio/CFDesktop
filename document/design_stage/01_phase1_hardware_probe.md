# Phase 1: 硬件探针与能力分级详细设计文档

## 文档信息
| 项目 | 内容 |
|------|------|
| 文档版本 | v1.0 |
| 创建日期 | 2026-02-20 |
| 阶段代号 | Phase 1 - 硬件探针与能力分级 |
| 预计周期 | 2~3 周 |
| 依赖阶段 | Phase 0 |

---

## 一、阶段目标

### 1.1 核心目标
实现开机自动硬件检测，输出能力档位枚举，作为后续所有模块的行为裁剪依据。

### 1.2 具体交付物
- [ ] `HardwareProbe` 模块及其单元测试
- [ ] `CapabilityPolicy` 策略引擎
- [ ] `HWTier` 枚举定义及查询接口
- [ ] 设备配置文件 `/etc/CFDesktop/device.conf`
- [ ] Mock 数据集用于单元测试

---

## 二、模块架构设计

### 2.1 整体架构图

```
┌─────────────────────────────────────────────────────────┐
│                    Application Layer                    │
│                    (使用 HWTier 查询)                   │
├─────────────────────────────────────────────────────────┤
│              CapabilityPolicy (策略引擎)                │
│    ┌────────────────────────────────────────────┐      │
│    │  getAnimationPolicy()                      │      │
│    │  getRenderingBackend()                     │      │
│    │  getVideoDecoderPolicy()                   │      │
│    │  getMemoryPolicy()                         │      │
│    └────────────────────────────────────────────┘      │
├─────────────────────────────────────────────────────────┤
│              HardwareProbe (硬件探针)                   │
│    ┌────────────────────────────────────────────┐      │
│    │  detectCPU()                               │      │
│    │  detectGPU()                               │      │
│    │  detectMemory()                            │      │
│    │  detectNetwork()                           │      │
│    │  calculateTier()                           │      │
│    └────────────────────────────────────────────┘      │
├─────────────────────────────────────────────────────────┤
│           Platform Abstraction (平台抽象)               │
│    /proc/cpuinfo  /sys/class/  /dev/dri/  evdev        │
└─────────────────────────────────────────────────────────┘
```

### 2.2 文件结构

```
src/base/
├── include/CFDesktop/Base/HardwareProbe/
│   ├── HWTier.h              # 档位枚举定义
│   ├── HardwareInfo.h        # 硬件信息结构体
│   ├── HardwareProbe.h       # 探针主类
│   ├── CapabilityPolicy.h    # 策略引擎
│   └── DeviceConfig.h        # 配置文件读取
│
└── src/hardware/
    ├── HardwareProbe.cpp
    ├── CapabilityPolicy.cpp
    ├── DeviceConfig.cpp
    ├── detectors/            # 各检测器实现
    │   ├── CPUDetector.cpp
    │   ├── GPUDetector.cpp
    │   ├── MemoryDetector.cpp
    │   └── NetworkDetector.cpp
    └── platform/             # 平台特定实现
        ├── LinuxDetector.cpp
        └── WindowsDetector.cpp
```

---

## 三、数据结构定义

### 3.1 HWTier 枚举

**文件**: `include/CFDesktop/Base/HardwareProbe/HWTier.h`

```cpp
#pragma once
#include <QtGlobal>

namespace CFDesktop::Base {

/**
 * @brief 硬件能力档位枚举
 *
 * 定义三个标准档位，每个档位对应不同的硬件配置和性能等级。
 * 后续所有模块根据此枚举决定行为策略。
 */
enum class HWTier {
    /**
     * @brief 低端档位
     *
     * 典型硬件：IMX6ULL (528MHz Cortex-A7, 无GPU/软GPU)
     * - 禁用所有动画
     * - 使用 linuxfb 渲染后端
     * - 软件视频解码
     * - 限制内存使用 < 64MB
     */
    Low,

    /**
     * @brief 中端档位
     *
     * 典型硬件：RK3568 (4xCortex-A55, Mali-G52)
     * - 部分动画（淡入淡出、位移）
     * - eglfs 可选
     * - 硬件视频解码（H.264/H.265 部分）
     * - 内存使用 < 256MB
     */
    Mid,

    /**
     * @brief 高端档位
     *
     * 典型硬件：RK3588 (8xCortex-A76/A55, Mali-G610)
     * - 全动画支持（包括缩放、旋转、复杂效果）
     * - 强制 eglfs + OpenGL ES 3.2+
     * - 全格式硬件视频解码
     * - 内存使用 < 1GB
     */
    High
};

/**
 * @brief 获取档位字符串描述
 */
QString tierToString(HWTier tier);

/**
 * @brief 从字符串解析档位
 */
HWTier tierFromString(const QString& str);

} // namespace CFDesktop::Base
```

### 3.2 HardwareInfo 结构体

**文件**: `include/CFDesktop/Base/HardwareProbe/HardwareInfo.h`

```cpp
#pragma once
#include <QString>
#include <QList>
#include <QPair>
#include "HWTier.h"

namespace CFDesktop::Base {

/**
 * @brief CPU 信息
 */
struct CPUInfo {
    QString model;           // CPU 型号名称
    int cores = 0;           // 逻辑核心数
    int frequencyMHz = 0;    // 主频 (MHz)
    QString architecture;    // 架构 (armv7l, aarch64, x86_64)
    QStringList features;    // CPU 特性 (neon, vfpv4, etc.)
};

/**
 * @brief GPU 信息
 */
struct GPUInfo {
    QString renderer;        // 渲染器名称
    QString vendor;          // 供应商
    QString version;         // 驱动版本
    bool hasHardwareAcceleration = false;  // 是否有硬件加速
    QString driverPath;      // 驱动设备路径 (/dev/dri/card0)
    int maxTextureSize = 0;  // 最大纹理尺寸
    QStringList extensions;  // OpenGL 扩展列表
};

/**
 * @brief 内存信息
 */
struct MemoryInfo {
    quint64 totalBytes = 0;      // 总内存 (字节)
    quint64 availableBytes = 0;  // 可用内存 (字节)
    quint64 totalSwap = 0;       // 总交换空间
    quint64 freeSwap = 0;        // 可用交换空间
    QString lowMemoryBehavior;   // 低内存行为 (oom, compress, etc.)
};

/**
 * @brief 网络接口信息
 */
struct NetworkInterface {
    QString name;           // 接口名称 (eth0, wlan0)
    bool isUp = false;      // 是否启用
    bool isWireless = false; // 是否无线
    QString macAddress;     // MAC 地址
    QStringList ipAddresses; // IP 地址列表
};

/**
 * @brief 完整硬件信息
 */
struct HardwareInfo {
    HWTier tier = HWTier::Low;     // 计算得出的档位

    CPUInfo cpu;                   // CPU 信息
    GPUInfo gpu;                   // GPU 信息
    MemoryInfo memory;             // 内存信息
    QList<NetworkInterface> networkInterfaces; // 网络接口

    QString deviceTreeCompatible;  // 设备树 compatible 字符串
    QString boardName;             // 板载名称

    bool isUserOverridden = false; // 是否用户手动覆盖
};

} // namespace CFDesktop::Base
```

### 3.3 CapabilityPolicy 配置结构

**文件**: `include/CFDesktop/Base/HardwareProbe/CapabilityPolicy.h`

```cpp
#pragma once
#include "HWTier.h"
#include <QString>

namespace CFDesktop::Base {

/**
 * @brief 动画策略
 */
struct AnimationPolicy {
    bool enabled = false;           // 是否启用动画
    int defaultDurationMs = 0;      // 默认动画时长 (毫秒)
    int maxConcurrentAnimations = 0; // 最大并发动画数
    bool allowComplexEffects = false; // 是否允许复杂效果 (模糊、阴影等)
};

/**
 * @brief 渲染后端策略
 */
struct RenderingPolicy {
    QString qtPlatform;             // Qt 平台插件 (linuxfb, eglfs, xcb)
    bool useOpenGL = false;         // 是否使用 OpenGL
    QString openGLVersion;          // OpenGL 版本要求
    bool useVSync = false;          // 是否垂直同步
    int maxFPS = 60;                // 最大帧率
};

/**
 * @brief 视频解码策略
 */
struct VideoDecoderPolicy {
    bool useHardwareDecoder = false; // 是否使用硬件解码
    QStringList supportedCodecs;    // 支持的编解码器
    int maxResolution = 0;          // 最大分辨率 (宽)
    int maxBitrate = 0;             // 最大码率 (bps)
    bool allowSimultaneousPlayback = false; // 是否允许同时播放多个
};

/**
 * @brief 内存策略
 */
struct MemoryPolicy {
    int maxImageCacheBytes = 0;     // 图片缓存大小
    int maxFontCacheBytes = 0;      // 字体缓存大小
    bool enableTextureCompression = false; // 是否启用纹理压缩
    int maxWindowSurfaces = 0;      // 最大窗口表面数
};

} // namespace CFDesktop::Base
```

---

## 四、类接口设计

### 4.1 HardwareProbe 主类

**文件**: `include/CFDesktop/Base/HardwareProbe/HardwareProbe.h`

```cpp
#pragma once
#include "HardwareInfo.h"
#include <QObject>
#include <QSharedPointer>
#include <QStringList>

namespace CFDesktop::Base {

/**
 * @brief 硬件探针主类
 *
 * 负责检测系统硬件能力，输出 HardwareInfo 结构。
 * 单例模式，进程内唯一实例。
 */
class HardwareProbe : public QObject {
    Q_OBJECT

public:
    /**
     * @brief 获取单例实例
     */
    static HardwareProbe* instance();

    /**
     * @brief 执行硬件检测
     * @return 检测结果
     *
     * 首次调用会执行完整检测，后续调用返回缓存结果。
     * 可通过 forceRedetect() 强制重新检测。
     */
    HardwareInfo probe();

    /**
     * @brief 强制重新检测
     */
    HardwareInfo forceRedetect();

    /**
     * @brief 获取当前硬件档位
     */
    HWTier currentTier() const;

    /**
     * @brief 获取完整硬件信息
     */
    const HardwareInfo& hardwareInfo() const;

    /**
     * @brief 设置 Mock 数据（仅用于测试）
     */
    void setMockData(const HardwareInfo& mockInfo);

    /**
     * @brief 检查特定功能是否可用
     */
    bool hasFeature(const QString& feature) const;

signals:
    /**
     * @brief 硬件检测完成信号
     */
    void probeCompleted(const HardwareInfo& info);

    /**
     * @brief 档位变化信号（热插拔设备等情况）
     */
    void tierChanged(HWTier newTier);

private:
    HardwareProbe(QObject* parent = nullptr);
    ~HardwareProbe() = default;

    // 禁用拷贝和移动
    HardwareProbe(const HardwareProbe&) = delete;
    HardwareProbe& operator=(const HardwareProbe&) = delete;

    // 内部检测方法
    void detectCPU(HardwareInfo& info);
    void detectGPU(HardwareInfo& info);
    void detectMemory(HardwareInfo& info);
    void detectNetwork(HardwareInfo& info);
    void calculateTier(HardwareInfo& info);

    // 平台特定实现
    void probeLinux(HardwareInfo& info);
    void probeWindows(HardwareInfo& info);

private:
    static HardwareProbe* s_instance;
    HardwareInfo m_cachedInfo;
    bool m_isProbed = false;
    bool m_useMockData = false;
    HardwareInfo m_mockInfo;
};

} // namespace CFDesktop::Base
```

### 4.2 CapabilityPolicy 策略引擎

**文件**: `include/CFDesktop/Base/HardwareProbe/CapabilityPolicy.h`

```cpp
#pragma once
#include "HWTier.h"
#include <QObject>

namespace CFDesktop::Base {

// 前向声明
struct AnimationPolicy;
struct RenderingPolicy;
struct VideoDecoderPolicy;
struct MemoryPolicy;

/**
 * @brief 能力策略引擎
 *
 * 根据 HWTier 提供各模块的能力配置。
 * 单例模式，根据当前硬件档位返回对应策略。
 */
class CapabilityPolicy : public QObject {
    Q_OBJECT

public:
    static CapabilityPolicy* instance();

    /**
     * @brief 获取动画策略
     */
    AnimationPolicy getAnimationPolicy() const;

    /**
     * @brief 获取渲染策略
     */
    RenderingPolicy getRenderingPolicy() const;

    /**
     * @brief 获取视频解码策略
     */
    VideoDecoderPolicy getVideoDecoderPolicy() const;

    /**
     * @brief 获取内存策略
     */
    MemoryPolicy getMemoryPolicy() const;

    /**
     * @brief 获取当前档位
     */
    HWTier currentTier() const;

    /**
     * @brief 强制设置档位（用于测试或用户覆盖）
     */
    void overrideTier(HWTier tier);

signals:
    void policyChanged(HWTier newTier);

private:
    CapabilityPolicy(QObject* parent = nullptr);
    ~CapabilityPolicy() = default;

    // 档位特定策略配置
    AnimationPolicy getAnimationPolicyForLow() const;
    AnimationPolicy getAnimationPolicyForMid() const;
    AnimationPolicy getAnimationPolicyForHigh() const;

    RenderingPolicy getRenderingPolicyForLow() const;
    RenderingPolicy getRenderingPolicyForMid() const;
    RenderingPolicy getRenderingPolicyForHigh() const;

    VideoDecoderPolicy getVideoPolicyForLow() const;
    VideoDecoderPolicy getVideoPolicyForMid() const;
    VideoDecoderPolicy getVideoPolicyForHigh() const;

    MemoryPolicy getMemoryPolicyForLow() const;
    MemoryPolicy getMemoryPolicyForMid() const;
    MemoryPolicy getMemoryPolicyForHigh() const;

private:
    static CapabilityPolicy* s_instance;
    HWTier m_currentTier = HWTier::Low;
    bool m_isOverridden = false;
};

} // namespace CFDesktop::Base
```

### 4.3 DeviceConfig 配置文件

**文件**: `include/CFDesktop/Base/HardwareProbe/DeviceConfig.h`

```cpp
#pragma once
#include "HWTier.h"
#include <QString>
#include <QVariantMap>

namespace CFDesktop::Base {

/**
 * @brief 设备配置文件读取器
 *
 * 读取 /etc/CFDesktop/device.conf，允许用户手动覆盖硬件检测。
 *
 * 配置文件格式:
 * [Device]
 * Tier=auto|low|mid|high
 * CustomScript=/path/to/detection/script
 *
 * [Overrides]
 * EnableAnimations=true|false
 * ForceOpenGL=true|false
 */
class DeviceConfig {
public:
    struct Config {
        bool tierAutoDetect = true;
        HWTier forcedTier = HWTier::Low;
        QString customScriptPath;
        QVariantMap overrides;
    };

    /**
     * @brief 加载配置文件
     */
    static Config load(const QString& path = "/etc/CFDesktop/device.conf");

    /**
     * @brief 保存配置文件
     */
    static bool save(const Config& config,
                     const QString& path = "/etc/CFDesktop/device.conf");

    /**
     * @brief 执行自定义检测脚本
     *
     * 脚本输出格式: JSON
     * {
     *   "tier": "low|mid|high",
     *   "properties": {...}
     * }
     */
    static QVariantMap executeCustomScript(const QString& scriptPath);
};

} // namespace CFDesktop::Base
```

---

## 五、检测逻辑详细设计

### 5.1 CPU 检测

#### 5.1.1 Linux 平台

**检测文件**: `src/hardware/detectors/CPUDetector.cpp`

```cpp
CPUInfo CPUDetector::detectCPU() {
    CPUInfo info;

    // 1. 读取 /proc/cpuinfo
    QFile cpuInfo("/proc/cpuinfo");
    if (cpuInfo.open(QIODevice::ReadOnly)) {
        QTextStream in(&cpuInfo);
        QSet<QString> uniqueProcessors;

        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.startsWith("Hardware")) {
                info.model = line.section(':', 1).trimmed();
            } else if (line.startsWith("Processor")) {
                uniqueProcessors.insert(line);
            } else if (line.startsWith("CPU implementer")) {
                info.architecture = detectArchitecture(line);
            } else if (line.startsWith("Features")) {
                info.features = line.section(':', 1).trimmed().split(' ');
            } else if (line.startsWith("BogoMIPS")) {
                info.frequencyMHz = static_cast<int>(
                    line.section(':', 1).trimmed().toDouble() * 2);
            }
        }
        info.cores = uniqueProcessors.size();
    }

    // 2. 通过 uname 确认架构
    QProcess uname;
    uname.start("uname", QStringList() << "-m");
    if (uname.waitForStarted() && uname.waitForFinished()) {
        info.architecture = uname.readAllStandardOutput().trimmed();
    }

    // 3. 读取设备树 (如果存在)
    QFile compatible("/sys/firmware/devicetree/base/compatible");
    if (compatible.open(QIODevice::ReadOnly)) {
        QByteArray data = compatible.readAll();
        // 设备树字符串以 null 结尾，可能有多项
        QList<QByteArray> compatList = data.split('\0');
        if (!compatList.isEmpty() && !compatList.first().isEmpty()) {
            info.model = compatList.first();
        }
    }

    return info;
}

QString CPUDetector::detectArchitecture(const QString& cpuInfoLine) {
    // CPU implementer 值映射
    QHash<QString, QString> implementerMap = {
        {"0x41", "ARM"},
        {"0x42", "Broadcom"},
        {"0x43", "Cavium"},
        {"0x44", "DEC"},
        {"0x49", "Infineon"},
        {"0x4d", "Motorola/Freescale"},
        {"0x4e", "NVIDIA"},
        {"0x50", "APM"},
        {"0x51", "Qualcomm"},
        {"0x56", "Marvell"},
        {"0x69", "Intel"},
    };

    QString implementer = /* 从 cpuinfo 提取 */;
    return implementerMap.value(implementer, "Unknown");
}
```

#### 5.1.2 Windows 平台

```cpp
CPUInfo CPUDetector::detectCPUWindows() {
    CPUInfo info;

    // 使用 WMI 查询
    QProcess wmic;
    wmic.start("wmic", QStringList()
        << "cpu" << "get" << "Name,NumberOfCores,MaxClockSpeed");
    // 解析输出...

    return info;
}
```

### 5.2 GPU 检测

```cpp
GPUInfo GPUDetector::detectGPU() {
    GPUInfo info;

    // 1. 检查 DRM 设备
    QDir dri("/dev/dri");
    if (dri.exists()) {
        QStringList devices = dri.entryList(QStringList() << "card*", QDir::Files);
        if (!devices.isEmpty()) {
            info.hasHardwareAcceleration = true;
            info.driverPath = "/dev/dri/" + devices.first();
        }
    }

    // 2. 尝试创建 OpenGL 上下文
    QOffscreenSurface surface;
    QOpenGLContext context;
    if (context.create()) {
        context.makeCurrent(&surface);

        auto* functions = context.functions();
        info.vendor = reinterpret_cast<const char*>(
            functions->glGetString(GL_VENDOR));
        info.renderer = reinterpret_cast<const char*>(
            functions->glGetString(GL_RENDERER));
        info.version = reinterpret_cast<const char*>(
            functions->glGetString(GL_VERSION));

        // 获取扩展列表
        auto* extraFunctions = context.extraFunctions();
        GLint numExtensions = 0;
        functions->glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
        for (int i = 0; i < numExtensions; ++i) {
            const char* ext = reinterpret_cast<const char*>(
                functions->glGetStringi(GL_EXTENSIONS, i));
            info.extensions.append(ext);
        }

        context.doneCurrent();
    }

    // 3. 检查特定 SoC 的 GPU 型号
    detectSoCGPU(info);

    return info;
}
```

### 5.3 内存检测

```cpp
MemoryInfo MemoryDetector::detectMemory() {
    MemoryInfo info;

    // Linux: 读取 /proc/meminfo
    QFile memInfo("/proc/meminfo");
    if (memInfo.open(QIODevice::ReadOnly)) {
        QTextStream in(&memInfo);
        QHash<QString, quint64> memData;

        while (!in.atEnd()) {
            QString line = in.readLine();
            QString key = line.section(':', 0, 0);
            QString value = line.section(':', 1).section(' ', 0, 0);
            memData[key] = value.toULongLong() * 1024; // kB to bytes
        }

        info.totalBytes = memData["MemTotal"];
        info.availableBytes = memData["MemAvailable"];
        info.totalSwap = memData["SwapTotal"];
        info.freeSwap = memData["SwapFree"];
    }

    return info;
}
```

### 5.4 网络检测

```cpp
QList<NetworkInterface> NetworkDetector::detectNetwork() {
    QList<NetworkInterface> interfaces;

    // 读取 /sys/class/net
    QDir netDir("/sys/class/net");
    QStringList devices = netDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QString& device : devices) {
        NetworkInterface iface;
        iface.name = device;

        // 检查是否启用
        QFile operstate(QString("/sys/class/net/%1/operstate").arg(device));
        if (operstate.open(QIODevice::ReadOnly)) {
            iface.isUp = operstate.readAll().trimmed() == "up";
        }

        // 检查是否无线
        QDir wireless("/sys/class/net/" + device + "/wireless");
        iface.isWireless = wireless.exists();

        // 读取 MAC 地址
        QFile address(QString("/sys/class/net/%1/address").arg(device));
        if (address.open(QIODevice::ReadOnly)) {
            iface.macAddress = address.readAll().trimmed();
        }

        // 通过 Socket ioctl 获取 IP 地址
        iface.ipAddresses = getIPAddresses(device);

        interfaces.append(iface);
    }

    return interfaces;
}
```

### 5.5 档位计算逻辑

```cpp
void HardwareProbe::calculateTier(HardwareInfo& info) {
    // 默认为 Low 档
    HWTier calculatedTier = HWTier::Low;
    int score = 0;

    // CPU 评分
    score += std::min(info.cpu.cores, 8) * 10;  // 最多 80 分
    if (info.cpu.frequencyMHz > 1000) score += 20;
    if (info.cpu.features.contains("neon")) score += 10;

    // GPU 评分
    if (info.gpu.hasHardwareAcceleration) score += 50;
    if (!info.gpu.driverPath.isEmpty()) score += 20;

    // 内存评分
    int memoryMB = info.memory.totalBytes / (1024 * 1024);
    if (memoryMB >= 512) score += 20;
    if (memoryMB >= 1024) score += 20;
    if (memoryMB >= 2048) score += 10;

    // 根据评分决定档位
    // Low: 0-60, Mid: 61-120, High: 121+
    if (score >= 121) {
        calculatedTier = HWTier::High;
    } else if (score >= 61) {
        calculatedTier = HWTier::Mid;
    } else {
        calculatedTier = HWTier::Low;
    }

    info.tier = calculatedTier;
}
```

---

## 六、设备配置文件格式

### 6.1 配置文件模板

**文件**: `configs/device.conf.template`

```ini
# CFDesktop 设备配置文件
# 部署位置: /etc/CFDesktop/device.conf

[Device]
# 档位设置: auto(自动检测) | low | mid | high
Tier=auto

# 自定义检测脚本路径（可选）
# 脚本应输出 JSON 格式到 stdout
CustomScript=/opt/cfdesktop/detect-hardware.sh

# 板载名称（可选，用于日志）
BoardName=Generic-Board

[Overrides]
# 强制覆盖特定能力（可选）

# 动画设置
EnableAnimations=true
AnimationDuration=200

# 渲染后端: auto | linuxfb | eglfs | xcb
RenderingBackend=auto

# OpenGL 设置
ForceOpenGL=false

# 视频解码: auto | software | hardware
VideoDecoder=auto

[Logging]
# 硬件检测日志级别
LogLevel=Info
```

### 6.2 自定义检测脚本示例

```bash
#!/bin/bash
# /opt/cfdesktop/detect-hardware.sh

# 检测是否连接了特定扩展硬件
if [ -e /sys/bus/i2c/devices/0-0050 ]; then
    # 检测到扩展显示屏，提升档位
    echo '{"tier": "mid", "properties": {"externalDisplay": true}}'
else
    echo '{"tier": "low"}'
fi
```

---

## 七、单元测试设计

### 7.1 Mock 数据目录结构

```
tests/mock/
└── proc/
    ├── cpuinfo_imx6ull          # IMX6ULL CPU 信息
    ├── cpuinfo_rk3568           # RK3568 CPU 信息
    ├── cpuinfo_rk3588           # RK3588 CPU 信息
    ├── meminfo_512mb            # 512MB 内存
    ├── meminfo_1gb              # 1GB 内存
    ├── meminfo_4gb              # 4GB 内存
    └── devices/                 # 模拟设备文件
        └── dri/
            └── card0            # 模拟 GPU 设备
```

### 7.2 测试用例清单

**文件**: `tests/unit/base/hardware/test_hardware_probe.cpp`

```cpp
class TestHardwareProbe : public QObject {
    Q_OBJECT

private slots:
    // ========== CPU 检测测试 ==========
    void testDetectCPU_IMX6ULL();
    void testDetectCPU_RK3568();
    void testDetectCPU_RK3588();
    void testDetectCPU_X86_64();

    // ========== GPU 检测测试 ==========
    void testDetectGPU_WithDRM();
    void testDetectGPU_NoDRM();
    void testDetectGPU_OpenGLContext();

    // ========== 内存检测测试 ==========
    void testDetectMemory_512MB();
    void testDetectMemory_1GB();
    void testDetectMemory_4GB();

    // ========== 档位计算测试 ==========
    void testCalculateTier_IMX6ULL_returnsLow();
    void testCalculateTier_RK3568_returnsMid();
    void testCalculateTier_RK3588_returnsHigh();

    // ========== 配置文件测试 ==========
    void testDeviceConfig_LoadDefault();
    void testDeviceConfig_OverrideTier();
    void testDeviceConfig_CustomScript();

    // ========== 策略引擎测试 ==========
    void testCapabilityPolicy_LowTier();
    void testCapabilityPolicy_MidTier();
    void testCapabilityPolicy_HighTier();
    void testCapabilityPolicy_AnimationDisabledOnLow();

    // ========== 边界情况 ==========
    void testEmptyProcFiles();
    void testMalformedConfig();
    void testTierOverride();
};
```

### 7.3 示例测试用例

```cpp
void TestHardwareProbe::testDetectCPU_IMX6ULL() {
    // 1. 设置 Mock 环境变量
    qputenv("CFDESKTOP_MOCK_PROC", "/path/to/mock/proc/cpuinfo_imx6ull");

    // 2. 创建探针实例
    HardwareProbe probe;
    HardwareInfo info = probe.probe();

    // 3. 验证结果
    QCOMPARE(info.cpu.model, "Freescale i.MX6 UltraLite");
    QCOMPARE(info.cpu.cores, 1);
    QVERIFY(info.cpu.features.contains("neon"));
    QCOMPARE(info.cpu.architecture, "armv7l");

    // 4. 验证档位
    QCOMPARE(info.tier, HWTier::Low);
}
```

---

## 八、详细任务清单

### 8.1 Week 1: 基础检测器实现

#### Day 1-2: CPU 检测器
- [ ] 创建 `CPUDetector` 类框架
- [ ] 实现 `/proc/cpuinfo` 解析
- [ ] 实现设备树 compatible 读取
- [ ] 实现 `uname` 架构检测
- [ ] 编写 CPU 检测单元测试

#### Day 3: GPU 检测器
- [ ] 创建 `GPUDetector` 类框架
- [ ] 实现 DRM 设备检测
- [ ] 实现 OpenGL 上下文创建
- [ ] 实现 GPU 能力查询
- [ ] 编写 GPU 检测单元测试

#### Day 4: 内存与网络检测器
- [ ] 实现 `/proc/meminfo` 解析
- [ ] 实现网络接口枚举
- [ ] 实现 IP 地址获取
- [ ] 编写相应单元测试

#### Day 5: 档位计算逻辑
- [ ] 实现评分算法
- [ ] 定义档位阈值
- [ ] 实现 `calculateTier()` 方法
- [ ] 验证各板卡档位判定

### 8.2 Week 2: 策略引擎与配置

#### Day 1-2: CapabilityPolicy 实现
- [ ] 定义各策略结构体
- [ ] 实现档位特定策略配置
- [ ] 实现 `getAnimationPolicy()`
- [ ] 实现 `getRenderingPolicy()`
- [ ] 实现 `getVideoDecoderPolicy()`
- [ ] 实现 `getMemoryPolicy()`

#### Day 3: DeviceConfig 实现
- [ ] 实现配置文件解析
- [ ] 实现配置文件写入
- [ ] 实现自定义脚本执行
- [ ] 编写配置测试

#### Day 4: 集成测试
- [ ] 创建端到端测试用例
- [ ] 测试 IMX6ULL 真机
- [ ] 测试 RK3568 真机
- [ ] 验证自动档位判定

#### Day 5: 文档与优化
- [ ] 编写 API 文档
- [ ] 优化检测性能
- [ ] 添加性能日志
- [ ] Code Review

### 8.3 Week 3: 完善与验证

#### Day 1-2: 跨平台支持
- [ ] 实现 Windows 平台检测
- [ ] 实现模拟器平台适配
- [ ] 编写平台特定测试

#### Day 3-4: 真机验证
- [ ] 在 IMX6ULL 上完整测试
- [ ] 在 RK3568 上完整测试
- [ ] 在 RK3588 上完整测试
- [ ] 收集性能数据

#### Day 5: 发布准备
- [ ] 最终测试
- [ ] 更新文档
- [ ] 准备演示
- [ ] 合并主分支

---

## 九、验收标准

### 9.1 功能验收
- [ ] 在 IMX6ULL 上正确识别为 Low 档
- [ ] 在 RK3568 上正确识别为 Mid 档
- [ ] 在 RK3588 上正确识别为 High 档
- [ ] 所有单元测试通过（覆盖率 > 90%）
- [ ] 配置文件覆盖功能正常
- [ ] 自定义脚本执行正常

### 9.2 性能验收
- [ ] 硬件检测耗时 < 500ms
- [ ] 内存占用 < 5MB
- [ ] 不影响系统启动时间

### 9.3 代码质量
- [ ] 符合代码规范
- [ ] 通过 clang-tidy 检查
- [ ] API 文档完整
- [ ] 代码审查通过

---

## 十、已知问题与风险

| 问题 | 风险 | 缓解措施 |
|------|------|----------|
| 某些板卡没有设备树 | 档位误判 | 提供 CPU 特性回退检测 |
| GPU 驱动不稳定 | 检测失败 | 添加异常处理，设置超时 |
| 交叉编译测试困难 | 覆盖不足 | 使用 QEMU 用户模式模拟 |

---

## 十一、下一步行动

完成 Phase 1 后，进入 **Phase 2: Base 库核心**。
