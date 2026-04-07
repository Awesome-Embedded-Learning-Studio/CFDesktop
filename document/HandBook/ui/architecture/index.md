# 架构总览

CFDesktop UI 框架采用五层架构设计，遵循依赖倒置原则：上层依赖抽象接口，下层提供具体实现。

```
┌─────────────────────────────────────────┐
│  Layer 5 · Widget 适配层                │  Button, Checkbox, Slider...
├─────────────────────────────────────────┤
│  Layer 4 · Material 行为层              │  状态机, Ripple, Elevation
├─────────────────────────────────────────┤
│  Layer 3 · 动画引擎层                   │  Timing, Spring, 工厂+策略
├─────────────────────────────────────────┤
│  Layer 2 · 主题引擎层                   │  Token, 配色, 排版, 形状
├─────────────────────────────────────────┤
│  Layer 1 · 数学工具层                   │  HCT 色彩, 几何, 像素适配
└─────────────────────────────────────────┘
```

## 各层详情

- **[第一层 · 数学工具](layer-1-math-utility/01-why-we-need-own-math-layer.md)** -- 为什么需要自己的数学库、HCT 色彩系统、几何与设备像素
- **[第二层 · 主题引擎](layer-2-theme-engine/01-theme-system-design.md)** -- 主题系统设计、Token 系统、配色方案、排版/形状/运动
- **[第三层 · 动画引擎](layer-3-animation-engine/01-animation-architecture.md)** -- 动画架构、Timing 与 Spring 动画、工厂与策略模式
- **[第四层 · Material 行为](layer-4-material-behavior/01-state-machine.md)** -- 状态机、Ripple 与 Elevation、焦点指示器
- **[第五层 · Widget 适配](layer-5-widget-adapter/01-adapter-pattern.md)** -- 适配器模式、Button 深入、绘制管线
