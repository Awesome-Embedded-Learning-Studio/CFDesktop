---
title: "weakptrfactory - 弱引用工厂"
description: 是弱引用机制的核心工厂类，负责创建和管理某个对象的弱引用。每个需要对外提供弱引用的对象都应该持有一个
---

# weak_ptr_factory - 弱引用工厂

`WeakPtrFactory<T>` 是弱引用机制的核心工厂类，负责创建和管理某个对象的弱引用。每个需要对外提供弱引用的对象都应该持有一个 `WeakPtrFactory` 成员，把它作为接口的守门员。

## 基本用法

在类中添加弱引用支持很简单，声明一个 `WeakPtrFactory` 成员即可：

```cpp
#include "base/weak_ptr/weak_ptr_factory.h"

class NetworkManager {
public:
    void SendRequest(const std::string& url) {
        // 发送请求...
    }

    // 对外提供弱引用获取接口
    cf::WeakPtr<NetworkManager> GetWeakPtr() {
        return weak_factory_.GetWeakPtr();
    }

private:
    // 必须是最后一个成员变量
    cf::WeakPtrFactory<NetworkManager> weak_factory_{this};
};
```text

构造时传入 `this` 指针，工厂会记住对象的位置。每次调用 `GetWeakPtr()` 就会创建一个新的弱引用，指向同一个对象。

## 成员变量顺序

⚠️ 这是使用 `WeakPtrFactory` 最重要的规则：**必须声明为类的最后一个成员**。

原因在于 C++ 的析构顺序——成员变量按照声明相反的顺序销毁。工厂最后声明，所以最先销毁，这样其他成员析构时如果持有自己的弱引用，能正确检测到失效：

```cpp
class MyClass {
private:
    // 这些成员先销毁
    std::vector<Callback> callbacks_;
    std::mutex mutex_;

    // 工厂最后销毁
    cf::WeakPtrFactory<MyClass> weak_factory_{this};
};
```text

如果把工厂放在中间，某些成员析构时可能仍然检测到弱引用"有效"，然后尝试访问已经被析构的部分对象，后果是未定义行为。

## 创建弱引用

通过 `GetWeakPtr()` 创建弱引用：

```cpp
MyClass obj;

// 创建多个弱引用
auto weak1 = obj.GetWeakPtr();
auto weak2 = obj.GetWeakPtr();
auto weak3 = obj.GetWeakPtr();

// 所有弱引用都指向同一个对象
assert(weak1.Get() == weak2.Get());
assert(weak2.Get() == weak3.Get());
```text

每次调用都创建一个新的 `WeakPtr` 对象，但它们共享同一个内部的"存活标志"。对象销毁或调用 `InvalidateWeakPtrs()` 后，所有弱引用同时失效。存活标志（`WeakReferenceFlag`）直接嵌入在 `WeakPtrFactory` 内部，不涉及任何堆分配。所有 `WeakPtr` 实例通过裸指针引用这个标志，因此 `WeakPtr` 的创建开销极小。

## 手动失效

有时候你想显式让所有弱引用失效，而不是真的销毁对象：

```cpp
class ConfigManager {
public:
    void Reload() {
        // 让所有旧的弱引用失效
        weak_factory_.InvalidateWeakPtrs();

        // 重新加载配置...
    }

    cf::WeakPtr<ConfigManager> GetWeakPtr() {
        return weak_factory_.GetWeakPtr();
    }

private:
    cf::WeakPtrFactory<ConfigManager> weak_factory_{this};
};
```text

`InvalidateWeakPtrs()` 会把内部的存活标志设为失效。失效前创建的所有弱引用都会变成无效。注意，与旧版实现不同，失效后**不能再创建新的弱引用**——后续调用 `GetWeakPtr()` 会触发断言失败。这是因为标志直接嵌入在工厂中，失效操作只是将 `std::atomic<bool>` 设为 `false`，不会分配新的标志。

这个功能在对象"重置"或"关闭"时很有用——你想通知所有观察者对象不再可用，但对象本身还在。如果你需要"重启"后继续创建弱引用，应该考虑使用一个全新的工厂实例。

## 不可复制不可移动

`WeakPtrFactory` 禁止复制和移动：

```cpp
class MyClass {
private:
    cf::WeakPtrFactory<MyClass> weak_factory_{this};
};

MyClass a;
MyClass b = a;  // 编译错误：WeakPtrFactory 不可复制
MyClass c = std::move(a);  // 编译错误：WeakPtrFactory 不可移动
```text

这个设计是有意为之的。工厂和对象的生命周期绑定在一起，复制或移动会破坏这个关系。如果你确实需要移动对象，得先清理所有弱引用，但这个场景在我们的使用中极少出现，干脆直接禁了。

## 常见使用模式

### 回调管理

```cpp
class AsyncWorker {
public:
    using Callback = std::function<void(int)>;

    void DoWork(Callback callback) {
        // 保存弱引用而不是裸指针
        callbacks_.push_back([weak = GetWeakPtr(), callback](int result) {
            if (weak) {  // 检查对象是否存活
                callback(result);
            }
        });

        // 实际的工作...
    }

private:
    std::vector<Callback> callbacks_;
    cf::WeakPtrFactory<AsyncWorker> weak_factory_{this};
};
```text

### 观察者模式

```cpp
class Subject {
public:
    void AddObserver(cf::WeakPtr<Observer> observer) {
        observers_.push_back(observer);
    }

    void NotifyEvent() {
        // 自动清理已失效的观察者
        auto it = std::remove_if(observers_.begin(), observers_.end(),
            [](const auto& weak) { return !weak.IsValid(); });
        observers_.erase(it, observers_.end());

        // 通知剩余的观察者
        for (auto& weak : observers_) {
            if (weak) {
                weak->OnEvent();
            }
        }
    }

private:
    std::vector<cf::WeakPtr<Observer>> observers_;
    cf::WeakPtrFactory<Subject> weak_factory_{this};
};
```text

### 单次失效模式

由于 `InvalidateWeakPtrs()` 后不能再创建新的弱引用，推荐的使用模式是"失效即终局"——调用一次 `InvalidateWeakPtrs()` 表示对象进入不可用状态，之后不再创建新的弱引用：

```cpp
class ResourceManager {
public:
    void UnloadResource(const std::string& id) {
        // 让所有弱引用失效，之后不能再创建新的
        weak_factory_.InvalidateWeakPtrs();

        // 安全地清理资源，所有外部弱引用已失效
        resources_.erase(id);
    }

    // 注意：UnloadResource 后 GetWeakPtr() 会断言失败
    cf::WeakPtr<ResourceManager> GetWeakPtr() {
        return weak_factory_.GetWeakPtr();
    }

private:
    cf::WeakPtrFactory<ResourceManager> weak_factory_{this};
};
```text

## 注意事项

第一，不要在构造函数里就把 `this` 传给其他地方。对象构造完成前，其他成员还没初始化，通过弱引用访问会导致未定义行为。`WeakPtrFactory` 的构造函数里有断言检查 `this` 非空，但不会检查构造是否完成。

第二，不要在析构函数里调用 `GetWeakPtr()`。对象已经在销毁过程中，创建新的弱引用没有意义。`WeakPtrFactory` 的析构函数会自动调用 `InvalidateWeakPtrs()`，之后 `GetWeakPtr()` 会断言失败。

第三，存活标志使用 `std::atomic<bool>` 实现，`IsAlive()` 和 `Invalidate()` 本身是线程安全的。但 `WeakPtr` 的"检查有效性 + 访问对象"两步操作不是原子的，仍然需要在同一线程（或序列）中使用。

第四，所有 `WeakPtr` 实例的生命周期不能超过工厂。因为 `WeakPtr` 持有的是指向工厂内嵌标志的裸指针（不是 `shared_ptr`），工厂析构后，任何对 `WeakPtr` 的访问都是未定义行为。这也是为什么工厂必须声明为最后一个成员——确保标志在其他成员的析构函数运行之前就失效，而不是等到工厂自身析构。

## 与标准库的对比

`WeakPtrFactory` 没有标准库的直接对应物。`std::enable_shared_from_this` 提供了类似的功能，但它是配合 `shared_ptr` 使用的，而我们假设对象有明确的唯一拥有者。内部标志直接嵌入在工厂中（无堆分配），`WeakPtr` 通过裸指针引用该标志，整体开销比 `shared_ptr` 方案小得多。

如果你需要在已有代码中引入 `WeakPtrFactory`，最简单的迁移方式是把原来持有裸指针或引用的地方改成持有 `WeakPtr`。这通常不需要大幅改动调用代码，只需要在使用前检查有效性。

## 相关文档

- [weak_ptr - 弱引用指针](./weak_ptr.md)
- [ScopeGuard - 作用域守卫](./scope_guard.md)
- [基础工具类概述](./overview.md)
