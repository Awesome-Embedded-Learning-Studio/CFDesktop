---
title: "weakptr - 非拥有弱引用指针"
description: 是一套非拥有的弱引用机制，和  有本质区别。标准库的  依赖引用计数，配合  使用，而我们的  假设
---

# weak_ptr - 非拥有弱引用指针

`WeakPtr<T>` 是一套非拥有的弱引用机制，和 `std::weak_ptr` 有本质区别。标准库的 `weak_ptr` 依赖引用计数，配合 `shared_ptr` 使用，而我们的 `WeakPtr` 假设对象有唯一的拥有者，它只是一个"取票凭证"——拥有者销毁后，所有凭证自动失效。

## 为什么需要 WeakPtr

标准库的 `weak_ptr` 在某些场景下过于重量级。你需要把对象用 `shared_ptr` 管理，即使对象本身有明确的生命周期所有者。这会带来额外的引用计数开销，而且容易误用——比如两个 `shared_ptr` 互相持有对方的 `weak_ptr`，本意是想打破循环，结果导致对象永远不被释放。

我们的设计更简单直接：对象有明确的拥有者（通常是栈上的对象或者唯一的 `unique_ptr`），其他地方只持有弱引用。拥有者销毁时，所有弱引用自动失效，不需要引用计数参与。

## 基本用法

`WeakPtr` 需要配合 `WeakPtrFactory` 使用。在拥有者类中声明一个工厂成员，然后通过它创建弱引用：

```cpp
#include "base/weak_ptr/weak_ptr_factory.h"

class ThemeManager {
public:
    void ApplyTheme() { /* ... */ }

    // 提供获取弱引用的接口
    cf::WeakPtr<ThemeManager> GetWeakPtr() {
        return weak_factory_.GetWeakPtr();
    }

private:
    // 重要：必须是最后一个成员变量
    cf::WeakPtrFactory<ThemeManager> weak_factory_{this};
};

// 使用方
ThemeManager manager;
auto weak_ref = manager.GetWeakPtr();

if (weak_ref) {  // 检查对象是否存活
    weak_ref->ApplyTheme();
}
```text

⚠️ `WeakPtrFactory` 必须声明为类的最后一个成员。C++ 按声明顺序的逆序销毁成员，这样可以确保工厂先失效，其他成员的析构函数中如果持有弱引用也能正确检测到失效。

## 访问方式

`WeakPtr` 提供了多种访问对象的方式：

```cpp
cf::WeakPtr<MyClass> weak = /* ... */;

// 方式一：显式检查后访问
if (weak.IsValid()) {  // 或 if (weak)
    weak->DoSomething();  // 安全访问
}

// 方式二：获取原始指针
if (MyClass* ptr = weak.Get()) {
    ptr->DoSomething();
}

// 方式三：直接解引用（会断言，仅确定对象存在时使用）
*weak;  // 如果无效会触发 assert
weak->Method();  // 同上
```cpp

直接解引用会触发断言，这是有意为之的设计。如果你用了 `operator->` 或 `operator*`，说明你已经确定对象存在，不会再检查。如果你不敢确定，应该用 `Get()` 或 `IsValid()` 先检查。

## 生命周期管理

`WeakPtr` 不参与对象的生命周期管理，它只是一个观察者：

```cpp
{
    MyClass obj;
    auto weak = obj.GetWeakPtr();

    // 对象存活
    assert(weak.IsValid());

}  // obj 销毁，weak 自动失效

assert(!weak.IsValid());
assert(weak.Get() == nullptr);
```text

这个设计避免了 `shared_ptr` 的隐式生命周期延长问题。持有 `WeakPtr` 不会阻止对象被销毁，这也是它和 `std::weak_ptr` 的核心区别之一。

## 类型转换

`WeakPtr` 支持协变类型转换，子类到父类的转换是隐式的：

```cpp
class Base {
public:
    virtual ~Base() = default;
    void BaseMethod() {}
};

class Derived : public Base {
public:
    void DerivedMethod() {}
};

Derived derived;
cf::WeakPtr<Derived> derived_weak = derived.GetWeakPtr();

// 隐式向上转换
cf::WeakPtr<Base> base_weak = derived_weak;
if (base_weak) {
    base_weak->BaseMethod();  // OK
}

// 显式向下转换（使用 DynamicCast）
cf::WeakPtr<Derived> derived_again =
    cf::WeakPtr<Derived>::DynamicCast(base_weak);
if (derived_again) {
    derived_again->DerivedMethod();
}
```text

`DynamicCast` 会在运行时检查类型，如果转换失败返回无效的 `WeakPtr`。这个操作不是免费的，但比直接 `dynamic_cast` 原始指针要安全，因为转换失败得到的是空指针而不是未定义行为。

## 线程安全考虑

`WeakPtr` **不是线程安全的**，应该在同一个线程（或序列）中使用。"检查有效性"和"访问对象"这两步操作不是原子的：

```cpp
// 危险：多线程环境下
// 线程 1
if (weak.IsValid()) {  // 检查通过
    // 线程 2 在这里销毁了对象
    weak->Method();  // 未定义行为！
}

// 正确做法：在单线程序列中使用
// 或者用其他同步机制保护整个检查+访问过程
```text

这个限制和 `std::weak_ptr::lock()` 不一样。标准库的 `lock()` 是原子的，可以返回一个 `shared_ptr` 保证对象在使用期间存活。我们选择不提供这个功能，是因为项目里的使用场景大多是单线程的，不需要这个开销。

## 手动失效

`WeakPtrFactory` 提供了手动使所有弱引用失效的接口：

```cpp
class MyClass {
public:
    void InvalidateAllRefs() {
        weak_factory_.InvalidateWeakPtrs();
    }

    cf::WeakPtr<MyClass> GetWeakPtr() {
        return weak_factory_.GetWeakPtr();
    }

private:
    cf::WeakPtrFactory<MyClass> weak_factory_{this};
};

// 使用
MyClass obj;
auto weak1 = obj.GetWeakPtr();
auto weak2 = obj.GetWeakPtr();

assert(weak1.IsValid());  // 有效

obj.InvalidateAllRefs();  // 手动失效

assert(!weak1.IsValid());  // 失效
assert(!weak2.IsValid());  // 失效

// 失效后仍然可以创建新的弱引用
auto weak3 = obj.GetWeakPtr();
assert(weak3.IsValid());  // 新的弱引用有效
```text

这个功能在某些场景下很有用，比如你想显式通知所有观察者对象不再可用，但又不想真的销毁对象。注意失效后创建的新弱引用是有效的，因为工厂内部会分配新的标志位。

## 检查外部引用

`WeakPtrFactory::HasWeakPtrs()` 可以检查是否有外部持有的弱引用：

```cpp
class MyClass {
private:
    cf::WeakPtrFactory<MyClass> weak_factory_{this};

public:
    bool HasExternalReferences() const {
        return weak_factory_.HasWeakPtrs();
    }
};
```bash

这个接口通过 `shared_ptr::use_count()` 实现，所以是 O(1) 的。如果 `use_count() > 1`，说明除了工厂自己外，还有其他地方持有弱引用标志位。这个功能在调试或内存泄漏排查时有用。

## 与 std::weak_ptr 的对比

| 特性 | cf::WeakPtr | std::weak_ptr |
|------|-------------|---------------|
| 所有权模型 | 独占拥有者 | 引用计数 |
| 性能开销 | 极小 | 较小（引用计数） |
| 线程安全 | 不保证 | lock() 原子操作 |
| 使用场景 | 明确生命周期的对象 | 共享所有权 |
| 依赖工厂 | 需要 WeakPtrFactory | 需要 shared_ptr |

选择 `cf::WeakPtr` 的场景通常满足这些条件：对象有明确的拥有者、不需要跨线程共享、想避免引用计数开销。如果你的对象本身就需要用 `shared_ptr` 管理，那直接用 `std::weak_ptr` 更合适。

## 常见陷阱

第一个陷阱是在 `WeakPtrFactory` 之前声明其他成员。如果这些成员的析构函数里尝试访问弱引用，会看到对象已经"失效"，因为工厂先销毁了：

```cpp
// 错误示例
class Bad {
public:
    Bad() : weak_factory_(this) {}

private:
    SomeResource resource_;  // 先销毁
    cf::WeakPtrFactory<Bad> weak_factory_{this};  // 后销毁
};

// resource_ 的析构函数中，如果持有 Bad 的 WeakPtr，会看到失效
```text

第二个陷阱是忘记检查有效性直接访问。这在异步代码里特别容易出现，因为回调执行时对象可能已经被销毁：

```cpp
// 危险
auto weak = obj.GetWeakPtr();
post_task([weak]() {
    weak->Method();  // 如果 obj 已经被销毁，这里是 UB
});

// 正确
auto weak = obj.GetWeakPtr();
post_task([weak]() {
    if (weak) {
        weak->Method();
    }
});
```text

## 相关文档

- [weak_ptr_factory - 弱引用工厂](./weak_ptr_factory.md)
- [ScopeGuard - 作用域守卫](./scope_guard.md)
- [基础工具类概述](./overview.md)
