---
title: ConfigStore 手册
description: ConfigStore 是 CFDesktop 的四层优先级配置管理系统，支持分层存储、结构化键名管理和变更监听机制。
---

# ConfigStore 手册

ConfigStore 是 CFDesktop 桌面框架的配置管理中心，提供分层存储、结构化键名管理和变更监听机制。基于 Qt 的 QSettings 实现，采用 INI 格式存储配置，支持跨平台使用。

## 四层存储架构

ConfigStore 采用四层优先级架构，支持配置的自然覆盖：

| 层级 | 优先级 | 读写权限 | 路径 (Linux) | 用途 |
|------|--------|----------|---------------|------|
| **Temp** | 最高 (3) | 读写 | 内存 | 临时配置，测试用，不持久化 |
| **App** | 高 (2) | 读写 | `<app>/config/app.ini` | 应用运行时配置 |
| **User** | 中 (1) | 读写 | `~/.config/cfdesktop/user.ini` | 用户个人配置 |
| **System** | 低 (0) | 读写 | `{app_dir}/system.ini` | 系统默认配置 (CFDesktop 自管理目录) |

查询时按优先级从高到低查找，写入时默认写入 App 层。

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

---

*Last updated: 2026-03-20*
