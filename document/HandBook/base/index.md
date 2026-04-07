# 基础工具库

零 Qt 依赖的跨平台工具集。设计原则：标准库特性的降级实现和便利封装，方便在无 Qt 环境下复用。

[阅读总览](overview.md)

## 工具列表

| 工具 | 说明 |
|------|------|
| [expected](expected.md) | 函数式错误处理 (`std::expected` 降级) |
| [span](span.md) | 非拥有视图 (`std::span` 降级) |
| [singleton](singleton.md) | 线程安全的单例模式 |
| [scope_guard](scope_guard.md) | RAII 风格的资源管理 |
| [factory](factory.md) | 工厂模式工具 |
| [macros](macros.md) | 便利宏集合 |
| [hash](hash.md) | 哈希工具 |
| [mpsc_queue](mpsc_queue.md) | 多生产者单消费者队列 |
| [once_init](once_init.md) | 一次初始化 |
| [policy_chain](policy_chain.md) | 策略链 |
| [weak_ptr](weak_ptr.md) | 弱引用 |
| [weak_ptr_factory](weak_ptr_factory.md) | 弱引用工厂 |
