---
title: "Phase 6: 多平台模拟器详细设计文档"
description: "在桌面端还原嵌入式设备 UI 效果，实现所见即所得开发体验"
---

# Phase 6: 多平台模拟器 -- 设计意图

## 为什么选择这种方案

CFDesktop 的目标设备包括 IMX6ULL (480x272)、RK3568 (1024x600)、RK3588 (1920x1200) 等差异极大的嵌入式屏幕。开发者日常在 Windows/Ubuntu 桌面机上工作，如果没有模拟器，每次 UI 调整都必须烧录固件到真机验证，迭代周期从秒级拉长到分钟级。

核心思路是 **DeviceFrame + 注入层**：模拟器窗口由 `DeviceFrame`（设备外壳渲染，提供真实屏幕边框和尺寸感）包裹 Shell UI 内容，下方 `ControlPanel` 提供设备/分辨率/档位切换。关键在于 Shell UI 代码与真机完全共享——模拟器不做任何 UI 级别的模拟，而是通过注入层（DPIInjector / HardwareMock / InputSimulator）将模拟的屏幕参数、硬件信息、输入事件注入到 base 层。这样模拟器中看到的 UI 效果与真机一致，且切换设备配置时 DPI、主题降级、动画开关等行为即时生效。

触摸可视化（TouchVisualizer）通过涟漪动画显示触摸点位置和轨迹，帮助开发者在鼠标模拟触摸时直观确认输入行为。设备配置以 JSON 文件描述（DeviceProfile），预设覆盖主流嵌入式屏幕，也可自定义扩展。

## 关键决策

| 决策 | 理由 | 被否决的替代方案 |
|------|------|------------------|
| DeviceFrame 包裹式架构（共享 Shell UI 代码） | 模拟器与真机跑同一份 UI 代码，保证一致性，避免两套代码漂移 | 模拟器独立渲染一套简化 UI |
| 注入层 (DPIInjector/HardwareMock/InputSimulator) | 通过注入模拟参数让 base 层"以为"自己在真机上运行，不侵入业务代码 | 在 Shell UI 层到处加 `#ifdef SIMULATOR` 条件编译 |
| JSON 设备配置文件 (DeviceProfile) | 设备参数可热加载、用户可自定义新增设备，无需重编译 | 硬编码设备参数 |
| ControlPanel 运行时切换设备/档位 | 无需重启即可验证不同分辨率和硬件档位下的 UI 表现 | 每次切换需重启模拟器 |
| 触摸涟漪可视化 | 鼠标模拟触摸时没有触觉反馈，涟漪帮助确认点击位置和手势轨迹 | 不做可视化，依赖日志判断 |

## 当前状态

未实现。参见 [status/current.md](../status/current.md) Phase 6 部分。
