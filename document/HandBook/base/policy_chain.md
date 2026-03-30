# PolicyChain - 策略链

`cf::PolicyChain` 是责任链模式（Chain of Responsibility）的一种实现，按优先级顺序执行一系列策略函数。如果某个策略返回了有效值（非 `std::nullopt`），链就停止；如果返回 `std::nullopt`，则尝试下一个策略。这种模式特别适合"多种可能的解决方案，优先使用第一个有效的"场景。

## 为什么需要 PolicyChain

很多系统都有"尝试多种方案"的逻辑。比如字体渲染：

1. 先尝试系统原生渲染
2. 如果不行，尝试 Freetype
3. 还不行，回退到内置光栅化器

不用 PolicyChain 的话，代码会是一串嵌套的 if-else：

```cpp
std::optional<Font> get_font(const std::string& name) {
    if (auto f = try_system_font(name)) return f;
    if (auto f = try_freetype(name))    return f;
    if (auto f = try_builtin(name))     return f;
    return std::nullopt;
}
```

这看起来还好，但当策略数量增加、策略需要动态注册时，硬编码的 if-else 就不够灵活了。PolicyChain 把这些策略变成可组合、可动态管理的链。

## 基本用法

### 直接构造

```cpp
#include "base/policy_chain/policy_chain.hpp"

cf::PolicyChain<int, const std::string&> chain;

// 添加策略（优先级从高到低）
chain.add_front([](const std::string& s) -> std::optional<int> {
    if (!s.empty()) return std::stoi(s);
    return std::nullopt;
});

chain.add_back([](const std::string& s) -> std::optional<int> {
    return static_cast<int>(s.length());  // 兜底：返回字符串长度
});

// 执行
auto result = chain.execute("42");
if (result) {
    std::cout << "Result: " << *result << std::endl;  // Result: 42
}
```

### 使用工厂函数

```cpp
auto chain = cf::make_policy_chain<int, const std::string&>(
    [](const std::string& s) -> std::optional<int> {
        if (!s.empty()) return std::stoi(s);
        return std::nullopt;
    },
    [](const std::string& s) -> std::optional<int> {
        return static_cast<int>(s.length());
    },
    [](const std::string&) -> std::optional<int> {
        return 0;  // 最终兜底
    }
);

auto r1 = chain.execute("hello");  // 返回 std::optional(5) — stoi 失败但 length 可用
auto r2 = chain.execute("123");    // 返回 std::optional(123) — stoi 成功
auto r3 = chain.execute("");       // 返回 std::optional(0) — 兜底策略
```

`make_policy_chain` 按参数顺序添加策略，第一个参数优先级最高。

### 使用 Builder 模式

```cpp
auto chain = cf::policy_chain_builder<int, int>()
    .then([](int x) -> std::optional<int> {
        if (x > 0) return x * 2;
        return std::nullopt;
    })
    .then([](int x) -> std::optional<int> {
        if (x < 0) return -x;
        return std::nullopt;
    })
    .then([](int) -> std::optional<int> {
        return 0;  // 默认值
    })
    .build();

auto r1 = chain.execute(5);   // 10 — 第一个策略处理
auto r2 = chain.execute(-3);  // 3  — 第二个策略处理
auto r3 = chain.execute(0);   // 0  — 兜底策略
```

Builder 的 `then` 按调用顺序添加策略，先添加的优先级高。

## 核心接口

### PolicyChain

```cpp
template <typename Ret, typename... Args> class PolicyChain {
    // 添加策略到链头（最高优先级）
    void add_front(PolicyType policy);

    // 添加策略到链尾（最低优先级）
    void add_back(PolicyType policy);

    // 执行链，返回第一个非 nullopt 的结果
    [[nodiscard]] std::optional<Ret> execute(Args... args) const;

    // 函数调用语法糖，等价于 execute
    [[nodiscard]] std::optional<Ret> operator()(Args... args) const;

    // 管理操作
    void clear();
    [[nodiscard]] bool empty() const;
    [[nodiscard]] SizeType size() const;
};
```

### 工厂函数和 Builder

```cpp
// 工厂函数：直接创建链
template <typename T, typename... Args, typename... Policies>
auto make_policy_chain(Policies&&... policies);

// Builder 创建器
template <typename T, typename... Args>
auto policy_chain_builder();
```

## 执行语义

链按策略添加的顺序执行（`add_front` 的先执行）。每个策略函数签名为 `std::optional<Ret>(Args...)`：

- 返回有效值：链停止，返回该值
- 返回 `std::nullopt`：继续执行下一个策略
- 所有策略都返回 `std::nullopt`：整个链返回 `std::nullopt`

```cpp
auto result = chain("input");
if (result) {
    // 有策略成功处理
} else {
    // 所有策略都未能处理
}
```

## 典型使用场景

### 平台特性检测

```cpp
auto renderer_chain = cf::make_policy_chain<std::string>(
    []() -> std::optional<std::string> {
        if (vulkan_available()) return "Vulkan";
        return std::nullopt;
    },
    []() -> std::optional<std::string> {
        if (opengl_available()) return "OpenGL";
        return std::nullopt;
    },
    []() -> std::optional<std::string> {
        return "Software";  // 兜底
    }
);

auto renderer = renderer_chain.execute();
```

### 配置值解析

```cpp
auto config_chain = cf::policy_chain_builder<std::string, const std::string&>()
    .then([](const std::string& key) -> std::optional<std::string> {
        return try_env_variable(key);  // 优先环境变量
    })
    .then([](const std::string& key) -> std::optional<std::string> {
        return try_config_file(key);   // 其次配置文件
    })
    .then([](const std::string& key) -> std::optional<std::string> {
        return get_default(key);        // 最后默认值
    })
    .build();
```

### 资源加载

```cpp
auto loader_chain = cf::make_policy_chain<QImage, const QString&>(
    [](const QString& path) -> std::optional<QImage> {
        return try_load_from_cache(path);
    },
    [](const QString& path) -> std::optional<QImage> {
        return try_load_from_disk(path);
    },
    [](const QString& path) -> std::optional<QImage> {
        return try_load_from_network(path);
    }
);
```

## 线程安全

`PolicyChain` 本身不提供线程安全保证。如果在多线程环境下使用：

- **只读场景**（构建完成后只调用 `execute`）：安全，因为 `execute` 是 `const` 方法，内部数据不会被修改。
- **动态修改**（运行时调用 `add_front` / `add_back`）：需要外部同步。

推荐的做法是在初始化阶段构建好链，运行时只调用 `execute`。

## 性能考虑

- 每个策略通过 `std::function` 存储，有一次间接调用的开销。
- 策略存储在 `std::list` 中，遍历时有缓存不友好的问题。但通常策略数量不多（几个到十几个），影响可以忽略。
- `execute` 在最坏情况下会遍历所有策略，时间复杂度 O(n)。

如果策略数量很少（比如 2-3 个）且性能极其敏感，直接用 if-else 可能比 PolicyChain 更快。PolicyChain 的优势在于灵活性和可组合性。

## 相关文档

- [Factory - 工厂模式](./factory.md)
- [Singleton - 单例模式](./singleton.md)
- [基础工具类概述](./overview.md)
