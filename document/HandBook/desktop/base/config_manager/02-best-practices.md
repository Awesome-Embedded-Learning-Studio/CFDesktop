---
title: ConfigStore 最佳实践
description: ConfigStore 提供四层存储架构，每一层都有其特定的使用场景：
---

# ConfigStore 最佳实践

| 项目 | 内容 |
|------|------|
| 文档版本 | v1.0 |
| 所属模块 | cf::config (ConfigStore) |
| 前置知识 | 快速入门指南 (01-quick-start.md) |

---

## 一、键命名规范

### 键名格式

```text
[level1].[level2].[level3].[level4]
   |        |        |        |
   |        |        |        +-- 具体属性名 (必需)
   |        +----------- 子模块 (可选)
   +-------------------- 功能模块 (必需)
+----------------------------- 命名空间/域 (必需)
```

### 命名规则

| 规则 | 说明 | 正确示例 | 错误示例 |
|------|------|----------|----------|
| 小写字母 | 所有层级使用小写 | `app.theme.name` | `App.Theme.Name` |
| 点分隔 | `.` 作为层级分隔符 | `ui.window.width` | `ui/window/width` |
| 语义清晰 | 使用描述性名称，避免缩写 | `max_file_size` | `mfs` |
| 下划线连接 | 多词属性用 `_` 连接 | `dark_mode` | `darkMode` |
| 避免数字 | 除非是版本或编号 | `schema_v2` | `style1` |
| 全英文 | 键名使用英文 | `background_color` | `背景颜色` |

### 命名空间预留

```text
app.*          # 应用程序级配置
ui.*           # 用户界面配置
editor.*       # 编辑器配置
network.*      # 网络配置
database.*     # 数据库配置
logger.*       # 日志配置
```

在代码中定义常量以避免拼写错误：

```cpp
namespace ConfigKeys {
    constexpr std::string_view APP_PREFIX = "app";
    inline constexpr KeyView APP_THEME_NAME{.group = "app.theme", .key = "name"};
    inline constexpr KeyView UI_WINDOW_WIDTH{.group = "ui.window", .key = "width"};
}
```

---

## 二、层级使用策略

### 层级优先级

```text
+-----------------------------+
|    Temp 层 (优先级 3)        |  内存临时数据，进程重启后丢失
+-----------------------------+
                    |  覆盖
+-----------------------------+
|    App 层 (优先级 2)         |  应用运行时配置，会话有效
+-----------------------------+
                    |  覆盖
+-----------------------------+
|    User 层 (优先级 1)        |  用户个人偏好，跨会话持久化
+-----------------------------+
                    |  覆盖
+-----------------------------+
|    System 层 (优先级 0)      |  系统默认配置，管理员维护
+-----------------------------+
```

### 各层使用规则

| 层级 | 持久化 | 谁可修改 | 存储内容 |
|------|--------|----------|----------|
| **System** | 是 | 安装程序/管理员 | 默认配置值、系统级限制（最大文件大小）、硬件相关默认设置、部署环境配置 |
| **User** | 是 | 最终用户 | 主题/外观设置、语言/区域设置、用户习惯（窗口大小/位置）、功能开关 |
| **App** | 是 | 应用程序 | 窗口几何状态、最近打开文件列表、会话恢复数据、应用内部状态 |
| **Temp** | 否 | 应用程序/测试代码 | 单元测试临时配置、功能预览模式、会话令牌、调试标志 |

### 层级选择决策表

| 场景 | 推荐层级 |
|------|---------|
| 应用默认值 / 系统限制 | System |
| 用户偏好 / 功能开关 | User |
| 窗口状态 / 会话数据 | App |
| 测试数据 / 会话令牌 | Temp |

### 各层代码示例

```cpp
// System 层：系统默认（安装时或首次运行时设置）
store.register_key(Key{.full_key = "file.max_size_mb",
                       .full_description = "Max file size in MB"}, 100, Layer::System);

// User 层：用户偏好
store.set(KeyView{.group = "user.theme", .key = "dark_mode"}, true, Layer::User);
store.set(KeyView{.group = "user.interface", .key = "language"}, "zh_CN", Layer::User);
store.sync(SyncMethod::Async);

// App 层：运行时状态
store.set(KeyView{.group = "app.window", .key = "width"}, geometry.width(), Layer::App);
store.set(KeyView{.group = "app.window", .key = "maximized"}, window->isMaximized(), Layer::App);

// Temp 层：临时数据（不持久化）
store.set(KeyView{.group = "debug", .key = "enabled"}, true, Layer::Temp);
```

---

## 三、性能优化

### 缓存利用

| 做法 | 推荐/不推荐 | 说明 |
|------|-------------|------|
| 优先级查询（省略 Layer 参数） | 推荐 | 自动返回最高优先级值 |
| 手动遍历各层查询 | 不推荐 | 冗余且低效 |
| 批量读取配置到结构体 | 推荐 | 减少 API 调用次数 |
| 在高频函数中重复 `query()` | 不推荐 | 缓存值 + 监听变化更优 |

### 批量操作

```cpp
// 使用 Manual 策略批量修改，一次性触发 Watcher
NotifyPolicy manual = NotifyPolicy::Manual;
store.set(KeyView{.group = "ui", .key = "theme"}, "dark", Layer::User, manual);
store.set(KeyView{.group = "ui", .key = "font_size"}, 14, Layer::User, manual);
store.set(KeyView{.group = "ui", .key = "scale"}, 1.25, Layer::User, manual);
store.notify();
store.sync(SyncMethod::Async);
```

### 异步持久化

频繁写入时使用 `SyncMethod::Async` 避免阻塞主线程；应用退出时使用 `SyncMethod::Sync` 确保数据落盘。

```cpp
// 运行时：延迟异步保存
ConfigStore::instance().sync(SyncMethod::Async);

// 退出时：立即同步保存
ConfigStore::instance().sync(SyncMethod::Sync);
```

---

## 四、线程安全

### 规则速查

| 规则 | 说明 |
|------|------|
| 读并发安全 | `query()` 内部使用 `shared_lock`，多线程可并发读取 |
| 禁止在 Watcher 回调中调用 `set()` | 可能导致死锁；改用原子标志位延迟处理 |
| 持有外部锁时避免调用 ConfigStore | 先释放外部锁，再调用 ConfigStore |
| Watcher 回调应轻量 | 提取信息后入队，由主线程或专用线程处理 |

### 典型错误与修正

```cpp
// 危险：在 Watcher 回调中再次写入
auto handle = store.watch("app.theme", [](const Key&, auto, auto, Layer) {
    ConfigStore::instance().set(KeyView{.group = "app", .key = "changed"}, true); // 死锁!
});

// 安全：使用原子标志延迟处理
std::atomic<bool> theme_changed{false};
auto handle = store.watch("app.theme", [&theme_changed](const Key&, auto, auto, Layer) {
    theme_changed.store(true);
});
```

---

## 五、错误处理

| 策略 | 说明 |
|------|------|
| 提供默认值 | `query<int>(kv, default_value)` 在键不存在时返回默认值 |
| 范围校验 | 对读取的值做 min/max 裁剪，防止配置文件被手动篡改导致异常 |
| 链式降级 | 先尝试主键，再尝试备用键，最后用硬编码默认值 |

```cpp
// 带范围校验的读取
int timeout = store.query<int>(KeyView{.group = "network", .key = "timeout"}, 1000);
timeout = std::clamp(timeout, 100, 60000);
```

---

## 六、实践模式速查

| 模式 | 用途 | 关键思路 |
|------|------|----------|
| 配置管理器封装 | 类型安全的配置访问 | 单例 + 结构体封装 get/set，集中管理键名和默认值 |
| 热重载 | 开发环境监听配置文件变化 | `QFileSystemWatcher` + 防抖定时器 + `ConfigStore::reload()` |
| 配置迁移 | 版本升级时数据迁移 | 递增版本号 + `migrate_key()` 逐键迁移 + `NotifyPolicy::Manual` |
| 多实例隔离 | 测试或多用户场景 | 自定义 `IConfigStorePathProvider`，为每个实例提供独立文件路径 |

---

## 附录：快速参考

### 层级优先级速查

| 层级 | 优先级 | 持久化 | 用途 |
|------|--------|--------|------|
| Temp | 3 (最高) | 否 | 临时数据、测试 |
| App | 2 | 是 | 运行时状态 |
| User | 1 | 是 | 用户偏好 |
| System | 0 (最低) | 是 | 系统默认 |

### 常用代码片段

```cpp
// 设置用户偏好
ConfigStore::instance().set(KeyView{.group = "user.theme", .key = "name"},
                            "dark", Layer::User);

// 读取带默认值
auto theme = ConfigStore::instance().query<std::string>(
    KeyView{.group = "user.theme", .key = "name"}, "default");

// 批量修改
store.set(..., Layer::User, NotifyPolicy::Manual);
store.notify();
store.sync(SyncMethod::Async);

// 监听变化
auto handle = store.watch("user.theme.*", [](auto... args) { /* 处理变化 */ });
store.unwatch(handle);
```

### 检查清单

- 查询时是否提供合理的默认值？
- 类型转换是否考虑了失败情况？
- 键名是否进行了验证？
- Watcher 回调是否避免再次调用 ConfigStore？
- 是否使用了异步持久化避免阻塞？
- 多线程场景是否考虑了锁的顺序？

---

**维护者：** CFDesktop 团队
