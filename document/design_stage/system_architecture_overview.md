---
title: CFDesktop 系统架构总览
description: "三层单向依赖架构、平台抽象层、DAG 初始化链的设计意图"
---

# CFDesktop 系统架构总览 -- 设计意图

## 为什么选择这种方案

CFDesktop 的核心挑战是在 Windows 客户端、Linux X11/Wayland 合成器、嵌入式直接渲染等截然不同的运行场景中复用同一套 Shell / Panel / WindowManager 代码。为此，系统采用 **base -> ui -> desktop 三层单向依赖架构**：底层 `base` 提供硬件探测与基础工具，中层 `ui` 构建完整的 Material Design 3 设计系统（颜色、排版、形状、动效），顶层 `desktop` 组合两者实现桌面环境。严格单向依赖（`desktop` 可依赖 `ui` 和 `base`，反之不可）确保底层模块不感知上层业务，使每层可独立测试、独立替换。

平台差异通过 **Factory + Strategy 双模式抽象层** 屏蔽。`DisplayServerBackendFactory` 负责创建平台特定的显示后端对象，`IDesktopDisplaySizeStrategy` 封装平台特定的窗口行为策略。每个平台只需实现两个分发函数（`native_impl()` 和 `native_display()`），公共 Helper 统一调度，新增平台无需修改已有代码。

初始化采用 **DAG 阶段链**（`RunEarlyInit` -> `RunStageInit` -> `boot_desktop` -> `exec`），而非扁平的 init 列表。阶段之间有明确的数据依赖：早期初始化建立日志和配置，阶段初始化完成系统检测和资源预加载，最后才创建 Qt Widget 和平台后端。`CFDesktopEntity` 作为中央单例管理整个生命周期，其 `release()` 必须在 `QApplication` 存活时调用（内部持有 QWidget 实例）。

## 关键决策

| 决策 | 理由 | 被否决的替代方案 |
|------|------|------------------|
| 三层单向依赖（base/ui/desktop） | 防止循环依赖，每层可独立编译测试，底层不感知上层业务 | 扁平单层结构（耦合严重，无法独立测试） |
| Factory + Strategy 平台抽象 | 新增平台只实现接口，不修改已有代码；运行时可查询能力做降级 | 大量 `#ifdef` 平台分支（维护困难，违反开闭原则） |
| UI 五层流水线（数学->主题->动画->行为->控件） | MD3 设计系统本身分层清晰，逐层构建避免跨层抽象泄漏 | 单体 Widget 直接硬编码样式（无法换肤，无法复用行为） |
| DAG 阶段初始化链 | 阶段间有硬数据依赖（日志先于检测，检测先于 Widget 创建），错误可精确定位到阶段 | 随意顺序初始化（隐式依赖导致启动失败难以调试） |
| `--whole-archive` 共享库构建 | 工厂自动注册函数可能不被直接引用，whole-archive 确保符号不丢失 | 手动注册每个工厂（易遗漏，违背自动化设计） |
| `CFDesktopEntity` 单例生命周期管理 | 集中管理 Widget/后端/策略的析构顺序，保证 Qt 对象树正确销毁 | 分散在各模块各自清理（销毁顺序不可控，UAF 风险） |

## 模块层次

```text
desktop/ (Layer 3)  -- Shell, Panel, 后端, 初始化
    |
ui/      (Layer 2)  -- MD3 组件, 主题, 动画, 20+ Widget
    |
base/    (Layer 1)  -- CPU/GPU/Memory 探测, 工具库, 工厂基础设施
    |
Qt 6 / OS API
```

## 当前状态

已实现。详细接口规格参见 HandBook，构建产物与 CMake 目标参见 CLAUDE.md 模块地图。
