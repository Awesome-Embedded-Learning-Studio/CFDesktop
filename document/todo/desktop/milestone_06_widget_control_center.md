---
title: "Milestone 6: 小组件 + 控制中心"
description: "预计周期: 7-10 天，前置依赖: Milestone 2: 状态栏 (下拉触发点)"
---

# Milestone 6: 小组件 + 控制中心

> **状态**: ⬜ 待开始
> **预计周期**: 7-10 天
> **前置依赖**: [Milestone 2: 状态栏](milestone_02_status_bar.md) (下拉触发点)
> **可与 MS3/MS4/MS5 并行**
> **目标**: 桌面上有时钟小组件，从状态栏下拉打开简易控制中心 (亮度/音量/主题切换)

---

## 一、阶段目标

实现桌面小组件框架的最小可用版本 (ClockWidget)，以及一个从状态栏下拉触发的控制中心面板。

**完成标志**: 桌面上显示一个可拖拽的时钟小组件；点击/下拉状态栏弹出控制中心面板，可切换 Light/Dark 主题。

---

## 二、当前状态分析

### 已有基础设施

| 组件 | 文件 | 状态 |
|------|------|------|
| `ShellLayer` | MS1 中实现 | ✅ 桌面内容容器 |
| `ThemeManager` | `ui/core/theme_manager.h` | ✅ 主题切换功能已有 |
| `ICFTheme` | `ui/core/i_cf_theme.h` | ✅ colorScheme / typography / motionSpec |
| Material Slider | `ui/widget/material/widget/slider/` | ✅ 亮度/音量滑块 |
| Material Switch | `ui/widget/material/widget/switch_/` | ✅ 开关控件 |
| Material Button | `ui/widget/material/widget/button/` | ✅ 按钮控件 |
| Animation Engine | `ui/components/animation/` | ✅ 动画 |
| StatusBar | MS2 中实现 | ✅ 触发控制中心 |

### 关键缺口

1. **没有桌面小组件框架** — WidgetBase / WidgetContainer
2. **没有时钟小组件** — ClockWidget
3. **没有控制中心面板** — ControlCenter
4. **没有下拉手势/点击触发** — StatusBar → ControlCenter 连接

---

## 三、待实现任务

### Day 1-2: 桌面小组件框架

#### Step 1: 定义小组件基类
- [ ] 创建文件 `desktop/ui/widget/desktop_widget/widget_base.h`
  ```cpp
  class WidgetBase : public QWidget {
      Q_OBJECT
  public:
      explicit WidgetBase(QWidget* parent = nullptr);

      virtual QString widgetId() const = 0;
      virtual QString widgetName() const = 0;
      virtual QSize defaultSize() const = 0;

      void setEditable(bool editable);
      bool isEditable() const;

  signals:
      void widgetMoved(const QPoint& pos);
      void widgetResized(const QSize& size);
      void editModeChanged(bool editable);

  protected:
      void mousePressEvent(QMouseEvent*) override;
      void mouseMoveEvent(QMouseEvent*) override;  // 拖拽
  };
  ```

#### Step 2: 实现小组件容器
- [ ] 创建文件 `desktop/ui/widget/desktop_widget/widget_container.h/.cpp`
- [ ] 功能：
  - 管理桌面上的 WidgetBase 实例列表
  - 提供 `addWidget()` / `removeWidget()` 方法
  - 作为 ShellLayer 的子 Widget
  - 处理小组件布局 (自由定位，pos 由拖拽决定)
  - 编辑模式：所有小组件进入可拖拽/可缩放状态

#### Step 3: 实现拖拽定位
- [ ] WidgetBase 的 mousePressEvent / mouseMoveEvent：
  - 按下时记录起始位置
  - 移动时更新 `move(pos + delta)`
  - 松开时 emit `widgetMoved()`
- [ ] 简化版：不需要 resize handle，时钟小组件固定大小即可

### Day 3-4: ClockWidget 时钟小组件

#### Step 4: 创建 ClockWidget
- [ ] 创建文件 `desktop/ui/widget/desktop_widget/clock_widget.h/.cpp`
- [ ] 继承 WidgetBase
- [ ] 功能：
  ```cpp
  class ClockWidget : public WidgetBase {
      Q_OBJECT
  public:
      QString widgetId() const override { return "clock"; }
      QString widgetName() const override { return "时钟"; }
      QSize defaultSize() const override { return QSize(320, 160); }
  protected:
      void paintEvent(QPaintEvent*) override;
  private slots:
      void onTimeout();
  private:
      QTimer* timer_{nullptr};
  };
  ```
- [ ] `paintEvent()` 绘制内容：
  - 圆角卡片背景 (`theme->colorScheme().surfaceContainer()`)
  - 当前时间 (大字体，如 `HH:MM`)
  - 当前日期 (小字体，如 `2026-03-31 周二`)
  - 阴影效果 (`MdElevationController` level 1)
- [ ] `onTimeout()`:
  - QTimer 每秒触发
  - `update()` 触发重绘
- [ ] 字体设置：
  - 时间: `theme->typography().displayLarge()` 或自定义大字体
  - 日期: `theme->typography().bodyMedium()`
- [ ] 颜色：
  - 时间文字: `theme->colorScheme().onSurface()`
  - 日期文字: `theme->colorScheme().onSurfaceVariant()`

**可复用**:
- `ui/widget/material/base/md_elevation_controller.h` — 阴影
- `ui/core/theme_manager.h` — 主题
- SimpleBootWidget 的 QPainter 绘制模式

### Day 5-7: 控制中心面板

#### Step 5: 创建 ControlCenter 类
- [ ] 创建文件 `desktop/ui/widget/control_center/control_center.h/.cpp`
- [ ] 类声明：
  ```cpp
  class ControlCenter : public QWidget {
      Q_OBJECT
  public:
      explicit ControlCenter(QWidget* parent = nullptr);
      void showPanel();
      void hidePanel();
  signals:
      void brightnessChanged(int value);
      void volumeChanged(int value);
      void themeToggleRequested();
      void wifiToggled(bool on);
      void bluetoothToggled(bool on);
  protected:
      void paintEvent(QPaintEvent*) override;
  private:
      void setupUi();
      void applyTheme();
  };
  ```

#### Step 6: 控制中心布局
- [ ] 位置：从状态栏下拉展开，位于屏幕右上角
- [ ] 尺寸：宽度约 360dp，高度自适应内容
- [ ] 内容区域：
  ```
  ┌──────────────────────────┐
  │  ☀ 亮度      ━━━━━━━━━━ │  ← Material Slider
  │  🔊 音量     ━━━━━━━━━━ │  ← Material Slider
  │                          │
  │  [WiFi]  [蓝牙]  [DND]  │  ← Material Switch 平铺
  │                          │
  │  [🌙 Dark Mode]          │  ← 主题切换按钮
  │  [📷 截屏]               │  ← 占位按钮
  └──────────────────────────┘
  ```
- [ ] 使用 `Qt::Popup` 或 overlay 模式
- [ ] 圆角卡片 + 阴影 (同 AppLauncher 风格)

#### Step 7: 实现亮度/音量滑块
- [ ] 使用 `ui/widget/material/widget/slider/` 的 Material Slider
  - 亮度 Slider: range 0-100, step 1
  - 音量 Slider: range 0-100, step 1
- [ ] 值变化时 emit 对应信号
- [ ] **注意**: 第一版只是 UI，实际调节亮度/音量需要平台适配 (可延后)

#### Step 8: 实现功能开关
- [ ] 使用 `ui/widget/material/widget/switch_/` 的 Material Switch
  - WiFi: 默认 off (纯 UI)
  - 蓝牙: 默认 off (纯 UI)
  - DND (勿扰模式): 默认 off (纯 UI)
- [ ] 每个开关旁有图标 + 文字

#### Step 9: 实现主题切换
- [ ] 添加主题切换按钮
  ```cpp
  connect(themeButton, &Button::clicked, []() {
      auto& tm = ThemeManager::instance();
      if (tm.currentTheme()->colorScheme().isDark()) {
          tm.setTheme("light");
      } else {
          tm.setTheme("dark");
      }
  });
  ```
- [ ] 切换后所有组件自动响应 (已有 ThemeManager 通知机制)

### Day 8: 入场/退场动画

#### Step 10: 控制中心弹出动画
- [ ] 入场: 从顶部状态栏位置向下滑出 + 淡入
  - 复用 SlideAnimation + FadeAnimation
  - duration: `theme->motionSpec().medium()`
- [ ] 退场: 向上滑回 + 淡出
- [ ] 点击外部/ESC 关闭

### Day 9-10: 集成与验证

#### Step 11: StatusBar → ControlCenter 连接
- [ ] StatusBar 点击时间区域 → emit `timeClicked()`
- [ ] CFDesktopEntity 中连接：
  ```cpp
  connect(status_bar, &StatusBar::timeClicked,
          control_center, &ControlCenter::showPanel);
  ```

#### Step 12: ClockWidget 放置到桌面
- [ ] 在 ShellLayer 初始化时创建 WidgetContainer
- [ ] 添加 ClockWidget 到默认位置 (屏幕中偏上)
- [ ] 验证可拖拽移动

#### Step 13: 全流程验证
- [ ] 桌面显示时钟小组件
- [ ] 拖拽时钟到其他位置
- [ ] 点击状态栏时间 → 控制中心弹出
- [ ] 控制中心切换 Dark/Light → 全部 UI 变色 (状态栏 + 任务栏 + 时钟)
- [ ] 控制中心滑块可拖动 (视觉上)
- [ ] 控制中心点击外部可关闭

---

## 四、关键文件清单

### 需要新建的文件
| 文件 | 内容 |
|------|------|
| `desktop/ui/widget/desktop_widget/widget_base.h` | 小组件基类 |
| `desktop/ui/widget/desktop_widget/widget_container.h` | 小组件容器声明 |
| `desktop/ui/widget/desktop_widget/widget_container.cpp` | 小组件容器实现 |
| `desktop/ui/widget/desktop_widget/clock_widget.h` | 时钟小组件声明 |
| `desktop/ui/widget/desktop_widget/clock_widget.cpp` | 时钟小组件实现 |
| `desktop/ui/widget/control_center/control_center.h` | 控制中心声明 |
| `desktop/ui/widget/control_center/control_center.cpp` | 控制中心实现 |

### 需要修改的文件
| 文件 | 修改内容 |
|------|----------|
| `desktop/ui/CFDesktopEntity.cpp` | 创建 WidgetContainer、ClockWidget、ControlCenter，连接信号 |
| `desktop/ui/components/statusbar/status_bar.h` | 确认 `timeClicked()` 信号 |
| `desktop/ui/components/CMakeLists.txt` | 添加新文件 |

### 参考文件 (只读)
| 文件 | 用途 |
|------|------|
| `ui/widget/material/widget/slider/` | Material Slider |
| `ui/widget/material/widget/switch_/` | Material Switch |
| `ui/widget/material/widget/button/` | Material Button |
| `ui/widget/material/base/md_elevation_controller.h` | 阴影系统 |
| `ui/core/theme_manager.h` | ThemeManager |
| `ui/components/animation/` | 动画工厂 |
| `desktop/ui/widget/init_session/simple_boot_widget.cpp` | QPainter 绘制参考 |
| `document/todo/desktop/11_notification_control.md` | 控制中心原始设计 |
| `document/todo/desktop/13_widget_apps.md` | 小组件原始设计 |

---

## 五、验收标准

- [ ] 桌面上显示时钟小组件 (时间 + 日期)
- [ ] 时钟每秒更新
- [ ] 时钟可拖拽移动到桌面任意位置
- [ ] 点击状态栏时间区域弹出控制中心
- [ ] 控制中心有亮度、音量滑块 (可拖动)
- [ ] 控制中心有 WiFi/蓝牙/DND 开关 (可点击)
- [ ] 控制中心有主题切换按钮，点击可切换 Light/Dark
- [ ] 主题切换后状态栏、任务栏、时钟、控制中心全部变色
- [ ] 控制中心有入场/退场动画
- [ ] 点击外部或 ESC 可关闭控制中心

---

*最后更新: 2026-03-31*
