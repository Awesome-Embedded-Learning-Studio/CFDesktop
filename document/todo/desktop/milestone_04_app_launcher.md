---
title: "Milestone 4: 应用启动器"
description: "预计周期: 5-7 天，前置依赖: Milestone 3: 任务栏"
---

# Milestone 4: 应用启动器

> **状态**: 🚧 闭环跑通·网格待做（`app_launcher` / `launcher_tile` / `app_launch_service` 已落地，点 Files/Browser 可真打开外部进程；开始菜单弹窗网格 + 搜索框待做）
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

#### Step 5: 搜索框（明确待办，非可选）
> 注：上一版标"可选"导致代码为零（三层落差）。6ULL 上只做 App 名匹配，不做全文索引。
- [ ] 在顶部添加搜索 `QLineEdit`
  - Material TextField 样式
  - 实时过滤应用列表
  - Placeholder: "搜索应用..."
- [ ] 应用索引源：扫描 `~/.local/share/applications` + `/usr/share/applications` 的 `.desktop` 文件，解析 display_name/Icon/Exec
- [ ] 过滤逻辑：
  - 按 display_name 模糊匹配
  - 隐藏不匹配的 grid item

#### Step 6: 外部进程启动（`AppLaunchService` 已落地）
- [x] `AppLaunchService` 类已存在（`desktop/ui/components/launcher/app_launch_service.h/.cpp`），走 `QProcess::startDetached` 真启动并记 PID
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
| ~~`desktop/ui/components/launcher/app_launcher.h`~~ | ⚠️ **已落地** |
| ~~`desktop/ui/components/launcher/app_launcher.cpp`~~ | ⚠️ **已落地** |
| `desktop/ui/components/launcher/launcher_tile.h/.cpp` | 网格图标项（已落地，原计划名 launcher_grid_item） |
| ~~`desktop/ui/components/launcher/app_launch_service.h`~~ | ⚠️ **已落地**（QProcess::startDetached 真启动 + 记 PID） |
| ~~`desktop/ui/components/launcher/app_launch_service.cpp`~~ | ⚠️ **已落地** |

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
- [ ] 搜索框实时过滤应用列表（扫描 .desktop，按 display_name 模糊匹配）
- [ ] Light/Dark 主题切换时启动器正确变色

---

## 六、builtin 正式化 + 双态框架（2026-07 落地）

> **状态**: ✅ 已落地（`feat/app-dual-mode` 分支）

把 calculator 路线的「独立进程 App」与 about 的「进程内 builtin」**正式化为统一双态框架**,由硬件档位裁决。

### 落地点

- **`LaunchKind` 枚举**([app_entry.h](../../../desktop/ui/components/app_entry.h)):`Auto`/`DetachedProcess`/`BuiltinPanel`,替代原 `builtin:` 字符串前缀 hack
- **`IBuiltinPanel` 接口**([ibuiltin_panel.h](../../../desktop/ui/components/builtin_apps/ibuiltin_panel.h)):照 IPanel 范式(无导出宏/纯虚)
- **`BuiltinPanelRegistry`**([builtin_panel_registry.{h,cpp}](../../../desktop/ui/components/builtin_apps/)):map-based 自写(aex 无 named factory 变体),按 app_id 查 panel 单例,未命中记 warning(不静默)
- **AboutPanel 实现 IBuiltinPanel** + **补网格入口**(原 `builtin:about` 入口未接——点不出 About,现已补)
- **消灭 `if (id=="about")` 链**([CFDesktopEntity.cpp](../../../desktop/ui/CFDesktopEntity.cpp)):改按 `entry.launch_kind` 分发
- **calculator 双态**:`CalculatorBuiltinPanel` 组合适配器包住 CalculatorPanel,同一份 panel 源码两种加载方式
- **`HardwareTierCapabilities::prefer_inprocess_apps`**([hardware_tier_data.h](../../../base/include/system/hardware_tier/hardware_tier_data.h) + [default_policy.cpp](../../../base/system/hardware_tier/default/default_policy.cpp)):Low/Unknown 档=true
- **manifest `launch_kind`**([app_discoverer.cpp](../../../desktop/ui/components/launcher/app_discoverer.cpp)):`auto`(默认,tier 裁决)/`detached`/`builtin`
- **`loadAppsConfig` 合并 builtin + discovered**,Auto 按 `prefer_inprocess && registry 有实现` 裁决,无实现则降级 detached 并记日志(不静默 builtin)
- **单测**:BuiltinPanelRegistry 5 例、AppDiscoverer launch_kind 3 例,全过

### ⚠️ 架构例外(已登记)

desktop 编译期引用 `apps/calculator/calculator_panel.cpp` 源文件 + `cfdesktop_calculator_parser` lib(desktop→apps 反向依赖,违反分层 spirit)。顶层 CMake 已调 `apps` 在 `desktop` 前配置。**架构债待还**:迁 calculator 核心到中立层,见 [milestone_07 §依赖与架构债](milestone_07_app_ecosystem.md)。

### 验证

点 About 弹出 panel(补入口);calculator 在 High 档走独立进程、`setDeviceConfigOverride(Low)` 下走 builtin;builtin 不产生额外进程(linuxfb/6ULL 心智模型成立)。

---

## 七、搜索 + .desktop 扫描 + Noter 移植（2026-07 落地）

> **状态**: ✅ 已落地（`feat/noter-and-search` 分支）

- **`DesktopEntryIndex`**([desktop_entry_index.{h,cpp}](../../../desktop/ui/components/launcher/)):扫 XDG `.desktop`(`~/.local/share/applications` + `/usr/share/applications`),freedesktop Type=Application && !NoDisplay,Exec 清理 `%` 占位符(`firefox %u`→`firefox`),app_id=文件 basename,进网格作 DetachedProcess。`loadAppsConfig` 合并 builtin + manifest + .desktop 三源
- **AppLauncher 搜索框**:加 QLineEdit,`textChanged` 实时按 display_name 模糊过滤网格(大小写不敏感,不区分平台)
- **Noter 移植**([apps/noter/](../../../apps/noter/)):CCIMXNoter → CFDesktop 第二个独立 App,QuarkWidgets MD3 Button 重写工具栏(Open/Save/Bold/Italic + 字号 QSlider),QTextEdit 编辑,manifest `launch_kind:auto`
- 单测:DesktopEntryIndex 7 例(解析/NoDisplay/非 Application/Exec 清理/basename 回退),全过

### 待做

- 入场/退场动画(Day 5 原计划:Slide+Fade 组合,复用 `ui/components/animation/`)
- 搜索框换 QuarkWidgets MD3 TextField(现用 QLineEdit 占位)

---

*最后更新: 2026-07-01（搜索 + .desktop + Noter 移植落地,`feat/noter-and-search` 分支；前置双态框架见 §六;第三方平台见 milestone_07）*
