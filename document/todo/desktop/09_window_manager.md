# Phase 9: 窗口管理器 TODO

> **状态**: 🚧 部分完成
> **预计周期**: 3 周
> **依赖阶段**: Phase 6, Phase 7
> **已完成归档**: [done/14_display_backend_status.md](../done/14_display_backend_status.md)

## 已完成模块

> 基础 WindowManager (弱引用模式)、基础 IWindow 接口、IWindowBackend 接口已完成。
> Windows 和 WSL X11 平台后端已实现。
> 详细状态: [done/14_display_backend_status.md](../done/14_display_backend_status.md)

### 已实现
- [x] `WindowManager` 窗口注册/查询/关闭/置顶 (弱引用模式)
- [x] `IWindow` 平台无关窗口接口 (title/geometry/close/raise)
- [x] `IWindowBackend` 窗口创建/销毁/信号
- [x] `PanelManager` 面板管理器 (边缘布局算法)
- [x] Windows 平台窗口后端 (HWND + SetWinEventHook)
- [x] WSL X11 窗口后端 (XCB + QSocketNotifier)

---

## 一、阶段目标

### 核心目标
实现桌面 Shell 的窗口管理系统，支持多种布局策略（全屏/浮动/分屏）和流畅的窗口动效。

### 具体交付物
- [ ] `WindowManager` 窗口管理器核心
- [ ] `WindowInfo` 窗口模型
- [ ] `ZOrderManager` Z-order 层级管理
- [ ] 布局策略（FullscreenPolicy / FloatingPolicy / SplitPolicy）
- [ ] 窗口动效
- [ ] 任务切换器（AppSwitcher）

---

## 二、待实现任务

### Week 1: 窗口模型与状态机

#### Day 1-2: WindowInfo 数据结构
- [ ] 定义 WindowInfo 结构
  - [ ] `windowId`（唯一标识符）
  - [ ] `title`（窗口标题）
  - [ ] `icon`（窗口图标）
  - [ ] `pid`（应用进程ID）
  - [ ] `geometry`（位置和尺寸）
  - [ ] `state`（窗口状态）
  - [ ] `zIndex`（层级值）
  - [ ] `flags`（窗口标志：模态/置顶/等）
  - [ ] `createdAt`（创建时间）
- [ ] 定义 WindowState 枚举
  - [ ] `Normal`（普通状态）
  - [ ] `Maximized`（最大化）
  - [ ] `Minimized`（最小化）
  - [ ] `Fullscreen`（全屏）
  - [ ] `Closing`（关闭中）
  - [ ] `Closed`（已关闭）
- [ ] 实现窗口状态机
  - [ ] 状态转换规则
  - [ ] 状态转换验证
  - [ ] 状态变更通知
- [ ] 编写单元测试

#### Day 3-4: ZOrderManager 层级管理
- [ ] 创建 ZOrderManager 类
  - [ ] 单例模式
- [ ] 实现层级列表维护
  - [ ] `addWindow(windowId, zIndex)`
  - [ ] `removeWindow(windowId)`
  - [ ] `getZOrder()` 获取层级列表
- [ ] 实现层级操作
  - [ ] `raiseToTop(windowId)` 提升到最前
  - [ ] `lowerToBottom(windowId)` 降低到最后
  - [ ] `raiseAbove(windowId, aboveId)` 提升到指定窗口之上
- [ ] 实现 AlwaysOnTop 支持
  - [ ] `setAlwaysOnTop(windowId, enabled)`
  - [ ] 维护置顶窗口列表
- [ ] 实现焦点窗口管理
  - [ ] `getFocusedWindow()` 获取焦点窗口
  - [ ] `setFocusedWindow(windowId)` 设置焦点窗口
  - [ ] 焦点丢失通知
- [ ] 编写单元测试

#### Day 5: WindowManager 核心框架
- [ ] 创建 WindowManager 单例
- [ ] 实现窗口注册
  - [ ] `registerWindow(const WindowInfo&)`
  - [ ] `unregisterWindow(windowId)`
  - [ ] `updateWindow(windowId, updates)`
- [ ] 实现窗口查询
  - [ ] `getWindow(windowId)` 获取单个窗口
  - [ ] `getAllWindows()` 获取所有窗口
  - [ ] `getWindowsByPid(pid)` 按进程查询
  - [ ] `getWindowsInZOrder()` 按层级排序
  - [ ] `getVisibleWindows()` 获取可见窗口
- [ ] 实现窗口操作接口
  - [ ] `closeWindow(windowId)`
  - [ ] `minimizeWindow(windowId)`
  - [ ] `maximizeWindow(windowId)`
  - [ ] `restoreWindow(windowId)`
  - [ ] `moveWindow(windowId, x, y)`
  - [ ] `resizeWindow(windowId, w, h)`
- [ ] 实现信号槽
  - [ ] `windowAdded(windowId)`
  - [ ] `windowRemoved(windowId)`
  - [ ] `windowStateChanged(windowId, newState)`
  - [ ] `focusedWindowChanged(windowId)`

---

### Week 2: 布局策略

#### Day 1-2: LayoutPolicy 接口定义
- [ ] 定义 LayoutPolicy 抽象接口
  - [ ] `applyLayout(windowId)` 应用布局
  - [ ] `calculateGeometry(windowId, mode)` 计算几何
  - [ ] `getOpenAnimation()` 获取开启动效
  - [ ] `getCloseAnimation()` 获取关闭动效
  - [ ] `supportsResize()` 是否支持缩放
  - [ ] `supportsMove()` 是否支持移动
- [ ] 实现 PolicyRegistry 注册表
  - [ ] `registerPolicy(name, policy)`
  - [ ] `unregisterPolicy(name)`
  - [ ] `getPolicy(name)` 获取策略
  - [ ] `setDefaultPolicy(name)`
- [ ] 定义 WindowMode 枚举
  - [ ] `Fullscreen`（全屏模式）
  - [ ] `Floating`（浮动模式）
  - [ ] `SplitLeft`（左分屏）
  - [ ] `SplitRight`（右分屏）
  - [ ] `SplitTop`（上分屏）
  - [ ] `SplitBottom`（下分屏）

#### Day 3: FullscreenPolicy
- [ ] 创建 FullscreenPolicy 类
  - [ ] 继承 LayoutPolicy
- [ ] 实现 iOS 风格全屏策略
  - [ ] 窗口占满整个屏幕
  - [ ] 禁用拖拽/缩放
  - [ ] 隐藏窗口装饰
- [ ] 实现滑入/滑出动效
  - [ ] 从底部滑入（Spring 动画）
  - [ ] 向下滑出（Spring 动画）
- [ ] 实现 HWTier 联动
  - [ ] Low Tier 可选择关闭动效
- [ ] 编写单元测试

#### Day 4: FloatingPolicy
- [ ] 创建 FloatingPolicy 类
  - [ ] 继承 LayoutPolicy
- [ ] 实现 Windows 风格浮动策略
  - [ ] 可拖拽窗口
  - [ ] 可缩放窗口（8个方向）
  - [ ] 标题栏+窗口控制按钮
  - [ ] 窗口阴影
- [ ] 实现缩放淡入淡出动效
  - [ ] 缩放+透明度组合（Ease 曲线）
- [ ] 实现窗口约束
  - [ ] 最小尺寸限制
  - [ ] 最大尺寸限制
  - [ ] 边界约束
- [ ] 编写单元测试

#### Day 5: SplitPolicy
- [ ] 创建 SplitPolicy 类
  - [ ] 继承 LayoutPolicy
- [ ] 实现分屏布局
  - [ ] 左右分屏（50:50）
  - [ ] 上下分屏（50:50）
  - [ ] 自定义比例
- [ ] 实现分屏分隔条
  - [ ] 可拖拽调整比例
  - [ ] 双击恢复均分
- [ ] 实现分屏窗口切换
  - [ ] Tab/焦点切换
- [ ] 编写单元测试

---

### Week 3: 窗口动效与任务切换

#### Day 1-2: 窗口动效系统
- [ ] 创建 WindowAnimationManager 类
- [ ] 实现开启动效
  - [ ] iOS 风格：从底部滑入（Spring）
  - [ ] Windows 风格：缩放淡入（EaseInOut）
- [ ] 实现关闭动效
  - [ ] iOS 风格：向下滑出
  - [ ] Windows 风格：缩放淡出
- [ ] 实现最小化/恢复动效
  - [ ] 缩小到任务栏
  - [ ] 从任务栏恢复
- [ ] 实现最大化/恢复动效
  - [ ] 平滑过渡到目标尺寸
- [ ] 实现 HWTier 联动
  - [ ] Low Tier 关闭所有窗口动效
  - [ ] Mid Tier 使用简化动效
- [ ] 编写单元测试

#### Day 3-4: AppSwitcher 任务切换器
- [ ] 创建 AppSwitcher 基类
- [ ] 实现 iOS 风格卡片切换器
  - [ ] 上滑调出（手势识别）
  - [ ] 卡片缩略图预览
  - [ ] 左右滑动切换
  - [ ] 上滑关闭应用
  - [ ] 点击切换到应用
- [ ] 实现 Windows 风格 Task View
  - [ ] 任务栏悬停预览
  - [ ] Win+Tab 全览模式
  - [ ] 虚拟桌面（可选）
- [ ] 与导航系统集成
  - [ ] 响应手势事件
  - [ ] 响应快捷键
- [ ] 编写单元测试

#### Day 5: 集成测试与优化
- [ ] 编写集成测试
  - [ ] 多窗口并发测试
  - [ ] 策略切换测试
  - [ ] 动效性能测试
- [ ] 性能优化
  - [ ] 减少不必要的重绘
  - [ ] 动画批处理
- [ ] 内存泄漏检查

---

## 三、验收标准

### 功能验收
- [ ] 可打开/关闭/切换至少 3 个并发应用
- [ ] iOS/Windows 两种布局策略均可运作
- [ ] 窗口 Z-order 管理正确
- [ ] 焦点窗口切换正常
- [ ] 分屏功能正常

### 性能验收
- [ ] 窗口切换延迟 < 100ms
- [ ] 动画帧率稳定 60fps（Mid/High Tier）
- [ ] 动画帧率 30fps（Low Tier，如启用）
- [ ] 内存占用合理（< 50MB）

### 兼容性验收
- [ ] Linux EGLFS 环境正常
- [ ] Windows 开发环境正常

---

## 四、文件清单（待实现）

### 头文件
- [ ] `ui/desktop/window_manager.h`
- [ ] `ui/desktop/window_info.h`
- [ ] `ui/desktop/window_state.h`
- [ ] `ui/desktop/zorder_manager.h`
- [ ] `ui/desktop/layout_policy.h`
- [ ] `ui/desktop/policy_registry.h`
- [ ] `ui/desktop/policy/fullscreen_policy.h`
- [ ] `ui/desktop/policy/floating_policy.h`
- [ ] `ui/desktop/policy/split_policy.h`
- [ ] `ui/desktop/window_animation_manager.h`
- [ ] `ui/desktop/app_switcher.h`
- [ ] `ui/desktop/ios_app_switcher.h`
- [ ] `ui/desktop/windows_task_view.h`

### 源文件
- [ ] `src/desktop/window_manager.cpp`
- [ ] `src/desktop/window_info.cpp`
- [ ] `src/desktop/zorder_manager.cpp`
- [ ] `src/desktop/layout_policy.cpp`
- [ ] `src/desktop/policy_registry.cpp`
- [ ] `src/desktop/policy/fullscreen_policy.cpp`
- [ ] `src/desktop/policy/floating_policy.cpp`
- [ ] `src/desktop/policy/split_policy.cpp`
- [ ] `src/desktop/window_animation_manager.cpp`
- [ ] `src/desktop/app_switcher.cpp`
- [ ] `src/desktop/ios_app_switcher.cpp`
- [ ] `src/desktop/windows_task_view.cpp`

### 测试文件
- [ ] `tests/unit/desktop/window/test_window_manager.cpp`
- [ ] `tests/unit/desktop/window/test_window_info.cpp`
- [ ] `tests/unit/desktop/window/test_zorder_manager.cpp`
- [ ] `tests/unit/desktop/window/test_layout_policy.cpp`
- [ ] `tests/unit/desktop/window/test_fullscreen_policy.cpp`
- [ ] `tests/unit/desktop/window/test_floating_policy.cpp`
- [ ] `tests/unit/desktop/window/test_split_policy.cpp`
- [ ] `tests/unit/desktop/window/test_window_animation.cpp`
- [ ] `tests/integration/desktop/test_window_integration.cpp`

---

## 五、相关文档

- 设计文档: [../desktop/summary.md](summary.md) Phase D 节
- 依赖: [06_infrastructure.md](06_infrastructure.md), [07_render_backend.md](07_render_backend.md)
- 参考项目: Mutter, KWin, Compiz

---

*最后更新: 2026-03-12*
