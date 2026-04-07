# UI 框架

CFDesktop 的 UI 框架是一套基于 Material Design 3 规范的完整实现，采用五层架构设计，每一层职责明确、可独立测试。

## 架构概览

| 层级 | 名称 | 职责 |
|------|------|------|
| 第一层 | [数学工具](architecture/layer-1-math-utility/01-why-we-need-own-math-layer.md) | HCT 色彩空间、几何计算、设备像素适配 |
| 第二层 | [主题引擎](architecture/layer-2-theme-engine/01-theme-system-design.md) | Token 系统、配色方案、排版/形状/运动 |
| 第三层 | [动画引擎](architecture/layer-3-animation-engine/01-animation-architecture.md) | Timing/Spring 动画、工厂+策略模式 |
| 第四层 | [Material 行为](architecture/layer-4-material-behavior/01-state-machine.md) | 状态机、Ripple、Elevation、焦点 |
| 第五层 | [Widget 适配](architecture/layer-5-widget-adapter/01-adapter-pattern.md) | Qt Widget 适配器、绘制管线 |

## 子模块

- **[基础类型](base/color.md)**: 颜色、设备像素、缓动函数、几何工具、数学工具
- **[核心模块](core/theme.md)**: 主题管理、配色方案、Token 系统
- **[组件系统](components/animation.md)**: 动画组件、动画组、Spring/Timing 动画
- **[Material Widget](material/widget/button.md)**: Button、Checkbox、Slider、Switch 等 20+ 控件
- **[应用层](application/application.md)**: 顶层 Application 集成
