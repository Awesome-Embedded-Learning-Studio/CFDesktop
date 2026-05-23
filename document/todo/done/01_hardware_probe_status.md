---
title: "Phase 1: 硬件探针与能力分级 - 状态文档"
description: "模块ID: Phase 1，状态: 🚧 部分完成"
---

# Phase 1: 硬件探针与能力分级 - 状态文档

> **模块ID**: Phase 1
> **状态**: ✅ 完成
> **总体进度**: 100%
> **最后更新**: 2026-05-23

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
| HWTier 系统 | 100% | ✅ 完成 |
| CapabilityPolicy | 100% | ✅ 完成 |
| HardwareProbe 主类 | 100% | ✅ 完成 |

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
```yaml

---

## 五、HWTier 系统 (Phase 1 核心功能) — ✅ 已完成

> **注意**: GPU 和 Network 检测器已完成，HWTier 系统已实现，Phase 1 核心功能全部完成。

### 5.1 HWTier 枚举定义 ✅

**实现位置**: `base/include/system/hardware_tier/hardware_tier_data.h`

- [x] `HardwareTierLevel` 枚举 (Unknown/Low/Mid/High)
- [x] 档位字符串转换函数 `hardwareTierLevelToString()`
- [x] 各档位对应硬件配置说明 (i.MX6ULL / RK3568 / RK3588)

### 5.2 硬件数据收集器 ✅

**实现位置**: `base/system/hardware_tier/hardware_tier_collector.h`, `base/system/hardware_tier/default/default_collector.cpp`

- [x] `IHardwareCollector` 接口 — 可插拔收集器
- [x] `HardwareData` 结构体 — CPU/GPU/Memory/Display 原始数据
- [x] 默认收集器 — 整合 CPU/GPU/Memory/Display 检测器
- [x] 跨平台支持 (Linux/Windows)

### 5.3 评分引擎 ✅

**实现位置**: `base/system/hardware_tier/hardware_tier_scorer.h`, `base/system/hardware_tier/default/default_*_scorer.cpp`

- [x] `IHardwareScorer` 接口 — 可插拔评分器
- [x] 维度评分类型: `CpuScore`, `GpuScore`, `MemoryScore`, `DisplayScore` (各 0-100)
- [x] 默认评分器: CPU/GPU/Memory/Display 四维度独立评分
- [x] 可通过 `registerScorer()` 替换各维度评分逻辑

### 5.4 档位评估器 ✅

**实现位置**: `base/system/hardware_tier/hardware_tier_assessor.h`, `base/system/hardware_tier/default/default_assessor.cpp`

- [x] `IHardwareAssessor` 接口 — 可插拔评估器
- [x] `HardwareTierAssessment` 结构体 — 四维度评分 + 总档位 + 覆盖信息
- [x] 默认评估器 — 综合四维度分数计算档位

### 5.5 策略引擎 ✅

**实现位置**: `base/system/hardware_tier/hardware_tier_policy.h`, `base/system/hardware_tier/default/default_policy.cpp`

- [x] `IHardwarePolicy` 接口 — 可插拔策略
- [x] `HardwareTierCapabilities` 结构体 — 能力标志 (OpenGL/软件渲染/动画/硬件解码/EGLFS/LinuxFB)
- [x] 默认策略 — 根据 Low/Mid/High 档位映射能力标志
- [x] 部分动画支持 (`enable_partial_animation`)

### 5.6 DeviceConfig 覆盖 ✅

**实现位置**: `base/include/system/hardware_tier/hardware_tier.h`

- [x] `setDeviceConfigOverride()` — 手动强制档位
- [x] `clearDeviceConfigOverride()` — 清除覆盖
- [x] 覆盖时跳过收集/评分，直接使用指定档位
- [x] 覆盖原因记录 (`override_reason`)

### 5.7 管线注册 API ✅

**实现位置**: `base/include/system/hardware_tier/hardware_tier.h`

- [x] `registerCollector()` — 注册自定义收集器
- [x] `registerScorer()` — 注册自定义评分器 (按维度)
- [x] `registerAssessor()` — 注册自定义评估器
- [x] `registerPolicy()` — 注册自定义策略
- [x] `assessHardware()` — 执行完整管线 (缓存支持)
- [x] `getHardwareTierCapabilities()` — 查询能力标志

### 5.8 示例代码 ✅

**实现位置**: `example/base/system/example_hardware_tier.cpp`

- [x] 完整的硬件分级评估示例
- [x] 四维度评分展示
- [x] 能力标志展示

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
```text

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
```yaml

---

## 七、关键文件路径

### 已实现文件

```text
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
```text

### 待创建文件

> HWTier 系统已全部实现，无待创建文件。

```text
base/
├── include/system/hardware_tier/
│   ├── hardware_tier.h                # 公共 API (管线注册 + 评估入口)
│   └── hardware_tier_data.h           # 数据结构 (枚举/评分/结果/能力标志)
├── system/hardware_tier/
│   ├── hardware_tier_collector.h      # IHardwareCollector 接口
│   ├── hardware_tier_scorer.h         # IHardwareScorer 接口
│   ├── hardware_tier_assessor.h       # IHardwareAssessor 接口
│   ├── hardware_tier_policy.h         # IHardwarePolicy 接口
│   ├── default_factories.h            # 默认工厂函数
│   ├── hardware_tier.cpp              # 管线实现
│   └── default/
│       ├── default_collector.cpp      # 默认收集器
│       ├── default_cpu_scorer.cpp     # 默认 CPU 评分
│       ├── default_gpu_scorer.cpp     # 默认 GPU 评分
│       ├── default_memory_scorer.cpp  # 默认 Memory 评分
│       ├── default_display_scorer.cpp # 默认 Display 评分
│       ├── default_assessor.cpp       # 默认评估器
│       └── default_policy.cpp         # 默认策略
```text

---

## 八、下一步行动建议

### 已完成 ✅

1. **HWTier 枚举定义** — ✅ 已实现 (`HardwareTierLevel`)
2. **HardwareProbe 管线** — ✅ 已实现 (可插拔 Collect→Score→Assess→Policy 四阶段管线)
3. **CapabilityPolicy 策略引擎** — ✅ 已实现 (`IHardwarePolicy` + 默认策略)
4. **DeviceConfig 覆盖** — ✅ 已实现 (`setDeviceConfigOverride()`)

### 后续可选增强

1. **集成 ConfigStore 查询覆盖** — 从 ConfigStore 读取设备档位配置，自动调用 `setDeviceConfigOverride()`
2. **自定义检测脚本执行** — 支持外部脚本注入硬件数据
3. **Mock 数据集** — 单元测试用模拟数据
4. **性能测试** — 各评分器在不同硬件下的基准测试

---

## 九、相关文档

- 原始TODO: ~~已归档~~（原 `01_hardware_probe.md` 已删除）
- 设计文档: `../../../design_stage/01_phase1_hardware_probe/`
- CPU 实现: `../../../HandBook/api/system/cpu/overview/`
- 内存实现: `../../../HandBook/api/system/memory/memory_info/`

---

*文档版本: v2.0*
*生成时间: 2026-03-11*
*最后更新: 2026-05-23 (HWTier 系统完成)*
