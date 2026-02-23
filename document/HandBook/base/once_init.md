# CallOnceInit - 懒加载初始化

## 简介

`CallOnceInit<T>` 是一个线程安全的懒加载模板，用于实现资源的延迟初始化。它确保资源只在首次访问时初始化一次，并支持强制重新初始化。

## 头文件

```cpp
#include "base/helpers/once_init.hpp"
```

## 特性

- **线程安全**：使用 `std::call_once` 确保初始化只执行一次
- **懒加载**：首次访问时才初始化
- **可重置**：支持强制重新初始化
- **虚拟接口**：通过派生类自定义初始化逻辑

## 基本用法

### 定义派生类

```cpp
#include "base/helpers/once_init.hpp"

class CPUInfoCache : public cf::CallOnceInit<CPUInfo> {
protected:
    // 首次初始化时调用
    bool init_resources() override {
        // 执行实际的初始化逻辑
        resource.model = queryModel();
        resource.arch = queryArch();
        resource.manufacturer = queryManufacturer();
        return true;
    }

    // 强制重新初始化时调用
    bool force_do_reinit() override {
        // 重新执行初始化逻辑
        return init_resources();
    }
};
```

### 使用

```cpp
// 全局实例
static CPUInfoCache g_cpu_cache;

// 首次调用会执行初始化
auto& info1 = g_cpu_cache.get_resources();

// 后续调用返回缓存的数据
auto& info2 = g_cpu_cache.get_resources();

// 强制重新初始化
g_cpu_cache.force_reinit();
```

## 成员函数

### get_resources()

```cpp
Resources& get_resources();
```

获取资源引用，首次调用时执行初始化。

**返回值**：资源的引用

### force_reinit()

```cpp
void force_reinit();
```

强制重新初始化资源。

**注意**：此函数不是线程安全的，与 `get_resources()` 并发调用时需要外部同步。

### force_reinit(Args&&... args)

```cpp
template <typename... Args>
void force_reinit(Args&&... args);
```

用新参数重建资源。

## 使用场景

### 1. 缓存系统信息

```cpp
class SystemInfoCache : public cf::CallOnceInit<SystemInfo> {
protected:
    bool init_resources() override {
        resource.cpu_count = std::thread::hardware_concurrency();
        resource.total_memory = get_total_memory();
        return true;
    }

    bool force_do_reinit() override {
        return init_resources();
    }
};

// 使用
SystemInfoCache sys_info;
auto& info = sys_info.get_resources();
std::cout << "CPU 核心数: " << info.cpu_count << std::endl;
```

### 2. 延迟加载配置

```cpp
class ConfigCache : public cf::CallOnceInit<Config> {
protected:
    bool init_resources() override {
        resource = load_config_file("/etc/app/config.json");
        return resource.valid();
    }

    bool force_do_reinit() override {
        // 重新加载配置文件
        resource = load_config_file("/etc/app/config.json");
        return resource.valid();
    }
};

// 使用
ConfigCache config;
if (config.get_resources().debug_mode) {
    // ...
}
```

### 3. 单例模式

```cpp
class Database : public cf::CallOnceInit<Database> {
public:
    void query(const std::string& sql) {
        auto& db = get_resources();
        // 执行查询...
    }

protected:
    bool init_resources() override {
        resource.connect("localhost", 5432);
        return true;
    }

    bool force_do_reinit() override {
        resource.disconnect();
        resource.connect("localhost", 5432);
        return true;
    }
};

// 使用
Database::instance().query("SELECT * FROM users");
```

## 线程安全

- `get_resources()` 是线程安全的，多线程同时首次调用会安全地等待初始化完成
- `force_reinit()` 不是线程安全的，需要外部同步

```cpp
// 不安全的并发操作
std::thread t1([&]() { cache.force_reinit(); });  // 危险!
std::thread t2([&]() { cache.get_resources(); });

// 正确的做法
std::mutex mtx;
std::lock_guard<std::mutex> lock(mtx);
cache.force_reinit();
```

## 注意事项

1. **虚析构函数**：`CallOnceInit` 没有虚析构函数，避免多态删除

2. **初始化失败**：如果 `init_resources()` 返回 false，下次调用 `get_resources()` 会重试

3. **资源释放**：析构时不会自动释放资源，需要手动管理

## 相关文档

- [ScopeGuard - 资源管理](./scope_guard.md)
- [基础工具类概述](./overview.md)
