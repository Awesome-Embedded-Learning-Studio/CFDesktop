# Singleton - 单例模式

`cf` 命名空间提供了两种单例实现：`Singleton` 和 `SimpleSingleton`。两者的区别在于初始化方式——`Singleton` 需要显式调用 `init()` 并支持参数化构造，而 `SimpleSingleton` 利用 Meyer's Singleton 模式自动初始化，要求目标类有默认构造函数。

## 为什么需要两种单例

单例模式看似简单，实际上有几个关键的设计选择：

1. **何时初始化**：启动时显式初始化 vs 首次访问时自动初始化
2. **如何初始化**：需要传参数 vs 默认构造就行
3. **能否重置**：测试时需要重置单例状态

`Singleton` 适用于需要参数化构造和精确控制初始化时机的场景。`SimpleSingleton` 适用于构造不需要参数、希望自动初始化的场景。

## SimpleSingleton - 简单单例

最简实现，利用 C++11 标准保证的函数局部静态变量线程安全初始化。

### 基本用法

```cpp
#include "base/singleton/simple_singleton.hpp"

class Logger {
public:
    void log(const std::string& msg) { /* ... */ }
};

using LoggerSingleton = cf::SimpleSingleton<Logger>;

// 在任何地方
LoggerSingleton::instance().log("Hello");
```

不需要手动初始化，第一次调用 `instance()` 时自动构造。后续调用返回同一个实例。

### 继承使用

更常见的用法是让类自身继承 `SimpleSingleton`：

```cpp
class WindowManager : public cf::SimpleSingleton<WindowManager> {
public:
    void create_window(const std::string& title) { /* ... */ }
private:
    WindowManager() = default;  // 构造函数保持 private
    friend class SimpleSingleton<WindowManager>;
};

// 使用
auto& mgr = WindowManager::instance();
mgr.create_window("Main");
```

注意把构造函数设为 `private` 或 `protected`，并声明 `SimpleSingleton` 为友元。

### 特点

- **线程安全**：C++11 标准保证函数局部静态变量的初始化是线程安全的。
- **零开销**：没有互斥锁、没有 `std::call_once`，只有一个静态局部变量。
- **不可重置**：实例一旦创建就存在到程序结束，没有 `reset()` 方法。
- **要求默认构造**：目标类必须有可访问的默认构造函数。

## Singleton - 显式初始化单例

需要调用 `init()` 来创建实例，支持传参构造。使用 `std::call_once` 保证线程安全。

### 基本用法

```cpp
#include "base/singleton/singleton.hpp"

class Database {
public:
    Database(const std::string& conn_str, int pool_size)
        : conn_str_(conn_str), pool_size_(pool_size) {}

    void query(const std::string& sql) { /* ... */ }
private:
    std::string conn_str_;
    int pool_size_;
};

using DBSingleton = cf::Singleton<Database>;

// 初始化（通常在 main 函数或启动流程中）
DBSingleton::init("host=localhost port=5432", 10);

// 使用
DBSingleton::instance().query("SELECT * FROM users");
```

### 初始化时机控制

`Singleton` 的一个重要特性是初始化时机完全可控：

```cpp
// main.cpp
int main() {
    // 阶段 1：初始化基础设施
    cf::Singleton<Config>::init("config.json");
    cf::Singleton<Logger>::init(cf::Singleton<Config>::instance().log_path());

    // 阶段 2：初始化业务模块
    cf::Singleton<Database>::init(
        cf::Singleton<Config>::instance().db_connection_string(),
        cf::Singleton<Config>::instance().db_pool_size()
    );

    // 阶段 3：运行
    run_app();
}
```

如果调用 `instance()` 之前没有调用 `init()`，会抛出 `std::logic_error`：

```cpp
// 忘记初始化
try {
    DBSingleton::instance().query("SELECT 1");
} catch (const std::logic_error& e) {
    // "Singleton not initialized. Call init() first."
}
```

### 重置单例

`Singleton` 提供 `reset()` 方法，主要用于测试：

```cpp
// 测试中重置单例状态
void test_something() {
    cf::Singleton<Config>::init("test_config.json");
    // 运行测试...
    cf::Singleton<Config>::reset();

    // 下次使用前需要重新 init
    cf::Singleton<Config>::init("production_config.json");
}
```

`reset()` 之后必须重新调用 `init()` 才能使用 `instance()`。

### 重复初始化

多次调用 `init()` 是安全的——只有第一次调用生效，后续调用被 `std::call_once` 忽略：

```cpp
cf::Singleton<Config>::init("config1.json");  // 生效
cf::Singleton<Config>::init("config2.json");  // 被忽略，仍使用 config1
```

## 两种单例对比

| 特性 | SimpleSingleton | Singleton |
|------|----------------|-----------|
| 初始化方式 | 自动（首次访问） | 显式调用 `init()` |
| 构造参数 | 不支持 | 支持 |
| 线程安全 | 是（C++11 保证） | 是（`std::call_once`） |
| 重置 | 不支持 | 支持（`reset()`） |
| 未初始化访问 | N/A | 抛出 `std::logic_error` |
| 性能开销 | 极低（静态变量检查） | 稍高（指针判空 + call_once） |

## 常见陷阱

### 1. 析构顺序

单例的析构顺序和构造顺序相反。如果单例 A 的析构依赖单例 B，而 B 先于 A 析构，就会出问题。`SimpleSingleton` 更容易遇到这个问题，因为它的构造/析构时机不容易控制。

解决方案：让析构函数不依赖其他单例，或者用 `atexit` 显式控制析构顺序。

### 2. 循环依赖

如果单例 A 的初始化依赖单例 B，而 B 的初始化又依赖 A，就会死锁。这在 `Singleton` 中更容易发现（因为 `init()` 调用链是显式的），在 `SimpleSingleton` 中可能表现为首次访问时的递归初始化。

### 3. 生命周期

`SimpleSingleton` 的实例是函数局部静态变量，在 `main` 结束后的静态析构阶段销毁。如果你的单例持有需要在 `main` 之前或之后特殊管理的资源（比如线程、文件句柄），需要注意析构时机。

`Singleton` 的实例通过 `unique_ptr` 管理，也在静态析构阶段销毁，但可以通过 `reset()` 提前释放。

### 4. 不可复制、不可移动

两种单例都禁止复制和移动。这是有意为之的——单例意味着全局唯一，复制或移动会破坏这个语义。

## 线程安全

| 操作 | SimpleSingleton | Singleton |
|------|----------------|-----------|
| `instance()` | 线程安全（C++11 保证） | 线程安全（`call_once` 保护构造，但访问需要判空） |
| `init()` | N/A | 线程安全（`call_once`） |
| `reset()` | N/A | 不安全，需要外部同步 |

注意：`Singleton::reset()` 不是线程安全的。如果你需要在多线程环境下重置单例，需要自己的同步机制。通常重置只在测试中使用，单线程环境下没有问题。

## 相关文档

- [Factory - 工厂模式](./factory.md)
- [ScopeGuard - 资源管理](./scope_guard.md)
- [基础工具类概述](./overview.md)
