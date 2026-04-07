# Phase 1: 硬件探针与能力分级 - 状态文档

> **模块ID**: Phase 1
> **状态**: 🚧 部分完成
> **总体进度**: ~90%
> **最后更新**: 2026-03-27

---

## 一、模块概述

硬件探针模块是 CFDesktop 项目的能力分级核心，负责在启动时自动检测系统硬件能力，根据检测结果计算硬件档位 (HWTier)，并为上层模块提供对应的能力策略配置。

### 核心职责

1. **硬件检测** - CPU、GPU、内存、网络自动检测
2. **能力分级** - 根据检测结果计算 Low/Mid/High 三档
3. **策略引擎** - 为各模块提供对应档位的能力配置
4. **配置覆盖** - 支持手动配置和自定义检测脚本

### 档位定义

| 档位 | 典型硬件 | 动画 | 渲染 | 视频解码 | 内存限制 |
|------|----------|------|------|----------|----------|
| Low | IMX6ULL (528MHz Cortex-A7) | 禁用 | linuxfb | 软件 | < 64MB |
| Mid | RK3568 (4xCortex-A55, Mali-G52) | 部分启用 | eglfs可选 | H.264/H.265 部分 | < 256MB |
| High | RK3588 (8xCortex-A76/A55, Mali-G610) | 全部启用 | eglfs+OpenGL ES 3.2+ | 全格式硬件解码 | < 1GB |

---

## 二、完成状态总览

| 模块 | 完成度 | 状态 |
|------|--------|------|
| CPUDetector | 100% | ✅ 完成 |
| MemoryDetector | 95% | ✅ 完成 |
| GPUDetector | 90% | ✅ 核心功能已完成 |
| NetworkDetector | 85% | ✅ 核心功能已完成 |
| HWTier 系统 | 0% | ❌ 待实现 |
| CapabilityPolicy | 0% | ❌ 待实现 |
| HardwareProbe 主类 | 0% | ❌ 待实现 |

---

## 三、已完成工作

### 3.1 CPU 检测器 (80%)

#### 实现文件

| 文件路径 | 说明 |
|---------|------|
| `base/include/system/cpu/cfcpu.h` | CPU 基础信息接口 |
| `base/include/system/cpu/cfcpu_profile.h` | CPU 详细信息 (频率、核心数) |
| `base/include/system/cpu/cfcpu_bonus.h` | CPU 扩展信息 |
| `base/system/cpu/cfcpu.cpp` | 基础信息实现 |
| `base/system/cpu/cfcpu_profile.cpp` | 详细信息实现 |
| `base/system/cpu/cfcpu_bonus.cpp` | 扩展信息实现 |
| `base/system/cpu/private/linux_impl/*.cpp` | Linux 平台实现 |
| `base/system/cpu/private/win_impl/*.cpp` | Windows 平台实现 |

#### 功能实现

- [x] `CPUInfoView` 结构体 - 型号、架构、厂商
- [x] `/proc/cpuinfo` 解析 (Linux)
- [x] WMI 查询 (Windows)
- [x] CPU 核心数检测
- [x] CPU 频率检测
- [x] CPU 特性检测 (neon, vfpv4, AVX, etc.)
- [x] 设备树 compatible 读取
- [x] `uname` 架构检测

#### 测试

- 测试文件: `test/system/test_cpu_info_query.cpp`

---

### 3.2 内存检测器 (80%)

#### 实现文件

| 文件路径 | 说明 |
|---------|------|
| `base/include/system/memory/memory_info.h` | 内存信息接口 |
| `base/system/memory/memory_info.cpp` | 内存信息实现 |
| `base/system/memory/private/linux_impl/*.cpp` | Linux 平台实现 |
| `base/system/memory/private/win_impl/*.cpp` | Windows 平台实现 |

#### 功能实现

- [x] 总内存检测
- [x] 可用内存检测
- [x] Swap 内存检测
- [x] 物理内存详情 (DIMM)
- [x] 进程内存统计
- [x] 缓存内存统计

#### 测试

- 测试文件: `test/system/test_memory_info_query.cpp`

---

### 3.3 GPU 检测器 (100%)

#### 实现文件

| 文件路径 | 说明 |
|---------|------|
| `base/include/system/gpu/gpu.h` | GPU/显示信息接口 |
| `base/system/gpu/gpu.cpp` | 跨平台实现 |
| `base/system/gpu/private/linux_impl/gpu_info.cpp` | Linux 平台实现 |
| `base/system/gpu/private/win_impl/gpu_info.cpp` | Windows 平台实现 |

#### 功能实现

- [x] `GPUInfo` 结构体 - GPU 设备信息
- [x] `DisplayInfo` 结构体 - 显示器信息
- [x] `EnvironmentScore` 结构体 - 环境评分
- [x] Linux DRM 设备检测 (`/dev/dri/card*`)
- [x] Linux DeviceTree SoC 检测
- [x] WSL2 GPU 探测 (`/dev/dxg`)
- [x] Windows DXGI 检测
- [x] GPU 评分算法 (满分 50)
- [x] 显示评分算法 (满分 50)
- [x] 档位判定 (Low/Mid/High)

#### 示例

- 示例文件: `example/base/system/example_gpu_info.cpp`

---

### 3.4 网络检测器 (100%)

#### 实现文件

| 文件路径 | 说明 |
|---------|------|
| `base/include/system/network/network.h` | 网络信息接口 |
| `base/system/network/network.cpp` | 基于 Qt 的跨平台实现 |

#### 功能实现

- [x] `IpAddress` 结构体 - IPv4/IPv6 地址
- [x] `InterfaceInfo` 结构体 - 网卡信息
- [x] `AddressEntry` 结构体 - IP 地址条目
- [x] `InterfaceFlags` 结构体 - 接口标志
- [x] `NetworkStatus` 结构体 - 网络状态
- [x] `Reachability` 枚举 - 网络可达性
- [x] `TransportMedium` 枚举 - 传输介质
- [x] `DnsEligibility` 枚举 - DNS 可达性
- [x] `getNetworkInfo()` 函数

#### 示例

- 示例文件: `example/base/system/example_network_info.cpp`

---

## 四、使用示例

```cpp
#include <system/cpu/cfcpu.h>
#include <system/memory/memory_info.h>

// 查询 CPU 信息
auto cpu_info = cf::getCPUInfo();
if (cpu_info) {
    qDebug() << "CPU:" << cpu_info.value().model.data();
    qDebug() << "Arch:" << cpu_info.value().arch.data();
}

// 查询内存信息
auto mem_info = cf::getMemoryInfo();
if (mem_info) {
    qDebug() << "Total:" << mem_info.value().totalBytes;
    qDebug() << "Available:" << mem_info.value().availableBytes;
}
```

---

## 五、待完成模块

> **注意**: GPU 和 Network 检测器已完成，请参考上方"三、已完成工作"中的 3.3 和 3.4 节。

### 5.1 HWTier 枚举定义 (P0)

**需求描述**:
- 定义 Low/Mid/High 三档枚举
- 每档对应硬件配置说明
- 档位字符串转换函数

**建议文件路径**: `base/hardware/HWTier.h`

**接口设计**:
```cpp
namespace CFDesktop::Base {

enum class HWTier {
    Low,   // IMX6ULL 级别
    Mid,   // RK3568 级别
    High   // RK3588 级别
};

QString tierToString(HWTier tier);
HWTier tierFromString(const QString& str);

} // namespace CFDesktop::Base
```

---

### 5.2 HardwareProbe 主类 (P0)

**需求描述**:
- 整合所有检测器
- 实现档位计算逻辑
- 单例模式
- Mock 数据支持

**建议文件路径**:
- `base/hardware/HardwareProbe.h`
- `base/hardware/HardwareProbe.cpp`

**接口设计**:
```cpp
namespace CFDesktop::Base {

struct HardwareInfo {
    HWTier tier = HWTier::Low;
    CPUInfo cpu;
    GPUInfo gpu;
    MemoryInfo memory;
    QList<NetworkInterface> networkInterfaces;
    QString deviceTreeCompatible;
    bool isUserOverridden = false;
};

class HardwareProbe : public QObject {
    Q_OBJECT

public:
    static HardwareProbe* instance();
    HardwareInfo probe();
    HardwareInfo forceRedetect();
    HWTier currentTier() const;
    const HardwareInfo& hardwareInfo() const;
    void setMockData(const HardwareInfo& mockInfo);

signals:
    void probeCompleted(const HardwareInfo& info);
    void tierChanged(HWTier newTier);

private:
    HardwareProbe(QObject* parent = nullptr);
    void calculateTier(HardwareInfo& info);
};

} // namespace CFDesktop::Base
```

---

### 5.3 CapabilityPolicy 策略引擎 (P0)

**需求描述**:
- 定义各策略结构体
- 实现档位特定策略配置
- 策略查询接口

**建议文件路径**:
- `base/hardware/CapabilityPolicy.h`
- `base/hardware/CapabilityPolicy.cpp`

**接口设计**:
```cpp
namespace CFDesktop::Base {

struct AnimationPolicy {
    bool enabled = false;
    int defaultDurationMs = 0;
    int maxConcurrentAnimations = 0;
    bool allowComplexEffects = false;
};

struct RenderingPolicy {
    QString qtPlatform;
    bool useOpenGL = false;
    QString openGLVersion;
    bool useVSync = false;
    int maxFPS = 60;
};

struct VideoDecoderPolicy {
    bool useHardwareDecoder = false;
    QStringList supportedCodecs;
    int maxResolution = 0;
    int maxBitrate = 0;
};

struct MemoryPolicy {
    int maxImageCacheBytes = 0;
    int maxFontCacheBytes = 0;
    bool enableTextureCompression = false;
    int maxWindowSurfaces = 0;
};

class CapabilityPolicy : public QObject {
    Q_OBJECT

public:
    static CapabilityPolicy* instance();
    AnimationPolicy getAnimationPolicy() const;
    RenderingPolicy getRenderingPolicy() const;
    VideoDecoderPolicy getVideoDecoderPolicy() const;
    MemoryPolicy getMemoryPolicy() const;
    HWTier currentTier() const;
    void overrideTier(HWTier tier);

signals:
    void policyChanged(HWTier newTier);
};

} // namespace CFDesktop::Base
```

---

### 5.4 DeviceConfig 配置文件 (P1)

**需求描述**:
- INI 格式配置文件解析
- 支持档位手动覆盖
- 自定义检测脚本执行

**建议文件路径**:
- `base/hardware/DeviceConfig.h`
- `base/hardware/DeviceConfig.cpp`

**配置文件格式** (`/etc/CFDesktop/device.conf`):
```ini
[Device]
Tier=auto|low|mid|high
CustomScript=/opt/cfdesktop/detect-hardware.sh
BoardName=Generic-Board

[Overrides]
EnableAnimations=true
AnimationDuration=200
RenderingBackend=auto
ForceOpenGL=false
VideoDecoder=auto

[Logging]
LogLevel=Info
```

---

## 六、测试设计

### 6.1 单元测试用例

**文件**: `test/hardware/test_hardware_probe.cpp`

```cpp
class TestHardwareProbe : public QObject {
    Q_OBJECT

private slots:
    // CPU 检测测试
    void testDetectCPU_IMX6ULL();
    void testDetectCPU_RK3568();
    void testDetectCPU_RK3588();

    // GPU 检测测试
    void testDetectGPU_WithDRM();
    void testDetectGPU_NoDRM();
    void testDetectGPU_OpenGLContext();

    // 内存检测测试
    void testDetectMemory_512MB();
    void testDetectMemory_1GB();
    void testDetectMemory_4GB();

    // 档位计算测试
    void testCalculateTier_IMX6ULL_returnsLow();
    void testCalculateTier_RK3568_returnsMid();
    void testCalculateTier_RK3588_returnsHigh();

    // 配置文件测试
    void testDeviceConfig_LoadDefault();
    void testDeviceConfig_OverrideTier();
    void testDeviceConfig_CustomScript();

    // 策略引擎测试
    void testCapabilityPolicy_LowTier();
    void testCapabilityPolicy_MidTier();
    void testCapabilityPolicy_HighTier();
    void testCapabilityPolicy_AnimationDisabledOnLow();

    // 边界情况
    void testEmptyProcFiles();
    void testMalformedConfig();
    void testTierOverride();
};
```

### 6.2 评分算法

```cpp
void HardwareProbe::calculateTier(HardwareInfo& info) {
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

    // 档位判定
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

## 七、关键文件路径

### 已实现文件

```
base/
├── system/
│   ├── cpu/
│   │   ├── cfcpu.h                    # CPU 检测接口
│   │   ├── cfcpu_bonus.h              # CPU 扩展信息
│   │   ├── cfcpu_profile.h            # CPU 性能分析
│   │   └── private/
│   │       ├── linux_impl/            # Linux 实现
│   │       └── win_impl/              # Windows 实现
│   └── memory/
│       ├── memory_info.h              # 内存检测接口
│       └── private/
│           ├── linux_impl/            # Linux 实现
│           └── win_impl/              # Windows 实现
└── include/
    ├── system/cpu/                    # CPU 公共头文件
    └── system/memory/                 # 内存公共头文件
```

### 待创建文件

```
base/
├── hardware/
│   ├── HWTier.h                       # 档位枚举
│   ├── HardwareInfo.h                 # 硬件信息结构体
│   ├── HardwareProbe.h                # 探针主类
│   ├── CapabilityPolicy.h             # 策略引擎
│   ├── DeviceConfig.h                 # 配置文件
│   ├── detectors/
│   │   ├── GPUDetector.h              # GPU 检测器
│   │   └── NetworkDetector.h          # 网络检测器
│   └── platform/
│       ├── LinuxDetector.cpp          # Linux 平台实现
│       └── WindowsDetector.cpp        # Windows 平台实现
```

### 测试文件

```
tests/
├── hardware/
│   ├── test_hardware_probe.cpp        # 主测试
│   ├── test_capability_policy.cpp     # 策略测试
│   └── mock/
│       └── proc/                      # Mock 数据
```

---

## 八、下一步行动建议

### 优先级1 (高)

1. **定义 HWTier 枚举**
   - 优先级: 最高
   - 理由: 其他模块依赖此定义
   - 预计工作量: 0.5天

### 优先级2 (中)

2. **实现 HardwareProbe 主类**
   - 优先级: 中
   - 理由: 整合所有检测器
   - 预计工作量: 2-3天

3. **实现 CapabilityPolicy**
   - 优先级: 中
   - 理由: 为上层提供策略配置
   - 预计工作量: 2-3天

4. **实现 DeviceConfig**
   - 优先级: 中
   - 理由: 支持手动配置覆盖
   - 预计工作量: 1-2天

### 优先级3 (低)

5. **创建 Mock 数据集**
   - 优先级: 低
   - 理由: 单元测试需要
   - 预计工作量: 1天

6. **编写单元测试**
   - 优先级: 低
   - 理由: 确保代码质量
   - 预计工作量: 3-4天

---

## 九、相关文档

- 原始TODO: ~~已归档~~（原 `01_hardware_probe.md` 已删除）
- 设计文档: `../../../design_stage/01_phase1_hardware_probe/`
- CPU 实现: `../../../HandBook/api/system/cpu/overview/`
- 内存实现: `../../../HandBook/api/system/memory/memory_info/`

---

*文档版本: v1.0*
*生成时间: 2026-03-11*
