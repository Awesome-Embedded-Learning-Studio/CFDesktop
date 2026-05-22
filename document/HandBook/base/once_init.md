---
title: "CallOnceInit - 懒加载初始化"
description: 是线程安全的懒加载模板，资源只在首次访问时初始化一次。我们用它来缓存那些获取开销大但不会变化的数据—
---

# CallOnceInit - 懒加载初始化

`CallOnceInit<T>` 是线程安全的懒加载模板，资源只在首次访问时初始化一次。我们用它来缓存那些获取开销大但不会变化的数据——比如 CPU 型号、内存大小这些系统信息。

## 基本用法

继承 `CallOnceInit<资源类型>`，然后重写 `init_resources()` 方法实现初始化逻辑：

```cpp
#include "base/helpers/once_init.hpp"

class CPUInfoCache : public cf::CallOnceInit<CPUInfo> {
protected:
    bool init_resources() override {
        // 执行实际的初始化
        resource.model = queryModel();
        resource.arch = queryArch();
        resource.manufacturer = queryManufacturer();
        return true;  // 返回 true 表示初始化成功
    }

    bool force_do_reinit() override {
        // 强制重新初始化时的逻辑
        return init_resources();
    }
};
```text

首次调用 `get_resources()` 时，`init_resources()` 会被自动调用。后续调用直接返回缓存的资源，不会再执行初始化。

## 使用示例

创建全局或静态实例，然后通过 `get_resources()` 访问：

```cpp
static CPUInfoCache g_cpu_cache;

void print_cpu_info() {
    // 首次调用会执行初始化
    auto& info = g_cpu_cache.get_resources();

    std::cout << "CPU: " << info.model << std::endl;
    std::cout << "架构: " << info.arch << std::endl;

    // 后续调用返回缓存，不会重复查询
    auto& info2 = g_cpu_cache.get_resources();
    assert(&info == &info2);  // 同一个对象
}
```text

## 重新初始化

如果需要强制刷新缓存，调用 `force_reinit()`：

```cpp
// 强制重新初始化
g_cpu_cache.force_reinit();
```text

带参数的版本可以在重新初始化时传入新参数：

```cpp
class ConfigCache : public cf::CallOnceInit<Config> {
protected:
    bool init_resources() override {
        resource = load_default_config();
        return true;
    }

    bool force_do_reinit() override {
        // 使用新参数重新加载
        resource = load_config(new_config_path);
        return true;
    }

private:
    std::string new_config_path;
};

// 使用
config.force_reinit("/etc/app/new_config.json");
```text

⚠️ `force_reinit()` 不是线程安全的。如果可能和 `get_resources()` 并发调用，需要自己加锁保护。

## 常见场景

### 缓存系统信息

```cpp
class SystemInfoCache : public cf::CallOnceInit<SystemInfo> {
protected:
    bool init_resources() override {
        resource.cpu_count = std::thread::hardware_concurrency();
        resource.total_memory = get_total_memory();
        return true;
    }
};

SystemInfoCache sys_info;
auto& info = sys_info.get_resources();
std::cout << "CPU 核心数: " << info.cpu_count << std::endl;
```text

### 延迟加载配置

```cpp
class ConfigCache : public cf::CallOnceInit<Config> {
protected:
    bool init_resources() override {
        resource = load_config_file("/etc/app/config.json");
        return resource.valid();
    }

    bool force_do_reinit() override {
        // 配置文件可能被外部修改，支持重新加载
        resource = load_config_file("/etc/app/config.json");
        return resource.valid();
    }
};

ConfigCache config;
if (config.get_resources().debug_mode) {
    // ...
}
```text

## 线程安全保证

`get_resources()` 是线程安全的。多个线程同时首次调用时，只有一个线程会执行 `init_resources()`，其他线程会等待完成：

```cpp
// 以下代码是安全的
std::thread t1([&]() { auto& info = cache.get_resources(); });
std::thread t2([&]() { auto& info = cache.get_resources(); });

t1.join();
t2.join();
// init_resources() 只被执行一次
```text

但 `force_reinit()` 不是线程安全的。如果需要在线程间重新初始化，必须加锁：

```cpp
std::mutex cache_mutex;
std::lock_guard<std::mutex> lock(cache_mutex);
cache.force_reinit();  // 现在安全了
```text

## 注意事项

如果 `init_resources()` 返回 `false`，下次调用 `get_resources()` 会重试。这使得初始化失败可以恢复，但也意味着失败的初始化会被反复尝试——如果失败是不可恢复的，最好在初始化逻辑里处理。

另外，`CallOnceInit` 没有虚析构函数，这不是 bug，而是有意为之。我们不需要多态删除，避免虚函数开销。

## 相关文档

- [ScopeGuard - 资源管理](./scope_guard.md)
- [基础工具类概述](./overview.md)
