# Phase 6: 多平台模拟器详细设计文档

## 文档信息
| 项目 | 内容 |
|------|------|
| 文档版本 | v1.0 |
| 创建日期 | 2026-02-20 |
| 阶段代号 | Phase 6 - 多平台模拟器 |
| 预计周期 | 2~3 周 |
| 依赖阶段 | Phase 0, Phase 2, Phase 3 |

---

## 一、阶段目标

### 1.1 核心目标
在 Windows/Ubuntu 上通过 Qt Creator 还原真实嵌入式设备的 UI 效果，实现"所见即所得"的开发体验。

### 1.2 具体交付物
- [ ] `SimulatorWindow` 模拟器主窗口
- [ ] `DeviceFrame` 设备外壳渲染
- [ ] `TouchVisualizer` 触摸可视化
- [ ] `HWTierSelector` 硬件档位选择器
- [ ] `ResolutionPreset` 分辨率预设管理
- [ ] 独立可执行文件 `cfdesktop-sim`

---

## 二、模块架构设计

### 2.1 整体架构图

```
┌───────────────────────────────────────────────────────────────┐
│                    Simulator Window                            │
│  ┌─────────────────────────────────────────────────────────┐  │
│  │                    Device Frame                          │  │
│  │  ┌───────────────────────────────────────────────────┐  │  │
│  │  │              Shell UI Content                      │  │  │
│  │  │          (共享真实设备的 Shell 代码)               │  │  │
│  │  │                                                    │  │  │
│  │  │  [Launcher] [WindowManager] [StatusBar]            │  │  │
│  │  └───────────────────────────────────────────────────┘  │  │
│  └─────────────────────────────────────────────────────────┘  │
│                                                                  │
│  ┌─────────────────────────────────────────────────────────┐  │
│  │                   Control Panel                         │  │
│  │  [Device Selector] [Resolution] [HWTier] [Touch Viz]    │  │
│  └─────────────────────────────────────────────────────────┘  │
└───────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌───────────────────────────────────────────────────────────────┐
│                   Injection Layer                             │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────────┐    │
│  │ DPIManager   │  │HardwareProbe │  │  InputManager    │    │
│  │   Injection  │  │   Mock       │  │   Simulation     │    │
│  └──────────────┘  └──────────────┘  └──────────────────┘    │
└───────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌───────────────────────────────────────────────────────────────┐
│                   Shared Base & Shell                         │
│              (与真实设备完全相同的代码)                         │
└───────────────────────────────────────────────────────────────┘
```

### 2.2 文件结构

```
src/simulator/
├── include/CFDesktop/Simulator/
│   ├── SimulatorWindow.h           # 模拟器主窗口
│   ├── DeviceFrame.h               # 设备外壳
│   ├── DeviceProfile.h             # 设备配置文件
│   ├── TouchVisualizer.h           # 触摸可视化
│   ├── ControlPanel.h              # 控制面板
│   ├── HWTierSelector.h            # 硬件档位选择器
│   ├── ResolutionPreset.h          # 分辨率预设
│   └── SimulatorInjection.h        # 注入接口
│
└── src/
    ├── SimulatorWindow.cpp
    ├── DeviceFrame.cpp
    ├── TouchVisualizer.cpp
    ├── ControlPanel.cpp
    ├── HWTierSelector.cpp
    ├── ResolutionPreset.cpp
    └── injection/
        ├── DPIInjector.cpp         # DPI 注入
        ├── HardwareMock.cpp        # 硬件 Mock
        └── InputSimulator.cpp      # 输入模拟

assets/simulator/
├── devices/                        # 设备外壳图片
│   ├── phone-generic.png
│   ├── tablet-10inch.png
│   ├── panel-7inch.png
│   ├── panel-10inch.png
│   └── custom/
│       └── device.svg
├── profiles/                       # 设备配置
│   ├── imx6ull-4.3.json
│   ├── imx6ull-7.0.json
│   ├── rk3568-7.0.json
│   ├── rk3568-10.1.json
│   ├── rk3588-10.1.json
│   └── generic-1080p.json
└── effects/
    └── touch-ripple.png            # 触摸涟漪效果
```

---

## 三、模拟器主窗口 (SimulatorWindow)

### 3.1 SimulatorWindow 类接口

**文件**: `include/CFDesktop/Simulator/SimulatorWindow.h`

```cpp
#pragma once
#include <QMainWindow>
#include <QScopedPointer>
#include "DeviceProfile.h"

namespace CFDesktop::Simulator {

class DeviceFrame;
class ControlPanel;
class TouchVisualizer;
class DPIInjector;
class HardwareMock;

/**
 * @brief 模拟器主窗口
 *
 * 提供完整的嵌入式设备模拟环境。
 */
class SimulatorWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit SimulatorWindow(QWidget* parent = nullptr);
    ~SimulatorWindow();

    /**
     * @brief 加载设备配置
     */
    bool loadDeviceProfile(const QString& profilePath);

    /**
     * @brief 获取当前设备配置
     */
    DeviceProfile currentProfile() const;

    /**
     * @brief 设置显示内容
     *
     * 通常设置为 Shell UI 的主窗口。
     */
    void setContentWidget(QWidget* content);

    /**
     * @brief 获取内容区域
     */
    QWidget* contentWidget() const;

    /**
     * @brief 显示/隐藏控制面板
     */
    void setControlPanelVisible(bool visible);
    bool isControlPanelVisible() const;

    /**
     * @brief 显示/隐藏设备外壳
     */
    void setDeviceFrameVisible(bool visible);
    bool isDeviceFrameVisible() const;

    /**
     * @brief 启用/禁用触摸可视化
     */
    void setTouchVisualizationEnabled(bool enabled);
    bool isTouchVisualizationEnabled() const;

public slots:
    /**
     * @brief 切换全屏模式
     */
    void toggleFullscreen();

    /**
     * @brief 重置模拟状态
     */
    void resetSimulation();

    /**
     * @brief 截图
     */
    void takeScreenshot();

signals:
    /**
     * @brief 设备配置变化信号
     */
    void profileChanged(const DeviceProfile& profile);

    /**
     * @brief 分辨率变化信号
     */
    void resolutionChanged(const QSize& resolution);

    /**
     * @brief 硬件档位变化信号
     */
    void hwTierChanged(HWTier tier);

private:
    void setupUI();
    void setupConnections();
    void loadDefaultProfile();

private:
    QScopedPointer<DeviceFrame> m_deviceFrame;
    QScopedPointer<ControlPanel> m_controlPanel;
    QScopedPointer<TouchVisualizer> m_touchVisualizer;
    QScopedPointer<DPIInjector> m_dpiInjector;
    QScopedPointer<HardwareMock> m_hardwareMock;

    DeviceProfile m_currentProfile;
    QPointer<QWidget> m_contentWidget;
};

} // namespace CFDesktop::Simulator
```

### 3.2 窗口布局

```
┌─────────────────────────────────────────────────────────────┐
│ Simulator Window                                            │
├─────────────────────────────────────────────────────────────┤
│ ┌───┐                                                       │
│ │   │  ┌───────────────────────────────────────────────┐   │
│ │ D │  │                                               │   │
│ │ e │  │           Device Frame (外壳)                │   │
│ │ v │  │  ┌─────────────────────────────────────────┐  │   │
│ │ i │  │  │                                         │  │   │
│ │ c │  │  │        Screen Content Area              │  │   │
│ │ e │  │  │        (Shell UI 渲染)                   │  │   │
│ │   │  │  │                                         │  │   │
│ │ F │  │  │                                         │  │   │
│ │ r │  │  │                                         │  │   │
│ │ a │  │  │                                         │  │   │
│ │ m │  │  │                                         │  │   │
│ │ e │  │  └─────────────────────────────────────────┘  │   │
│ └───┘  │                                               │   │
│        └───────────────────────────────────────────────┘   │
├─────────────────────────────────────────────────────────────┤
│ ┌─────────────────────────────────────────────────────────┐ │
│ │                   Control Panel                         │ │
│ │ [Device▼] [800×480▼] [Low▼] [Touch:ON] [Screenshot]    │ │
│ └─────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
```

---

## 四、设备配置文件 (DeviceProfile)

### 4.1 DeviceProfile 数据结构

**文件**: `include/CFDesktop/Simulator/DeviceProfile.h`

```cpp
#pragma once
#include <QString>
#include <QSize>
#include <QRect>
#include "../Base/HardwareProbe/HWTier.h"

namespace CFDesktop::Simulator {

/**
 * @brief 设备类型
 */
enum class DeviceType {
    Phone,           # 手机
    Tablet,          # 平板
    Panel,           # 工控屏
    Custom           # 自定义
};

/**
 * @brief 设备配置
 */
struct DeviceProfile {
    // 基本信息
    QString name;                    # 设备名称
    QString manufacturer;            # 制造商
    QString model;                   # 型号
    DeviceType type = DeviceType::Panel;

    // 屏幕参数
    QSize screenSize;                # 屏幕分辨率
    QSizeF physicalSize;             # 物理尺寸 (毫米)
    qreal dpi = 96.0;                # DPI
    qreal devicePixelRatio = 1.0;    # 设备像素比

    // 屏幕区域 (在外壳中的位置和大小)
    QRect screenRect;                # 屏幕区域

    // 设备外壳
    QString frameImagePath;          # 外壳图片路径
    QRect frameRect;                 # 外壳区域
    QColor frameColor;               # 外壳颜色 (无图片时)
    int frameCornerRadius = 0;       # 外壳圆角

    // 硬件能力
    HWTier hwTier = HWTier::Low;     # 硬件档位

    // 输入配置
    bool hasTouchscreen = true;
    bool hasHardwareButtons = false;
    bool hasRotaryEncoder = false;

    // 其他配置
    QString description;
    QString version = "1.0";

    /**
     * @brief 从 JSON 加载
     */
    static DeviceProfile fromJson(const QString& path);

    /**
     * @brief 保存到 JSON
     */
    bool toJson(const QString& path) const;

    /**
     * @brief 获取默认配置
     */
    static DeviceProfile defaultProfile();
};

} // namespace CFDesktop::Simulator
```

### 4.2 设备配置 JSON 示例

**文件**: `assets/simulator/profiles/imx6ull-4.3.json`

```json
{
    "name": "IMX6ULL 4.3 inch Panel",
    "manufacturer": "NXP",
    "model": "IMX6ULL-EVK",
    "type": "panel",

    "screen": {
        "size": { "width": 480, "height": 272 },
        "physicalSize": { "width": 95.0, "height": 54.0 },
        "dpi": 125.0,
        "devicePixelRatio": 1.0,
        "rect": { "x": 20, "y": 20, "width": 480, "height": 272 }
    },

    "frame": {
        "image": "devices/panel-4.3.png",
        "rect": { "x": 0, "y": 0, "width": 520, "height": 312 },
        "color": "#333333",
        "cornerRadius": 8
    },

    "hardware": {
        "tier": "low"
    },

    "input": {
        "touchscreen": true,
        "hardwareButtons": true,
        "rotaryEncoder": false
    },

    "description": "4.3 inch WVGA panel with IMX6ULL",
    "version": "1.0"
}
```

### 4.3 预设设备配置列表

| 配置文件 | 设备 | 分辨率 | DPI | 档位 |
|----------|------|--------|-----|------|
| `imx6ull-4.3.json` | 4.3寸工控屏 | 480×272 | 125 | Low |
| `imx6ull-7.0.json` | 7.0寸工控屏 | 800×480 | 133 | Low |
| `rk3568-7.0.json` | 7.0寸工控屏 | 1024×600 | 170 | Mid |
| `rk3568-10.1.json` | 10.1寸平板 | 1280×800 | 149 | Mid |
| `rk3588-10.1.json` | 10.1寸高端屏 | 1920×1200 | 224 | High |

---

## 五、设备外壳 (DeviceFrame)

### 5.1 DeviceFrame 类接口

**文件**: `include/CFDesktop/Simulator/DeviceFrame.h`

```cpp
#pragma once
#include <QWidget>
#include "DeviceProfile.h"

namespace CFDesktop::Simulator {

/**
 * @brief 设备外壳渲染
 *
 * 渲染设备外壳和屏幕区域。
 */
class DeviceFrame : public QWidget {
    Q_OBJECT

public:
    explicit DeviceFrame(QWidget* parent = nullptr);
    ~DeviceFrame() = default;

    /**
     * @brief 设置设备配置
     */
    void setDeviceProfile(const DeviceProfile& profile);
    DeviceProfile deviceProfile() const;

    /**
     * @brief 获取屏幕区域 Widget
     *
     * Shell UI 内容应该作为这个 Widget 的子控件。
     */
    QWidget* screenContainer() const;

    /**
     * @brief 绘制模式
     */
    enum class RenderMode {
        Image,      # 使用图片
        Vector,     # 矢量绘制
        Simple      # 简单边框
    };
    void setRenderMode(RenderMode mode);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void drawFrameImage(QPainter& painter);
    void drawFrameVector(QPainter& painter);
    void drawFrameSimple(QPainter& painter);
    void updateLayout();

private:
    DeviceProfile m_profile;
    RenderMode m_renderMode = RenderMode::Image;
    QPixmap m_frameImage;
    QPointer<QWidget> m_screenContainer;
};

} // namespace CFDesktop::Simulator
```

### 5.2 矢量绘制示例

```cpp
void DeviceFrame::drawFrameVector(QPainter& painter) {
    painter.setRenderHint(QPainter::Antialiasing);

    // 外壳背景
    QPainterPath framePath;
    framePath.addRoundedRect(
        m_profile.frameRect,
        m_profile.frameCornerRadius,
        m_profile.frameCornerRadius
    );

    painter.fillPath(framePath, m_profile.frameColor);

    // 屏幕边框
    QPen borderPen(QColor(0, 0, 0, 100), 2);
    painter.setPen(borderPen);
    painter.drawRoundedRect(
        m_profile.screenRect.adjusted(-2, -2, 2, 2),
        4, 4
    );

    // 屏幕阴影
    QRect screenShadowRect = m_profile.screenRect;
    screenShadowRect.translate(2, 2);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 0, 0, 50));
    painter.drawRoundedRect(screenShadowRect, 4, 4);

    // 硬件按键
    if (m_profile.hasHardwareButtons) {
        drawHardwareButtons(painter);
    }
}
```

---

## 六、触摸可视化 (TouchVisualizer)

### 6.1 TouchVisualizer 类接口

**文件**: `include/CFDesktop/Simulator/TouchVisualizer.h`

```cpp
#pragma once
#include <QWidget>
#include <QHash>
#include <QPoint>
#include <QAnimationGroup>

namespace CFDesktop::Simulator {

/**
 * @brief 触摸点可视化数据
 */
struct TouchPointVisual {
    int id;
    QPointF position;
    qreal pressure = 0.0;
    qint64 startTime = 0;
    qreal radius = 20.0;
    QColor color = QColor(0, 150, 255, 150);
};

/**
 * @brief 触摸可视化
 *
 * 在点击位置显示涟漪效果。
 */
class TouchVisualizer : public QWidget {
    Q_OBJECT

public:
    explicit TouchVisualizer(QWidget* parent = nullptr);
    ~TouchVisualizer() = default;

    /**
     * @brief 启用/禁用
     */
    void setEnabled(bool enabled);
    bool isEnabled() const;

    /**
     * @brief 添加触摸点
     */
    void addTouchPoint(int id, const QPointF& position);

    /**
     * @brief 更新触摸点
     */
    void updateTouchPoint(int id, const QPointF& position);

    /**
     * @brief 移除触摸点
     */
    void removeTouchPoint(int id);

    /**
     * @brief 设置涟漪效果时长
     */
    void setRippleDuration(int milliseconds);

    /**
     * @brief 设置涟漪颜色
     */
    void setRippleColor(const QColor& color);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void startRippleAnimation(const QPointF& position);
    void cleanupExpiredTouches();

private:
    bool m_enabled = true;
    int m_rippleDuration = 300;
    QColor m_ippleColor;
    QHash<int, TouchPointVisual> m_activeTouches;
    QList<QPointer<QPropertyAnimation>> m_activeAnimations;
};

} // namespace CFDesktop::Simulator
```

---

## 七、控制面板 (ControlPanel)

### 7.1 ControlPanel 类接口

**文件**: `include/CFDesktop/Simulator/ControlPanel.h`

```cpp
#pragma once
#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QCheckBox>

namespace CFDesktop::Simulator {

/**
 * @brief 模拟器控制面板
 *
 * 提供设备选择、分辨率切换、档位切换等功能。
 */
class ControlPanel : public QWidget {
    Q_OBJECT

public:
    explicit ControlPanel(QWidget* parent = nullptr);
    ~ControlPanel() = default;

    /**
     * @brief 设置可用设备配置列表
     */
    void setAvailableProfiles(const QList<DeviceProfile>& profiles);

    /**
     * @brief 设置当前配置
     */
    void setCurrentProfile(const DeviceProfile& profile);

    /**
     * @brief 设置当前分辨率
     */
    void setResolution(const QSize& resolution);

    /**
     * @brief 设置当前硬件档位
     */
    void setHWTier(HWTier tier);

signals:
    /**
     * @brief 设备配置变化
     */
    void profileChanged(const DeviceProfile& profile);

    /**
     * @brief 分辨率变化
     */
    void resolutionChanged(const QSize& resolution);

    /**
     * @brief 硬件档位变化
     */
    void hwTierChanged(HWTier tier);

    /**
     * @brief 触摸可视化开关变化
     */
    void touchVisualizationChanged(bool enabled);

    /**
     * @brief 请求截图
     */
    void screenshotRequested();

    /**
     * @brief 请求重置
     */
    void resetRequested();

    /**
     * @brief 请求全屏
     */
    void fullscreenRequested();

private:
    void setupUI();
    void loadProfilesFromDirectory();

private:
    QComboBox* m_profileCombo;
    QComboBox* m_resolutionCombo;
    QComboBox* m_hwTierCombo;
    QCheckBox* m_touchVizCheck;
    QPushButton* m_screenshotButton;
    QPushButton* m_resetButton;
    QPushButton* m_fullscreenButton;

    QList<DeviceProfile> m_availableProfiles;
};

} // namespace CFDesktop::Simulator
```

### 7.2 硬件档位选择器 (HWTierSelector)

**文件**: `include/CFDesktop/Simulator/HWTierSelector.h`

```cpp
#pragma once
#include <QWidget>
#include <QButtonGroup>
#include <QRadioButton>

namespace CFDesktop::Simulator {

/**
 * @brief 硬件档位选择器
 *
 * 允许在模拟器中切换硬件档位，验证降级行为。
 */
class HWTierSelector : public QWidget {
    Q_OBJECT

public:
    explicit HWTierSelector(QWidget* parent = nullptr);
    ~HWTierSelector() = default;

    void setCurrentTier(HWTier tier);
    HWTier currentTier() const;

signals:
    void tierChanged(HWTier tier);

private:
    QButtonGroup* m_tierGroup;
    QRadioButton* m_lowRadio;
    QRadioButton* m_midRadio;
    QRadioButton* m_highRadio;
};

} // namespace CFDesktop::Simulator
```

---

## 八、注入接口

### 8.1 DPI 注入器

**文件**: `src/simulator/injection/DPIInjector.cpp`

```cpp
namespace CFDesktop::Simulator {

/**
 * @brief DPI 注入器
 *
 * 向 DPIManager 注入模拟的屏幕参数。
 */
class DPIInjector : public QObject {
    Q_OBJECT

public:
    explicit DPIInjector(QObject* parent = nullptr);

    /**
     * @brief 注入屏幕参数
     */
    void inject(
        const QSize& resolution,
        qreal dpi,
        qreal devicePixelRatio
    );

    /**
     * @brief 清除注入
     */
    void clear();

    /**
     * @brief 是否已注入
     */
    bool isInjected() const;

private:
    bool m_injected = false;
};

} // namespace CFDesktop::Simulator
```

### 8.2 硬件 Mock

**文件**: `src/simulator/injection/HardwareMock.cpp`

```cpp
namespace CFDesktop::Simulator {

/**
 * @brief 硬件 Mock
 *
 * 向 HardwareProbe 注入模拟的硬件信息。
 */
class HardwareMock : public QObject {
    Q_OBJECT

public:
    explicit HardwareMock(QObject* parent = nullptr);

    /**
     * @brief 注入硬件信息
     */
    void injectHardwareInfo(const HardwareInfo& info);

    /**
     * @brief 快速注入档位
     */
    void injectTier(HWTier tier);

    /**
     * @brief 清除 Mock
     */
    void clear();

signals:
    /**
     * @brief 档位变化信号
     */
    void tierChanged(HWTier tier);

private:
    bool m_active = false;
};

} // namespace CFDesktop::Simulator
```

### 8.3 输入模拟器

**文件**: `src/simulator/injection/InputSimulator.cpp`

```cpp
namespace CFDesktop::Simulator {

/**
 * @brief 输入模拟器配置
 */
struct InputSimulationConfig {
    bool mouseEmulatesTouch = true;
    bool keyboardEmulatesButtons = true;
    bool mouseWheelEmulatesRotary = true;
    bool showTouchFeedback = true;
};

/**
 * @brief 输入模拟器
 *
 * 将鼠标/键盘输入转换为触摸/按键事件。
 */
class InputSimulator : public QObject {
    Q_OBJECT

public:
    explicit InputSimulator(QObject* parent = nullptr);

    void setConfig(const InputSimulationConfig& config);
    InputSimulationConfig config() const;

    void setEnabled(bool enabled);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    bool handleMouseEvent(QMouseEvent* event);
    bool handleWheelEvent(QWheelEvent* event);
    bool handleKeyEvent(QKeyEvent* event);
    PointerEvent* createTouchEvent(QMouseEvent* event);

private:
    InputSimulationConfig m_config;
    bool m_enabled = true;
    QPointF m_lastTouchPosition;
};

} // namespace CFDesktop::Simulator
```

---

## 九、详细任务清单

### 9.1 Week 1: 基础框架

#### Day 1-2: 主窗口与设备外壳
- [ ] 创建 SimulatorWindow 类
- [ ] 实现 DeviceFrame 基础绘制
- [ ] 实现矢量外壳绘制
- [ ] 支持图片外壳

#### Day 3: 设备配置
- [ ] 定义 DeviceProfile 结构
- [ ] 实现 JSON 序列化
- [ ] 创建预设配置文件
- [ ] 实现配置加载

#### Day 4: 控制面板
- [ ] 创建 ControlPanel 类
- [ ] 实现设备选择器
- [ ] 实现分辨率选择器
- [ ] 实现档位选择器

#### Day 5: 集成测试
- [ ] 集成各模块
- [ ] 基本功能测试

### 9.2 Week 2: 注入与模拟

#### Day 1-2: DPI 注入
- [ ] 实现 DPI 注入器
- [ ] 修改 DPIManager 支持注入
- [ ] 测试不同分辨率

#### Day 3: 硬件 Mock
- [ ] 实现硬件信息 Mock
- [ ] 修改 HardwareProbe 支持 Mock
- [ ] 测试档位切换

#### Day 4: 输入模拟
- [ ] 实现输入模拟器
- [ ] 鼠标转触摸
- [ ] 键盘转按键

#### Day 5: 触摸可视化
- [ ] 实现 TouchVisualizer
- [ ] 涟漪动画
- [ ] 多点触摸支持

### 9.3 Week 3: 完善与优化

#### Day 1-2: UI 完善
- [ ] 完善设备外壳
- [ ] 添加更多预设
- [ ] 美化控制面板

#### Day 3: 高级功能
- [ ] 截图功能
- [ ] 录屏功能
- [ ] 性能监控

#### Day 4: 文档
- [ ] 使用说明
- [ ] API 文档
- [ ] 示例代码

#### Day 5: 测试
- [ ] 跨平台测试
- [ ] 性能测试
- [ ] 修复 Bug

---

## 十、验收标准

### 10.1 功能验收
- [ ] 能正确显示各种分辨率
- [ ] 档位切换生效
- [ ] 触摸模拟正常
- [ ] 截图功能正常

### 10.2 性能验收
- [ ] 启动时间 < 2 秒
- [ ] 帧率稳定 60fps

### 10.3 兼容性验收
- [ ] Windows 10/11 正常运行
- [ ] Ubuntu 20.04+ 正常运行
- [ ] macOS 12+ 正常运行

---

## 十一、使用示例

### 11.1 启动模拟器

```cpp
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // 创建模拟器窗口
    SimulatorWindow simulator;
    simulator.show();

    // 创建 Shell UI
    ShellWindow* shell = new ShellWindow();
    simulator.setContentWidget(shell);

    return app.exec();
}
```

### 11.2 切换设备配置

```cpp
// 加载新配置
simulator.loadDeviceProfile("/path/to/imx6ull-7.0.json");

// 或者通过控制面板选择
simulator.controlPanel()->setCurrentProfile("IMX6ULL 7.0 inch");
```

---

## 十二、下一步行动

完成 Phase 6 后，进入 **Phase 4: Shell UI 主体** 或 **Phase 5: SDK 导出层**。
