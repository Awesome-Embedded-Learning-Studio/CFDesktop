# CFDesktop 多显示后端架构设计

> **状态**: 设计中
> **版本**: 0.1.0
> **最后更新**: 2026-03-29

---

## 一、设计目标

CFDesktop 需要在以下场景中运行：

| 场景 | 角色定位 | 运行环境 |
|------|---------|---------|
| Windows 伪桌面 | 客户端应用 | Windows 10/11，覆盖在 Windows 桌面上 |
| Linux 有桌面环境 | 客户端应用 | 有 Gnome/KDE 等完整桌面环境 |
| Linux 无桌面 (X11) | X11 窗口管理器 | 仅有 libx11/libxcb，无窗口管理器 |
| Linux 无桌面 (Wayland) | Wayland 合成器 | 仅有 libwayland，无合成器 |
| Linux 嵌入式 | 直接渲染 | 仅 libdrm/libgbm 或 linuxfb |

核心设计原则：**同一套 Shell/Panel/WindowManager 代码在所有场景中复用，通过抽象层屏蔽平台差异。**

---

## 二、系统角色模型

```
┌─────────────────────────────────────────────────────┐
│                    CFDesktop Shell                    │
│  (ShellLayer, PanelManager, WindowManager, IWindow)   │
├───────────────────────────────────────────────────────┤
│              IDisplayServerBackend                     │
│  ┌─────────┬──────────┬──────────┬────────────────┐  │
│  │ Client  │Compositor│Compositor│  DirectRender  │  │
│  │  Mode   │ X11 WM   │ Wayland  │  EGLFS/FB      │  │
│  └────┬────┴────┬─────┴────┬─────┴───────┬────────┘  │
│       │         │          │             │            │
│  ┌────▼────┐┌───▼────┐┌───▼────┐┌───────▼──────┐    │
│  │QWidget  ││XCB WM  ││QtWayland││Qt linuxfb/   │    │
│  │Windows  ││EWMH    ││Compositor││EGLFS plugin  │    │
│  └─────────┘└────────┘└────────┘└──────────────┘    │
├───────────────────────────────────────────────────────┤
│                  RenderBackend                         │
│  (RHI abstraction, swapBuffers, capabilities)          │
├───────────────────────────────────────────────────────┤
│              硬件层 (GPU / DRM / FB / Win32)            │
└───────────────────────────────────────────────────────┘
```

### DisplayServerRole 枚举

| 值 | 含义 | 典型场景 |
|----|------|---------|
| `Client` | 在已有桌面环境中作为应用运行 | Windows, Linux with DE |
| `Compositor` | 自己就是显示服务器/合成器 | 无桌面 Linux (X11/Wayland) |
| `DirectRender` | 直接渲染到 framebuffer | 嵌入式设备 (EGLFS/linuxfb) |

---

## 三、新增接口定义

### 3.1 IDisplayServerBackend (新增)

**文件**: `desktop/ui/components/IDisplayServerBackend.h`

```cpp
class IDisplayServerBackend : public QObject {
    Q_OBJECT
public:
    virtual DisplayServerRole role() const = 0;
    virtual DisplayServerCapabilities capabilities() const = 0;
    virtual bool initialize(int argc, char** argv) = 0;
    virtual void shutdown() = 0;
    virtual int runEventLoop() = 0;
    virtual WeakPtr<IWindowBackend> windowBackend() = 0;
    virtual QList<QRect> outputs() const = 0;

signals:
    void outputChanged();
    void externalWindowAppeared(WeakPtr<IWindow> window);
    void externalWindowDisappeared(WeakPtr<IWindow> window);
};
```

**职责**:
- 决定 CFDesktop 的运行角色（客户端 / 合成器 / 直接渲染）
- 管理显示输出的生命周期
- 在合成器模式下，桥接外部窗口事件到 WindowManager

### 3.2 RenderBackend (新增)

**文件**: `desktop/ui/render/render_backend.h`

```cpp
class RenderBackend {
public:
    virtual ~RenderBackend() = default;
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual BackendCapabilities capabilities() const = 0;
    virtual QSize screenSize() const = 0;
    virtual void* nativeHandle() const = 0;  // EGLDisplay, HWND 等
};
```

**职责**:
- 抽象底层渲染硬件初始化
- 提供渲染能力查询
- 不负责具体绘制（由 Qt RHI 或 QPainter 负责）

### 3.3 BackendCapabilities (新增)

**文件**: `desktop/ui/render/backend_capabilities.h`

```cpp
struct BackendCapabilities {
    bool supportsMultiWindow = true;
    bool supportsTransparency = true;
    bool hasHardwareAcceleration = true;
    bool supportsVSync = true;
    bool supportsScreenshot = true;
    int maxTextureSize = 4096;
};
```

---

## 四、现有接口修改

### 4.1 IWindowBackend — 增加 capabilities()

在 `IWindowBackend` 中增加:
```cpp
virtual BackendCapabilities capabilities() const = 0;
```

允许 WindowManager 和 Shell 在运行时查询后端能力，做出适配决策。

### 4.2 qt_backend.h — 扩展枚举和检测

```cpp
enum class LinuxQtBackend {
    X11,      // X11 窗口系统
    Wayland,  // Wayland 合成器
    EGLFS,    // EGL + OpenGL ES (无窗口系统)
    LinuxFB,  // Linux 帧缓冲 (纯软件渲染)
    Unknown
};

enum class DisplayServerMode {
    Client,        // 有现成的窗口系统可用
    NeedCompositor, // 需要自己做合成器
    DirectRender   // 直接渲染到硬件
};

DisplayServerMode DetectDisplayServerMode();
```

`DetectDisplayServerMode()` 检测逻辑:
1. 检查环境变量 `CFDESKTOP_DISPLAY_SERVER` (强制覆盖)
2. 检查 `WAYLAND_DISPLAY` 或 `DISPLAY` (有显示服务器 → Client)
3. 检查 `/dev/dri/card*` (可做 EGLFS → DirectRender)
4. 检查 `/dev/fb0` (可做 linuxfb → DirectRender)
5. 默认: Client

### 4.3 ShellLayer — 解耦 QWidget

新增纯接口 `IShellLayer`:
```cpp
class IShellLayer {
public:
    virtual ~IShellLayer() = default;
    virtual void setStrategy(std::unique_ptr<IShellLayerStrategy> strategy) = 0;
    virtual QRect geometry() const = 0;
};
```

`ShellLayer` 同时继承 `IShellLayer` 和 `QWidget`:
```cpp
class ShellLayer : public QWidget, public IShellLayer {
    // QWidget 实现 — Client 模式
};

// Wayland 合成器可以实现 IShellLayer 而不继承 QWidget
```

---

## 五、各后端实现指导

### 5.1 Windows 伪桌面

- **角色**: `DisplayServerRole::Client`
- **RenderBackend**: WindowsBackend (封装 Win32 + Qt RHI)
- **IWindowBackend**: 每个窗口 = QWidget 子控件
- **特殊处理**: 无边框全屏 + WS_EX_TOOLWINDOW 避免任务栏
- **现有代码复用**: `windows_display_size_policy.cpp`

### 5.2 X11 客户端

- **角色**: `DisplayServerRole::Client`
- **RenderBackend**: 默认 Qt RHI
- **IWindowBackend**: QWidget 直接映射为 X11 Window
- **现有代码复用**: `linux_wsl_display_size_policy.cpp` 的 X11 路径

### 5.3 X11 窗口管理器 (无桌面)

- **角色**: `DisplayServerRole::Compositor`
- **关键技术**: XCB SubstructureRedirect + EWMH + XComposite
- **IWindowBackend**: XCB Window → `IWindow` 适配器
- **合成**: XComposite 重定向到 offscreen pixmap, OpenGL 合成
- **输入**: XCB 事件循环转发

### 5.4 Wayland 合成器 (无桌面)

- **角色**: `DisplayServerRole::Compositor`
- **关键技术**: QtWaylandCompositor C++ API + xdg-shell
- **IWindowBackend**: QWaylandSurface → `IWindow` 适配器
- **合成**: Qt RHI (OpenGL/Vulkan) 直接合成
- **输出**: DRM/KMS via QWaylandOutput
- **依赖**: Qt6::WaylandCompositor, libwayland-server, libdrm

### 5.5 EGLFS / linuxfb

- **角色**: `DisplayServerRole::DirectRender`
- **关键技术**: Qt EGLFS/linuxfb 平台插件
- **IWindowBackend**: 全部"窗口" = QWidget 子控件
- **输入**: evdev (udev) 直接读取
- **限制**: 无多窗口、无外部应用管理

---

## 六、组件复用矩阵

| 组件 | Client | X11 WM | Wayland | DirectRender |
|------|--------|--------|---------|-------------|
| `IWindow` | 直接复用 | XCB Window 适配 | Surface 适配 | QWidget 子控件适配 |
| `IWindowBackend` | QWidget 实现 | XCB WM 实现 | Wayland 实现 | FB 单窗口实现 |
| `WindowManager` | 直接复用 | 直接复用 | 直接复用 | 直接复用 |
| `PanelManager` | 直接复用 | 直接复用 | 直接复用 | 直接复用 |
| `ShellLayer` | QWidget 实现 | QWidget 实现 | IShellLayer 实现 | QWidget 实现 |
| `IShellLayerStrategy` | 直接复用 | 直接复用 | 需新策略 | 直接复用 |

---

## 七、构建系统设计

```cmake
# 条件编译选项
option(CFDESKTOP_ENABLE_WAYLAND_COMPOSITOR "Enable Wayland compositor backend" ON)
option(CFDESKTOP_ENABLE_X11_WM "Enable X11 window manager backend" ON)
option(CFDESKTOP_ENABLE_LINUXFB "Enable linuxfb backend" ON)

# 自动检测依赖
if(CFDESKTOP_ENABLE_WAYLAND_COMPOSITOR)
    find_package(Qt6 OPTIONAL_COMPONENTS WaylandCompositor)
endif()

if(CFDESKTOP_ENABLE_X11_WM)
    find_package(X11)
    find_package(PkgConfig)
    pkg_check_modules(XCB OPTIONAL xcb xcb-composite xcb-ewmh)
endif()
```

---

## 八、运行时后端选择流程

```
CFDesktop 启动
    │
    ▼
DetectDisplayServerMode()
    │
    ├── 环境变量 CFDESKTOP_DISPLAY_SERVER?
    │   └── 强制使用指定后端
    │
    ├── WAYLAND_DISPLAY 存在?
    │   └── Client 模式 (Wayland 客户端)
    │
    ├── DISPLAY 存在?
    │   ├── 有其他 WM 在运行?
    │   │   └── Client 模式 (X11 客户端)
    │   └── 无 WM?
    │       └── Compositor 模式 (X11 WM)
    │
    ├── /dev/dri/card* 存在?
    │   └── DirectRender 模式 (EGLFS)
    │
    ├── /dev/fb0 存在?
    │   └── DirectRender 模式 (linuxfb)
    │
    └── 默认: Client 模式
```
