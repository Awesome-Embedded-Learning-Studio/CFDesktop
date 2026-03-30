# Phase 7: 渲染后端抽象 TODO

> **状态**: 🚧 部分完成 (接口已实现，具体后端待开发)
> **预计周期**: 2 周
> **依赖阶段**: Phase 6
> **已完成归档**: [done/14_display_backend_status.md](../done/14_display_backend_status.md)

## 已完成模块

> RenderBackend 抽象接口、BackendCapabilities、RenderBackendFactory 已实现。
> 显示后端架构 (IDisplayServerBackend)、Windows 后端、WSL X11 后端已完成。
> 详细状态请参考: [done/14_display_backend_status.md](../done/14_display_backend_status.md)

---

## 一、阶段目标

### 核心目标
实现渲染后端抽象层，使同一 Shell 代码可在 EGLFS/Wayland/X11/Windows 上运行。

### 具体交付物
- [x] `RenderBackend` 抽象接口 (接口已实现)
- [x] `BackendCapabilities` 能力标志 (已实现)
- [x] `RenderBackendFactory` 工厂 (已实现)
- [ ] `EGLFSBackend` 实现
- [ ] `WindowsBackend` 实现
- [ ] 屏幕尺寸模拟器（Windows 开发调试用）
- [ ] `WaylandBackend` 实现（可选）
- [ ] `X11Backend` 实现（可选）

---

## 二、待实现任务

### Week 1: 后端接口与 EGLFS

#### Day 1-2: RenderBackend 接口
- [ ] 定义 RenderBackend 抽象接口
  - [ ] `initialize()` 初始化
  - [ ] `shutdown()` 清理资源
  - [ ] `swapBuffers()` 交换缓冲
  - [ ] `createSurface()` 创建渲染表面
  - [ ] `setScreenSize()` 设置屏幕尺寸
  - [ ] `getScreenSize()` 获取当前尺寸
- [ ] 实现后端注册机制
  - [ ] `RenderBackendFactory` 工厂类
  - [ ] 运行时按环境选择
  - [ ] 环境变量 `CFDESKTOP_RENDER_BACKEND`
- [ ] 定义后端能力查询
  - [ ] `hasVSync()` VSync 支持
  - [ ] `supportsScreenshot()` 截图支持
  - [ ] `maxTextureSize()` 最大纹理尺寸
  - [ ] `supportsTransparency()` 透明度支持

#### Day 3-4: EGLFS 后端
- [ ] 创建 EGLFSBackend 类
  - [ ] 继承 RenderBackend
  - [ ] Qt EGLFS 平台插件封装
- [ ] 实现 HWTier 联动
  - [ ] High Tier: OpenGL ES 合成
  - [ ] Mid Tier: 软件降级可选
  - [ ] Low Tier: LinuxFB 直连
- [ ] 实现屏幕尺寸配置
  - [ ] 读取 `/etc/CFDesktop/display.conf`
  - [ ] 命令行参数 `--screen-size=WxH`
  - [ ] 环境变量 `CFDESKTOP_SCREEN_SIZE`
- [ ] 实现 DRM 设备选择
  - [ ] 自动检测 `/dev/dri/card*`
  - [ ] 配置文件指定设备
  - [ ] 多 GPU 环境选择

#### Day 5: EGLFS 测试与集成
- [ ] 编写单元测试
  - [ ] 后端初始化测试
  - [ ] HWTier 联动测试
  - [ ] Mock DRM 测试
- [ ] 集成到 CMake 构建系统
  - [ ] EGLFS 条件编译
  - [ ] 依赖检测（libdrm, gbm）

---

### Week 2: Windows 后端与模拟器

#### Day 1-2: Windows 后端
- [ ] 创建 WindowsBackend 类
  - [ ] 继承 RenderBackend
  - [ ] Qt Windows 平台插件封装
- [ ] 实现窗口配置
  - [ ] 无边框全屏窗口
  - [ ] 固定尺寸窗口
  - [ ] 任务栏隐藏选项
- [ ] 实现 DPI 注入
  - [ ] 与 DPIManager 集成
  - [ ] 精确还原目标 DPI
  - [ ] 高 DPI 缩放支持

#### Day 3-4: 屏幕尺寸模拟器
- [ ] 实现预设尺寸
  - [ ] 5.5" (720 x 1280)
  - [ ] 7.0" (1200 x 1920)
  - [ ] 8.0" (1536 x 2048)
  - [ ] 10.1" (1920 x 1200)
- [ ] 实现快捷切换
  - [ ] 快捷键 Ctrl+1/2/3/4
  - [ ] 菜单栏选择器
  - [ ] 配置文件持久化
- [ ] 实现触摸模拟
  - [ ] 鼠标左键 → 单点触摸
  - [ ] Shift+点击 → 多点触摸
  - [ ] 鼠标拖拽 → 滑动手势
- [ ] 实现开发调试辅助
  - [ ] 显示边界框
  - [ ] 显示触摸点
  - [ ] FPS 显示

#### Day 5: Wayland/X11 后端（次优先级）
- [ ] 创建 WaylandBackend 类（可选）
  - [ ] Qt Wayland 平台插件封装
- [ ] 创建 X11Backend 类（可选）
  - [ ] Qt X11 平台插件封装
- [ ] 编写跨平台集成测试
  - [ ] Linux EGLFS 启动测试
  - [ ] Windows 后端启动测试
  - [ ] 屏幕尺寸正确性测试

---

## 三、验收标准

### 功能验收
- [ ] 同一代码在 Linux EGLFS 和 Windows 均可启动
- [ ] 屏幕尺寸模拟器可精确还原嵌入式屏幕
- [ ] HWTier 联动正确生效
- [ ] 触摸模拟在 Windows 上正常工作

### 兼容性验收
- [ ] 支持 Qt 6.8.3+
- [ ] 支持 Linux 嵌入式环境
- [ ] 支持 Windows 10/11 开发环境
- [ ] （可选）支持 Wayland/X11 环境

### 性能验收
- [ ] 后端切换延迟 < 100ms
- [ ] 屏幕尺寸切换 < 50ms
- [ ] 不影响渲染帧率

---

## 四、文件清单（待实现）

### 头文件
- [ ] `ui/desktop/render/render_backend.h`
- [ ] `ui/desktop/render/render_backend_factory.h`
- [ ] `ui/desktop/render/backend_capabilities.h`
- [ ] `ui/desktop/render/eglfs_backend.h`
- [ ] `ui/desktop/render/windows_backend.h`
- [ ] `ui/desktop/render/wayland_backend.h` （可选）
- [ ] `ui/desktop/render/x11_backend.h` （可选）
- [ ] `ui/desktop/render/screen_simulator.h`

### 源文件
- [ ] `src/desktop/render/render_backend.cpp`
- [ ] `src/desktop/render/render_backend_factory.cpp`
- [ ] `src/desktop/render/eglfs_backend.cpp`
- [ ] `src/desktop/render/windows_backend.cpp`
- [ ] `src/desktop/render/wayland_backend.cpp` （可选）
- [ ] `src/desktop/render/x11_backend.cpp` （可选）
- [ ] `src/desktop/render/screen_simulator.cpp`

### 配置文件
- [ ] `/etc/CFDesktop/display.conf` （Linux）
- [ ] `configs/display.conf` （Windows 开发）

### 测试文件
- [ ] `tests/unit/desktop/render/test_render_backend.cpp`
- [ ] `tests/unit/desktop/render/test_eglfs_backend.cpp`
- [ ] `tests/unit/desktop/render/test_windows_backend.cpp`
- [ ] `tests/unit/desktop/render/test_screen_simulator.cpp`

---

## 五、相关文档

- 设计文档: [../desktop/summary.md](summary.md) Phase B 节
- 依赖: [06_infrastructure.md](06_infrastructure.md)
- Qt 平台插件文档: https://doc.qt.io/qt-6/platform-notes.html
- EGLFS 文档: https://doc.qt.io/qt-6/embedded-linux.html

---

*最后更新: 2026-03-12*
