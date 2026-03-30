# GPU API - 系统图形硬件信息

GPU API 提供跨平台的图形处理器和显示器信息查询功能，包括 GPU 设备属性、显示特性、环境能力评分和错误处理。

## 头文件

```cpp
#include "base/include/system/gpu/gpu.h"
```

命名空间：`cf`

## 核心 API

### getGpuDisplayInfo()

查询当前系统的 GPU 和显示器信息。

```cpp
cf::expected<GpuDisplayInfo, GpuDisplayInfoError> CF_BASE_EXPORT getGpuDisplayInfo() noexcept;
```

- 返回 `expected<GpuDisplayInfo, GpuDisplayInfoError>`
- `noexcept` 保证不抛出异常
- 查询失败时返回 `GpuDisplayInfoError`

### 使用示例

```cpp
#include "base/include/system/gpu/gpu.h"

auto result = cf::getGpuDisplayInfo();
if (result) {
    auto& info = result.value();
    qDebug() << "GPU:" << info.gpu.name.c_str();
    qDebug() << "Driver:" << info.gpu.driverVersion.c_str();
    qDebug() << "Display:" << info.display.width << "x" << info.display.height;
    qDebug() << "Score:" << info.score.level.c_str();
} else {
    qDebug() << "Failed to query GPU info";
}
```

## 数据结构

### GPUInfo

GPU 设备信息。

| 字段 | 类型 | 说明 |
|------|------|------|
| `name` | `std::string` | GPU 设备名称（人类可读） |
| `backend` | `std::string` | 图形后端 API 名称（如 OpenGL、Vulkan） |
| `driverVersion` | `std::string` | 驱动版本字符串 |
| `vendorId` | `uint32_t` | PCI 供应商 ID |
| `deviceId` | `uint32_t` | PCI 设备 ID |
| `isDiscrete` | `bool` | 是否为独立显卡 |
| `isIntegrated` | `bool` | 是否为集成显卡 |
| `hasSoftware` | `bool` | 是否支持软件渲染 |
| `isWSL` | `bool` | 是否在 WSL2 环境中运行 |

### DisplayInfo

显示器信息。

| 字段 | 类型 | 说明 |
|------|------|------|
| `width` | `int` | 显示宽度（像素） |
| `height` | `int` | 显示高度（像素） |
| `refreshRate` | `double` | 刷新率（Hz） |
| `dpi` | `double` | 物理像素密度（DPI） |
| `devicePixelRatio` | `double` | 逻辑/物理像素比 |
| `isVirtual` | `bool` | 是否为虚拟显示（WSLg、headless） |

### EnvironmentScore

环境能力评分。

| 字段 | 类型 | 说明 |
|------|------|------|
| `gpu` | `int` | GPU 能力评分 |
| `display` | `int` | 显示器能力评分 |
| `total` | `int` | 综合评分 |
| `level` | `std::string` | 定性能力等级（如 "low"、"medium"、"high"） |

### GpuDisplayInfo

聚合信息结构。

| 字段 | 类型 | 说明 |
|------|------|------|
| `gpu` | `GPUInfo` | GPU 设备信息 |
| `display` | `DisplayInfo` | 显示器信息 |
| `score` | `EnvironmentScore` | 环境能力评分 |

### GpuDisplayInfoError

错误码枚举。

| 值 | 说明 |
|------|------|
| `OK` | 无错误 |

## 平台实现

GPU API 使用平台特定的后端实现查询：

### Windows

- GPU 信息：使用 DXGI 查询
- 实现文件：`base/system/gpu/private/win_impl/gpu_info.cpp`
- 函数：`query_gpu_info_win()`, `query_display_info_win()`

### Linux

- GPU 信息：使用 DRM 和 sysfs 查询
- 支持桌面 Linux（DRM）和嵌入式 Linux（设备树）
- 实现文件：`base/system/gpu/private/linux_impl/gpu_info.cpp`
- 函数：`query_gpu_info_linux()`, `query_display_info_linux()`

## 内部数据结构

平台实现使用 `GPUInfoHost` 和 `DisplayInfoHost`（定义在 `base/system/gpu/private/gpu_host.h`）存储原始数据，然后转换为公开 API 的结构。

> 注意：`gpu_host.h` 是内部头文件，不应直接包含。

## 使用场景

```cpp
// 1. 根据 GPU 能力选择渲染策略
auto info = cf::getGpuDisplayInfo();
if (info && info->score.level == "high") {
    enableHighQualityRendering();
} else {
    enableFallbackRendering();
}

// 2. 检测 WSL2 环境
auto info = cf::getGpuDisplayInfo();
if (info && info->gpu.isWSL) {
    configureWSLRendering();
}

// 3. 获取显示信息用于布局计算
auto info = cf::getGpuDisplayInfo();
if (info) {
    float dpr = info->display.devicePixelRatio;
    int width = info->display.width;
    configureLayout(dpr, width);
}
```

## 相关文件

| 文件 | 说明 |
|------|------|
| `base/include/system/gpu/gpu.h` | 公开 API 头文件 |
| `base/system/gpu/gpu.cpp` | 公开 API 实现 |
| `base/system/gpu/private/gpu_host.h` | 内部数据结构 |
| `base/system/gpu/private/win_impl/gpu_info.h` | Windows 查询接口 |
| `base/system/gpu/private/linux_impl/gpu_info.h` | Linux 查询接口 |
| `base/system/gpu/CMakeLists.txt` | 构建配置 |
