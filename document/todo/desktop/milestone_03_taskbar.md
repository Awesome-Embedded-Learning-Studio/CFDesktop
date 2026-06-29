---
title: "Milestone 3: 任务栏/导航栏"
description: "预计周期: 5-7 天，前置依赖: Milestone 2: 状态栏"
---

# Milestone 3: 任务栏/导航栏

> **状态**: ✅ 最小切片完成（`centered_taskbar` / `start_button` / `taskbar_icon` 已落地：Bottom 面板注册 + 居中图标 + hover 放大 + 自绘 ripple + 运行指示器；`appClicked` 已接 `AppLaunchService::launch` 真启动）
> **预计周期**: 5-7 天
> **前置依赖**: [Milestone 2: 状态栏](milestone_02_status_bar.md)
> **目标**: 屏幕底部出现任务栏 (Windows 11 风格居中图标)，支持 hover 动画和点击反馈

---

## 一、阶段目标

实现 Windows 11 风格的 `CenteredTaskbar` 面板，注册到 PanelManager 作为 Bottom edge panel，展示居中的应用图标，带 hover 放大效果和点击反馈。

**完成标志**: 屏幕底部有一条半透明任务栏，居中显示若干应用图标，鼠标 hover 时图标放大，点击有 ripple 反馈。

---

## 二、当前状态分析

### 已有基础设施

| 组件 | 文件 | 状态 |
|------|------|------|
| `IPanel` 接口 | `desktop/ui/components/IPanel.h` | ✅ 可注册 Bottom 面板 |
| `PanelManager` | `desktop/ui/components/PanelManager.h` | ✅ 支持 `PanelPosition::Bottom` |
| Material Button (5 变体) | `ui/widget/material/widget/button/` | ✅ 可作为图标按钮 |
| StateMachine | `ui/widget/material/base/state_machine.h` | ✅ Pressed/Hovered/Focused/Disabled 状态 |
| RippleHelper | `ui/widget/material/base/ripple_helper.h` | ✅ Material ripple 点击效果 |
| MdElevationController | `ui/widget/material/base/md_elevation_controller.h` | ✅ 阴影/高度管理 |
| Animation Factory | `ui/components/animation/` | ✅ Fade/Slide/Scale 动画 |
| ThemeManager | `ui/core/theme_manager.h` | ✅ 主题系统 |

### 关键缺口

1. **没有 Taskbar 类** — 需要新建
2. **没有应用图标数据模型** — 需要 `AppEntry` 结构体
3. **没有 TaskbarIcon Widget** — 需要带动画的图标控件
4. **没有运行中应用指示器** — 底部小圆点/下划线

---

## 三、待实现任务

### Day 1-2: 数据模型与 TaskbarIcon 控件

#### Step 1: 定义应用条目数据结构
- [ ] 创建文件 `desktop/ui/components/taskbar/app_entry.h`
  ```cpp
  struct AppEntry {
      QString app_id;       // 唯一标识
      QString display_name; // 显示名称
      QString icon_path;    // 图标路径 (先留空用占位)
      QString exec_command; // 启动命令 (QProcess 用)
      bool is_running{false};
  };
  ```
- [ ] 提供 `defaultApps()` 工厂函数，返回一组示例条目：
  - 文件管理器、终端、设置、浏览器 (占位)

#### Step 2: 创建 TaskbarIcon Widget
- [ ] 创建文件 `desktop/ui/components/taskbar/taskbar_icon.h/.cpp`
- [ ] 功能：
  - 继承 QWidget
  - 绘制 48x48 的图标区域 (先用纯色圆角方块 + 首字母)
  - hover 时放大到 56x56 (使用 QPropertyAnimation 或 ScaleAnimation)
  - 点击时触发 ripple 效果 (复用 RippleHelper)
  - 底部小圆点指示器 (当 `is_running` 时显示)
  - 信号: `clicked(app_id)`
- [ ] paintEvent 实现：
  ```cpp
  void TaskbarIcon::paintEvent(QPaintEvent*) {
      QPainter p(this);
      p.setRenderHint(QPainter::Antialiasing);
      // 1. 图标方块 (圆角矩形)
      // 2. 文字 (首字母)
      // 3. 运行指示器 (底部小圆点)
      // 4. Ripple 效果 (由 RippleHelper 管理)
  }
  ```
- [ ] enterEvent / leaveEvent：
  - 触发 ScaleAnimation (复用 `ui/components/animation/`)

**可复用**:
- `ui/widget/material/base/state_machine.h` — 状态管理
- `ui/widget/material/base/ripple_helper.h` — ripple 效果
- `ui/components/animation/` — 动画工厂

### Day 3-4: CenteredTaskbar 面板实现

#### Step 3: 创建 CenteredTaskbar 类
- [ ] 创建文件 `desktop/ui/components/taskbar/centered_taskbar.h/.cpp`
- [ ] 类声明：
  ```cpp
  class CenteredTaskbar : public QWidget, public IPanel {
      Q_OBJECT
  public:
      explicit CenteredTaskbar(QWidget* parent = nullptr);

      // IPanel
      PanelPosition position() const override { return PanelPosition::Bottom; }
      int priority() const override { return 100; }
      int preferredSize() const override { return 64; }  // 64dp
      QWidget* widget() const override;

      void setApps(const QList<AppEntry>& apps);
      void updateRunningState(const QString& app_id, bool running);

  signals:
      void appClicked(const QString& app_id);
      void launcherRequested();  // 点击"开始"按钮

  protected:
      void paintEvent(QPaintEvent*) override;
  };
  ```

#### Step 4: 实现 CenteredTaskbar 布局
- [ ] `setupUi()`:
  - 创建 `QHBoxLayout`
  - 左侧弹簧 → 居中图标区 → 右侧系统区
  - 每个 AppEntry 创建一个 TaskbarIcon
  - 可选: 最左侧添加"开始"按钮 (触发 launcherRequested)
- [ ] `paintEvent()`:
  - 绘制半透明背景 (使用 `theme->colorScheme().surfaceContainer()` + 透明度)
  - 顶部 1px 分割线
  - 毛玻璃效果 (可选，Low Tier 跳过)
- [ ] 主题联动：
  - 背景色: `theme->colorScheme().surfaceContainer()`
  - 文字色: `theme->colorScheme().onSurface()`
  - 图标区域色: `theme->colorScheme().surfaceContainerHighest()`

#### Step 5: 实现 hover/点击动画
- [ ] TaskbarIcon 的 hover 放大动画:
  - 使用 `ui/components/animation/` 的 ScaleAnimation
  - hover → scale(1.15)，leave → scale(1.0)
  - duration: `theme->motionSpec().medium()`
- [ ] TaskbarIcon 的点击 ripple:
  - 复用 `ui/widget/material/base/ripple_helper.h`
  - ripple 颜色: `theme->colorScheme().onSurfaceVariant()` + 半透明

### Day 5: 注册到 PanelManager

#### Step 6: 在 CFDesktopEntity::run_init() 中接入
- [ ] 创建 CenteredTaskbar 实例
- [ ] 加载默认应用列表
- [ ] 注册到 PanelManager：
  ```cpp
  auto* taskbar = new CenteredTaskbar(desktop_entity_);
  taskbar->setApps(defaultApps());
  panel_mgr->registerPanel(taskbar->GetWeak());
  ```
- [ ] 连接 `appClicked` 信号 (暂时只打 log)
- [ ] 连接 `launcherRequested` 信号 (MS4 会用到)

#### Step 7: 验证布局联动
- [ ] PanelManager 现在同时有 Top (StatusBar) 和 Bottom (Taskbar)
- [ ] `availableGeometry()` 顶部和底部各缩小
- [ ] ShellLayer 居中显示

### Day 6-7: 细节调优

#### Step 8: 视觉细节
- [ ] Taskbar 圆角 (顶部两角圆角)
- [ ] 图标间距微调 (8-12dp)
- [ ] 运行中应用指示器样式 (底部小圆点 / 下划线)
- [ ] 动画曲线调整 (Material Motion)

#### Step 9: 主题切换测试
- [ ] Light/Dark 切换时 taskbar 正确变色
- [ ] 动画参数跟随主题 (duration, curve)

---

## 四、关键文件清单

### 需要新建的文件
| 文件 | 内容 |
|------|------|
| `desktop/ui/components/taskbar/app_entry.h` | 应用条目数据结构 |
| `desktop/ui/components/taskbar/taskbar_icon.h` | 图标 Widget 声明 |
| `desktop/ui/components/taskbar/taskbar_icon.cpp` | 图标 Widget 实现 |
| `desktop/ui/components/taskbar/centered_taskbar.h` | 任务栏声明 |
| `desktop/ui/components/taskbar/centered_taskbar.cpp` | 任务栏实现 |

### 需要修改的文件
| 文件 | 修改内容 |
|------|----------|
| `desktop/ui/CFDesktopEntity.cpp` | 创建 Taskbar 并注册 |
| `desktop/ui/components/CMakeLists.txt` | 添加新文件 |

### 参考文件 (只读)
| 文件 | 用途 |
|------|------|
| `desktop/ui/components/IPanel.h:27-32` | `PanelPosition::Bottom` |
| `ui/widget/material/widget/button/` | 按钮实现参考 (hover/ripple) |
| `ui/widget/material/base/state_machine.h` | 状态机 |
| `ui/widget/material/base/ripple_helper.h` | ripple 效果 |
| `ui/components/animation/` | 动画工厂 |
| `ui/core/theme_manager.h` | 主题系统 |
| `document/todo/desktop/10_shell_navigation.md:144-165` | CenteredTaskbar 原始设计 |

---

## 五、验收标准

- [ ] 屏幕底部有一条高度约 64dp 的任务栏
- [ ] 图标居中排列，显示 4-6 个占位应用
- [ ] 鼠标 hover 时图标放大，带平滑动画
- [ ] 点击图标有 ripple 反馈
- [ ] 运行中应用底部有指示器 (圆点或下划线)
- [ ] PanelManager 的 `availableGeometry()` 正确扣除顶部和底部
- [ ] Light/Dark 主题切换时任务栏正确变色

---

*最后更新: 2026-03-31*
