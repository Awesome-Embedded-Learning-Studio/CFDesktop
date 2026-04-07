# Phase 3: 输入抽象层 TODO

> **状态**: ⬜ 待开始
> **预计周期**: 1~2 周
> **依赖阶段**: Phase 0, Phase 1, Phase 2
> **目标交付物**: 统一输入分发、触摸处理、按键处理、手势识别、焦点导航

---

## 一、阶段目标

### 核心目标
屏蔽底层输入差异，统一触摸、物理按键、旋钮等输入事件，支持焦点导航模式。

### 具体交付物
- [ ] `InputManager` 统一分发层
- [ ] `TouchInputHandler` 触摸处理器
- [ ] `KeyInputHandler` 按键处理器
- [ ] `RotaryInputHandler` 旋钮处理器
- [ ] `FocusNavigator` 焦点导航器
- [ ] 单元测试

---

## 二、Week 1: 核心处理器

### Day 1-2: InputManager 基础
- [ ] 创建 InputManager 类
  - [ ] 单例模式
  - [ ] 设备注册/注销
  - [ ] 事件分发机制
- [ ] 实现设备注册/注销
  - [ ] `registerDevice()`
  - [ ] `unregisterDevice()`
  - [ ] 设备列表管理
- [ ] 实现事件分发机制
  - [ ] `dispatchEvent()`
  - [ ] 事件过滤器
- [ ] 添加事件过滤器支持
  - [ ] `addEventFilter()`
  - [ ] `removeEventFilter()`

### Day 3: TouchInputHandler
- [ ] 创建 TouchInputHandler 类
  - [ ] 触摸点跟踪
  - [ ] 手势检测基础
- [ ] 实现触摸点跟踪
  - [ ] `TouchPoint` 结构
  - [ ] 多点触摸支持
  - [ ] 压力检测
- [ ] 实现单击/双击检测
  - [ ] 点击阈值判定
  - [ ] 时间窗口判定
- [ ] 实现长按检测
  - [ ] 定时器实现
  - [ ] 移动距离判定

### Day 4: KeyInputHandler
- [ ] 创建 KeyInputHandler 类
  - [ ] 按键配置支持
  - [ ] 状态跟踪
- [ ] 实现按键状态跟踪
  - [ ] 按键列表
  - [ ] 时间戳记录
- [ ] 实现长按检测
  - [ ] 阈值配置
  - [ ] 信号触发
- [ ] 实现连击检测
  - [ ] 计数器
  - [ ] 时间窗口

### Day 5: RotaryInputHandler
- [ ] 创建 RotaryInputHandler 类
  - [ ] 旋钮配置
  - [ ] 速度计算
- [ ] 实现旋转解码
  - [ ] AB 相位解码
  - [ ] 方向判定
- [ ] 实现速度计算
  - [ ] 滑动平均
  - [ ] 加速因子
- [ ] 实现加速功能
  - [ ] 速度映射
  - [ ] 可配置系数

---

## 三、Week 2: 手势与导航

### Day 1-2: GestureRecognizer
- [ ] 创建 GestureRecognizer 类
  - [ ] 手势状态机
  - [ ] 配置支持
- [ ] 实现滑动手势
  - [ ] 方向判定
  - [ ] 距离阈值
  - [ ] 超时检测
- [ ] 实现捏合手势
  - [ ] 双指检测
  - [ ] 缩放计算
- [ ] 实现旋转手势
  - [ ] 角度计算
  - [ ] 方向判定

### Day 3: FocusNavigator
- [ ] 创建 FocusNavigator 类
  - [ ] 单例模式
  - [ ] 焦点策略
- [ ] 实现方向导航算法
  - [ ] 四方向查找
  - [ ] 距离计算
  - [ ] 方向判定
- [ ] 实现焦点链自定义
  - [ ] `addFocusChain()`
  - [ ] 自定义跳转
- [ ] 实现边界策略
  - [ ] 循环策略
  - [ ] 停止策略

### Day 4: 原生设备
- [ ] 实现 EvdevDevice
  - [ ] `/dev/input/eventX` 读取
  - [ ] 事件解析
  - [ ] 设备识别
- [ ] 实现 GPIOButton
  - [ ] sysfs 接口
  - [ ] 边沿检测
  - [ ] 防抖处理
- [ ] 实现 RotaryEncoder
  - [ ] GPIO 输入
  - [ ] 状态解码
  - [ ] 位置计算

### Day 5: 测试与集成
- [ ] 编写单元测试
  - [ ] 触摸处理测试
  - [ ] 按键处理测试
  - [ ] 手势识别测试
  - [ ] 焦点导航测试
- [ ] 集成测试
  - [ ] 多设备协同
  - [ ] 事件流测试
- [ ] 性能测试
  - [ ] 事件延迟测试
  - [ ] CPU 占用测试

---

## 四、验收标准

### 功能验收
- [ ] 触摸输入正常响应
- [ ] 手势识别准确率 > 95%
- [ ] 焦点导航无死循环
- [ ] 原生设备正常读取

### 性能验收
- [ ] 事件延迟 < 16ms
- [ ] CPU 占用 < 5%

### 兼容性验收
- [ ] 模拟器和真机行为一致

---

## 五、文件清单

### 核心接口
- [ ] `include/CFDesktop/Base/Input/InputManager.h`
- [ ] `include/CFDesktop/Base/Input/InputEvent.h`
- [ ] `include/CFDesktop/Base/Input/InputDevice.h`

### 处理器
- [ ] `include/CFDesktop/Base/Input/TouchInputHandler.h`
- [ ] `include/CFDesktop/Base/Input/KeyInputHandler.h`
- [ ] `include/CFDesktop/Base/Input/RotaryInputHandler.h`
- [ ] `include/CFDesktop/Base/Input/GestureRecognizer.h`
- [ ] `include/CFDesktop/Base/Input/FocusNavigator.h`

### 实现
- [ ] `src/base/input/InputManager.cpp`
- [ ] `src/base/input/TouchInputHandler.cpp`
- [ ] `src/base/input/KeyInputHandler.cpp`
- [ ] `src/base/input/RotaryInputHandler.cpp`
- [ ] `src/base/input/GestureRecognizer.cpp`
- [ ] `src/base/input/FocusNavigator.cpp`

### 原生设备
- [ ] `src/base/input/native/EvdevDevice.cpp`
- [ ] `src/base/input/native/GPIOButton.cpp`
- [ ] `src/base/input/native/RotaryEncoder.cpp`
- [ ] `src/base/input/simulator/SimulatedInput.cpp`

### 测试
- [ ] `tests/unit/base/input/test_input_manager.cpp`
- [ ] `tests/unit/base/input/test_touch_handler.cpp`
- [ ] `tests/unit/base/input/test_key_handler.cpp`
- [ ] `tests/unit/base/input/test_rotary_handler.cpp`
- [ ] `tests/unit/base/input/test_gesture_recognizer.cpp`
- [ ] `tests/unit/base/input/test_focus_navigator.cpp`

---

## 六、相关文档

- 设计文档: [../../design_stage/03_phase3_input_layer.md](../../design_stage/03_phase3_input_layer.md)
- 依赖: [工程骨架状态](../done/00_project_skeleton_status.md), [硬件探针状态](../done/01_hardware_probe_status.md)
- Base库已完成: [done/02_base_library_status.md](../done/02_base_library_status.md)

---

*最后更新: 2026-03-05*
