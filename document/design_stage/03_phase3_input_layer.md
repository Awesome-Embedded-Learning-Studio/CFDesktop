---
title: "Phase 3: 输入抽象层详细设计文档"
description: "统一触摸、按键、旋钮等输入事件，支持焦点导航和手势识别"
---

# Phase 3: 输入抽象层 -- 设计意图

## 为什么选择这种方案

CFDesktop 面向嵌入式工控屏、平板和桌面多种形态，输入源差异极大：低端工控屏仅有电阻触摸+GPIO 按键，中端设备带旋转编码器，桌面端则是鼠标键盘。如果不做输入抽象，每个 Widget 必须分别处理 QTouchEvent、QKeyEvent、QWheelEvent 以及原生 evdev/GPIO 事件，导致大量重复代码且难以保证跨设备行为一致。

因此设计了 `InputManager` 单一分发层：所有输入源（Qt 事件、evdev 设备、GPIO、旋转编码器）先被各自的 Handler 转换为统一的 `InputEvent` 体系（PointerEvent / KeyEvent / RotaryEvent / GestureEvent），再由 InputManager 统一分发到目标控件。这样 Widget 层只对接一套事件接口，新增输入类型只需添加新的 Handler，不影响上层。

手势识别（GestureRecognizer）从触摸点流中检测 Swipe/Pinch/LongPress/DoubleTap 等手势，将结果作为 GestureEvent 分发。焦点导航（FocusNavigator）为无触摸设备提供基于空间距离的方向导航算法，支持自定义焦点链和循环策略。模拟器通过 `SimulatedInputConfig` 将鼠标/键盘映射为触摸/旋钮事件，确保开发机和目标设备行为一致。

## 关键决策

| 决策 | 理由 | 被否决的替代方案 |
|------|------|------------------|
| 统一 InputEvent 体系 (PointerEvent/KeyEvent/RotaryEvent/GestureEvent) | Widget 只需对接一套接口；新增输入类型只需加 Handler | 让 Widget 直接处理 Qt 原生事件 + evdev 并行 |
| InputManager 单例 + 事件过滤器链 | 集中管理设备生命周期，支持全局事件拦截（如全局手势） | 每个 Widget 独立注册输入源 |
| 手势识别器独立于 TouchHandler | 解耦触摸处理与手势判定，可单独禁用/替换手势引擎 | 将手势检测逻辑内嵌在 TouchHandler 中 |
| 空间距离焦点导航算法 | 工控屏布局不规则，Tab 顺序不够直观；空间距离更符合用户预期 | 仅使用 Qt Tab 顺序 |
| 原生设备层 (evdev/GPIO/RotaryEncoder) 直接读取 | linuxfb/eglfs 平台下 Qt 不一定转发所有输入；直接读取保证可靠性 | 完全依赖 Qt 平台插件转发 |
| 模拟器输入映射 (鼠标->触摸, 滚轮->旋钮) | 开发机没有触摸/旋钮硬件，映射确保桌面端可调试嵌入式行为 | 要求必须有触摸硬件才能开发 |

## 当前状态

未实现。参见 [status/current.md](../status/current.md) Phase 3 部分。
