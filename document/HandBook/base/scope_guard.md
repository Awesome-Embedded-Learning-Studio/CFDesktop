---
title: "scopeguard - 作用域守卫"
description: 是 RAII（Resource Acquisition Is Initialization）模式的轻
---

# scope_guard - 作用域守卫

`ScopeGuard` 是 RAII（Resource Acquisition Is Initialization）模式的轻量级实现，确保一段代码在作用域结束时执行，无论是因为正常返回还是抛出异常。这个看似简单的工具在实际代码里非常好用——它能把"清理资源"和"业务逻辑"分开，让代码更清晰，也更容易避免资源泄漏。

## 为什么需要 ScopeGuard

考虑一个需要手动管理的资源：

```cpp
// 没有 ScopeGuard 的写法
void process_file(const std::string& path) {
    FILE* f = fopen(path.c_str(), "r");
    if (!f) return;

    void* buffer = malloc(1024);
    if (!buffer) {
        fclose(f);  // 别忘了关闭文件
        return;
    }

    if (some_condition) {
        free(buffer);  // 别忘了释放内存
        fclose(f);     // 别忘了关闭文件
        return;
    }

    // 更多代码...

    free(buffer);  // 三个出口，三个地方写清理代码
    fclose(f);
}
```text

每个可能的返回路径都要记得清理所有资源，漏一个就泄漏。用 `ScopeGuard` 就简单多了：

```cpp
// 有 ScopeGuard 的写法
void process_file(const std::string& path) {
    FILE* f = fopen(path.c_str(), "r");
    if (!f) return;
    cf::ScopeGuard close_file([&f]() { fclose(f); });

    void* buffer = malloc(1024);
    if (!buffer) return;
    cf::ScopeGuard free_buffer([&buffer]() { free(buffer); });

    if (some_condition) return;  // 自动清理

    // 更多代码...
}
```text

无论从哪个路径退出，`ScopeGuard` 都会执行对应的清理代码。你不需要在每个返回点都写一遍，也不容易漏。

## 基本用法

`ScopeGuard` 接受一个可调用对象（通常是 lambda），在销毁时执行：

```cpp
#include "base/scope_guard/scope_guard.hpp"

{
    int counter = 0;

    cf::ScopeGuard guard([&counter]() {
        counter = 42;
    });

    // 做一些事情...
    // 离开作用域时 counter 变成 42
}
```text

lambda 按引用捕获 `counter`，所以在守卫内部可以修改它。你也可以按值捕获，看具体需求。

## 取消防护

有时候你不想让守卫执行清理代码，可以调用 `dismiss()`：

```cpp
void save_config(const std::string& path) {
    std::string temp_path = path + ".tmp";

    // 创建临时文件
    FILE* f = fopen(temp_path.c_str(), "w");
    cf::ScopeGuard cleanup([&temp_path]() {
        // 失败时删除临时文件
        std::remove(temp_path.c_str());
    });

    // 写入配置...

    // 原子重命名
    if (std::rename(temp_path.c_str(), path.c_str()) == 0) {
        cleanup.dismiss();  // 成功，不需要删除临时文件
    }
}
```text

`dismiss()` 是不可逆的，一旦调用就不能再恢复。多次调用 `dismiss()` 是安全的，不会有额外效果。

## 多个守卫的执行顺序

同一个作用域可以有多个 `ScopeGuard`，它们按照**创建相反的顺序**执行：

```cpp
{
    std::vector<int> order;

    cf::ScopeGuard guard1([&order]() { order.push_back(1); });
    cf::ScopeGuard guard2([&order]() { order.push_back(2); });
    cf::ScopeGuard guard3([&order]() { order.push_back(3); });
}
// order = {3, 2, 1}
```text

这和 C++ 局部变量的析构顺序一致——后创建的先析构。这个顺序很重要，如果多个守卫之间有依赖，你需要知道哪个先执行。比如先分配的资源应该后释放（LIFO），正好符合这个顺序。

## 异常安全

`ScopeGuard` 的清理代码在异常抛出时也会执行：

```cpp
try {
    cf::ScopeGuard guard([]() {
        printf("Cleanup executed\n");
    });

    throw std::runtime_error("error");
    // guard 仍然会执行
} catch (...) {
    // 异常被捕获，但清理已经执行
}
```text

⚠️ 如果清理代码本身抛出异常，这个异常会传播出去。如果在栈展开过程中（已经有一个异常在处理）清理代码又抛出异常，程序会调用 `std::terminate`。所以确保清理代码不会抛异常，或者把可能抛异常的代码用 `try-catch` 包起来。

## 典型使用场景

### 文件句柄管理

```cpp
void read_config(const std::string& path) {
    FILE* f = fopen(path.c_str(), "r");
    if (!f) return;
    cf::ScopeGuard close_file([f]() { fclose(f); });

    // 使用文件...
    // 离开作用域自动关闭
}
```text

### 状态回滚

```cpp
void update_state(State& s) {
    State backup = s;

    cf::ScopeGuard rollback([&s, backup]() {
        s = backup;  // 失败时恢复
    });

    // 尝试修改状态...

    rollback.dismiss();  // 成功，不需要回滚
}
```text

### 锁的释放

```cpp
void critical_section() {
    mutex.lock();
    cf::ScopeGuard unlock([&mutex]() { mutex.unlock(); });

    // 临界区代码...
}
```text

当然更推荐直接用 `std::lock_guard` 或 `std::unique_lock`，但 `ScopeGuard` 可以处理更复杂的场景。

### 恢复修改过的变量

```cpp
void process_item(Item& item) {
    auto original_priority = item.priority();

    cf::ScopeGuard restore([&item, original_priority]() {
        item.set_priority(original_priority);
    });

    item.set_priority(Priority::High);

    // 临时提高优先级处理...

    // 离开作用域自动恢复
}
```text

## 限制和注意事项

`ScopeGuard` 不可复制也不可移动：

```cpp
cf::ScopeGuard guard1([]() {});

cf::ScopeGuard guard2 = guard1;  // 编译错误
cf::ScopeGuard guard3 = std::move(guard1);  // 编译错误
```text

这个设计是为了确保清理代码只执行一次。如果允许复制，同一个守卫可能被复制到多个地方，不清楚应该由谁负责清理。如果允许移动，移动后原守卫的清理代码就不应该再执行，但这会让语义变得复杂。

另一个限制是内部使用 `std::function` 存储，所以 lambda 必须可复制。这意味着你不能按值捕获只能移动的类型（如 `std::unique_ptr`）。解决方法是按引用捕获，或者用 `std::shared_ptr` 包装。

```cpp
// 这样不行
auto ptr = std::make_unique<int>(42);
cf::ScopeGuard guard([ptr]() {});  // 编译错误

// 可以这样
auto ptr = std::make_unique<int>(42);
cf::ScopeGuard guard([&ptr]() {});  // 按引用捕获
```text

## 性能考虑

`ScopeGuard` 的开销主要是 `std::function` 的类型擦除。一个守卫对象通常占用 32-64 字节（取决于平台），构造和析构各有一次虚函数调用（通过 `std::function` 的机制）。

在绝大多数场景下这个开销是可以忽略的。如果你在极度性能敏感的代码里使用，而且清理代码是固定的，可以考虑写一个专门的 RAII 类。但 99% 的情况下，`ScopeGuard` 的便利性远大于这点开销。

## 控制流交互

`ScopeGuard` 和各种控制流都能正确配合：

```cpp
// early return
for (int i = 0; i < 10; ++i) {
    cf::ScopeGuard guard([]() { /* cleanup */ });
    if (i == 5) return;  // 仍然执行清理
}

// break/continue
for (int i = 0; i < 10; ++i) {
    cf::ScopeGuard guard([i]() { printf("%d\n", i); });
    if (i == 5) break;  // 守卫仍然执行
}
// 输出: 0 1 2 3 4 5

// goto
{
    cf::ScopeGuard guard([]() { /* cleanup */ });
    goto end;
end:;
}
// 守卫仍然执行
```text

无论控制流怎么跳转，只要离开了守卫所在的作用域，清理代码就会执行。这得益于 C++ 的 RAII 机制——析构函数总会在作用域结束时被调用。

## 设计决策

我们没有使用更复杂的实现（比如支持 dismiss 后重新激活，或者支持移动），是因为简单够用。`ScopeGuard` 的定位就是"创建时注册，销毁时执行"，额外的功能只会增加理解和维护成本。

也不提供类似 `SCOPE_EXIT` 宏的实现。宏确实可以让代码更紧凑，但会引入一些问题：调试时栈追踪不清楚，命名空间污染，以及宏展开的意外行为。显式的变量声明虽然多打几个字，但意图更清晰。

## 相关文档

- [weak_ptr - 弱引用指针](./weak_ptr.md)
- [expected - 错误处理](./expected.md)
- [基础工具类概述](./overview.md)
