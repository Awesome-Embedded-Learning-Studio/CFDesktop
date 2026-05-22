---
title: "expected - 错误处理"
description: 是 C++23 引入的错误处理模板，我们提供了一份 backport 实现，确保在所有目标编译器上可
---

# expected - 错误处理

`expected<T, E>` 是 C++23 引入的错误处理模板，我们提供了一份 backport 实现，确保在所有目标编译器上可用。核心思想很简单——用返回值显式表示"可能失败的操作"，而不是靠异常把错误往外扔。这个设计特别适合我们这种需要跨平台、甚至在嵌入式环境下跑的代码，毕竟很多嵌入式编译器压根不支持异常，而且开启异常后二进制体积会显著膨胀。

## 为什么需要 expected

传统的 C++ 错误处理无非就几条路：抛异常、返回错误码、用输出参数。但这几条路都有自己的问题。

```cpp
// 方案一：异常
// 问题：很多环境禁用异常，且异常的开销不明确
std::ifstream open_file(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        throw std::runtime_error("Failed to open file");
    }
    return file;
}

// 方案二：错误码
// 问题：错误码可能被忽略，且无法携带返回值
bool open_file(const std::string& path, std::ifstream& out) {
    std::ifstream file(path);
    if (!file) return false;
    out = std::move(file);
    return true;
}
// 调用方可能忘记检查返回值
open_file("data.txt", file);  // 忘记检查，静默失败

// 方案三：expected
// 问题：几乎没有，错误必须被显式处理
cf::expected<std::ifstream, std::string> open_file(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        return cf::unexpected("Failed to open file: " + path);
    }
    return file;
}
```text

`expected` 强制调用者处理错误——你想拿到值，就必须先检查有没有错误。而且类型系统会帮你看住：一个 `expected<int, ErrorCode>` 要么包含 `int`，要么包含 `ErrorCode`，不可能同时存在或都不存在。

## 基本用法

创建一个 `expected` 有两种方式：直接返回值，或者返回 `unexpected`：

```cpp
#include "base/expected/expected.hpp"

enum class ParseError { InvalidInput, Overflow };

cf::expected<int, ParseError> parse_number(std::string_view str) {
    if (str.empty()) {
        return cf::unexpected(ParseError::InvalidInput);
    }

    try {
        return std::stoi(std::string(str));
    } catch (const std::out_of_range&) {
        return cf::unexpected(ParseError::Overflow);
    }
}
```text

调用方需要检查结果：

```cpp
auto result = parse_number("42");

if (result.has_value()) {
    std::cout << "解析成功: " << result.value() << std::endl;
} else {
    switch (result.error()) {
        case ParseError::InvalidInput:
            std::cout << "输入为空" << std::endl;
            break;
        case ParseError::Overflow:
            std::cout << "数值溢出" << std::endl;
            break;
    }
}
```text

⚠️ 如果不检查直接调用 `value()`，会抛出 `bad_expected_access` 异常。但这个异常是你在"错误地使用 expected"时才抛出的，和业务逻辑异常是两码事。正常流程下，`expected` 的使用是不抛异常的。

## 值访问方式

`expected` 提供了多种访问值的方式，取决于你想怎么处理错误情况：

```cpp
cf::expected<int, std::string> result = /* ... */;

// 方式一：先检查再访问
if (result) {
    int value = *result;       // operator*
    int value2 = result.value();  // value() 方法
}

// 方式二：提供默认值
int value = result.value_or(-1);  // 如果是错误状态，返回 -1

// 方式三：直接访问（如果确实是错误状态，会抛异常）
int value = result.value();  // 可能抛 bad_expected_access
```cpp

`operator*` 和 `operator->` 的行为类似指针，但不做边界检查——如果 `expected` 处于错误状态，调用它们的后果是未定义行为。这和原生指针的越界访问一样，性能优先，安全你自己负责。

## void 特化

有些操作没有返回值，只需要表示成功或失败。这时可以用 `expected<void, E>`：

```cpp
cf::expected<void, std::string> save_config(const std::string& path) {
    std::ofstream file(path);
    if (!file) {
        return cf::unexpected("无法打开文件");
    }
    // 写入配置...
    return {};  // 成功，返回空 expected
}

// 调用
auto result = save_config("config.txt");
if (!result) {
    std::cerr << "保存失败: " << result.error() << std::endl;
}
```text

`expected<void, E>` 的"值"是虚拟的，成功状态下没有实际数据存储，只有一个标志位。这意味着它的内存开销比 `expected<T, E>` 小——只需要存一个 `bool` 和可能的 `E`。

## 单态操作

C++23 的 `expected` 带来了函数式风格的链式调用，我们也实现了这些接口。`and_then`、`or_else`、`transform` 和 `transform_error` 让你可以把多个可能失败的操作串起来，而不需要嵌套的 `if-else`：

```cpp
// and_then: 如果当前有值，用这个值调用函数，返回新的 expected
// 如果当前是错误，直接传播错误
cf::expected<std::string, ParseError> read_user_id(int user_id) {
    return parse_number(std::to_string(user_id))
        .and_then([](int id) {
            return fetch_user_name(id);  // 返回 expected<string, ParseError>
        });
}

// transform: 如果当前有值，应用函数转换值，错误保持不变
cf::expected<std::string, ParseError> result =
    parse_number("42")
        .transform([](int value) {
            return "数字是: " + std::to_string(value);
        });
// 结果: expected<string, ParseError>，值为 "数字是: 42"

// or_else: 如果当前是错误，用这个错误调用恢复函数
cf::expected<int, ParseError> result =
    parse_number("invalid")
        .or_else([](ParseError err) {
            return cf::expected<int, ParseError>{0};  // 提供默认值
        });
// 结果: expected<int, ParseError>，值为 0

// transform_error: 转换错误类型，值保持不变
cf::expected<int, std::string> result =
    parse_number("42")
        .transform_error([](ParseError err) {
            return "解析错误: " + std::to_string(static_cast<int>(err));
        });
```text

这些操作的组合可以实现复杂的错误处理逻辑，而且代码是线性的，不是嵌套的：

```cpp
// 不用 monadic 操作
auto result1 = parse_number(input);
if (!result1) {
    return result1;
}
auto result2 = fetch_user(*result1);
if (!result2) {
    return result2;
}
auto result3 = calculate_score(*result2);
if (!result3) {
    return result3;
}
return result3;

// 用 monadic 操作
return parse_number(input)
    .and_then(fetch_user)
    .and_then(calculate_score);
```text

## 与异常的对比

`expected` 不是要完全替代异常，而是在特定场景下提供一种更可控的错误处理方式。异常适合"真正的异常情况"——那些理论上不应该发生、发生了最好让程序停下来思考的事情。而 `expected` 适合"预期的错误"——输入格式不对、文件不存在、网络超时这些正常流程中可能出现的情况。

从性能角度看，`expected` 的优势在于：
- 成功路径上没有额外开销（不需要 try-catch 的栈展开机制）
- 错误处理的成本是显式的（构造一个 `E` 类型的对象）
- 二进制体积更小（不需要异常元数据）

从代码可读性角度看，`expected` 的优势在于：
- 错误处理路径在类型系统中是可见的
- 不容易忘记处理错误（虽然还是可以忽略，但至少需要显式地忽略）
- 错误类型是类型签名的一部分，不需要去翻文档才知道可能出什么错

## 性能考虑

`expected<T, E>` 的内存布局是一个联合体加上一个标志位：

```cpp
union {
    T value;
    E error;
} storage_;
bool has_value_;
```cpp

大小是 `max(sizeof(T), sizeof(E)) + sizeof(bool)`，对齐后可能会有一点 padding。如果你在意内存占用，可以让错误类型尽量小——比如用 `enum` 代替 `std::string`。

另一个需要考虑的是移动语义。`expected` 支持移动，而且移动操作是 `noexcept` 的（如果 T 和 E 的移动构造是 `noexcept` 的）。这意味着你可以放心地在容器里存 `expected`，不用担心异常安全的问题。

## 注意事项

使用 `expected` 时有几个常见的坑：

第一，不要在错误类型里存大对象。`std::string` 作为错误类型很方便，但每次返回错误都要拷贝字符串。如果是频繁调用的底层函数，考虑用错误码或错误码加上一个小型消息缓冲区。

第二，`unexpected` 不能被忽略，但可以没有被检查。如果你写 `parse_number("42");` 而不接收返回值，编译器不会警告你（除非你开启了特定的警告）。这一点和 `[[nodiscard]]` 不同——我们确实想让某些情况下错误被静默忽略成为可能。

第三，`operator*` 和 `operator->` 不做检查。如果你在错误状态下调用它们，结果是未定义行为。这个设计是有意为之的——`expected` 的使用场景里，调用方通常已经用 `has_value()` 检查过状态了，再做一次检查就是纯粹的开销。

## 与 std::expected 的兼容性

我们的实现尽量保持与 C++23 `std::expected` 的接口一致。如果将来编译器升级到 C++23，你可以相对平滑地迁移：

```cpp
// 现在
namespace cf {
    template <typename T, typename E>
    class expected { /* ... */ };
}

// 将来（假设 C++23）
namespace std {
    template <typename T, typename E>
    class expected { /* ... */ };
}

// 迁移时只需要把 cf::expected 替换成 std::expected
// 类型别名可以帮助过渡
template <typename T, typename E>
using expected = std::expected<T, E>;
```text

当然，我们还是建议直接用 `cf::expected`，这样可以保持代码的跨平台兼容性，而且我们可以根据自己的需求定制实现。

## 相关文档

- [span - 容器视图](./span.md)
- [ScopeGuard - 资源管理](./scope_guard.md)
- [基础工具类概述](./overview.md)
