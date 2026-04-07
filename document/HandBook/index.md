# 开发手册

CFDesktop 开发手册涵盖所有模块的 API 文档、架构设计、使用指南和平台实现细节。

## 模块导航

### 基础工具库
零 Qt 依赖的跨平台工具集：`expected<T,E>` 错误处理、`span` 视图、RAII 守卫、单例模式、工厂模式等。
[浏览基础工具库](base/overview.md)

### UI 框架
基于 Material Design 3 的五层架构实现，从底层数学工具到顶层 Widget 适配。
[了解架构设计](ui/architecture/index.md) | [查看组件](ui/components/index.md) | [Material Widget](ui/material/index.md)

### 桌面模块
Desktop Shell 的基础组件，包括配置管理器和日志系统。
[浏览桌面模块](desktop/index.md)

### API 参考
Doxygen 自动生成的 C++ API 文档，覆盖系统探针、硬件信息等模块。
[查看 API](api/index.md)

### 平台实现
Linux 和 Windows 平台特定的实现细节。
[查看实现](implementation/index.md)

### 示例代码
常见使用场景的代码示例。
[查看示例](examples/cpu_info_example.md)
