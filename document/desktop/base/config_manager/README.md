# ConfigStore API 参考手册

完整的 ConfigStore API 参考。

## 目录

### 类型定义
| 文档 | 描述 |
|------|------|
| [核心类型](./02-api-types.md) | Layer、NotifyPolicy、KeyView、Key、Watcher 等类型定义 |

### API 分类
| 文档 | 描述 |
|------|------|
| [单例访问](./07-api-singleton.md) | instance()、initialize() |
| [查询操作](./03-api-query.md) | query()、has_key() |
| [写入操作](./04-api-write.md) | set()、register_key()、unregister_key()、clear() |
| [监听操作](./05-api-watch.md) | watch()、unwatch()、notify() |
| [持久化操作](./06-api-persist.md) | sync()、reload() |

## 相关文档

- [使用手册](../../HandBook/desktop/base/config_manager/) - 快速入门和最佳实践
