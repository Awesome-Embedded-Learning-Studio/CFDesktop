# CFDesktop 系统架构总览

> **状态**: 已实现
> **版本**: 0.13.1
> **最后更新**: 2026-03-30

---

## 一、项目概述

**CFDesktop** 是一个基于 Qt 6 / C++23 的 Material Design 3 桌面环境框架，面向嵌入式设备设计，同时支持在 Windows 和 Linux/WSL 上以 Client 模式运行。

| 属性 | 值 |
|------|-----|
| 定位 | 便携式桌面环境框架 |
| 版本 | 0.13.1 |
| 语言 | C++23 |
| UI 框架 | Qt 6.8.3 |
| 设计系统 | Material Design 3 |
| 构建系统 | CMake 3.16+ |
| 目标平台 | Windows 10/11, Linux/WSL, 嵌入式 ARM |

### 设计目标

1. **跨平台统一**：同一套 Shell / Panel / WindowManager 代码在所有平台复用
2. **Material Design 3**：完整的 MD3 设计系统实现（颜色、排版、形状、动效）
3. **嵌入式适配**：动态检测硬件能力，优雅降级到低性能模式
4. **可扩展性**：通过 Factory / Strategy 模式轻松添加新平台后端

---

## 二、模块层次与依赖

### 2.1 三层模块架构

```
┌─────────────────────────────────────────────────────────┐
│                    desktop 模块                          │
│         桌面环境实现 — Shell, Panel, 后端               │
├─────────────────────────────────────────────────────────┤
│                      ui 模块                             │
│      UI 框架 — Material Design 3 组件与动画             │
├─────────────────────────────────────────────────────────┤
│                     base 模块                            │
│     基础库 — 工具、系统检测、工厂模式基础设施           │
├─────────────────────────────────────────────────────────┤
│                    Qt 6 / OS API                         │
└─────────────────────────────────────────────────────────┘
```

**依赖规则**：`desktop` → `ui` → `base`，严格单向依赖。

### 2.2 模块职责一览

| 模块 | 子模块 | 构建目标 | 职责 |
|------|--------|----------|------|
| **base** | `include/` | `cfbase_headers` | 头文件工具库（Factory、Singleton、WeakPtr、宏） |
| | `system/cpu/` | `cfbase_cpu` | CPU 检测与性能分级 |
| | `system/memory/` | `cfbase_memory` | 内存检测 |
| | `system/gpu/` | `cfbase_gpu` | GPU 检测 |
| | `system/network/` | `cfbase_network` | 网络状态检测 |
| | `device/console/` | `cfbase_console` | 控制台工具 |
| | — | **`cfbase`** | 统一聚合静态库 |
| **ui** | `base/` | `cf_ui_base` | 数学、颜色、几何、easing |
| | `core/` | `cf_ui_core` | 主题引擎、设计 Token |
| | `components/` | `cf_ui_components` | 动画框架 |
| | `widget/material/` | `cf_ui_components_material` | Material Design 3 控件 |
| | `widget/` | `cf_ui_widget` | Widget 适配层 |
| | `models/` | — | 数据模型 |
| | — | **`cfui`** | 统一聚合静态库 |
| **desktop** | `main/` | `CFDesktopMain` | 初始化链、启动入口 |
| | `ui/components/` | `cf_desktop_components` | 核心接口（IWindow, IWindowBackend, IDisplayServerBackend） |
| | `ui/platform/` | `cf_desktop_ui_platform` | 平台抽象层（Windows / WSL） |
| | `ui/render/` | `cf_desktop_render` | 渲染后端抽象 |
| | `ui/widget/` | — | 桌面特有 Widget |
| | `base/` | — | 配置、日志、文件操作 |
| | — | **`CFDesktop_shared`** | 统一聚合共享库（.dll/.so） |
| | — | **`CFDesktop`** | 薄壳可执行文件 |

### 2.3 最终构建产物

```
CFDesktop.exe / CFDesktop           ← 薄壳 main.cpp
    └── CFDesktop_shared.dll/.so    ← 统一共享库（聚合所有静态库）
         ├── CFDesktopMain          ← 初始化与启动
         ├── CFDesktopUi            ← 桌面 UI 聚合
         │    ├── cf_desktop_ui_platform  ← 平台抽象层
         │    ├── cf_desktop_components    ← 核心接口
         │    └── cf_desktop_render        ← 渲染抽象
         ├── cfui                   ← UI 框架聚合
         │    ├── cf_ui_base
         │    ├── cf_ui_core
         │    ├── cf_ui_components
         │    └── cf_ui_widget
         ├── cfbase                 ← 基础库聚合
         │    ├── cfbase_cpu
         │    ├── cfbase_memory
         │    ├── cfbase_gpu
         │    └── cfbase_console
         ├── cflogger
         ├── cffilesystem
         ├── cfconfig
         └── cfasciiart
```

---

## 三、核心接口体系

### 3.1 接口层次关系

```
┌─────────────────────────────────────────────────────┐
│              IDisplayServerBackend                   │
│  (顶层抽象：角色、能力、生命周期、事件循环)         │
│                                                      │
│  ┌────────────────────────────────────────────────┐  │
│  │              IWindowBackend                    │  │
│  │  (窗口创建/销毁/跟踪，发出 window_came/gone)   │  │
│  │                                                 │  │
│  │  ┌──────────────────────────────────────────┐  │  │
│  │  │              IWindow                     │  │  │
│  │  │  (单个窗口：ID、标题、几何、关闭、置顶)  │  │  │
│  │  └──────────────────────────────────────────┘  │  │
│  └────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────┘
```

### 3.2 IDisplayServerBackend

**文件**: `desktop/ui/components/IDisplayServerBackend.h`

顶层显示后端抽象，定义 CFDesktop 在显示栈中的角色：

```cpp
enum class DisplayServerRole {
    Client,       // 在已有桌面环境中运行
    Compositor,   // CFDesktop 就是显示服务器
    DirectRender  // 直接渲染到 framebuffer
};

struct DisplayServerCapabilities {
    DisplayServerRole role;
    bool canManageExternalWindows;    // 能否管理外部应用窗口
    bool needsOwnCompositor;          // 是否需要自己的合成场景
    bool supportsWaylandProtocol;
    bool supportsX11Protocol;
};
```

**核心方法**:

| 方法 | 说明 |
|------|------|
| `role()` | 返回运行角色 |
| `capabilities()` | 返回能力描述 |
| `initialize(argc, argv)` | 初始化后端 |
| `shutdown()` | 关闭后端 |
| `runEventLoop()` | 运行事件循环（通常调用 `QApplication::exec()`） |
| `windowBackend()` | 返回窗口后端弱引用 |
| `outputs()` | 返回屏幕几何列表 |

**信号**:

| 信号 | 触发时机 |
|------|---------|
| `outputChanged()` | 屏幕增减或分辨率变化 |
| `externalWindowAppeared()` | 外部应用窗口出现 |
| `externalWindowDisappeared()` | 外部应用窗口消失 |

### 3.3 IWindowBackend

**文件**: `desktop/ui/components/IWindowBackend.h`

窗口创建与跟踪接口：

| 方法 | 说明 |
|------|------|
| `createWindow(appId)` | 创建内部窗口 |
| `destroyWindow(window)` | 销毁窗口 |
| `windows()` | 返回所有跟踪中的窗口 |
| `capabilities()` | 返回渲染能力 |
| `make_weak()` | 返回后端弱引用 |

**信号**: `window_came(WeakPtr<IWindow>)`, `window_gone(WeakPtr<IWindow>)`

### 3.4 IWindow

**文件**: `desktop/ui/components/IWindow.h`

平台无关的窗口抽象：

| 方法 | 说明 |
|------|------|
| `windowID()` | 唯一标识符（`win_id_t` = `QString`） |
| `title()` | 窗口标题 |
| `geometry()` | 窗口几何（设备像素） |
| `set_geometry(r)` | 移动/缩放窗口 |
| `requestClose()` | 请求关闭 |
| `raise()` | 置顶 |

**信号**: `closeRequested()`, `titleChanged()`, `geometryChanged()`

### 3.5 信号流转

```
外部窗口出现（OS 层事件）
    │
    ▼
IWindowBackend::registerWindow()          ← 平台后端检测到新窗口
    │
    ├─ 创建 IWindow 实例
    ├─ emit window_came(weak)
    │
    ▼
IDisplayServerBackend                     ← 转发信号
    │  (connect IWindowBackend::window_came → externalWindowAppeared)
    │
    ▼
CFDesktopEntity::run_init()               ← 日志记录 + Shell 分发
    │  (connect IWindowBackend::window_came → slot)
    │
    ▼
WindowManager / ShellLayer                ← UI 响应
```

---

## 四、平台抽象层

### 4.1 设计模式

平台抽象层使用两种互补的设计模式：

**Factory 模式** — 创建平台特定对象：

```
┌─────────────────────────────────────────────────┐
│          RegisteredFactory<Interface>            │
│  creator_: std::function<Interface*()>          │
│  destroyer_: std::function<void(Interface*)>    │
│                                                  │
│  make_unique() → unique_ptr<Interface, Destroyer>│
│  make_shared() → shared_ptr<Interface>           │
├─────────────────────────────────────────────────┤
│       StaticRegisteredFactory<Interface>         │
│  (单例 + RegisteredFactory)                      │
├─────────────────────────────────────────────────┤
│   DisplayServerBackendFactory                    │
│  : StaticRegisteredFactory<IDisplayServerBackend>│
└─────────────────────────────────────────────────┘
```

**Strategy 模式** — 封装平台特定行为：

```
IDesktopPropertyStrategy
├── IDesktopDisplaySizeStrategy       ← 窗口尺寸与行为策略
│    ├── WindowsDisplaySizePolicy     ← 无边框、置底、避免系统 UI
│    └── LinuxWSLDisplaySizePolicy    ← WSL 环境下的窗口策略
└── (可扩展其他策略类型)
```

### 4.2 平台分发机制

每个平台目录提供两个分发函数，由公共 Helper 统一调度：

```
platform_helper.h            ← 通用接口
    native_impl()            → PlatformFactoryAPI（策略工厂）
    native_display()         → DisplayBackendFactoryAPI（后端工厂）

display_backend_helper.h     ← 显示后端专用接口
    native_display()         → 转发到 native_display_impl()
    native_display_impl()    ← 每个平台实现此函数
```

**调用链**：

```
CFDesktopEntity 构造
    │
    ├─ platform::native_display()
    │    └─ native_display_impl()            ← 平台分发
    │         ├─ Windows: new WindowsDisplayServerBackend
    │         └─ WSL:     new WSLDisplayServerBackend
    │
    └─ DisplayServerBackendFactory::register_creator(creator, release)
```

### 4.3 WSL 检测

**文件**: `cmake/env_check/detail_os_checker.cmake`

三重检测机制（按优先级）：

1. **环境变量**: `WSL_DISTRO_NAME` 存在
2. **内核版本**: `uname -r` 包含 `microsoft` 或 `wsl`
3. **procfs**: `/proc/version` 包含 `microsoft` 或 `wsl`

检测到 WSL 后，CMake 设置 `IS_WSL=TRUE` 并定义 `CFDESKTOP_OS_WSL` 宏。

---

## 五、显示后端实现

### 5.1 后端对比

| 维度 | Windows 后端 | WSL/X11 后端 |
|------|-------------|-------------|
| **角色** | Compositor（伪桌面） | Compositor（伪桌面） |
| **运行模式** | 全屏覆盖层 | XWayland 客户端 |
| **窗口跟踪** | `SetWinEventHook` + `EnumWindows` | XCB `SubstructureNotify` + `xcb_query_tree` |
| **窗口封装** | `WindowsWindow(HWND)` | `WSLX11Window(xcb_window_t)` |
| **标题查询** | `GetWindowTextW` | `xcb_get_property(_NET_WM_NAME)` |
| **几何查询** | `GetWindowRect` | `xcb_get_geometry` + `xcb_translate_coordinates` |
| **关闭窗口** | `PostMessage(WM_CLOSE)` | `xcb_send_event(WM_DELETE_WINDOW)` |
| **事件集成** | `WINEVENT_OUTOFCONTEXT` → GUI 线程 | `QSocketNotifier` → Qt 事件循环 |
| **显示服务器** | Windows DWM | XWayland / Weston |

### 5.2 Windows 后端数据流

```
┌─────────────────────────────────────────────────────┐
│           WindowsDisplayServerBackend                │
│  initialize():                                      │
│    ├─ QApplication::instance() 检查                │
│    ├─ WindowsWindowBackend::startTracking()         │
│    │    ├─ SetWinEventHook(CREATE/SHOW/DESTROY)     │
│    │    └─ EnumWindows() 扫描已有窗口               │
│    └─ connect signals → externalWindowAppeared/...  │
├─────────────────────────────────────────────────────┤
│           WindowsWindowBackend                       │
│  WinEventProc (static callback)                     │
│    ├─ EVENT_OBJECT_SHOW → onExternalWindowShown     │
│    │    ├─ shouldTrackWindow(hwnd)                  │
│    │    │    ├─ IsWindow + IsWindowVisible           │
│    │    │    ├─ GetAncestor(GA_ROOT) == hwnd         │
│    │    │    ├─ GetWindowTextLength > 0              │
│    │    │    ├─ PID != 自身                          │
│    │    │    └─ 排除 Progman/Shell_TrayWnd/WorkerW   │
│    │    └─ registerWindow(hwnd) → emit window_came  │
│    └─ EVENT_OBJECT_DESTROY → unregisterWindow       │
├─────────────────────────────────────────────────────┤
│           WindowsWindow(HWND)                        │
│  title()     → GetWindowTextW                       │
│  geometry()  → GetWindowRect                        │
│  set_geometry() → SetWindowPos                      │
│  requestClose() → PostMessage(WM_CLOSE)             │
│  raise()     → SetForegroundWindow + BringWindowToTop│
└─────────────────────────────────────────────────────┘
```

### 5.3 WSL/X11 后端数据流

```
┌─────────────────────────────────────────────────────┐
│           WSLDisplayServerBackend                    │
│  initialize():                                      │
│    ├─ connectToX11()                                │
│    │    ├─ xcb_connect(nullptr, &screen_num)        │
│    │    ├─ xcb_get_setup → xcb_setup_roots_iterator │
│    │    └─ 获取 root_window_                        │
│    ├─ WSLX11WindowBackend::startTracking(conn,root) │
│    │    ├─ internAtoms() → 缓存 9 个 X11 Atom      │
│    │    ├─ xcb_change_window_attributes(             │
│    │    │    SUBSTRUCTURE_NOTIFY)                    │
│    │    ├─ xcb_query_tree → 扫描已有窗口            │
│    │    └─ QSocketNotifier(xcb_fd) → processEvents  │
│    └─ connect signals → externalWindowAppeared/...  │
├─────────────────────────────────────────────────────┤
│           WSLX11WindowBackend                        │
│  processXcbEvents() ← QSocketNotifier::activated    │
│    ├─ XCB_CREATE_NOTIFY                             │
│    │    └─ shouldTrackWindow → registerWindow       │
│    ├─ XCB_MAP_NOTIFY                                │
│    │    └─ shouldTrackWindow → registerWindow       │
│    ├─ XCB_DESTROY_NOTIFY → unregisterWindow         │
│    ├─ XCB_CONFIGURE_NOTIFY → geometryChanged signal │
│    └─ XCB_PROPERTY_NOTIFY → titleChanged signal     │
│                                                     │
│  shouldTrackWindow(win):                            │
│    ├─ xcb_get_window_attributes → viewable?         │
│    ├─ !override_redirect                            │
│    ├─ _NET_WM_NAME / WM_NAME 非空                   │
│    ├─ _NET_WM_PID != 自身 PID                       │
│    └─ _NET_WM_WINDOW_TYPE != DOCK/DESKTOP           │
├─────────────────────────────────────────────────────┤
│           WSLX11Window(xcb_window_t)                 │
│  title()     → xcb_get_property(_NET_WM_NAME)       │
│               → fallback: WM_NAME                   │
│  geometry()  → xcb_get_geometry                     │
│               + xcb_translate_coordinates → 根坐标  │
│  set_geometry() → xcb_configure_window              │
│  requestClose() → WM_DELETE_WINDOW ClientMessage    │
│                  → fallback: xcb_kill_client        │
│  raise()     → xcb_configure_window(ABOVE)          │
└─────────────────────────────────────────────────────┘
```

### 5.4 未来后端规划

| 后端 | 角色 | 关键技术 | 状态 |
|------|------|---------|------|
| X11 窗口管理器 | Compositor | XCB SubstructureRedirect + EWMH + XComposite | 规划中 |
| Wayland 合成器 | Compositor | QtWaylandCompositor + xdg-shell + DRM/KMS | 规划中 |
| EGLFS | DirectRender | Qt EGLFS + libdrm/libgbm | 规划中 |
| LinuxFB | DirectRender | Qt linuxfb + evdev | 规划中 |

---

## 六、UI 框架分层

UI 模块采用五层架构，从底层数学工具到顶层控件，逐层构建 Material Design 3 设计系统：

```
┌─────────────────────────────────────────────────┐
│  Layer 5: Widget 适配层       (cf_ui_widget)    │
│  Button, TextField, Switch, TabView...          │
│  (20+ 生产级 Widget，面向开发者)                │
├─────────────────────────────────────────────────┤
│  Layer 4: Material 行为层    (cf_ui_components)  │
│  StateMachine, Ripple, Elevation, FocusRing     │
│  (MD3 交互行为：水波纹、状态机、焦点)          │
│  + Material 动画实现                             │
│  (Fade, Slide, Scale, Spring)                   │
├─────────────────────────────────────────────────┤
│  Layer 3: 动画引擎层         (cf_ui_components)  │
│  Animation, AnimationFactory, Timing/Spring     │
│  (动画基础设施：时间线、弹性物理、工厂)        │
├─────────────────────────────────────────────────┤
│  Layer 2: 主题引擎层          (cf_ui_core)       │
│  ThemeManager, Token 系统                        │
│  (MD3 Token: 颜色、排版、形状、动效)            │
├─────────────────────────────────────────────────┤
│  Layer 1: 数学与工具层         (cf_ui_base)      │
│  Color, Math, Geometry, Easing, DevicePixel     │
│  (基础数学：HCT 色彩、easing 曲线、DPI)         │
└─────────────────────────────────────────────────┘
```

### 各层详情

**Layer 1 — 数学与工具层** (`ui/base/`)
- `color.h`: HCT 色彩系统（Hue-Chroma-Tone）
- `math_helper.h`: 数学辅助函数
- `geometry_helper.h`: 几何计算
- `easing.h`: 缓动曲线（Material Motion 标准）
- `device_pixel.h`: DPI 适配与设备像素比

**Layer 2 — 主题引擎层** (`ui/core/`)
- `theme_manager.h`: 主题管理器，动态切换主题
- `token.hpp`: 设计 Token 系统（~35KB），包含完整的 MD3 Token 定义
- `color_scheme.h`: 颜色方案（Light/Dark/动态）
- `font_type.h`: 排版系统
- `motion_spec.h`: 动效规格

**Layer 3 — 动画引擎层** (`ui/components/`)
- `animation.h`: 动画基类
- `animation_factory_manager.h`: 动画工厂管理器
- `timing_animation.h`: 基于时间线的动画
- Material 动画实现：Fade, Slide, Scale, Spring

**Layer 4 — Material 行为层** (`ui/widget/material/base/`)
- `state_machine.h`: Widget 状态机（Pressed/Hovered/Focused/Disabled）
- `ripple_helper.h`: Material 水波纹效果
- `elevation_controller.h`: Z 轴高度管理（阴影层次）
- `focus_ring.h`: 焦点指示环

**Layer 5 — Widget 适配层** (`ui/widget/material/widget/`)
- 20+ 生产级 Widget：Button, Checkbox, TextField, Slider, Switch, ComboBox, ListView, TableView, TreeView, TabView, ScrollView 等

---

## 七、初始化与生命周期

### 7.1 完整启动流程

```
main(argc, argv)
│
├─ QApplication cf_desktop_app(argc, argv)     ← Qt 应用创建
│
└─ run_desktop_session()                       ← DLL 入口
     │
     ├─ init_session::RunEarlyInit()           ← 早期初始化
     │    └─ 日志、路径、配置
     │
     ├─ init_session::RunStageInit()           ← 阶段初始化
     │    └─ 系统检测、资源预加载
     │
     ├─ boot_desktop()                         ← 桌面启动
     │    │
     │    ├─ CFDesktopEntity::instance()       ← 创建单例
     │    │    ├─ new CFDesktop(this)          ← 创建桌面 Widget
     │    │    ├─ new PlatformFactory          ← 创建策略工厂
     │    │    │
     │    │    └─ platform::native_display()   ← 注册显示后端
     │    │         └─ DisplayServerBackendFactory::register_creator()
     │    │
     │    ├─ CFDesktopEntity::run_init()
     │    │    ├─ 创建 IDesktopDisplaySizeStrategy
     │    │    ├─ CFDesktopWindowProxy::set_window_display_strategy()
     │    │    ├─ DisplayServerBackendFactory::make_unique()  ← 创建后端
     │    │    │    └─ native_display_impl()
     │    │    │         ├─ Windows: new WindowsDisplayServerBackend
     │    │    │         └─ WSL:    new WSLDisplayServerBackend
     │    │    │
     │    │    ├─ display_backend_->initialize()
     │    │    │    ├─ connectToX11() / Win32 初始化
     │    │    │    ├─ window_backend_->startTracking()
     │    │    │    └─ connect signals
     │    │    │
     │    │    └─ connect window_came/gone → 日志
     │    │
     │    ├─ CFDesktopWindowProxy::show_desktop()  ← 显示桌面窗口
     │    └─ init_session::ReleaseStageInitOldResources()
     │
     ├─ QApplication::exec()                   ← 事件循环
     │
     ├─ CFDesktopEntity::release()             ← 清理（QApplication 存活时）
     └─ Logger::flush_sync()                   ← 刷日志
```

### 7.2 CFDesktopEntity 生命周期

`CFDesktopEntity` 是桌面环境的中央单例，管理整个生命周期：

```
构造 ──────────────────────────────────────── 析构
  │                                             │
  ├─ CFDesktop* (Widget)                        ├─ stopTracking()
  ├─ PlatformFactory (策略)                     ├─ xcb_disconnect()
  ├─ DisplayServerBackend (后端)                └─ 清理 Widget
  │    └─ IWindowBackend (窗口跟踪)
  │         └─ IWindow[] (窗口列表)
  │
  └─ run_init() 初始化全部组件
```

**关键约束**：`CFDesktopEntity::release()` 必须在 `QApplication` 存活时调用，因为其内部持有 `QWidget` 实例。

---

## 八、构建系统概览

### 8.1 顶层 CMake 结构

```cmake
project(CFDesktop VERSION 0.13.1)

# 预配置：WSL 检测、构建类型、Qt 设置
include(cmake/check_pre_configure.cmake)
include(cmake/generate_meta_info.cmake)

# 三大模块，按依赖顺序添加
add_subdirectory(base)      # 1. 基础库
add_subdirectory(ui)        # 2. UI 框架（依赖 base）
add_subdirectory(desktop)   # 3. 桌面实现（依赖 base + ui）

# 辅助模块
add_subdirectory(example)   # 示例应用
add_subdirectory(test)      # 测试套件
```

### 8.2 条件编译

| 条件 | 效果 |
|------|------|
| `WIN32` | 编译 Windows 平台代码，链接 Win32 系统库 |
| `UNIX AND NOT APPLE` + `IS_WSL` | 编译 Linux/WSL 平台代码，链接 XCB |
| `CFDESKTOP_OS_WSL` | C++ 宏，标记 WSL 环境 |
| `CFDESKTOP_HAS_XCB` | C++ 宏，标记 XCB 可用 |

### 8.3 共享库构建技巧

所有静态库通过 `--whole-archive` 链接到 `CFDesktop_shared`，确保即使没有被直接引用的符号（如工厂注册函数）也被包含：

```cmake
target_link_libraries(CFDesktop_shared PRIVATE
    "-Wl,--whole-archive"
    ${CFDESKTOP_STATIC_LIBS}
    "-Wl,--no-whole-archive"
)
```

最终可执行文件 `CFDesktop` 仅包含 `main.cpp`，链接 `CFDesktop_shared`。

---

## 九、WSL/X11 后端文件清单

本次实现新增的 WSL X11 后端文件：

| 文件路径 | 职责 |
|----------|------|
| `desktop/ui/platform/linux_wsl/wsl_x11_window.h` | `WSLX11Window` — IWindow 的 XCB 实现 |
| `desktop/ui/platform/linux_wsl/wsl_x11_window.cpp` | 标题查询、几何操作、关闭/置顶 |
| `desktop/ui/platform/linux_wsl/wsl_x11_window_backend.h` | `WSLX11WindowBackend` — IWindowBackend 的 XCB 实现 |
| `desktop/ui/platform/linux_wsl/wsl_x11_window_backend.cpp` | Atom 缓存、事件监听、窗口过滤、信号发射 |
| `desktop/ui/platform/linux_wsl/wsl_x11_display_server_backend.h` | `WSLDisplayServerBackend` — IDisplayServerBackend 实现 |
| `desktop/ui/platform/linux_wsl/wsl_x11_display_server_backend.cpp` | XCB 连接管理、初始化/关闭、信号转发 |

修改的现有文件：

| 文件路径 | 变更 |
|----------|------|
| `desktop/ui/platform/linux_wsl/linux_wsl_platform.cpp` | `native_display_impl()` 从返回 nullptr 改为返回后端创建函数 |
| `desktop/ui/platform/CMakeLists.txt` | 添加 XCB 依赖（`pkg_check_modules` + `target_link_libraries`） |

---

*本文档随项目开发持续更新。*
