# Phase 3: 输入抽象层详细设计文档

## 文档信息
| 项目 | 内容 |
|------|------|
| 文档版本 | v1.0 |
| 创建日期 | 2026-02-20 |
| 阶段代号 | Phase 3 - 输入抽象层 |
| 预计周期 | 1~2 周 |
| 依赖阶段 | Phase 0, Phase 1, Phase 2 |

---

## 一、阶段目标

### 1.1 核心目标
屏蔽底层输入差异，统一触摸、物理按键、旋钮等输入事件，支持焦点导航模式。

### 1.2 具体交付物
- [ ] `InputManager` 统一分发层
- [ ] `TouchInputHandler` 触摸处理器
- [ ] `KeyInputHandler` 按键处理器
- [ ] `RotaryInputHandler` 旋钮处理器
- [ ] `FocusNavigator` 焦点导航器
- [ ] 单元测试

---

## 二、模块架构设计

### 2.1 整体架构图

```
┌─────────────────────────────────────────────────────────────┐
│                      Application Layer                       │
│                  (Widgets receive events)                   │
├─────────────────────────────────────────────────────────────┤
│                       Input Manager                         │
│  ┌────────────────────────────────────────────────────┐    │
│  │  事件转换与分发                                      │    │
│  │  - 统一事件接口                                      │    │
│  │  - 事件过滤与拦截                                    │    │
│  │  - 手势识别                                          │    │
│  └────────────────────────────────────────────────────┘    │
├──────────────────────┬──────────────────────────────────────┤
│  Native Input Sources│   Qt Input Sources                   │
│  ┌────────────────┐ │  ┌─────────────────────────────────┐ │
│  │ evdev Devices  │ │  │ QTouchEvent (linuxfb/eglfs)     │ │
│  │ /dev/input/event│ │  │ QKeyEvent (键盘)                │ │
│  │ GPIO Buttons   │ │  │ QMouseEvent (鼠标)              │ │
│  │ Rotary Encoder │ │  │ QWheelEvent (滚轮)              │ │
│  └────────────────┘ │  └─────────────────────────────────┘ │
├──────────────────────┴──────────────────────────────────────┤
│                    Kernel / Driver Layer                    │
│  evdev, GPIO, I2C, SPI input drivers                       │
└─────────────────────────────────────────────────────────────┘
```

### 2.2 文件结构

```
src/base/
├── include/CFDesktop/Base/Input/
│   ├── InputManager.h              # 输入管理器主类
│   ├── InputEvent.h                # 统一事件结构
│   ├── TouchInputHandler.h         # 触摸处理器
│   ├── KeyInputHandler.h           # 按键处理器
│   ├── RotaryInputHandler.h        # 旋钮处理器
│   ├── GestureRecognizer.h         # 手势识别器
│   ├── FocusNavigator.h            # 焦点导航器
│   └── InputDevice.h               # 输入设备抽象
│
└── src/input/
    ├── InputManager.cpp
    ├── TouchInputHandler.cpp
    ├── KeyInputHandler.cpp
    ├── RotaryInputHandler.cpp
    ├── GestureRecognizer.cpp
    ├── FocusNavigator.cpp
    ├── native/                     # 平台特定实现
    │   ├── EvdevDevice.cpp         # Linux evdev 设备
    │   ├── GPIOButton.cpp          # GPIO 按键
    │   └── RotaryEncoder.cpp       # 旋转编码器
    └── simulator/                  # 模拟器支持
        └── SimulatedInput.cpp
```

---

## 三、统一事件结构

### 3.1 输入事件类型

**文件**: `include/CFDesktop/Base/Input/InputEvent.h`

```cpp
#pragma once
#include <QObject>
#include <QPoint>
#include <QEvent>
#include <QVariant>

namespace CFDesktop::Base {

/**
 * @brief 输入设备类型
 */
enum class InputDeviceType {
    Unknown,
    Touchscreen,      # 触摸屏
    Mouse,            # 鼠标
    Keyboard,         # 键盘
    PhysicalButton,   # 物理按键
    RotaryEncoder,    # 旋转编码器
    Gamepad,          # 游戏手柄
    Custom            # 自定义设备
};

/**
 * @brief 统一输入事件基类
 */
class InputEvent {
public:
    InputDeviceType deviceType = InputDeviceType::Unknown;
    QString deviceId;              # 设备 ID
    quint64 timestamp;             # 时间戳 (微秒)

    virtual ~InputEvent() = default;
    virtual QEvent* toQtEvent() const = 0;
};

/**
 * @brief 触摸/指针位置事件
 */
class PointerEvent : public InputEvent {
public:
    enum class Type {
        Press,
        Release,
        Move,
        Enter,
        Leave
    };

    Type type;
    QPointF position;              # 屏幕坐标
    int pointerId = -1;            # 多点触摸 ID
    qreal pressure = 0.0;          # 压力 (0.0 - 1.0)
    int buttons = 0;               # 按钮状态

    QEvent* toQtEvent() const override;
};

/**
 * @brief 按键事件
 */
class KeyEvent : public InputEvent {
public:
    enum class Type {
        Press,
        Release,
        Repeat,
        LongPress,        # 长按
        MultiPress        # 多连击
    };

    Type type;
    int keyCode;                   # Qt Key_Code
    quint32 nativeCode;            # 原生扫描码
    QString text;                  # 文本内容
    bool isAutoRepeat = false;
    int clickCount = 0;            # 连击计数

    QEvent* toQtEvent() const override;
};

/**
 * @brief 旋钮事件
 */
class RotaryEvent : public InputEvent {
public:
    enum class Direction {
        Clockwise,
        CounterClockwise
    };

    Direction direction;
    int steps = 1;                 # 步数
    qreal angleDelta = 0.0;        # 角度变化
    bool isPressed = false;        # 是否被按下
    int clickCount = 0;            # 旋转档位

    QEvent* toQtEvent() const override;
};

/**
 * @brief 手势事件
 */
class GestureEvent : public InputEvent {
public:
    enum class Type {
        Swipe,
        Pinch,
        Rotate,
        LongPress,
        DoubleTap,
        Custom
    };

    Type type;
    QVariantMap data;              # 手势特定数据
    QPointF centerPoint;
    qreal angle = 0.0;
    qreal scale = 1.0;
    QPointF delta;
};

} // namespace CFDesktop::Base
```

---

## 四、输入管理器 (InputManager)

### 4.1 InputManager 类接口

**文件**: `include/CFDesktop/Base/Input/InputManager.h`

```cpp
#pragma once
#include <QObject>
#include <QList>
#include <QPointer>
#include "InputEvent.h"

namespace CFDesktop::Base {

class InputDevice;
class TouchInputHandler;
class KeyInputHandler;
class RotaryInputHandler;
class GestureRecognizer;

/**
 * @brief 输入管理器
 *
 * 统一管理所有输入设备，提供事件分发和过滤。
 * 单例模式，在应用启动时初始化。
 */
class InputManager : public QObject {
    Q_OBJECT

public:
    static InputManager* instance();

    /**
     * @brief 初始化输入管理器
     */
    bool initialize();

    /**
     * @brief 注册输入设备
     */
    void registerDevice(InputDevice* device);

    /**
     * @brief 注销输入设备
     */
    void unregisterDevice(InputDevice* device);

    /**
     * @brief 获取所有设备
     */
    QList<InputDevice*> devices() const;

    /**
     * @brief 获取指定类型的设备
     */
    QList<InputDevice*> devices(InputDeviceType type) const;

    /**
     * @brief 添加事件过滤器
     *
     * 过滤器返回 true 表示事件被拦截。
     */
    void addEventFilter(QObject* filter);

    /**
     * @brief 移除事件过滤器
     */
    void removeEventFilter(QObject* filter);

    /**
     * @brief 分发事件到指定对象
     */
    bool dispatchEvent(QObject* receiver, InputEvent* event);

    /**
     * @brief 启用/禁用输入
     */
    void setEnabled(bool enabled);
    bool isEnabled() const;

signals:
    /**
     * @brief 设备连接信号
     */
    void deviceConnected(InputDevice* device);

    /**
     * @brief 设备断开信号
     */
    void deviceDisconnected(InputDevice* device);

    /**
     * @brief 输入事件信号（用于调试）
     */
    void inputEventReceived(InputEvent* event);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    InputManager(QObject* parent = nullptr);
    ~InputManager() = default;

    void setupNativeInputDevices();
    void processQtEvent(QEvent* event);

private:
    static InputManager* s_instance;
    QList<QPointer<InputDevice>> m_devices;
    QList<QPointer<QObject>> m_eventFilters;
    QPointer<TouchInputHandler> m_touchHandler;
    QPointer<KeyInputHandler> m_keyHandler;
    QPointer<RotaryInputHandler> m_rotaryHandler;
    QPointer<GestureRecognizer> m_gestureRecognizer;
    bool m_enabled = true;
};

} // namespace CFDesktop::Base
```

---

## 五、触摸处理器 (TouchInputHandler)

### 5.1 TouchInputHandler 类接口

**文件**: `include/CFDesktop/Base/Input/TouchInputHandler.h`

```cpp
#pragma once
#include <QObject>
#include <QHash>
#include "InputEvent.h"

namespace CFDesktop::Base {

/**
 * @brief 触摸点状态
 */
struct TouchPoint {
    int id;
    QPointF position;
    QPointF startPosition;
    qreal pressure = 0.0;
    qint64 startTime = 0;
    bool isPressed = false;
};

/**
 * @brief 触摸处理器
 *
 * 处理 Qt 原生触摸事件，转换为统一事件格式。
 * 支持多点触摸和手势识别。
 */
class TouchInputHandler : public QObject {
    Q_OBJECT

public:
    explicit TouchInputHandler(QObject* parent = nullptr);
    ~TouchInputHandler() = default;

    /**
     * @brief 处理 Qt 触摸事件
     */
    bool handleEvent(QTouchEvent* event);

    /**
     * @brief 启用/禁用触摸输入
     */
    void setEnabled(bool enabled);
    bool isEnabled() const;

    /**
     * @brief 设置点击判定阈值
     *
     * 移动距离小于此值视为点击。
     */
    void setClickThreshold(qreal pixels);

    /**
     * @brief 设置长按超时
     */
    void setLongPressTimeout(int milliseconds);

    /**
     * @brief 获取当前触摸点
     */
    QHash<int, TouchPoint> activeTouches() const;

signals:
    /**
     * @brief 触摸事件信号
     */
    void touchEvent(PointerEvent* event);

    /**
     * @brief 单击信号
     */
    void singleTap(const QPointF& position);

    /**
     * @brief 双击信号
     */
    void doubleTap(const QPointF& position);

    /**
     * @brief 长按信号
     */
    void longPress(const QPointF& position);

private:
    void updateTouchPoints(const QTouchEvent::TouchPoint& point);
    void detectGestures();
    void resetGestureState();

private:
    bool m_enabled = true;
    qreal m_clickThreshold = 10.0;  # dp(10)
    int m_longPressTimeout = 500;
    QHash<int, TouchPoint> m_activeTouches;
    QTimer* m_longPressTimer;
    QPointF m_lastPosition;
};

} // namespace CFDesktop::Base
```

### 5.2 触摸事件流程图

```
QTouchEvent
    │
    ├─> TouchPress
    │       │
    │       ├─> 记录触摸点
    │       ├─> 启动长按定时器
    │       └─> emit touchEvent()
    │
    ├─> TouchMove
    │       │
    │       ├─> 更新触摸点位置
    │       ├─> 检测是否超过点击阈值
    │       ├─> 如果超过阈值，取消长按定时器
    │       └─> emit touchEvent()
    │
    └─> TouchRelease
            │
            ├─> 计算移动距离
            ├─> 如果距离小且时间短：单击
            ├─> 如果是第二次单击：双击
            ├─> 如果定时器触发：长按
            └─> 清理触摸点
```

---

## 六、按键处理器 (KeyInputHandler)

### 6.1 KeyInputHandler 类接口

**文件**: `include/CFDesktop/Base/Input/KeyInputHandler.h`

```cpp
#pragma once
#include <QObject>
#include <QHash>
#include "InputEvent.h"

namespace CFDesktop::Base {

/**
 * @brief 按键配置
 */
struct KeyConfig {
    int keyCode;                    # Qt 键码
    QString action;                 # 动作名称
    int longPressThreshold = 500;   # 长按阈值 (毫秒)
    int repeatDelay = 300;          # 重复延迟
    int repeatInterval = 100;       # 重复间隔
    bool supportLongPress = true;
    bool supportMultiPress = true;
    int maxMultiPressCount = 3;     # 最大连击数
};

/**
 * @brief 按键处理器
 *
 * 处理键盘和物理按键输入，支持长按、连击检测。
 */
class KeyInputHandler : public QObject {
    Q_OBJECT

public:
    explicit KeyInputHandler(QObject* parent = nullptr);
    ~KeyInputHandler() = default;

    /**
     * @brief 处理 Qt 按键事件
     */
    bool handleEvent(QKeyEvent* event);

    /**
     * @brief 注册按键配置
     */
    void registerKey(const KeyConfig& config);

    /**
     * @brief 取消按键注册
     */
    void unregisterKey(int keyCode);

    /**
     * @brief 映射物理按键到 Qt 键码
     *
     * 用于 evdev 扫描码到 Qt Key 的映射。
     */
    void mapKey(int nativeScanCode, int qtKeyCode);

    /**
     * @brief 获取当前按下的按键
     */
    QList<int> pressedKeys() const;

signals:
    /**
     * @brief 按键事件信号
     */
    void keyEvent(KeyEvent* event);

    /**
     * @brief 动作触发信号
     */
    void actionTriggered(const QString& action, int count);

    /**
     * @brief 长按信号
     */
    void longPress(int keyCode);

    /**
     * @brief 组合键信号
     */
    void combinationPressed(const QList<int>& keyCodes);

private:
    void handleKeyPress(QKeyEvent* event);
    void handleKeyRelease(QKeyEvent* event);
    bool detectCombination();

private:
    QHash<int, KeyConfig> m_keyConfigs;
    QHash<int, int> m_scanCodeToQtKey;
    QList<int> m_pressedKeys;
    QHash<int, qint64> m_keyPressTime;
    QHash<int, int> m_keyPressCount;
    QHash<int, QTimer*> m_longPressTimers;
    QHash<int, QTimer*> m_repeatTimers;
};

} // namespace CFDesktop::Base
```

### 6.2 预定义按键映射

```cpp
namespace CFDesktop::Base {

/**
 * @brief 预定义按键动作
 */
enum class StandardAction {
    // 导航
    Up,
    Down,
    Left,
    Right,
    Enter,
    Back,
    Home,
    Menu,

    // 媒体
    VolumeUp,
    VolumeDown,
    Mute,
    PlayPause,
    Next,
    Previous,

    // 系统
    Power,
    Sleep,
    WakeUp,

    // 自定义
    Custom1,
    Custom2,
    Custom3,
    Custom4
};

/**
 * @brief 标准按键映射表
 */
struct StandardKeyMapping {
    static void initializeDefaults(KeyInputHandler* handler);

    // GPIO 按键默认映射
    static const QHash<int, StandardAction> gpioButtonMap;
};

} // namespace CFDesktop::Base
```

---

## 七、旋钮处理器 (RotaryInputHandler)

### 7.1 RotaryInputHandler 类接口

**文件**: `include/CFDesktop/Base/Input/RotaryInputHandler.h`

```cpp
#pragma once
#include <QObject>
#include "InputEvent.h"

namespace CFDesktop::Base {

/**
 * @brief 旋钮配置
 */
struct RotaryConfig {
    int deviceId = 0;
    int stepsPerRevolution = 24;    # 每圈步数
    bool hasButton = true;          # 是否带按钮
    qreal acceleration = 1.0;       # 加速因子
    int velocitySamples = 3;        # 速度采样数
};

/**
 * @brief 旋钮处理器
 *
 * 处理旋转编码器输入，支持加速和按钮模式。
 */
class RotaryInputHandler : public QObject {
    Q_OBJECT

public:
    explicit RotaryInputHandler(QObject* parent = nullptr);
    ~RotaryInputHandler() = default;

    /**
     * @brief 处理旋钮事件
     */
    bool handleEvent(int delta, bool buttonPressed);

    /**
     * @brief 设置旋钮配置
     */
    void setConfig(const RotaryConfig& config);

    /**
     * @brief 获取当前旋转速度
     *
     * 返回步数/秒
     */
    qreal currentVelocity() const;

    /**
     * @brief 获取累计旋转角度
     */
    qreal accumulatedAngle() const;

    /**
     * @brief 重置累计角度
     */
    void resetAccumulatedAngle();

signals:
    /**
     * @brief 旋转事件信号
     */
    void rotated(RotaryEvent* event);

    /**
     * @brief 简单旋转信号
     */
    void stepped(int steps, RotaryEvent::Direction direction);

    /**
     * @brief 按钮按下信号
     */
    void buttonPressed();

    /**
     * @brief 按钮释放信号
     */
    void buttonReleased();

    /**
     * @brief 速度变化信号
     */
    void velocityChanged(qreal velocity);

private:
    void updateVelocity();
    int calculateAcceleratedSteps(int rawSteps);

private:
    RotaryConfig m_config;
    qreal m_accumulatedAngle = 0.0;
    QList<qint64> m_timestampSamples;
    QList<int> m_deltaSamples;
    qreal m_currentVelocity = 0.0;
    bool m_buttonPressed = false;
};

} // namespace CFDesktop::Base
```

---

## 八、手势识别器 (GestureRecognizer)

### 8.1 GestureRecognizer 类接口

**文件**: `include/CFDesktop/Base/Input/GestureRecognizer.h`

```cpp
#pragma once
#include <QObject>
#include <QPointF>
#include "InputEvent.h"

namespace CFDesktop::Base {

/**
 * @brief 手势类型
 */
enum class GestureType {
    None,
    Swipe,
    Pinch,
    Rotate,
    LongPress,
    DoubleTap,
    TwoFingerTap,
    ThreeFingerSwipe,
    Custom
};

/**
 * @brief 手势方向
 */
enum class GestureDirection {
    Up,
    Down,
    Left,
    Right,
    In,
    Out,
    Clockwise,
    CounterClockwise
};

/**
 * @brief 手势识别结果
 */
struct GestureResult {
    GestureType type = GestureType::None;
    GestureDirection direction = GestureDirection::Up;
    QPointF centerPoint;
    QPointF displacement;           # 位移
    qreal scale = 1.0;              # 缩放比例
    qreal angle = 0.0;              # 旋转角度
    qint64 duration = 0;            # 持续时间
    int fingerCount = 1;            # 触摸点数
};

/**
 * @brief 手势配置
 */
struct GestureConfig {
    int swipeMinDistance = 50;      # 最小滑动距离 (dp)
    int swipeMaxDeviation = 30;     # 最大偏离 (dp)
    int swipeTimeout = 500;         # 滑动超时 (毫秒)

    int pinchMinDistance = 20;      # 最小捏合距离
    qreal pinchMinScale = 0.5;      # 最小缩放比例

    int longPressTimeout = 500;     # 长按超时
    int longPressMaxMovement = 10;  # 长按最大移动

    int doubleTapInterval = 300;    # 双击间隔
    int doubleTapMaxMovement = 20;  # 双击最大移动

    bool enableAll = true;          # 启用所有手势
};

/**
 * @brief 手势识别器
 *
 * 从触摸点流中识别常见手势。
 */
class GestureRecognizer : public QObject {
    Q_OBJECT

public:
    explicit GestureRecognizer(QObject* parent = nullptr);
    ~GestureRecognizer() = default;

    /**
     * @brief 处理触摸点更新
     */
    void processTouchPoints(const QHash<int, TouchPoint>& points);

    /**
     * @brief 设置手势配置
     */
    void setConfig(const GestureConfig& config);

    /**
     * @brief 启用/禁用特定手势
     */
    void setGestureEnabled(GestureType type, bool enabled);

    /**
     * @brief 取消当前手势
     */
    void cancelCurrentGesture();

signals:
    /**
     * @brief 手势识别信号
     */
    void gestureRecognized(const GestureResult& gesture);

    /**
     * @brief 手势开始信号
     */
    void gestureStarted(GestureType type);

    /**
     * @brief 手势更新信号
     */
    void gestureUpdated(const GestureResult& gesture);

    /**
     * @brief 手势结束信号
     */
    void gestureEnded(const GestureResult& gesture);

    /**
     * @brief 手势取消信号
     */
    void gestureCancelled(GestureType type);

private:
    bool detectSwipe();
    bool detectPinch();
    bool detectRotate();
    bool detectLongPress();
    bool detectDoubleTap();
    GestureDirection detectDirection(const QPointF& delta);

    void reset();

private:
    GestureConfig m_config;
    QSet<GestureType> m_enabledGestures;
    QHash<int, TouchPoint> m_activeTouches;
    QList<TouchPoint> m_startPoints;
    GestureResult m_currentGesture;
    qint64 m_gestureStartTime = 0;
    bool m_isGestureActive = false;
};

} // namespace CFDesktop::Base
```

---

## 九、焦点导航器 (FocusNavigator)

### 9.1 FocusNavigator 类接口

**文件**: `include/CFDesktop/Base/Input/FocusNavigator.h`

```cpp
#pragma once
#include <QObject>
#include <QWidget>
#include "InputEvent.h"

namespace CFDesktop::Base {

/**
 * @brief 焦点移动方向
 */
enum class FocusDirection {
    Up,
    Down,
    Left,
    Right,
    Next,       # Tab 顺序下一个
    Previous,   # Tab 顺序上一个
    First,      # 第一个控件
    Last        # 最后一个控件
};

/**
 * @brief 焦点策略
 */
enum class FocusPolicy {
    // 自动策略
    Auto,

    // 严格策略：只跳到可视控件
    Strict,

    // 包含策略：包含隐藏/禁用控件
    Inclusive,

    // 循环策略：边界循环
    Wrap,

    // 自定义策略
    Custom
};

/**
 * @brief 焦点导航器
 *
 * 为无触摸设备提供键盘/按键焦点导航。
 */
class FocusNavigator : public QObject {
    Q_OBJECT

public:
    static FocusNavigator* instance();

    /**
     * @brief 设置导航根窗口
     */
    void setRootWidget(QWidget* widget);

    /**
     * @brief 移动焦点
     */
    bool moveFocus(FocusDirection direction);

    /**
     * @brief 设置焦点策略
     */
    void setFocusPolicy(FocusPolicy policy);

    /**
     * @brief 添加焦点链
     *
     * 自定义焦点跳转顺序。
     */
    void addFocusChain(QWidget* from, QWidget* to, FocusDirection direction);

    /**
     * @brief 移除焦点链
     */
    void removeFocusChain(QWidget* from, FocusDirection direction);

    /**
     * @brief 设置当前焦点控件
     */
    void setFocusWidget(QWidget* widget);

    /**
     * @brief 获取当前焦点控件
     */
    QWidget* currentFocusWidget() const;

    /**
     * @brief 获取下一个焦点控件
     */
    QWidget* nextFocusWidget(FocusDirection direction) const;

signals:
    /**
     * @brief 焦点变化信号
     */
    void focusChanged(QWidget* oldWidget, QWidget* newWidget);

    /**
     * @brief 无焦点可移动信号
     */
    void focusBlocked(QWidget* currentWidget, FocusDirection direction);

private:
    FocusNavigator(QObject* parent = nullptr);
    ~FocusNavigator() = default;

    QWidget* findNearestWidget(QWidget* start, FocusDirection direction) const;
    bool isFocusable(QWidget* widget) const;
    qreal calculateDistance(QWidget* from, QWidget* to, FocusDirection direction) const;
    bool isInDirection(QWidget* from, QWidget* to, FocusDirection direction) const;

private:
    static FocusNavigator* s_instance;
    QPointer<QWidget> m_rootWidget;
    QPointer<QWidget> m_currentWidget;
    FocusPolicy m_policy = FocusPolicy::Auto;
    QMultiHash<QWidget*, QPair<QWidget*, FocusDirection>> m_focusChains;
};

} // namespace CFDesktop::Base
```

### 9.2 焦点导航算法

```cpp
QWidget* FocusNavigator::findNearestWidget(
    QWidget* start,
    FocusDirection direction
) const {
    if (!start || !m_rootWidget) {
        return nullptr;
    }

    QWidget* nearest = nullptr;
    qreal minDistance = std::numeric_limits<qreal>::max();

    // 获取所有可聚焦的子控件
    QList<QWidget*> candidates = m_rootWidget->findChildren<QWidget*>();
    QRect startRect = start->geometry();

    for (QWidget* candidate : candidates) {
        if (!isFocusable(candidate) || candidate == start) {
            continue;
        }

        if (!isInDirection(start, candidate, direction)) {
            continue;
        }

        qreal distance = calculateDistance(start, candidate, direction);
        if (distance < minDistance) {
            minDistance = distance;
            nearest = candidate;
        }
    }

    return nearest;
}

bool FocusNavigator::isInDirection(
    QWidget* from,
    QWidget* to,
    FocusDirection direction
) const {
    QRect fromRect = from->geometry();
    QRect toRect = to->geometry();
    QPointF fromCenter = fromRect.center();
    QPointF toCenter = toRect.center();

    switch (direction) {
    case FocusDirection::Up:
        return toCenter.y() < fromCenter.y();
    case FocusDirection::Down:
        return toCenter.y() > fromCenter.y();
    case FocusDirection::Left:
        return toCenter.x() < fromCenter.x();
    case FocusDirection::Right:
        return toCenter.x() > fromCenter.x();
    default:
        return true;
    }
}
```

---

## 十、原生输入设备

### 10.1 Evdev 设备

**文件**: `src/input/native/EvdevDevice.h`

```cpp
#pragma once
#include <QObject>
#include <QString>
#include "InputDevice.h"

namespace CFDesktop::Base {

/**
 * @brief Linux evdev 输入设备
 *
 * 直接读取 /dev/input/eventX 设备。
 * 支持按键、相对轴（鼠标）、绝对轴（触摸屏）。
 */
class EvdevDevice : public InputDevice {
    Q_OBJECT

public:
    /**
     * @brief 设备类型
     */
    enum class EvdevType {
        Unknown,
        Keyboard,
        Mouse,
        Touchscreen,
        Joystick,
        Accelerometer
    };

    /**
     * @brief 打开设备
     */
    static EvdevDevice* open(const QString& devicePath);

    ~EvdevDevice() override;

    /**
     * @brief 开始读取事件
     */
    bool start() override;

    /**
     * @brief 停止读取事件
     */
    void stop() override;

    /**
     * @brief 设备类型
     */
    EvdevType evdevType() const;

    /**
     * @brief 设备名称
     */
    QString deviceName() const;

signals:
    /**
     * @brief evdev 原始事件信号
     */
    void rawEvent(quint16 type, quint16 code, qint32 value);

private:
    EvdevDevice(const QString& path, int fd, QObject* parent = nullptr);

    void readEvents();
    void identifyDevice();

private:
    QString m_devicePath;
    int m_fd = -1;
    EvdevType m_type = EvdevType::Unknown;
    QString m_name;
    QSocketNotifier* m_notifier = nullptr;
};

} // namespace CFDesktop::Base
```

### 10.2 GPIO 按键

**文件**: `src/input/native/GPIOButton.h`

```cpp
#pragma once
#include <QObject>
#include "InputDevice.h"

namespace CFDesktop::Base {

/**
 * @brief GPIO 按键设备
 *
 * 通过 sysfs 或 libgpiod 读取 GPIO 按键。
 */
class GPIOButton : public InputDevice {
    Q_OBJECT

public:
    /**
     * @brief 打开 GPIO 按键
     * @param gpioPin GPIO 引脚号
     * @param activeLow 低电平有效
     */
    static GPIOButton* open(int gpioPin, bool activeLow = true);

    ~GPIOButton() override;

    bool start() override;
    void stop() override;

    /**
     * @brief 关联的 Qt 键码
     */
    int mappedKeyCode() const;
    void setMappedKeyCode(int keyCode);

    /**
     * @brief 防抖延迟 (毫秒)
     */
    int debounceDelay() const;
    void setDebounceDelay(int milliseconds);

signals:
    /**
     * @brief 按钮按下信号
     */
    void pressed();

    /**
     * @brief 按钮释放信号
     */
    void released();

private:
    GPIOButton(int gpioPin, QObject* parent = nullptr);

    void handleEdgeDetect();
    void setupGPIO();

private:
    int m_gpioPin;
    bool m_activeLow = true;
    int m_mappedKeyCode = Qt::Key_unknown;
    int m_debounceDelay = 50;
    QFile m_valueFile;
    QTimer* m_debounceTimer = nullptr;
    bool m_lastState = false;
};

} // namespace CFDesktop::Base
```

### 10.3 旋转编码器

**文件**: `src/input/native/RotaryEncoder.h`

```cpp
#pragma once
#include <QObject>
#include "InputDevice.h"

namespace CFDesktop::Base {

/**
 * @brief 旋转编码器设备
 *
 * 支持 AB 相位编码器。
 */
class RotaryEncoder : public InputDevice {
    Q_OBJECT

public:
    /**
     * @brief 打开旋转编码器
     * @param pinA A 相引脚
     * @param pinB B 相引脚
     */
    static RotaryEncoder* open(int pinA, int pinB);

    ~RotaryEncoder() override;

    bool start() override;
    void stop() override;

    /**
     * @brief 每圈步数
     */
    void setStepsPerRevolution(int steps);

    /**
     * @brief 是否反转方向
     */
    void setInverted(bool inverted);

signals:
    /**
     * @brief 旋转信号
     */
    void rotated(int steps);

    /**
     * @brief 按钮信号（如果带按钮）
     */
    void buttonPressed();
    void buttonReleased();

private:
    RotaryEncoder(int pinA, int pinB, QObject* parent = nullptr);

    void decodeState(int stateA, int stateB);

private:
    int m_pinA;
    int m_pinB;
    int m_stepsPerRevolution = 24;
    bool m_inverted = false;
    int m_lastState = 0;
    int m_position = 0;
};

} // namespace CFDesktop::Base
```

---

## 十一、模拟器支持

### 11.1 模拟输入配置

```cpp
namespace CFDesktop::Base {

/**
 * @brief 模拟输入配置
 */
struct SimulatedInputConfig {
    // 鼠标模拟触摸
    bool mouseEmulatesTouch = true;

    // 键盘模拟方向键
    QHash<int, FocusDirection> keyToDirection = {
        { Qt::Key_Up, FocusDirection::Up },
        { Qt::Key_Down, FocusDirection::Down },
        { Qt::Key_Left, FocusDirection::Left },
        { Qt::Key_Right, FocusDirection::Right },
        { Qt::Key_Tab, FocusDirection::Next },
        { Qt::Key_Backtab, FocusDirection::Previous }
    };

    // 触摸视觉反馈
    bool showTouchRipple = true;
    int rippleDuration = 300;

    // 旋钮模拟
    bool mouseWheelEmulatesRotary = true;
    qreal wheelScale = 1.0;
};

} // namespace CFDesktop::Base
```

---

## 十二、详细任务清单

### 12.1 Week 1: 核心处理器

#### Day 1-2: InputManager 基础
- [ ] 创建 InputManager 类
- [ ] 实现设备注册/注销
- [ ] 实现事件分发机制
- [ ] 添加事件过滤器支持

#### Day 3: TouchInputHandler
- [ ] 创建 TouchInputHandler 类
- [ ] 实现触摸点跟踪
- [ ] 实现单击/双击检测
- [ ] 实现长按检测

#### Day 4: KeyInputHandler
- [ ] 创建 KeyInputHandler 类
- [ ] 实现按键状态跟踪
- [ ] 实现长按检测
- [ ] 实现连击检测

#### Day 5: RotaryInputHandler
- [ ] 创建 RotaryInputHandler 类
- [ ] 实现旋转解码
- [ ] 实现速度计算
- [ ] 实现加速功能

### 12.2 Week 2: 手势与导航

#### Day 1-2: GestureRecognizer
- [ ] 创建 GestureRecognizer 类
- [ ] 实现滑动手势
- [ ] 实现捏合手势
- [ ] 实现旋转手势

#### Day 3: FocusNavigator
- [ ] 创建 FocusNavigator 类
- [ ] 实现方向导航算法
- [ ] 实现焦点链自定义
- [ ] 实现边界策略

#### Day 4: 原生设备
- [ ] 实现 EvdevDevice
- [ ] 实现 GPIOButton
- [ ] 实现 RotaryEncoder

#### Day 5: 测试与集成
- [ ] 编写单元测试
- [ ] 集成测试
- [ ] 性能测试

---

## 十三、验收标准

### 13.1 功能验收
- [ ] 触摸输入正常响应
- [ ] 手势识别准确率 > 95%
- [ ] 焦点导航无死循环
- [ ] 原生设备正常读取

### 13.2 性能验收
- [ ] 事件延迟 < 16ms
- [ ] CPU 占用 < 5%

### 13.3 兼容性验收
- [ ] 模拟器和真机行为一致

---

## 十四、下一步行动

完成 Phase 3 后，进入 **Phase 4: Shell UI 主体**。
