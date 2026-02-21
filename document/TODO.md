# CFDesktop Phase 1 - 系统信息获取清单

> 创建日期: 2026-02-21
> 阶段: Phase 1 - 硬件探针与能力分级
> 状态: 进行中

---

## 概述

本文档列出了 CFDesktop 项目在 Phase 1 阶段需要获取的所有系统信息。这些信息将用于：
1. 硬件能力分级 (Low/Mid/High)
2. 系统行为裁剪与性能优化
3. 后续模块的功能适配

---

## 一、CPU 信息 (Central Processing Unit)

### 1.1 基础信息
- [ ] **CPU 型号名称** (Model Name)
  - Linux: `/proc/cpuinfo` 中的 `model name` / `Hardware` 字段
  - [x] Windows: WMIC `Win32_Processor.Name`
  - 用途: 识别 SoC 型号 (IMX6ULL, RK3568, RK3588 等)

- [ ] **CPU 制造商** (Vendor)
  - Linux: `/proc/cpuinfo` 中的 `vendor_id` / `CPU implementer`
  - [x]  Windows: WMIC `Win32_Processor.Manufacturer`
  - 用途: 区分 ARM/Intel/AMD 等架构

- [ ] **CPU 架构** (Architecture)
  - Linux: `uname -m` (armv7l, aarch64, x86_64, riscv64)
  - [x]  Windows: PROCESSOR_ARCHITECTURE 环境变量
  - 用途: 决定编译指令集优化

### 1.2 性能信息
- [ ] **逻辑核心数** (Logical Cores)
  - Linux: `/proc/cpuinfo` 处理器数量
  - Windows: `Win32_Processor.NumberOfLogicalProcessors`
  - 用途: 任务调度与线程池配置

- [ ] **物理核心数** (Physical Cores)
  - Linux: `/proc/cpuinfo` 中的 `cpu cores`
  - Windows: `Win32_Processor.NumberOfCores`
  - 用途: 性能评估

- [ ] **CPU 主频** (Frequency)
  - Linux: `/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq` (当前频率)
  - Linux: `/proc/cpuinfo` 中的 `BogoMIPS` (估算)
  - Windows: `Win32_Processor.MaxClockSpeed`
  - 用途: 性能评分

- [ ] **CPU 使用率** (Current Usage)
  - Linux: `/proc/stat` 计算
  - Windows: 性能计数器
  - 用途: 实时性能监控

### 1.3 特性与指令集
- [ ] **CPU 特性标志** (Features)
  - Linux: `/proc/cpuinfo` 中的 `Features` / `flags`
  - 常见特性: neon, vfpv4, aes, sha2, sve, avx2, avx512
  - 用途: 启用特定硬件加速

- [ ] **缓存信息** (Cache)
  - L1/L2/L3 缓存大小
  - Linux: `/sys/devices/system/cpu/cpu0/cache/`
  - 用途: 内存策略优化

### 1.4 温度与功耗
- [ ] **CPU 温度** (Temperature)
  - Linux: `/sys/class/thermal/thermal_zone*/temp`
  - 用途: 温控策略

- [ ] **能效信息** (Power/Efficiency)
  - ARM big.LITTLE / DynamIQ 架构检测
  - E-Core 与 P-Core 识别
  - 用途: 任务分配优化

---

## 二、GPU 信息 (Graphics Processing Unit)

### 2.1 基础信息
- [ ] **GPU 渲染器** (Renderer)
  - 通过 OpenGL 上下文获取 `GL_RENDERER`
  - 用途: GPU 型号识别

- [ ] **GPU 供应商** (Vendor)
  - 通过 OpenGL 上下文获取 `GL_VENDOR`
  - 常见: ARM, NVIDIA, AMD, Intel, Vivante, Mali
  - 用途: 驱动适配

- [ ] **驱动版本** (Driver Version)
  - OpenGL `GL_VERSION`
  - 用途: 兼容性判断

### 2.2 能力检测
- [ ] **硬件加速支持** (Hardware Acceleration)
  - Linux: `/dev/dri/card*` 设备检测
  - Windows: DirectX / OpenGL 支持
  - 用途: 决定是否启用硬件加速

- [ ] **OpenGL 版本** (OpenGL Version)
  - OpenGL ES 2.0 / 3.0 / 3.1 / 3.2
  - 用途: 着色器版本选择

- [ ] **OpenGL 扩展列表** (Extensions)
  - `GL_NUM_EXTENSIONS` 和 `GL_EXTENSIONS`
  - 关键扩展:
    - `GL_OES_texture_npot` - 非幂次纹理
    - `GL_EXT_texture_compression_s3tc` - 纹理压缩
    - `GL_OES_compressed_ETC1_RGB8_texture` - ETC1 压缩
  - 用途: 功能裁剪

- [ ] **最大纹理尺寸** (Max Texture Size)
  - `GL_MAX_TEXTURE_SIZE`
  - `GL_MAX_RENDERBUFFER_SIZE`
  - 用途: 纹理缓存策略

- [ ] **帧缓冲支持** (Framebuffer Support)
  - FBO (Framebuffer Object) 支持
  - 多重渲染目标 (MRT) 数量
  - 用途: 高级特效支持判断

### 2.3 特定平台检测
- [ ] **DRM 设备信息** (Linux)
  - `/dev/dri/card0`, `/dev/dri/renderD128`
  - `drm` 版本信息
  - 用途: 硬件视频解码器检测

- [ ] **Vulkan 支持** (Optional)
  - VkInstance 创建测试
  - 用途: 高级渲染管线

---

## 三、内存信息 (Memory)

### 3.1 物理内存
- [ ] **总内存** (Total Memory)
  - Linux: `/proc/meminfo` 中的 `MemTotal`
  - Windows: `Win32_ComputerSystem.TotalPhysicalMemory`
  - 用途: 档位评分关键指标

- [ ] **可用内存** (Available Memory)
  - Linux: `/proc/meminfo` 中的 `MemAvailable`
  - Windows: `Win32_OperatingSystem.FreePhysicalMemory`
  - 用途: 动态资源分配

- [ ] **空闲内存** (Free Memory)
  - Linux: `/proc/meminfo` 中的 `MemFree`
  - 用途: 内存压力检测

### 3.2 虚拟内存 / 交换分区
- [ ] **总交换空间** (Total Swap)
  - Linux: `/proc/meminfo` 中的 `SwapTotal`
  - Windows: `Win32_PageFileUsage.AllocatedBaseSize`
  - 用途: 内存溢出保护策略

- [ ] **可用交换空间** (Free Swap)
  - Linux: `/proc/meminfo` 中的 `SwapFree`
  - 用途: OOM 判断

### 3.3 内存详细信息
- [ ] **内存条信息** (DIMM Info)
  - 数量、类型 (DDR3/DDR4/LPDDR4)
  - 频率、时序
  - Linux: `dmidecode`
  - 用途: 带宽估算

- [ ] **共享内存** (Shared Memory)
  - Linux: `/proc/meminfo` 中的 `Shmem`
  - 用途: GPU 共享内存计算

- [ ] **缓冲/缓存内存** (Buffers/Cache)
  - Linux: `/proc/meminfo` 中的 `Buffers`, `Cached`
  - 用途: 可回收内存判断

### 3.4 内存使用统计
- [ ] **进程内存占用**
  - 当前进程 RSS (Resident Set Size)
  - `/proc/self/status` 中的 `VmRSS`
  - 用途: 内存泄漏监控

---

## 四、存储信息 (Storage)

### 4.1 磁盘空间
- [ ] **总容量** (Total Capacity)
  - `statvfs()` 系统调用
  - 用途: 缓存策略

- [ ] **可用空间** (Available Space)
  - `statvfs()` 系统调用
  - 用途: 日志与缓存大小限制

- [ ] **已用空间** (Used Space)
  - 用途: 磁盘满警告

### 4.2 存储设备信息
- [ ] **存储介质类型**
  - eMMC, SD Card, SSD, HDD, NVMe
  - Linux: `/sys/block/*/queue/rotational`
  - 用途: I/O 策略

- [ ] **存储设备名称** (Device Name)
  - `/dev/mmcblk0`, `/dev/sda`, 等
  - 用途: 性能预测

### 4.3 性能指标
- [ ] **读写速度** (I/O Throughput)
  - 可选: 基准测试
  - 用途: 大文件处理策略

---

## 五、网络信息 (Network)

### 5.1 网络接口
- [ ] **接口列表** (Interface List)
  - Linux: `/sys/class/net/`
  - Windows: `GetAdaptersAddresses`
  - 用途: 网络状态检测

- [ ] **接口状态** (Interface State)
  - 启用/禁用状态 (`operstate`)
  - Linux: `/sys/class/net/<if>/operstate`
  - 用途: 连接状态

- [ ] **接口类型** (Interface Type)
  - 有线 (eth0, enp*) / 无线 (wlan0, wlp*)
  - 虚拟接口 (lo, docker*, veth*)
  - Linux: 检查 `wireless` 目录存在性
  - 用途: 网络图标显示

### 5.2 网络地址
- [ ] **MAC 地址** (Hardware Address)
  - Linux: `/sys/class/net/<if>/address`
  - Windows: `GetAdaptersInfo`
  - 用途: 设备唯一标识

- [ ] **IPv4 地址** (IPv4 Address)
  - 通过 `ioctl` 或 `getifaddrs` 获取
  - 用途: 网络显示

- [ ] **IPv6 地址** (IPv6 Address)
  - 通过 `getifaddrs` 获取
  - 用途: 网络显示

### 5.3 网络状态
- [ ] **连接状态**
  - 是否有外部连接
  - 可通过 ping 或连接检测

- [ ] **网络速度** (Link Speed)
  - Linux: `/sys/class/net/<if>/speed`
  - 用途: 带宽自适应

---

## 六、显示信息 (Display)

### 6.1 屏幕信息
- [ ] **屏幕分辨率** (Resolution)
  - Qt: `QScreen::geometry()`
  - 用途: UI 布局适配

- [ ] **物理尺寸** (Physical Size)
  - Qt: `QScreen::physicalSize()`
  - 用途: DPI 计算

- [ ] **DPI / PPI** (Dots Per Inch)
  - Qt: `QScreen::physicalDotsPerInch()`
  - 用途: 字体缩放

- [ ] **刷新率** (Refresh Rate)
  - Qt: `QScreen::refreshRate()`
  - 用途: 动画帧率限制

### 6.2 多显示器
- [ ] **显示器数量**
  - Qt: `QGuiApplication::screens()`
  - 用途: 多屏支持判断

- [ ] **主显示器** (Primary Screen)
  - Qt: `QGuiApplication::primaryScreen()`
  - 用途: 默认窗口位置

### 6.3 显示能力
- [ ] **色彩深度** (Color Depth)
  - Qt: `QScreen::depth()`
  - 用途: 图像格式选择

- [ ] **HDR 支持** (HDR Support)
  - 可选高级检测
  - 用途: HDR 内容渲染

---

## 七、电池与电源信息 (Power & Battery)

### 7.1 电池状态 (Battery)
- [ ] **电池存在性** (Battery Present)
  - Linux: `/sys/class/power_supply/`
  - 检测 BAT* 或 battery 目录
  - 用途: 移动设备适配

- [ ] **电池容量** (Capacity)
  - Linux: `/sys/class/power_supply/*/capacity`
  - 当前百分比 (0-100)
  - 用途: 低电量模式

- [ ] **电池状态** (Battery Status)
  - Linux: `/sys/class/power_supply/*/status`
  - Charging / Discharging / Full / Unknown
  - 用途: 充电指示

- [ ] **电池健康度** (Battery Health)
  - Linux: `/sys/class/power_supply/*/capacity_level`
  - 或 `/sys/class/power_supply/*/charge_full_design`
  - 用途: 电池老化警告

### 7.2 电源状态
- [ ] **电源连接状态** (AC Power)
  - Linux: `/sys/class/power_supply/*/online` (AC adapter)
  - 用途: 充电动画

- [ ] **电源类型** (Power Supply Type)
  - Battery / USB / Mains / Unknown
  - Linux: `/sys/class/power_supply/*/type`
  - 用途: 充电速度判断

### 7.3 能耗管理
- [ ] **当前功耗** (Current Power Draw)
  - Linux: `/sys/class/power_supply/*/current_now`
  - 用途: 功耗监控

- [ ] **电压** (Voltage)
  - Linux: `/sys/class/power_supply/*/voltage_now`
  - 用途: 电源诊断

---

## 八、系统信息 (System)

### 8.1 操作系统
- [ ] **系统名称** (OS Name)
  - Linux: `/etc/os-release` 中的 `NAME` / `PRETTY_NAME`
  - Windows: `Win32_OperatingSystem.Caption`
  - 用途: 系统识别

- [ ] **系统版本** (OS Version)
  - Linux: `/etc/os-release` 中的 `VERSION`
  - Windows: `Win32_OperatingSystem.Version`
  - 用途: 兼容性判断

- [ ] **内核版本** (Kernel Version)
  - Linux: `uname -r`
  - 用途: 特性支持判断

- [ ] **系统架构** (System Architecture)
  - Linux: `uname -m`
  - Windows: `PROCESSOR_ARCHITECTURE`
  - 用途: 二进制兼容性

### 8.2 主机信息
- [ ] **主机名** (Hostname)
  - `gethostname()` 系统调用
  - 用途: 网络识别

- [ ] **主板型号** (Board Model)
  - Linux: `/sys/class/dmi/id/board_name`
  - 或 `/sys/firmware/devicetree/base/model`
  - 用途: 设备识别

- [ ] **设备树 compatible** (Device Tree)
  - Linux: `/sys/firmware/devicetree/base/compatible`
  - 用途: 嵌入式设备识别

### 8.3 运行时信息
- [ ] **系统运行时间** (Uptime)
  - Linux: `/proc/uptime` 或 `sysinfo()`
  - Windows: `GetTickCount64`
  - 用途: 诊断

- [ ] **当前用户** (Current User)
  - `getlogin()` 或环境变量
  - 用途: 权限判断

- [ ] **负载平均值** (Load Average)
  - Linux: `/proc/loadavg`
  - 用途: 性能监控

---

## 九、输入设备信息 (Input Devices)

### 9.1 指针设备
- [ ] **鼠标存在性** (Mouse Presence)
  - Linux: `/dev/input/mouse*` 检测
  - Qt: `QPointingDevice::devices()`
  - 用途: UI 交互模式

- [ ] **触摸板存在性** (Touchpad Presence)
  - Linux: `evtest` 或 X11 输入检测
  - 用途: 手势支持

### 9.2 键盘设备
- [ ] **键盘存在性** (Keyboard Presence)
  - Linux: `/dev/input/event*` 检测
  - Qt: `QKeyboardDevice::devices()`
  - 用途: 快捷键支持

### 9.3 触摸设备
- [ ] **触摸屏存在性** (Touchscreen Presence)
  - Linux: `/dev/input/event*` 中触摸设备检测
  - Qt: `QPointingDevice::deviceType() == QInputDevice::DeviceType::TouchScreen`
  - 用途: 触摸交互

- [ ] **触摸点数量** (Touch Points)
  - Qt: `QPointingDevice::maximumTouchPoints()`
  - 用途: 多点触控支持

### 9.4 其他输入
- [ ] **游戏手柄** (Gamepad/Joystick)
  - Qt: `QGamepadManager`
  - 用途: 游戏支持

- [ ] **遥控器** (Remote Control)
  - Linux: lirc 或 evdev 检测
  - 用途: 遥控器支持

---

## 十、音频信息 (Audio)

### 10.1 音频设备
- [ ] **音频输出设备** (Audio Output)
  - Linux: ALSA `/dev/snd/` 或 PulseAudio
  - 用途: 音频播放

- [ ] **音频输入设备** (Audio Input)
  - Linux: 麦克风检测
  - 用途: 录音功能

### 10.2 音频能力
- [ ] **音频格式支持** (Audio Formats)
  - PCM, AC3, DTS 等
  - 用途: 音频解码

- [ ] **音频通道数** (Audio Channels)
  - 单声道/立体声/5.1/7.1
  - 用途: 音频输出配置

---

## 十一、视频/编解码信息 (Video/Codec)

### 11.1 视频解码能力
- [ ] **硬件视频解码支持** (Hardware Video Decode)
  - Linux: V4L2 `/dev/video*` 检测
  - Linux: DRM 中的解码器检测
  - 用途: 视频播放优化

- [ ] **支持的编解码格式** (Supported Codecs)
  - H.264, H.265, VP8, VP9, AV1
  - 通过 media/v4l2 接口查询
  - 用途: 视频格式选择

### 11.2 视频编码能力
- [ ] **硬件视频编码支持** (Hardware Video Encode)
  - 检测编码器设备
  - 用途: 录屏/编码功能

---

## 十二、安全与DRM (Security & DRM)

### 12.1 安全特性
- [ ] **Secure Boot 状态**
  - Linux: `/sys/firmware/efi/efivars/SecureBoot-*`
  - 用途: 安全检测

### 12.2 DRM
- [ ] **DRM 支持** (Digital Rights Management)
  - Widevine, PlayReady 等
  - 用途: 受保护内容播放

---

## 十三、虚拟化与容器 (Virtualization)

### 13.1 虚拟化检测
- [ ] **是否运行在虚拟机** (Virtual Machine Detection)
  - 检测 `dmi` 中的系统制造商
  - 检测特定的 CPUID 指令
  - 用途: 性能预期调整

- [ ] **容器检测** (Container Detection)
  - 检测 `.dockerenv` 或 `/proc/1/cgroup`
  - 用途: 行为适配

---

## 十四、热插拔支持 (Hotplug)

### 14.1 可热插拔设备
- [ ] **USB 设备检测** (USB Devices)
  - Linux: `/sys/bus/usb/devices/`
  - 用途: 外设支持

- [ ] **PCI 设备检测** (PCI Devices)
  - Linux: `/sys/bus/pci/devices/`
  - 用途: 扩展卡支持

---

## 十五、环境信息 (Environment)

### 15.1 系统环境
- [ ] **语言/区域** (Locale)
  - `setlocale()` 或环境变量 `LANG`
  - 用途: 本地化

- [ ] **时区** (Timezone)
  - `/etc/localtime` 链接或 `tzset()`
  - 用途: 时间显示

- [ ] **环境变量** (Environment Variables)
  - Qt 相关: `QT_QPA_PLATFORM`, `QT_LOGGING_RULES`
  - 用途: 调试与配置

---

## 十六、Qt 相关信息

### 16.1 Qt 环境
- [ ] **Qt 版本** (Qt Version)
  - `qVersion()` 或 `QT_VERSION_STR`
  - 用途: 兼容性判断

- [ ] **Qt 平台插件** (Qt Platform Plugin)
  - `QGuiApplication::platformName()`
  - 可能值: eglfs, linuxfb, xcb, windows
  - 用途: 渲染后端确认

- [ ] **编译器版本** (Compiler Version)
  - `QSysInfo::buildCpuArchitecture()`
  - 用途: 编译时特性判断

---

## 十七、性能基准测试 (Benchmark) - 可选

### 17.1 CPU 性能
- [ ] **浮点运算性能**
  - 简单的数学运算基准
  - 用途: 更精确的性能分级

### 17.2 内存性能
- [ ] **内存带宽测试**
  - 顺序读写测试
  - 用途: 缓存策略

### 17.3 GPU 性能
- [ ] **图形渲染性能**
  - 简单的渲染测试
  - 用途: 动画复杂度选择

---

## 实现优先级

### P0 - 核心必需 (Phase 1 必须完成)
这些信息直接影响硬件档位判定和核心功能：

1. CPU 型号、核心数、主频
2. GPU 硬件加速、OpenGL 版本
3. 总内存、可用内存
4. 屏幕分辨率、刷新率
5. 电池状态 (移动设备)

### P1 - 重要 (Phase 1 推荐)
这些信息用于策略优化：

6. 存储空间
7. 网络接口状态
8. 系统架构与版本
9. 输入设备类型

### P2 - 可选 (后续阶段)
这些信息用于增强功能：

10. 音频设备
11. 视频编解码能力
12. 温度与功耗
13. 性能基准测试

---

## 数据结构映射

上述信息将映射到以下数据结构：

```cpp
// 核心结构
struct HardwareInfo {
    HWTier tier;
    CPUInfo cpu;
    GPUInfo gpu;
    MemoryInfo memory;
    QList<NetworkInterface> networkInterfaces;
    QString deviceTreeCompatible;
    QString boardName;
    bool isUserOverridden;
};

// 扩展结构 (新增建议)
struct SystemInfo {
    QString osName;
    QString osVersion;
    QString kernelVersion;
    QString architecture;
    QString hostname;
    quint64 uptime;
};

struct DisplayInfo {
    QSize resolution;
    QSize physicalSize;
    qreal dpi;
    qreal refreshRate;
    int colorDepth;
    int screenCount;
};

struct BatteryInfo {
    bool present;
    int capacity;      // 0-100
    QString status;    // Charging/Discharging/Full
    bool isOnACPower;
};

struct InputDeviceInfo {
    bool hasMouse;
    bool hasKeyboard;
    bool hasTouchscreen;
    int maxTouchPoints;
    bool hasGamepad;
};
```

---

## 注意事项

1. **性能**: 硬件检测应在 500ms 内完成
2. **缓存**: 检测结果应缓存，避免重复检测
3. **异步**: 部分检测可异步进行，不阻塞启动
4. **容错**: 任何检测失败都不应导致程序崩溃
5. **跨平台**: Linux 和 Windows 使用不同的检测方法
6. **模拟器**: 应支持模拟器模式，可注入 Mock 数据

---

## 相关文件

- 设计文档: [document/design_stage/01_phase1_hardware_probe.md](design_stage/01_phase1_hardware_probe.md)
- 代码目录: [base/hardware/](../base/hardware/)
- 测试目录: [test/hardware_test/](../test/hardware_test/)

---

*最后更新: 2026-02-21*
