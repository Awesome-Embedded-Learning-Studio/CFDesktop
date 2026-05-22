---
title: "Milestone 5: 窗口管理可见"
description: "预计周期: 7-10 天，前置依赖: Milestone 3: 任务栏 (任务栏联动)"
---

# Milestone 5: 窗口管理可见

> **状态**: ⬜ 待开始
> **预计周期**: 7-10 天
> **前置依赖**: [Milestone 3: 任务栏](milestone_03_taskbar.md) (任务栏联动)
> **目标**: 外部应用窗口能被管理——有窗口装饰，支持最小化/最大化/关闭，任务栏同步显示运行中应用

---

## 一、阶段目标

扩展 WindowManager 使其支持窗口状态管理，实现窗口装饰 Widget (标题栏 + 控制按钮)，并与 Taskbar 联动显示运行中应用。

**完成标志**: 启动外部应用后，任务栏出现对应图标，点击图标可 raise/minimize 窗口，窗口有自定义标题栏装饰。

---

## 二、当前状态分析

### 已有基础设施

| 组件 | 文件 | 状态 |
|------|------|------|
| `WindowManager` | `desktop/ui/components/WindowManager.h/.cpp` | ✅ 基础：`create_window()` / `find_window()` / `request_close_window()` / `raise_a_window()` |
| `IWindow` | `desktop/ui/components/IWindow.h` | ✅ `title()` / `geometry()` / `close()` / `raise()` / `set_geometry()` |
| `IWindowBackend` | `desktop/ui/components/IWindowBackend.h` | ✅ `window_came` / `window_gone` 信号 |
| `WSL X11 Backend` | `desktop/ui/platform/linux_wsl/` | ✅ XCB 窗口追踪 |
| `Windows Backend` | `desktop/ui/platform/windows/` | ✅ Win32 窗口追踪 |
| Material Button | `ui/widget/material/widget/button/` | ✅ 窗口控制按钮 |
| CenteredTaskbar | MS3 中实现 | ✅ `updateRunningState()` |

### 关键缺口

1. **没有 WindowInfo 数据结构** — 只有 IWindow 的基础属性
2. **没有 WindowState 状态机** — 窗口没有 Normal/Minimized/Maximized 概念
3. **没有窗口装饰** — 没有自定义标题栏
4. **没有布局策略** — 窗口没有布局管理
5. **Taskbar 未与 WindowManager 联动** — 运行状态未同步

---

## 三、待实现任务

### Day 1-2: 窗口数据模型

#### Step 1: 定义 WindowInfo
- [ ] 创建文件 `desktop/ui/components/window_manager/window_info.h`
  ```cpp
  enum class WindowState {
      Normal,
      Minimized,
      Maximized,
      Fullscreen,
      Closing,
      Closed
  };

  struct WindowInfo {
      QString window_id;
      QString title;
      QString icon_hint;     // 图标提示 (PID → 图标查找)
      qint64 pid{0};
      QRect geometry;
      WindowState state{WindowState::Normal};
      int z_index{0};
      bool is_always_on_top{false};
      QDateTime created_at;
  };
  ```
- [ ] 实现窗口状态转换规则：
  ```
  Normal    → Minimized  (最小化)
  Normal    → Maximized  (最大化)
  Normal    → Fullscreen (全屏)
  Minimized → Normal     (恢复)
  Maximized → Normal     (恢复)
  *         → Closing    (关闭中)
  Closing   → Closed     (已关闭)
  ```

#### Step 2: 扩展 WindowManager
- [ ] 修改 `desktop/ui/components/WindowManager.h`:
  - 添加 `QHash<win_id_t, WindowInfo> window_infos_` 存储窗口信息
  - 添加方法：
    ```cpp
    WindowInfo getWindowInfo(const win_id_t& id) const;
    QList<WindowInfo> getAllWindowInfos() const;
    void minimizeWindow(const win_id_t& id);
    void maximizeWindow(const win_id_t& id);
    void restoreWindow(const win_id_t& id);
    ```
  - 添加信号：
    ```cpp
    void windowInfoUpdated(const win_id_t& id, const WindowInfo& info);
    void windowStateChanged(const win_id_t& id, WindowState new_state);
    ```
- [ ] 在 `window_came` 回调中创建 WindowInfo 并填充
- [ ] 在 `window_gone` 回调中将状态设为 Closed

### Day 3-4: 窗口装饰 Widget

#### Step 3: 创建 WindowDecoration 类
- [ ] 创建文件 `desktop/ui/components/window_manager/window_decoration.h/.cpp`
- [ ] 这是一个**可视化装饰层**，显示在窗口上方 (overlay 模式)
  ```cpp
  class WindowDecoration : public QWidget {
      Q_OBJECT
  public:
      explicit WindowDecoration(QWidget* parent = nullptr);
      void setTitle(const QString& title);
      void setIcon(const QPixmap& icon);
  signals:
      void closeClicked();
      void minimizeClicked();
      void maximizeClicked();
  protected:
      void paintEvent(QPaintEvent*) override;
      void mousePressEvent(QMouseEvent*) override;
      void mouseMoveEvent(QMouseEvent*) override;  // 拖拽标题栏
  private:
      void setupButtons();
      void applyTheme();
  };
  ```
- [ ] 布局：
  ```
  ┌─────────────────────────────────────────────┐
  │ 📄 Window Title          ─ □ ✕ │  ← 标题栏 (32dp)
  ├─────────────────────────────────────────────┤
  │                                             │
  │              (窗口内容区)                     │
  │                                             │
  └─────────────────────────────────────────────┘
  ```
- [ ] 标题栏内容：
  - 左侧: 应用图标 + 窗口标题
  - 右侧: 最小化(─)、最大化(□)、关闭(✕) 按钮
- [ ] `paintEvent()`:
  - 绘制标题栏背景 (`theme->colorScheme().surface()`)
  - 绘制标题文字 (`theme->colorScheme().onSurface()`)
  - 绘制窗口边框/阴影
- [ ] 窗口控制按钮：
  - 使用 Material IconButton (文本变体) 或自定义绘制
  - 关闭按钮 hover: 红色背景 (`theme->colorScheme().error()`)
  - 其他按钮 hover: `theme->colorScheme().surfaceVariant()`

#### Step 4: 标题栏拖拽移动
- [ ] `mousePressEvent`: 记录起始位置
- [ ] `mouseMoveEvent`: 计算偏移 → 调用 `IWindow::set_geometry()`
- [ ] 双击标题栏 → 切换最大化/恢复

**注意**: 窗口装饰的位置取决于平台后端。在 WSL X11 客户端模式下，外部窗口由 XWayland/Weston 管理，CFDesktop 只能追踪而无法直接装饰。窗口装饰需要以下策略之一：
- **策略 A (推荐初期)**: 在 CFDesktop 自己的 overlay 上渲染装饰信息，作为纯视觉指示
- **策略 B (后期)**: 实现 X11 窗口管理器功能，实际管理外部窗口
- **策略 C (后期)**: 在 Wayland Compositor 模式下完全控制窗口装饰

### Day 5-6: Taskbar 联动

#### Step 5: Taskbar ↔ WindowManager 联动
- [ ] 在 CFDesktopEntity 中连接：
  ```cpp
  connect(wm, &WindowManager::windowInfoUpdated,
          taskbar, &CenteredTaskbar::onWindowInfoUpdated);
  connect(taskbar, &CenteredTaskbar::appClicked,
          wm, &WindowManager::onTaskbarAppClicked);
  ```
- [ ] `onWindowInfoUpdated()`:
  - 新窗口出现 → taskbar 添加图标 (匹配 AppEntry)
  - 窗口关闭 → taskbar 移除图标
  - 窗口状态变化 → 更新指示器
- [ ] `onTaskbarAppClicked()`:
  - 窗口已激活 → minimize
  - 窗口已最小化 → restore + raise
  - 未运行 → 启动应用

#### Step 6: 运行中应用列表同步
- [ ] WindowManager 维护 `QList<WindowInfo>` 活跃窗口列表
- [ ] 信号 `windowListChanged()` → Taskbar 刷新图标

### Day 7-8: 布局策略 (基础版)

#### Step 7: 实现 FloatingPolicy (最简版)
- [ ] 创建文件 `desktop/ui/components/window_manager/floating_policy.h/.cpp`
- [ ] 功能：
  - 新窗口出现在屏幕中心
  - 窗口可拖拽 (通过 WindowDecoration)
  - 窗口可缩放 (8 个方向的 resize handle，可延后)
  - 最小尺寸限制 (320x240)
- [ ] 计算新窗口初始位置：
  ```cpp
  QRect FloatingPolicy::initialGeometry(const QRect& available) {
      QSize size(800, 600);  // 默认大小
      QPoint center = available.center();
      return QRect(center.x() - size.width()/2,
                   center.y() - size.height()/2,
                   size.width(), size.height());
  }
  ```

### Day 9-10: 集成与验证

#### Step 8: 全流程集成
- [ ] 启动应用 → AppLauncher → QProcess
- [ ] 外部窗口出现 → IWindowBackend::window_came → WindowManager 注册
- [ ] WindowManager 更新 WindowInfo → 通知 Taskbar
- [ ] Taskbar 显示运行中图标
- [ ] 点击 Taskbar 图标 → raise/minimize 窗口

#### Step 9: 验证场景
- [ ] 启动 xterm → 任务栏出现终端图标
- [ ] 点击任务栏图标 → 终端窗口被 raise
- [ ] 再次点击 → 终端窗口被 minimize
- [ ] 关闭终端 → 任务栏图标消失

---

## 四、关键文件清单

### 需要新建的文件
| 文件 | 内容 |
|------|------|
| `desktop/ui/components/window_manager/window_info.h` | 窗口数据模型 |
| `desktop/ui/components/window_manager/window_decoration.h` | 窗口装饰声明 |
| `desktop/ui/components/window_manager/window_decoration.cpp` | 窗口装饰实现 |
| `desktop/ui/components/window_manager/floating_policy.h` | 浮动布局策略 |
| `desktop/ui/components/window_manager/floating_policy.cpp` | 浮动策略实现 |

### 需要修改的文件
| 文件 | 修改内容 |
|------|----------|
| `desktop/ui/components/WindowManager.h` | 添加 WindowInfo 存储、状态管理方法、新信号 |
| `desktop/ui/components/WindowManager.cpp` | 实现扩展方法 |
| `desktop/ui/CFDesktopEntity.cpp` | 连接 WM ↔ Taskbar 信号 |
| `desktop/ui/components/taskbar/centered_taskbar.h` | 添加 `onWindowInfoUpdated()` 方法 |
| `desktop/ui/components/CMakeLists.txt` | 添加新文件 |

### 参考文件 (只读)
| 文件 | 用途 |
|------|------|
| `desktop/ui/components/WindowManager.h:39` | 现有 WM 类 |
| `desktop/ui/components/IWindow.h` | IWindow 接口 |
| `desktop/ui/components/IWindowBackend.h` | 窗口后端信号 |
| `desktop/ui/platform/linux_wsl/wsl_x11_window_backend.cpp` | X11 窗口追踪 |
| `desktop/ui/platform/windows/windows_window_backend.cpp` | Windows 窗口追踪 |
| `document/todo/desktop/09_window_manager.md` | 原始窗口管理设计 |

---

## 五、验收标准

- [ ] 启动外部应用后 WindowManager 能追踪到新窗口
- [ ] WindowManager 维护 WindowInfo 数据 (标题、PID、状态)
- [ ] 窗口状态可在 Normal/Minimized/Maximized 之间转换
- [ ] Taskbar 同步显示运行中应用图标
- [ ] 点击 Taskbar 图标可 raise/minimize 对应窗口
- [ ] 窗口有自定义装饰 (至少标题 + 关闭按钮的视觉呈现)
- [ ] 窗口关闭后 Taskbar 图标自动消失
- [ ] Light/Dark 主题切换时装饰正确变色

---

*最后更新: 2026-03-31*
