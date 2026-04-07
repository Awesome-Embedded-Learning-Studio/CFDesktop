# ConfigStore - 配置管理中心

## 简介

ConfigStore 是 CFDesktop 桌面框架的配置管理中心，提供分层存储、结构化键名管理和变更监听机制。它基于 Qt 的 QSettings 实现，采用 INI 格式存储配置，支持跨平台使用。

## 核心特性

### 四层存储架构

ConfigStore 采用四层优先级架构，支持配置的自然覆盖：

| 层级 | 优先级 | 读写权限 | 路径 (Linux) | 用途 |
|------|--------|----------|---------------|------|
| **Temp** | 最高 (3) | 读写 | 内存 | 临时配置，测试用，不持久化 |
| **App** | 高 (2) | 读写 | `<app>/config/app.ini` | 应用运行时配置 |
| **User** | 中 (1) | 读写 | `~/.config/cfdesktop/user.ini` | 用户个人配置 |
| **System** | 低 (0) | 读写 | `/etc/cfdesktop/system.ini` | 系统默认配置 |

查询时按优先级从高到低查找，写入时默认写入 App 层。

### 类型安全

通过模板 API 提供类型安全的配置访问：

```cpp
// 查询配置
int width = ConfigStore::instance().query<int>(
    KeyView{.group = "ui", .key = "width"}, 800);

// 设置配置
ConfigStore::instance().set(
    KeyView{.group = "ui", .key = "width"}, 1024);
```

支持的基础类型：
- `int` / `unsigned` / `long` 等整数类型
- `double` / `float` 浮点类型
- `bool` 布尔类型
- `std::string` 字符串类型

### 变更监听

Watcher 系统支持通配符模式匹配的变更监听：

```cpp
// 监听所有 ui.* 的变更
auto handle = ConfigStore::instance().watch(
    "ui.*",
    [](const Key& k, const std::any* old, const std::any* new_v, Layer layer) {
        std::cout << "UI 配置变更: " << k.full_key << std::endl;
    }
);
```

支持两种通知策略：
- **Immediate**: 每次变更立即触发 Watcher
- **Manual**: 需要手动调用 `notify()` 触发

### 线程安全

ConfigStore 采用读写锁设计，确保多线程环境下的安全访问：

- **读操作**: 使用 `shared_lock`，支持多线程并发读取
- **写操作**: 使用 `unique_lock`，独占访问
- **延迟回调**: Watcher 回调在无锁状态下执行，避免死锁

### 高性能

- **内存缓存**: 热点数据缓存在内存中，减少 I/O
- **异步持久化**: 配置写入操作异步执行，不阻塞调用方
- **脏标记**: 只持久化实际修改的层，减少 I/O 量

## 快速示例

```cpp
#include "cfconfig.hpp"

using namespace cf::config;

int main() {
    // 获取单例实例
    auto& config = ConfigStore::instance();

    // 读取配置（使用默认值）
    std::string theme = config.query<std::string>(
        KeyView{.group = "app", .key = "theme"}, "default");

    // 写入配置（默认写入 App 层）
    config.set(KeyView{.group = "app", .key = "theme"}, std::string("dark"));

    // 监听变更
    config.watch("app.*", [](const Key& k, auto old, auto new_v, Layer layer) {
        std::cout << "配置变更: " << k.full_key << std::endl;
    });

    // 同步到磁盘
    config.sync(SyncMethod::Async);

    return 0;
}
```

## 文档导航

| 文档 | 描述 |
|------|------|
| [快速入门](./01-quick-start.md) | 从零开始使用 ConfigStore |
| [最佳实践](./02-best-practices.md) | 推荐的使用模式和设计建议 |
| [常见问题](./03-faq.md) | 问题诊断和故障排查 |
| [架构详解](./04-architecture.md) | 深入理解内部实现和设计决策 |

## 代码示例

运行示例：

```bash
cd build/example/desktop/base/config_manager
./example_usage
```

## 依赖模块

- **Qt 6.8+**: 提供 QSettings 后端存储
- **cfbase**: 提供 SimpleSingleton 基类

## 相关链接

- **API 参考手册**: [document/desktop/base/config_manager/](../../../../../document/desktop/base/config_manager/)
