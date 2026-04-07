# ConfigStore API - 单例访问

本文档介绍 ConfigStore 的单例访问和初始化 API。

## 目录

- [instance() - 获取单例](#instance---获取单例)
- [initialize() - 初始化](#initialize---初始化)
- [使用模式](#使用模式)

---

## instance() - 获取单例

获取 ConfigStore 的全局唯一实例。

### 函数签名

```cpp
static ConfigStore& instance();
```

### 返回值

| 类型 | 说明 |
|------|------|
| `ConfigStore&` | ConfigStore 的引用 |

### 单例模式

```
ConfigStore 单例模式:
   ┌─────────────────────────────┐
   │         ConfigStore         │
   │        (单例实例)            │
   │                             │
   │   instance() ──→ 引用       │
   └─────────────────────────────┘
        ▲
        │
   所有访问点通过 instance()
```

### 使用示例

```cpp
using namespace cf::config;

// 基本访问
ConfigStore& store = ConfigStore::instance();

// 链式调用
std::string theme = ConfigStore::instance().query<std::string>(
    KeyView{.group = "app.theme", .key = "name"},
    "default"
);

// 直接调用方法
ConfigStore::instance().set(
    KeyView{.group = "app", .key = "volume"},
    80,
    Layer::App
);

// 同步
ConfigStore::instance().sync(SyncMethod::Async);
```

---

## initialize() - 初始化

使用自定义路径提供者初始化 ConfigStore。

### 函数签名

```cpp
void initialize(std::shared_ptr<IConfigStorePathProvider> path_provider);
```

### 参数说明

| 参数 | 类型 | 说明 |
|------|------|------|
| `path_provider` | `std::shared_ptr<IConfigStorePathProvider>` | 路径提供者 |

### 初始化行为

```
initialize() 执行流程:
   ┌─────────────────────────────┐
   │  首次调用:                   │
   │  1. 保存路径提供者           │
   │  2. 创建存储层级             │
   │  3. 从磁盘加载配置文件       │
   │                             │
   │  后续调用:                   │
   │  ── 忽略（不产生效果）       │
   └─────────────────────────────┘
```

### 重要特性

- **仅首次生效**：第一次调用后的所有调用会被忽略
- **线程安全**：多线程环境下安全
- **可选**：不调用时使用默认路径

### 使用示例

```cpp
using namespace cf::config;

// 场景1: 使用默认路径
// 无需调用 initialize()，直接使用 instance()
auto& store = ConfigStore::instance();

// 场景2: 自定义路径（测试环境）
class TestPathProvider : public IConfigStorePathProvider {
  public:
    std::filesystem::path system_config() const override {
        return "/tmp/test_config/system.conf";
    }
    std::filesystem::path user_config() const override {
        return "/tmp/test_config/user.conf";
    }
    std::filesystem::path app_config() const override {
        return "/tmp/test_config/app.conf";
    }
};

void setup_test_environment() {
    auto test_provider = std::make_shared<TestPathProvider>();
    ConfigStore::instance().initialize(test_provider);
}

// 场景3: 便携式应用路径
class PortablePathProvider : public IConfigStorePathProvider {
  public:
    PortablePathProvider(const std::filesystem::path& base) : base_(base) {}

    std::filesystem::path system_config() const override {
        return base_ / "default.conf";
    }
    std::filesystem::path user_config() const override {
        return base_ / "user.conf";
    }
    std::filesystem::path app_config() const override {
        return base_ / "app.conf";
    }
  private:
    std::filesystem::path base_;
};

void setup_portable_mode(const std::filesystem::path& app_dir) {
    auto portable_provider = std::make_shared<PortablePathProvider>(app_dir);
    ConfigStore::instance().initialize(portable_provider);
}
```

---

## IConfigStorePathProvider 接口

路径提供者接口，用于自定义配置文件存储位置。

### 接口定义

```cpp
class IConfigStorePathProvider {
  public:
    virtual ~IConfigStorePathProvider() = default;

    // 系统默认配置路径
    virtual std::filesystem::path system_config() const = 0;

    // 用户配置路径
    virtual std::filesystem::path user_config() const = 0;

    // 应用配置路径
    virtual std::filesystem::path app_config() const = 0;
};
```

### 实现示例

```cpp
// Linux 标准路径实现
class LinuxPathProvider : public IConfigStorePathProvider {
  public:
    std::filesystem::path system_config() const override {
        return "/etc/cfdesktop/config/system.conf";
    }

    std::filesystem::path user_config() const override {
        if (const char* home = std::getenv("HOME")) {
            return std::filesystem::path(home) / ".config/cfdesktop/user.conf";
        }
        return ".config/cfdesktop/user.conf";
    }

    std::filesystem::path app_config() const override {
        if (const char* home = std::getenv("HOME")) {
            return std::filesystem::path(home) / ".config/cfdesktop/app.conf";
        }
        return ".config/cfdesktop/app.conf";
    }
};

// Windows 实现略...
// macOS 实现略...
```

---

## 使用模式

### 模式1: 默认初始化

```cpp
using namespace cf::config;

// 应用启动
int main() {
    // 直接使用，自动初始化为默认路径
    ConfigStore& store = ConfigStore::instance();

    // 正常使用
    std::string theme = store.query<std::string>(
        KeyView{.group = "app.theme", .key = "name"},
        "default"
    );

    return run_app();
}
```

### 模式2: 提前初始化

```cpp
using namespace cf::config;

// 应用启动时显式初始化
int main(int argc, char* argv[]) {
    // 解析命令行参数
    std::string config_dir = parse_config_dir(argc, argv);

    // 创建路径提供者
    auto provider = std::make_shared<CustomPathProvider>(config_dir);

    // 初始化
    ConfigStore::instance().initialize(provider);

    // 后续使用
    return run_app();
}
```

### 模式3: 测试环境初始化

```cpp
using namespace cf::config;

class ConfigTest : public ::testing::Test {
  protected:
    void SetUp() override {
        // 每个测试使用独立的临时目录
        test_dir_ = create_temp_dir();
        auto test_provider = std::make_shared<TestPathProvider>(test_dir_);

        // 重置并初始化（注意：仅在首次有效）
        ConfigStore::instance().initialize(test_provider);
    }

    void TearDown() override {
        // 清理临时目录
        cleanup_temp_dir(test_dir_);
    }

    std::string test_dir_;
};

TEST_F(ConfigTest, BasicOperations) {
    // 使用干净的配置存储进行测试
    ConfigStore::instance().set(
        KeyView{.group = "test", .key = "value"},
        42,
        Layer::App
    );

    int result = ConfigStore::instance().query<int>(
        KeyView{.group = "test", .key = "value"},
        0
    );

    EXPECT_EQ(result, 42);
}
```

### 模式4: 延迟初始化

```cpp
using namespace cf::config;

class ConfigManager {
  public:
    static ConfigManager& get() {
        static ConfigManager instance;
        return instance;
    }

    void init(const std::string& config_path) {
        auto provider = std::make_shared<CustomPathProvider>(config_path);
        ConfigStore::instance().initialize(provider);
        initialized_ = true;
    }

    ConfigStore& store() {
        if (!initialized_) {
            throw std::runtime_error("ConfigManager not initialized");
        }
        return ConfigStore::instance();
    }
  private:
    ConfigManager() = default;
    bool initialized_ = false;
};

// 使用
int main() {
    ConfigManager::get().init("/custom/path");
    auto& store = ConfigManager::get().store();
    // ...
}
```

---

## 初始化时序

```
推荐时序:
   ┌─────────────────────────────────────────────────────┐
   │                                                     │
   │   1. main() 启动                                    │
   │      ↓                                             │
   │   2. 解析命令行/确定配置路径                         │
   │      ↓                                             │
   │   3. ConfigStore::instance().initialize(provider)  │
   │      ↓                                             │
   │   4. ConfigStore::instance().query/set/...         │
   │                                                     │
   └─────────────────────────────────────────────────────┘

错误时序:
   ┌─────────────────────────────────────────────────────┐
   │                                                     │
   │   1. ConfigStore::instance().query() ──→ 使用默认路径│
   │      ↓                                             │
   │   2. ConfigStore::instance().initialize(provider)  │
 │      ↓                                             │
   │   3. ❌ 初始化被忽略，配置仍在默认路径               │
   │                                                     │
   └─────────────────────────────────────────────────────┘
```

---

## 完整应用示例

```cpp
using namespace cf::config;

class Application {
  public:
    int run(int argc, char* argv[]) {
        // 1. 解析参数
        if (!parse_args(argc, argv)) {
            return 1;
        }

        // 2. 初始化配置
        init_config();

        // 3. 注册监听器
        setup_watchers();

        // 4. 加载 UI
        create_ui();

        // 5. 运行事件循环
        return event_loop_.exec();
    }

  private:
    void init_config() {
        std::shared_ptr<IConfigStorePathProvider> provider;

        if (portable_mode_) {
            // 便携模式：使用程序目录
            provider = std::make_shared<PortablePathProvider>(
                get_app_dir()
            );
        } else {
            // 标准模式：使用系统路径
            provider = std::make_shared<StandardPathProvider>();
        }

        ConfigStore::instance().initialize(provider);

        // 确保基础配置存在
        ensure_default_config();
    }

    void setup_watchers() {
        ConfigStore::instance().watch(
            "app.theme.*",
            [this](const Key& k, const std::any* old, const std::any* neu, Layer) {
                this->on_theme_changed(k, old, neu);
            }
        );
    }

    void ensure_default_config() {
        // 检查是否首次运行
        if (!ConfigStore::instance().has_key(
                KeyView{.group = "app", .key = "initialized"})) {
            setup_default_config();
            ConfigStore::instance().set(
                KeyView{.group = "app", .key = "initialized"},
                true,
                Layer::App
            );
            ConfigStore::instance().sync(SyncMethod::Sync);
        }
    }

    bool portable_mode_ = false;
    EventLoop event_loop_;
};

int main(int argc, char* argv[]) {
    Application app;
    return app.run(argc, argv);
}
```

---

## API 参考

### instance()

```cpp
static ConfigStore& instance();
```

| 项目 | 说明 |
|------|------|
| 功能 | 获取 ConfigStore 单例引用 |
| 返回值 | ConfigStore& |
| 线程安全 | 是 |
| 首次调用 | 自动使用默认路径初始化 |

### initialize()

```cpp
void initialize(std::shared_ptr<IConfigStorePathProvider> path_provider);
```

| 项目 | 说明 |
|------|------|
| 功能 | 设置自定义配置文件路径 |
| 参数 | path_provider - 路径提供者 |
| 首次调用 | 设置路径并加载配置 |
| 后续调用 | 忽略，不产生效果 |
| 线程安全 | 是 |

---

## 下一篇

- [返回 Config Manager](./)
