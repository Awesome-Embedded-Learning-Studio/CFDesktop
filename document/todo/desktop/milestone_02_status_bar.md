---
title: "Milestone 2: 状态栏"
description: "预计周期: 3-5 天，前置依赖: Milestone 1: 桌面骨架可见"
---

# Milestone 2: 状态栏

> **状态**: ✅ 功能落地（v1 基础 + v2 MD3 美化）
> **预计周期**: 3-5 天
> **前置依赖**: [Milestone 1: 桌面骨架可见](../done/SUMMARY.md)
> **目标**: 屏幕顶部出现一条状态栏，显示时间、基础系统图标

## 实现结果与偏差（2026-06-15）

**已交付**：`StatusBar` 顶层面板，注册到 PanelManager 作为 Top edge panel，
顶部一条 48dp 状态栏，显示时间 (HH:MM)、系统图标簇（信号/电池/WiFi/音量），
背景与图标跟随 ThemeManager 主题；offscreen 启动通过、Doxygen lint 通过。

**与原计划的偏差**（实现时发现真实代码与本文档措辞不符，已按代码库实际 API 落地）：

1. **`PanelManager::registerPanel()` 原有 bug**：只校验、不把 panel 存入 `panels`
   向量，导致面板注册后永远不会被布局。已修复（`panels.push_back(panel)`）。
2. **主题 API 名称不同**：实际接口是 `ICFTheme::color_scheme()` / `font_type()`
   （非本文档写的 `colorScheme()` / `typography()`）；且**无 `surfaceContainer` token**，
   实际用 `SURFACE` / `ON_SURFACE` / `ON_SURFACE_VARIANT` / `OUTLINE_VARIANT`。
3. **主题管线原本未接通**：`main.cpp` 用裸 `QApplication`，而 Material 主题只在
   构造 `MaterialApplication` 时才注册到 ThemeManager。已将 `main.cpp` 切到
   `MaterialApplication`，主题在运行时真正可用（顺带让全部 MD3 控件拿到真实主题色）。
4. **v2 MD3 美化**（嵌入式安全，纯 QPainter，不引入 `Qt6::Svg` 依赖）：
   HCT 色调抬升的垂直渐变表面 + 底部接缝处 in-band 软阴影 + 横向渐隐发丝线 +
   开机 250ms 淡入。图标用 **icons8 单色 PNG 蒙版 + 运行时 `SourceIn` 染色**
   跟随主题（嵌入式只需 Qt GUI，无需 Svg 模块；资源经 qrc 编译进二进制，
   无文件系统依赖）。**无静默兜底**：mask 加载失败则该图标留可见空白并打
   WARNING 日志（不替换掩盖）。资源用 `Q_INIT_RESOURCE`（全局作用域 helper
   包裹，因该宏不能在命名空间内调用）防止静态库 qrc 被链接器剥离。
   图标来源/署名见 `desktop/ui/components/statusbar/icons/LICENSE.md`。

**待人工确认**：真机/WSLg 显示下的视觉效果（本地无显示设备，仅 offscreen 验证不崩）。

---

## 一、阶段目标

实现 `StatusBar` 面板，注册到 PanelManager 作为 Top edge panel，显示时间和基础系统图标，并跟随主题变色。

**完成标志**: 运行后屏幕顶部有一条高度 32-48dp 的状态栏，能看到当前时间 (HH:MM)，背景色跟随主题。

---

## 二、当前状态分析

### 已有基础设施

| 组件 | 文件 | 状态 |
|------|------|------|
| `IStatusBar` 接口 | `desktop/ui/components/statusbar/IStatusBar.h` | ✅ 继承自 `IPanel`，接口空壳 |
| `IPanel` 接口 | `desktop/ui/components/IPanel.h` | ✅ `position()` / `priority()` / `preferredSize()` / `widget()` |
| `PanelManager` | `desktop/ui/components/PanelManager.h/.cpp` | ✅ `registerPanel()` 接受 `WeakPtr<IPanel>` |
| `ThemeManager` | `ui/core/theme_manager.h` | ✅ 单例，`currentTheme()` → ICFTheme |
| `ICFTheme` | `ui/core/i_cf_theme.h` | ✅ `colorScheme()` / `typography()` / `motionSpec()` |
| Material Label | `ui/widget/material/widget/label/` | ✅ 文字显示控件 |
| Material Animation | `ui/components/animation/` | ✅ Fade/Slide/Scale 动画工厂 |

### 关键缺口

1. **IStatusBar 是空接口** — 没有定义任何方法，需要扩展
2. **没有 StatusBar 具体实现** — 需要 `StatusBar` 类
3. **没有系统信息获取** — 需要 QTime/QDateTime 读取
4. **StatusBar 未注册到 PanelManager** — CFDesktopEntity 中未接入

---

## 三、待实现任务

### Day 1: StatusBar 数据模型与接口扩展

#### Step 1: 扩展 IStatusBar 接口
- [ ] 修改 `desktop/ui/components/statusbar/IStatusBar.h`
  - 添加纯虚方法：
    ```cpp
    virtual void setTimeVisible(bool visible) = 0;
    virtual bool isTimeVisible() const = 0;
    virtual void setIconsVisible(bool visible) = 0;
    virtual bool isIconsVisible() const = 0;
    ```
  - 添加信号声明 (或用 Qt 的 signals/slots 机制)

#### Step 2: 定义 StatusBarStyle 枚举
- [ ] 在 `IStatusBar.h` 或新文件中定义：
  ```cpp
  enum class StatusBarStyle {
      IOs,      // 居中时间，极简
      Windows   // 左侧时间 + 右侧系统图标
  };
  ```

### Day 2: StatusBar Widget 实现

#### Step 3: 创建 StatusBar 类
- [ ] 创建文件 `desktop/ui/components/statusbar/status_bar.h`
- [ ] 创建文件 `desktop/ui/components/statusbar/status_bar.cpp`
- [ ] 类声明：
  ```cpp
  class StatusBar : public QWidget, public IStatusBar {
      Q_OBJECT
  public:
      explicit StatusBar(QWidget* parent = nullptr);
      // IPanel 接口
      PanelPosition position() const override { return PanelPosition::Top; }
      int priority() const override { return 100; }
      int preferredSize() const override { return 48; }  // 48dp
      QWidget* widget() const override { return const_cast<StatusBar*>(this); }

      // IStatusBar 接口
      void setTimeVisible(bool v) override;
      bool isTimeVisible() const override;
      void setIconsVisible(bool v) override;
      bool isIconsVisible() const override;
  protected:
      void paintEvent(QPaintEvent* event) override;
  private slots:
      void onTimeout();
  private:
      void setupUi();
      void applyTheme();
  };
  ```

#### Step 4: 实现 StatusBar 内容
- [ ] `setupUi()`:
  - 创建 `QHBoxLayout`
  - 左侧：可选的系统图标区 (先用占位 QLabel)
  - 中间：时间显示 (`QLabel` 或自定义绘制)
  - 右侧：系统图标区 (WiFi/电池/音量 占位)
- [ ] `paintEvent()`:
  - 绘制背景 (使用 `theme->colorScheme().surfaceContainer()`)
  - 底部分割线 (可选，1px `outline()` 颜色)
- [ ] `onTimeout()`:
  - `QTimer` 每秒触发，更新时间显示
  - 格式: `HH:MM` 或根据 StatusBarStyle 调整
- [ ] `applyTheme()`:
  - 获取 ThemeManager 当前主题
  - 设置文字颜色: `theme->colorScheme().onSurface()`
  - 设置字体: `theme->typography().bodyMedium()`
  - 设置背景色

**可复用**:
- `ui/widget/material/widget/label/` — Material Label 控件
- `ui/core/theme_manager.h` — ThemeManager
- `ui/base/color.h` — CFColor 工具

### Day 3: 系统图标占位

#### Step 5: 实现基础图标项
- [ ] 创建 `StatusIcon` 简单 Widget (或直接用 QLabel + Unicode 字符)
  - 时间图标: 显示时间文本
  - WiFi 图标: Unicode `📶` 或简单的 QPainter 绘制
  - 电池图标: Unicode `🔋` 或占位方块
  - 音量图标: Unicode `🔊` 或占位方块
- [ ] **建议**: 第一版用 Unicode 文本或纯色方块作为占位，不必等图标系统

### Day 4: 注册到 PanelManager

#### Step 6: 在 CFDesktopEntity::run_init() 中创建并注册 StatusBar
- [ ] 修改 `desktop/ui/CFDesktopEntity.cpp` 的 `run_init()`:
  ```cpp
  // 创建 StatusBar
  auto* status_bar = new StatusBar(desktop_entity_);
  panel_mgr->registerPanel(status_bar->GetWeak());
  ```
- [ ] 确保 StatusBar 被创建为 CFDesktop 的子 Widget
- [ ] PanelManager 会自动将其布局到顶部，并减小 availableGeometry

#### Step 7: 验证布局联动
- [ ] 确认 PanelManager::availableGeometry() 现在顶部缩小了 48dp
- [ ] 确认 ShellLayer 的 geometry 跟着调整
- [ ] 确认 StatusBar 的 position() 返回 `PanelPosition::Top`

### Day 5: 主题联动与收尾

#### Step 8: 主题切换响应
- [ ] StatusBar 监听 ThemeManager 的主题变更信号
- [ ] 切换主题时调用 `applyTheme()` 更新颜色和字体
- [ ] 验证 Light/Dark 切换时状态栏正确变色

#### Step 9: 基础测试
- [ ] 编写简单单元测试：
  - [ ] StatusBar 构造不 crash
  - [ ] `position()` 返回 `Top`
  - [ ] `preferredSize()` 返回正数
  - [ ] `widget()` 返回 self
- [ ] 手动验证：
  - [ ] 时间每秒更新
  - [ ] 主题切换生效

---

## 四、关键文件清单

### 需要修改的文件
| 文件 | 修改内容 |
|------|----------|
| `desktop/ui/components/statusbar/IStatusBar.h` | 扩展接口方法 |
| `desktop/ui/CFDesktopEntity.cpp` | 创建 StatusBar 并注册到 PanelManager |
| `desktop/ui/components/CMakeLists.txt` | 添加新文件 |

### 需要新建的文件
| 文件 | 内容 |
|------|------|
| `desktop/ui/components/statusbar/status_bar.h` | StatusBar 类声明 |
| `desktop/ui/components/statusbar/status_bar.cpp` | StatusBar 实现 |
| `desktop/ui/components/statusbar/status_icon.h` | 状态图标项 (可选) |
| `desktop/ui/components/statusbar/status_icon.cpp` | 状态图标实现 (可选) |

### 参考文件 (只读)
| 文件 | 用途 |
|------|------|
| `desktop/ui/components/statusbar/IStatusBar.h:29` | IStatusBar 接口 (继承 IPanel) |
| `desktop/ui/components/IPanel.h:42` | IPanel 接口定义 |
| `desktop/ui/components/PanelManager.h:30` | registerPanel() 方法 |
| `desktop/ui/components/PanelManager.cpp` | 面板布局实现 |
| `ui/core/theme_manager.h` | ThemeManager 单例 |
| `ui/widget/material/widget/label/` | Label 控件参考 |

---

## 五、验收标准

- [ ] 屏幕顶部有一条高度 32-48dp 的状态栏
- [ ] 显示当前时间 (HH:MM)，每秒更新
- [ ] 右侧有系统图标占位 (至少 WiFi、电池)
- [ ] 背景色和文字色跟随 ThemeManager 主题
- [ ] PanelManager 的 `availableGeometry()` 顶部缩小了对应高度
- [ ] Light/Dark 主题切换时状态栏正确变色

---

*最后更新: 2026-03-31*
