# Phase H: 显示后端与窗口管理系统状态

> **状态**: ✅ 核心完成
> **完成日期**: 2026-03-30
> **总体进度**: ~70% (两个平台后端已完成，Wayland/嵌入式待实现)

---

## 一、显示服务器抽象架构

### 1.1 IDisplayServerBackend 接口

#### 实现文件

| 文件路径 | 说明 |
|---------|------|
| [`desktop/ui/components/IDisplayServerBackend.h`](../../../desktop/ui/components/IDisplayServerBackend.h) | 显示服务器后端抽象接口 |
| [`desktop/ui/components/IDisplayServerBackend.cpp`](../../../desktop/ui/components/IDisplayServerBackend.cpp) | 接口实现 |

#### 功能实现

- [x] 三种运行模式定义 (`DisplayServerRole`)
  - Client: 作为应用运行在已有桌面环境中
  - Compositor: CFDesktop 自己就是显示服务器
  - DirectRender: 直接渲染到帧缓冲 (EGLFS/linuxfb)
- [x] 能力描述 (`DisplayServerCapabilities`)
  - `canManageExternalWindows` — 外部窗口管理能力
  - `needsOwnCompositor` — 是否需要独立合成器
  - `supportsWaylandProtocol` / `supportsX11Protocol` — 协议支持
- [x] 生命周期方法: `initialize()` / `shutdown()` / `runEventLoop()`
- [x] 窗口后端访问: `windowBackend()`
- [x] 多输出支持: `outputs()` (多显示器 QRect 列表)
- [x] 信号: `outputChanged()`, `externalWindowAppeared()`, `externalWindowDisappeared()`

---

### 1.2 IWindowBackend 接口

#### 实现文件

| 文件路径 | 说明 |
|---------|------|
| [`desktop/ui/components/IWindowBackend.h`](../../../desktop/ui/components/IWindowBackend.h) | 窗口后端抽象接口 |
| [`desktop/ui/components/IWindowBackend.cpp`](../../../desktop/ui/components/IWindowBackend.cpp) | 接口实现 |

#### 功能实现

- [x] 窗口创建/销毁: `createWindow()` / `destroyWindow()`
- [x] 窗口列表查询: `windows()`
- [x] 能力查询: `capabilities()`
- [x] 信号: `window_came` / `window_gone`

---

### 1.3 IWindow 接口

#### 实现文件

| 文件路径 | 说明 |
|---------|------|
| [`desktop/ui/components/IWindow.h`](../../../desktop/ui/components/IWindow.h) | 平台无关窗口接口 |
| [`desktop/ui/components/IWindow.cpp`](../../../desktop/ui/components/IWindow.cpp) | 接口实现 |

#### 功能实现

- [x] 窗口标识: `windowID()` (win_id_t = QString)
- [x] 窗口属性: `title()`, `geometry()`
- [x] 窗口操作: `set_geometry()`, `requestClose()`, `raise()`
- [x] 信号: `closeRequested()`, `titleChanged()`, `geometryChanged()`

---

## 二、Windows 平台后端

### 2.1 WindowsDisplayServerBackend

#### 实现文件

| 文件路径 | 说明 |
|---------|------|
| [`desktop/ui/platform/windows/windows_display_server_backend.h`](../../../desktop/ui/platform/windows/windows_display_server_backend.h) | Windows 显示服务器后端 |
| [`desktop/ui/platform/windows/windows_display_server_backend.cpp`](../../../desktop/ui/platform/windows/windows_display_server_backend.cpp) | 实现 |

#### 功能实现

- [x] 角色: Compositor (伪桌面模式)
- [x] Win32 事件钩子 (`SetWinEventHook`) 追踪第三方窗口
- [x] `EnumWindows` 扫描已有窗口
- [x] 能力:
  - `canManageExternalWindows = true`
  - `needsOwnCompositor = false` (Windows DWM 处理)
  - 硬件加速、透明度、多窗口、VSync、截图

### 2.2 WindowsWindowBackend

#### 实现文件

| 文件路径 | 说明 |
|---------|------|
| [`desktop/ui/platform/windows/windows_window_backend.h`](../../../desktop/ui/platform/windows/windows_window_backend.h) | Windows 窗口后端 |
| [`desktop/ui/platform/windows/windows_window_backend.cpp`](../../../desktop/ui/platform/windows/windows_window_backend.cpp) | 实现 |

#### 功能实现

- [x] `SetWinEventHook` + `WINEVENT_OUTOFCONTEXT` 事件追踪
- [x] `EnumWindows` 初始扫描
- [x] 事件处理: `EVENT_OBJECT_CREATE` / `SHOW` / `DESTROY`
- [x] 智能窗口过滤 (`shouldTrackWindow()`):
  - 必须可见、顶层、有标题
  - 跳过自身进程窗口
  - 过滤工具窗口、桌面、任务栏、IME 窗口

### 2.3 WindowsWindow

#### 实现文件

| 文件路径 | 说明 |
|---------|------|
| [`desktop/ui/platform/windows/windows_window.h`](../../../desktop/ui/platform/windows/windows_window.h) | Win32 HWND 窗口包装 |
| [`desktop/ui/platform/windows/windows_window.cpp`](../../../desktop/ui/platform/windows/windows_window.cpp) | 实现 |

#### 功能实现

- [x] 适配 Win32 HWND → IWindow 接口
- [x] 标题: `GetWindowTextLengthW` / `GetWindowTextW`
- [x] 几何: `GetWindowRect` / `SetWindowPos`
- [x] 关闭: `PostMessageW(WM_CLOSE)`
- [x] 置顶: `SetForegroundWindow` / `BringWindowToTop`

---

## 三、Linux/WSL X11 平台后端

### 3.1 WSLX11DisplayServerBackend

#### 实现文件

| 文件路径 | 说明 |
|---------|------|
| [`desktop/ui/platform/linux_wsl/wsl_x11_display_server_backend.h`](../../../desktop/ui/platform/linux_wsl/wsl_x11_display_server_backend.h) | X11 显示服务器后端 |
| [`desktop/ui/platform/linux_wsl/wsl_x11_display_server_backend.cpp`](../../../desktop/ui/platform/linux_wsl/wsl_x11_display_server_backend.cpp) | 实现 |

#### 功能实现

- [x] 角色: Compositor (通过 X11 追踪实现伪合成器)
- [x] XCB 连接管理 (`xcb_connect`)
- [x] 根窗口监控
- [x] 条件编译: `CFDESKTOP_HAS_XCB`

### 3.2 WSLX11WindowBackend

#### 实现文件

| 文件路径 | 说明 |
|---------|------|
| [`desktop/ui/platform/linux_wsl/wsl_x11_window_backend.h`](../../../desktop/ui/platform/linux_wsl/wsl_x11_window_backend.h) | X11 窗口后端 |
| [`desktop/ui/platform/linux_wsl/wsl_x11_window_backend.cpp`](../../../desktop/ui/platform/linux_wsl/wsl_x11_window_backend.cpp) | 实现 |

#### 功能实现

- [x] XCB 协议通信
- [x] `SubstructureNotifyMask` 根窗口事件监控
- [x] `QSocketNotifier` 集成 XCB 事件到 Qt 事件循环
- [x] 事件处理:
  - `XCB_CREATE_NOTIFY` → 窗口创建
  - `XCB_DESTROY_NOTIFY` → 窗口销毁
  - `XCB_MAP_NOTIFY` → 窗口映射
  - `XCB_CONFIGURE_NOTIFY` → 几何变化
  - `XCB_PROPERTY_NOTIFY` → 属性变化 (标题等)
- [x] Atom 缓存 (`_NET_WM_NAME`, `WM_NAME`, `_NET_WM_PID`, `WM_PROTOCOLS`, `WM_DELETE_WINDOW`, `_NET_WM_WINDOW_TYPE`)
- [x] 窗口过滤: 可见性、标题、PID、窗口类型 (排除 dock/desktop)

### 3.3 WSLX11Window

#### 实现文件

| 文件路径 | 说明 |
|---------|------|
| [`desktop/ui/platform/linux_wsl/wsl_x11_window.h`](../../../desktop/ui/platform/linux_wsl/wsl_x11_window.h) | X11 窗口包装 |
| [`desktop/ui/platform/linux_wsl/wsl_x11_window.cpp`](../../../desktop/ui/platform/linux_wsl/wsl_x11_window.cpp) | 实现 |

#### 功能实现

- [x] 适配 xcb_window_t → IWindow 接口
- [x] 标题: `_NET_WM_NAME` (UTF-8) → `WM_NAME` 降级
- [x] 几何: `xcb_get_geometry` + `xcb_translate_coordinates`
- [x] 设置几何: `xcb_configure_window`
- [x] 优雅关闭: `xcb_client_message_event` + `WM_DELETE_WINDOW`
- [x] 置顶: `xcb_configure_window` + `XCB_STACK_MODE_ABOVE`

---

## 四、组件管理层

### 4.1 WindowManager

#### 实现文件

| 文件路径 | 说明 |
|---------|------|
| [`desktop/ui/components/WindowManager.h`](../../../desktop/ui/components/WindowManager.h) | 窗口管理器 |
| [`desktop/ui/components/WindowManager.cpp`](../../../desktop/ui/components/WindowManager.cpp) | 实现 |

#### 功能实现

- [x] 窗口注册/查询/关闭/置顶
- [x] 仅持有弱引用 (`std::unordered_map<win_id_t, WeakPtr<IWindow>>`)
- [x] 不拥有窗口对象生命周期

### 4.2 PanelManager

#### 实现文件

| 文件路径 | 说明 |
|---------|------|
| [`desktop/ui/components/PanelManager.h`](../../../desktop/ui/components/PanelManager.h) | 面板管理器 |
| [`desktop/ui/components/PanelManager.cpp`](../../../desktop/ui/components/PanelManager.cpp) | 实现 |

#### 功能实现

- [x] 边缘占用面板注册/注销
- [x] 复杂布局算法: 按边缘分组 → 优先级排序 → 从边缘向内堆叠 → 计算可用几何
- [x] 信号: `availableGeometryChanged()`

### 4.3 ShellLayer / IShellLayer / IShellLayerStrategy

- [x] Shell 层抽象接口 (与 QWidget 解耦)
- [x] 策略模式: 可插拔的 Shell 行为 (`IShellLayerStrategy`)
- [x] QWidget 实现: `ShellLayer`

### 4.4 IPanel / IStatusBar

- [x] 边缘面板抽象接口 (`IPanel`)
- [x] 状态栏接口 (`IStatusBar`) — 桩实现

### 4.5 DisplayServerBackendFactory

- [x] 工厂模式创建平台特定后端
- [x] 使用 `StaticRegisteredFactory` 静态注册

---

## 五、平台策略层

### 5.1 显示尺寸策略

- [x] `IDesktopDisplaySizeStrategy` — 策略模式控制窗口行为
- [x] `WindowsDisplaySizePolicy` — Windows 实现
- [x] `WSLDisplaySizePolicy` — WSL/X11 实现

### 5.2 属性策略

- [x] `IDesktopPropertyStrategy` — 属性策略基类
- [x] `DesktopPropertyStrategyFactory` — 策略工厂
- [x] 平台检测和策略选择

### 5.3 平台辅助

- [x] `platform_helper` — 平台工具
- [x] `display_backend_helper` — 后端抽象
- [x] `qt_backend` — Qt 后端检测 (X11/Wayland/EGLFS/LinuxFB)

---

## 六、启动界面 Widget

### 6.1 BootProgressWidget

- [x] 抽象启动进度显示基类
- [x] 工厂模式: `GetBootWidget(BootstrapStyle)` — Simple/Perfect/SelfDefined

### 6.2 SimpleBootWidget

- [x] 现代简约启动画面
- [x] Logo + 品牌展示
- [x] 分步进度指示器 (StepDotWidget)
- [x] Material Design 风格进度条
- [x] 背景动画 (渐变色偏移)
- [x] 版本/版权信息

### 6.3 StepDotWidget

- [x] 自定义步骤点绘制
- [x] 三态: Inactive (灰色) / Active (紫色) / Completed (紫色 + 勾)

---

## 七、渲染后端抽象

### 7.1 RenderBackend 接口 (设计完成，实现待开发)

- [x] `render_backend.h` — 抽象接口
- [x] `backend_capabilities.h` — 能力标志
- [x] `render_backend_factory.h` — 工厂 (环境变量 → 平台检测 → 默认值)

#### 待实现

- [ ] WindowsBackend (Windows 桌面渲染)
- [ ] EGLFSBackend (嵌入式 OpenGL ES)
- [ ] WaylandBackend (可选)
- [ ] X11Backend (可选)

---

## 八、平台后端进度总结

| 平台 | 显示后端 | 窗口后端 | 渲染后端 | 状态 |
|------|---------|---------|---------|------|
| Windows | ✅ Win32 DWM | ✅ HWND + Hook | ❌ 待实现 | ✅ 可用 |
| Linux/WSL X11 | ✅ XCB + XWayland | ✅ XCB + QSocketNotifier | ❌ 待实现 | ✅ 可用 |
| Wayland | ❌ 未开始 | ❌ 未开始 | ❌ 待实现 | ⬜ 待开发 |
| EGLFS (嵌入式) | ❌ 未开始 | ❌ 未开始 | ❌ 待实现 | ⬜ 待开发 |
| LinuxFB | ❌ 未开始 | ❌ 未开始 | ❌ 待实现 | ⬜ 待开发 |

---

## 九、关键架构模式

| 模式 | 应用 |
|------|------|
| 工厂模式 | DisplayServerBackendFactory, RenderBackendFactory, PlatformFactory |
| 策略模式 | IShellLayerStrategy, IDesktopDisplaySizeStrategy, IDesktopPropertyStrategy |
| 代理模式 | CFDesktopProxy, CFDesktopWindowProxy |
| 单例模式 | CFDesktopEntity |
| 弱引用模式 | WeakPtr 贯穿所有窗口和后端引用 |
| 观察者模式 | Qt Signal/Slot 事件链 |

---

## 十、运行时平台检测链

优先级从高到低:

1. `CFDESKTOP_DISPLAY_SERVER` 环境变量强制模式
2. `WAYLAND_DISPLAY` 存在 → Client 模式
3. `DISPLAY` 存在 → Client 模式
4. `/dev/dri/card*` 存在 → DirectRender (EGLFS)
5. `/dev/fb0` 存在 → DirectRender (LinuxFB)
6. 默认 → Client 模式

编译时选择:
- Windows: CMake 自动包含 `windows/` 目录
- Linux/WSL: CMake 自动包含 `linux_wsl/` 目录，检测 XCB 依赖

---

*最后更新: 2026-03-30*
*基于: 7 Agent 全面扫描结果*