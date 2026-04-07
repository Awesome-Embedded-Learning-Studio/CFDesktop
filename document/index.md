# CFDesktop

**跨平台桌面环境框架** -- 基于 Qt 的 Material Design 3 实现

---

CFDesktop 是一套面向嵌入式和桌面场景的 UI 框架，包含完整的硬件探针层、基础工具库、五层 Material Design 3 架构，以及桌面 Shell 基础设施。

## 快速导航

<div class="grid cards" markdown>

-   :material-book-open-page-variant:{ .lg .middle } **开发手册**

    ---

    API 参考、组件文档、架构详解、平台实现指南

    [:octicons-arrow-right-24: 浏览手册](HandBook/index.md)

-   :material-developer-board:{ .lg .middle } **开发指南**

    ---

    环境搭建、构建系统、开发工具、Docker、Git Hooks

    [:octicons-arrow-right-24: 开始开发](development/01_prerequisites.md)

-   :material-palette:{ .lg .middle } **UI 框架**

    ---

    五层架构设计：数学工具 → 主题引擎 → 动画引擎 → Material 行为 → Widget 适配

    [:octicons-arrow-right-24: 了解架构](HandBook/ui/architecture/index.md)

-   :material-api:{ .lg .middle } **API 参考**

    ---

    Doxygen 自动生成的 C++ API 文档

    [:octicons-arrow-right-24: 查看 API](HandBook/api/index.md)

-   :material-clipboard-check:{ .lg .middle } **项目进度**

    ---

    TODO 看板、里程碑追踪、已完成状态

    [:octicons-arrow-right-24: 查看进度](todo/index.md)

-   :material-notebook:{ .lg .middle } **技术笔记**

    ---

    设计决策、架构分析、模式实战

    [:octicons-arrow-right-24: 阅读笔记](notes/index.md)

</div>

## 项目概况

| 项目 | 说明 |
|------|------|
| 语言 | C++17 / CMake |
| UI 框架 | Qt 6 + Material Design 3 |
| 目标平台 | Linux (X11/Wayland)、Windows、Embedded |
| 构建系统 | CMake + CI/CD (GitHub Actions) |
| 文档 | MkDocs Material + Doxygen |
