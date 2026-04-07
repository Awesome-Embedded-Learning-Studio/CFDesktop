# Phase 3: 输入抽象层 - 状态文档

> **模块ID**: Phase 3
> **状态**: <img src="x-data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' width='16' height='16'%3E%3Crect width='16' height='16' fill='none' stroke='gray' stroke-width='1'/%3E%3C/svg%3E" width="16" height="16"> 未开始
> **总体进度**: 0%
> **最后更新**: 2026-03-11

---

# 输入抽象层参考文档

> **文档版本**: 1.0
> **最后更新**: 2026-03-05
> **完成度**: 0%
> **说明**: 完全未实现，需要从头开发

---

## 一、模块概述

输入抽象层是CFDesktop项目的人机交互基础设施层，负责屏蔽底层输入设备差异，统一触摸、物理按键、旋钮等输入事件，并支持焦点导航模式。

### 核心职责

1. **统一输入分发** - 集中管理所有输入设备的注册、注销和事件分发
2. **触摸处理** - 单击/双击检测、长按检测、多点触摸支持
3. **按键处理** - 按键状态跟踪、长按检测、连击检测
4. **旋钮处理** - AB相位解码、方向判定、速度计算、加速功能
5. **手势识别** - 滑动、捏合、旋转手势识别
6. **焦点导航** - 四方向导航、自定义焦点链、边界策略

### 设计目标

- **设备无关性**: 抽象底层硬件差异，提供统一的输入接口
- **可扩展性**: 支持动态注册新类型输入设备
- **模拟器友好**: 支持模拟器环境下的输入注入
- **高性能**: 事件延迟 < 16ms，CPU占用 < 5%

---

## 二、当前实现状态

### 总体完成度: 0%

| 模块 | 完成度 | 实现位置 | 备注 |
|------|--------|----------|------|
| InputManager | 0% | - | 未实现 |
| TouchInputHandler | 0% | - | 未实现 |
| KeyInputHandler | 0% | - | 未实现 |
| RotaryInputHandler | 0% | - | 未实现 |
| GestureRecognizer | 0% | - | 未实现 |
| FocusNavigator | 0% | - | 未实现 |
| 原生设备驱动 | 0% | - | 未实现 |

---

## 三、待完成项清单

### 3.1 InputManager - 统一分发层 (0%)

**职责描述**:
- 设备注册/注销管理
- 事件分发机制
- 事件过滤器支持

**建议文件路径**:
- `include/CFDesktop/Base/Input/InputManager.h`
- `include/CFDesktop/Base/Input/InputEvent.h`
- `include/CFDesktop/Base/Input/InputDevice.h`
- `src/base/input/InputManager.cpp`

**关键接口设计**:
```cpp
namespace cf::base::input {

// 输入设备基类
class InputDevice {
public:
    virtual ~InputDevice() = default;
    virtual QString deviceId() const = 0;
    virtual QString deviceName() const = 0;
    virtual DeviceType deviceType() const = 0;
};

// 输入事件基类
class InputEvent {
public:
    virtual ~InputEvent() = default;
    virtual qint64 timestamp() const = 0;
    virtual InputDevice* sourceDevice() const = 0;
};

// 输入管理器
class InputManager : public QObject {
    Q_OBJECT
public:
    static InputManager& instance();

    // 设备管理
    void registerDevice(std::unique_ptr<InputDevice> device);
    void unregisterDevice(const QString& deviceId);
    QList<InputDevice*> devices() const;
    InputDevice* device(const QString& deviceId) const;

    // 事件分发
    void dispatchEvent(std::unique_ptr<InputEvent> event);

    // 事件过滤器
    void addEventFilter(QObject* filter);
    void removeEventFilter(QObject* filter);

signals:
    void deviceRegistered(InputDevice* device);
    void deviceUnregistered(const QString& deviceId);
    void eventReceived(InputEvent* event);
};

} // namespace cf::base::input
```

### 3.2 TouchInputHandler - 触摸处理器 (0%)

**职责描述**:
- 触摸点跟踪 (TouchPoint结构)
- 单击/双击检测
- 长按检测
- 多点触摸支持
- 压力检测

**建议文件路径**:
- `include/CFDesktop/Base/Input/TouchInputHandler.h`
- `src/base/input/TouchInputHandler.cpp`

**关键接口设计**:
```cpp
namespace cf::base::input {

struct TouchPoint {
    int id;              // 触摸点ID
    QPointF pos;         // 当前位置
    QPointF startPos;    // 起始位置
    float pressure;      // 压力值
    qint64 timestamp;    // 时间戳
};

class TouchInputHandler : public QObject {
    Q_OBJECT
public:
    explicit TouchInputHandler(QObject* parent = nullptr);

    // 配置参数
    void setClickThreshold(float pixels);    // 点击移动阈值
    void setDoubleClickInterval(int ms);     // 双击时间间隔
    void setLongPressTimeout(int ms);        // 长按超时时间

    // 触摸点管理
    void addTouchPoint(const TouchPoint& point);
    void updateTouchPoint(int id, const QPointF& newPos);
    void removeTouchPoint(int id);

signals:
    void clicked(const QPointF& pos);
    void doubleClicked(const QPointF& pos);
    void longPressed(const QPointF& pos);
    void touchMoved(const QList<TouchPoint>& points);

private:
    QMap<int, TouchPoint> activePoints_;
    float clickThreshold_ = 10.0f;
    int doubleClickInterval_ = 400;
    int longPressTimeout_ = 500;
};

} // namespace cf::base::input
```

### 3.3 KeyInputHandler - 按键处理器 (0%)

**职责描述**:
- 按键状态跟踪
- 长按检测
- 连击检测
- 按键配置支持

**建议文件路径**:
- `include/CFDesktop/Base/Input/KeyInputHandler.h`
- `src/base/input/KeyInputHandler.cpp`

**关键接口设计**:
```cpp
namespace cf::base::input {

struct KeyEvent {
    int keyCode;          // 按键代码
    bool pressed;         // true=按下, false=释放
    qint64 timestamp;     // 时间戳
};

class KeyInputHandler : public QObject {
    Q_OBJECT
public:
    explicit KeyInputHandler(QObject* parent = nullptr);

    // 按键处理
    void handleKeyEvent(const KeyEvent& event);

    // 配置参数
    void setLongPressThreshold(int ms);   // 长按阈值
    void setMultiClickWindow(int ms);     // 连击时间窗口

signals:
    void keyPressed(int keyCode);
    void keyReleased(int keyCode);
    void keyLongPressed(int keyCode);     // 长按触发
    void keyMultiClicked(int keyCode, int count);  // 连击

private:
    QMap<int, qint64> keyPressTime_;
    QMap<int, int> keyClickCount_;
    int longPressThreshold_ = 800;
    int multiClickWindow_ = 300;
};

} // namespace cf::base::input
```

### 3.4 RotaryInputHandler - 旋钮处理器 (0%)

**职责描述**:
- AB相位解码
- 方向判定
- 速度计算
- 加速功能

**建议文件路径**:
- `include/CFDesktop/Base/Input/RotaryInputHandler.h`
- `src/base/input/RotaryInputHandler.cpp`

**关键接口设计**:
```cpp
namespace cf::base::input {

enum class RotaryDirection { Clockwise, CounterClockwise };

class RotaryInputHandler : public QObject {
    Q_OBJECT
public:
    explicit RotaryInputHandler(QObject* parent = nullptr);

    // AB相位输入
    void handlePhaseA(bool state);
    void handlePhaseB(bool state);

    // 配置参数
    void setStepsPerRevolution(int steps);  // 每圈步数
    void setAccelerationFactor(float factor);  // 加速因子

signals:
    void rotated(RotaryDirection direction, int steps);
    void positionChanged(int absolutePosition);

private:
    bool phaseA_ = false;
    bool phaseB_ = false;
    int position_ = 0;
    QList<qint64> stepTimestamps_;  // 用于速度计算
    float accelerationFactor_ = 1.0f;
};

} // namespace cf::base::input
```

### 3.5 GestureRecognizer - 手势识别器 (0%)

**职责描述**:
- 滑动手势 (Swipe)
- 捏合手势 (Pinch)
- 旋转手势 (Rotation)

**建议文件路径**:
- `include/CFDesktop/Base/Input/GestureRecognizer.h`
- `src/base/input/GestureRecognizer.cpp`

**关键接口设计**:
```cpp
namespace cf::base::input {

enum class SwipeDirection { Up, Down, Left, Right };

class GestureRecognizer : public QObject {
    Q_OBJECT
public:
    explicit GestureRecognizer(QObject* parent = nullptr);

    // 触摸输入
    void handleTouchPoints(const QList<TouchPoint>& points);

    // 配置参数
    void setSwipeThreshold(float pixels);    // 滑动距离阈值
    void setSwipeTimeout(int ms);            // 滑动超时
    void setPinchThreshold(float ratio);     // 捏合阈值

signals:
    void swipeDetected(SwipeDirection direction);
    void pinchStarted(const QPointF& center);
    void pinchZoomChanged(float scale);
    void pinchFinished();
    void rotationStarted(const QPointF& center);
    void rotationAngleChanged(float angle);
    void rotationFinished();

private:
    QList<QPointF> lastTouchPositions_;
    qint64 lastTouchTime_ = 0;
    float swipeThreshold_ = 50.0f;
    int swipeTimeout_ = 300;
};

} // namespace cf::base::input
```

### 3.6 FocusNavigator - 焦点导航器 (0%)

**职责描述**:
- 四方向导航算法
- 自定义焦点链
- 边界策略 (循环/停止)

**建议文件路径**:
- `include/CFDesktop/Base/Input/FocusNavigator.h`
- `src/base/input/FocusNavigator.cpp`

**关键接口设计**:
```cpp
namespace cf::base::input {

enum class NavigationDirection { Up, Down, Left, Right };
enum class BoundaryPolicy { Stop, Wrap, Jump };

class FocusNavigator : public QObject {
    Q_OBJECT
public:
    static FocusNavigator& instance();

    // 焦点导航
    void navigate(NavigationDirection direction);
    void setFocus(QWidget* widget);
    QWidget* currentFocus() const;

    // 焦点策略
    void setBoundaryPolicy(BoundaryPolicy policy);

    // 自定义焦点链
    void addFocusChain(QWidget* from, NavigationDirection dir, QWidget* to);
    void removeFocusChain(QWidget* from);
    void clearFocusChains();

signals:
    void focusChanged(QWidget* oldFocus, QWidget* newFocus);
    void focusLost(QWidget* widget);

private:
    QWidget* currentFocus_ = nullptr;
    BoundaryPolicy boundaryPolicy_ = BoundaryPolicy::Stop;
    QMultiMap<QWidget*, QPair<NavigationDirection, QWidget*>> focusChains_;
};

} // namespace cf::base::input
```

---

## 四、原生设备驱动

### 4.1 EvdevDevice - Linux事件设备 (0%)

**职责**: 读取 `/dev/input/eventX` 设备事件

**建议文件路径**:
- `src/base/input/native/EvdevDevice.cpp`
- `src/base/input/native/EvdevDevice.h`

### 4.2 GPIOButton - GPIO按键 (0%)

**职责**: 通过sysfs接口读取GPIO按键状态

**建议文件路径**:
- `src/base/input/native/GPIOButton.cpp`
- `src/base/input/native/GPIOButton.h`

### 4.3 RotaryEncoder - 旋转编码器 (0%)

**职责**: 读取GPIO编码器状态并解码

**建议文件路径**:
- `src/base/input/native/RotaryEncoder.cpp`
- `src/base/input/native/RotaryEncoder.h`

### 4.4 SimulatedInput - 模拟器输入 (0%)

**职责**: 为模拟器环境提供输入注入接口

**建议文件路径**:
- `src/base/input/simulator/SimulatedInput.cpp`
- `src/base/input/simulator/SimulatedInput.h`

---

## 五、完成度百分比

| 模块 | 原计划需求 | 已实现 | 完成度 |
|------|-----------|--------|--------|
| InputManager | 100% | 0% | 0% |
| TouchInputHandler | 100% | 0% | 0% |
| KeyInputHandler | 100% | 0% | 0% |
| RotaryInputHandler | 100% | 0% | 0% |
| GestureRecognizer | 100% | 0% | 0% |
| FocusNavigator | 100% | 0% | 0% |
| 原生设备驱动 | 100% | 0% | 0% |
| **总体** | **100%** | **0%** | **0%** |

---

## 六、关键文件路径

### 待创建文件

```
include/CFDesktop/Base/Input/
├── InputManager.h           # 统一分发层
├── InputEvent.h             # 输入事件定义
├── InputDevice.h            # 输入设备基类
├── TouchInputHandler.h      # 触摸处理器
├── KeyInputHandler.h        # 按键处理器
├── RotaryInputHandler.h     # 旋钮处理器
├── GestureRecognizer.h      # 手势识别器
└── FocusNavigator.h         # 焦点导航器

src/base/input/
├── InputManager.cpp
├── TouchInputHandler.cpp
├── KeyInputHandler.cpp
├── RotaryInputHandler.cpp
├── GestureRecognizer.cpp
├── FocusNavigator.cpp
├── native/
│   ├── EvdevDevice.h/cpp     # Linux evdev设备
│   ├── GPIOButton.h/cpp      # GPIO按键
│   └── RotaryEncoder.h/cpp   # 旋转编码器
└── simulator/
    └── SimulatedInput.h/cpp  # 模拟器输入注入

tests/unit/base/input/
├── test_input_manager.cpp
├── test_touch_handler.cpp
├── test_key_handler.cpp
├── test_rotary_handler.cpp
├── test_gesture_recognizer.cpp
└── test_focus_navigator.cpp
```

---

## 七、下一步行动建议

### 优先级1 (高) - 核心框架

1. **实现InputManager核心框架**
   - 优先级: 最高 (其他模块依赖)
   - 预计工作量: 2-3天
   - 交付物: 设备注册、事件分发、过滤器支持

2. **实现TouchInputHandler**
   - 优先级: 高 (触摸屏设备)
   - 预计工作量: 3-4天
   - 交付物: 触摸点跟踪、点击/长按检测

### 优先级2 (中) - 特定设备

3. **实现KeyInputHandler**
   - 优先级: 中
   - 预计工作量: 2-3天
   - 交付物: 按键状态跟踪、长按/连击检测

4. **实现FocusNavigator**
   - 优先级: 中 (遥控器导航)
   - 预计工作量: 2-3天
   - 交付物: 四方向导航、边界策略

### 优先级3 (低) - 增强功能

5. **实现RotaryInputHandler**
   - 优先级: 低 (特定硬件)
   - 预计工作量: 2天

6. **实现GestureRecognizer**
   - 优先级: 低 (高级交互)
   - 预计工作量: 3-4天

7. **原生设备驱动实现**
   - 优先级: 低 (真机部署时)
   - 预计工作量: 4-5天

---

## 八、验收标准

### 功能验收

- [ ] 触摸输入正常响应 (单击/双击/长按)
- [ ] 手势识别准确率 > 95%
- [ ] 焦点导航无死循环
- [ ] 原生设备正常读取 (evdev/GPIO)

### 性能验收

- [ ] 事件延迟 < 16ms
- [ ] CPU占用 < 5%
- [ ] 无内存泄漏

### 兼容性验收

- [ ] 模拟器和真机行为一致
- [ ] Linux和Windows平台兼容

---

## 九、相关文档

- 原始TODO: [../base/02_input_layer.md](../base/02_input_layer.md)
- 设计文档: `../../design_stage/03_phase3_input_layer.md`
- 依赖模块: [Base库状态](./02_base_library_status.md)

---

*最后更新: 2026-03-05*
