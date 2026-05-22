---
title: "Milestone 4: 应用启动器"
description: "预计周期: 5-7 天，前置依赖: Milestone 3: 任务栏"
---

# Milestone 4: 应用启动器

> **状态**: ⬜ 待开始
> **预计周期**: 5-7 天
> **前置依赖**: [Milestone 3: 任务栏](milestone_03_taskbar.md)
> **目标**: 点击任务栏"开始"按钮或桌面区域，弹出一个应用网格，可启动外部程序

---

## 一、阶段目标

实现一个弹出式应用启动器 (类似 Windows 11 开始菜单)，展示应用图标网格，点击可启动外部进程。

**完成标志**: 点击任务栏"开始"按钮，弹出居中的应用网格弹窗，点击某个应用条目后启动外部程序 (如 xterm)。

---

## 二、当前状态分析

### 已有基础设施

| 组件 | 文件 | 状态 |
|------|------|------|
| `AppEntry` 数据结构 | MS3 中创建 | ✅ app_id / display_name / exec_command |
| Material Button | `ui/widget/material/widget/button/` | ✅ 可复用 |
| Material ListView | `ui/widget/material/widget/listview/` | ✅ 可用于列表模式 |
| Animation Engine | `ui/components/animation/` | ✅ Fade/Slide/Scale 动画 |
| ThemeManager | `ui/core/theme_manager.h` | ✅ 主题系统 |
| Taskbar `launcherRequested()` 信号 | MS3 中定义 | ✅ 触发启动器 |

### 关键缺口

1. **没有 AppLauncher 类** — 弹出式应用网格
2. **没有应用图标网格布局** — 需要自定义 grid
3. **没有外部进程启动逻辑** — 需要 QProcess
4. **没有启动器入场/退场动画**

---

## 三、待实现任务

### Day 1-2: AppLauncher 弹窗框架

#### Step 1: 创建 AppLauncher 类
- [ ] 创建文件 `desktop/ui/components/launcher/app_launcher.h/.cpp`
- [ ] 类声明：
  ```cpp
  class AppLauncher : public QWidget {
      Q_OBJECT
  public:
      explicit AppLauncher(QWidget* parent = nullptr);

      void setApps(const QList<AppEntry>& apps);
      void showLauncher();   // 带入场动画弹出
      void hideLauncher();   // 带退场动画收起
      bool isVisible() const;

  signals:
      void appLaunched(const QString& app_id, const QString& exec);
      void closed();

  protected:
      void paintEvent(QPaintEvent*) override;
      bool eventFilter(QObject* obj, QEvent* event) override;

  private:
      void setupUi();
      void applyTheme();
  };
  ```
- [ ] 继承 QWidget，设置 `Qt::Popup` 或 `Qt::Tool` 窗口属性
  - 推荐用 `Qt::Popup`：点击外部自动关闭
  - 或用 overlay widget 覆盖在 ShellLayer 上

#### Step 2: 实现弹窗定位
- [ ] 计算弹窗位置：
  - 水平居中
  - 底部对齐任务栏顶部，向上展开
  - 宽度: 屏幕宽度的 50-60%
  - 高度: 屏幕高度的 50-70% (或自适应内容)
- [ ] 设置 `Qt::FramelessWindowHint` 去掉系统边框

#### Step 3: 实现背景与圆角
- [ ] `paintEvent()`:
  - 绘制圆角矩形背景 (12dp 圆角)
  - 背景色: `theme->colorScheme().surfaceContainerLow()`
  - 外阴影: 使用 `MdElevationController` (level 3)
  - 或用 QPainterPath + QRadialGradient 模拟阴影

### Day 3-4: 应用网格与交互

#### Step 4: 实现应用图标网格
- [ ] 创建 `LauncherGridItem` Widget (或复用 TaskbarIcon 并扩展)
  - 64x64 图标区域 + 下方文字
  - hover 效果 (背景变色)
  - 点击 ripple
- [ ] 在 AppLauncher 中使用 `QGridLayout` 或自定义 grid:
  - 每行 4-6 个图标
  - 自动换行
  - 居中对齐
- [ ] 为每个 AppEntry 创建 LauncherGridItem
- [ ] 点击 item → emit `appLaunched(app_id, exec_command)`

#### Step 5: 搜索框 (可选，建议加入)
- [ ] 在顶部添加搜索 `QLineEdit`
  - Material TextField 样式
  - 实时过滤应用列表
  - Placeholder: "搜索应用..."
- [ ] 过滤逻辑：
  - 按 display_name 模糊匹配
  - 隐藏不匹配的 grid item

#### Step 6: 外部进程启动
- [ ] 创建 `AppLaunchService` 类 (或在 AppLauncher 中直接处理)
  ```cpp
  class AppLaunchService {
  public:
      static bool launchApp(const QString& exec_command);
  };
  ```
- [ ] 实现：
  ```cpp
  bool AppLaunchService::launchApp(const QString& exec) {
      return QProcess::startDetached(exec);
  }
  ```
- [ ] 连接信号：
  ```cpp
  connect(launcher, &AppLauncher::appLaunched,
          [](const QString& id, const QString& exec) {
              AppLaunchService::launchApp(exec);
              launcher->hideLauncher();
          });
  ```

### Day 5: 入场/退场动画

#### Step 7: 实现弹出动画
- [ ] 入场动画：
  - 从底部滑入 + 淡入 (组合动画)
  - 复用 `ui/components/animation/` 的 SlideAnimation + FadeAnimation
  - duration: `theme->motionSpec().medium()` (~300ms)
  - curve: `QEasingCurve::OutCubic`
- [ ] 退场动画：
  - 向底部滑出 + 淡出
  - duration: `theme->motionSpec().short()` (~200ms)
  - 动画完成后调用 `hide()`

#### Step 8: 点击外部关闭
- [ ] 如果使用 `Qt::Popup`，系统自动处理
- [ ] 如果使用 overlay widget，需要 eventFilter 检测外部点击
- [ ] ESC 键关闭

### Day 6: 集成到桌面

#### Step 9: 在 CFDesktopEntity 中连接
- [ ] 创建 AppLauncher 实例
- [ ] 设置应用列表 (与 Taskbar 共享同一份 AppEntry 列表)
- [ ] 连接 Taskbar 的 `launcherRequested()` → `appLauncher->showLauncher()`
- [ ] 连接 AppLauncher 的 `appLaunched()` → 启动进程
- [ ] 启动成功后更新 Taskbar 的运行状态指示器

#### Step 10: 示例应用条目
- [ ] 在 WSL 环境下提供可启动的应用：
  ```cpp
  QList<AppEntry> defaultApps() {
      return {
          {"terminal", "终端", "", "xterm"},
          {"files", "文件管理器", "", "pcmanfm"},
          {"editor", "文本编辑器", "", "gedit"},
          {"browser", "浏览器", "", "firefox"},
          {"settings", "设置", "", ""},
      };
  }
  ```
- [ ] Windows 环境下提供 `notepad`, `explorer`, `calc` 等

### Day 7: 调优与验证

#### Step 11: 视觉调优
- [ ] 网格间距: 8-12dp
- [ ] 图标文字截断 (过长时省略号)
- [ ] 弹窗最大高度 + 内部滚动 (如应用过多)
- [ ] 动画流畅度

---

## 四、关键文件清单

### 需要新建的文件
| 文件 | 内容 |
|------|------|
| `desktop/ui/components/launcher/app_launcher.h` | 启动器声明 |
| `desktop/ui/components/launcher/app_launcher.cpp` | 启动器实现 |
| `desktop/ui/components/launcher/launcher_grid_item.h` | 网格图标项 (可选) |
| `desktop/ui/components/launcher/launcher_grid_item.cpp` | 网格图标实现 (可选) |
| `desktop/ui/components/launcher/app_launch_service.h` | 进程启动服务 (可选) |
| `desktop/ui/components/launcher/app_launch_service.cpp` | 进程启动实现 (可选) |

### 需要修改的文件
| 文件 | 修改内容 |
|------|----------|
| `desktop/ui/CFDesktopEntity.cpp` | 创建 AppLauncher，连接信号 |
| `desktop/ui/components/taskbar/centered_taskbar.h` | 确认 launcherRequested() 信号 |

### 参考文件 (只读)
| 文件 | 用途 |
|------|------|
| `ui/widget/material/widget/button/` | 按钮参考 |
| `ui/widget/material/widget/textfield/` | 搜索框参考 |
| `ui/components/animation/` | 动画工厂 |
| `ui/widget/material/base/md_elevation_controller.h` | 阴影系统 |
| `document/todo/desktop/10_shell_navigation.md:186-203` | AppLauncher 原始设计 |

---

## 五、验收标准

- [ ] 点击任务栏"开始"按钮弹出应用启动器
- [ ] 启动器居中显示在任务栏上方
- [ ] 应用图标以网格形式排列
- [ ] 入场有滑入+淡入动画，退场有滑出+淡出动画
- [ ] 点击应用条目能启动外部进程 (如 xterm)
- [ ] 点击启动器外部或 ESC 可关闭
- [ ] 搜索框实时过滤应用列表 (如实现)
- [ ] Light/Dark 主题切换时启动器正确变色

---

*最后更新: 2026-03-31*
