# factory - 工厂模式

`cf` 命名空间下提供了一组工厂模板，覆盖了从裸指针到智能指针、从直接构造到注册创建的多种场景。所有工厂都是头文件模板，无需额外编译单元，且设计上尽量保持 ABI 友好。

## 为什么需要这么多工厂

工厂模式的核心问题是"谁来创建对象"以及"创建出来的对象用什么方式管理"。不同场景下答案不同：

- 跨动态库边界传递对象时，裸指针是最安全的（不依赖共享堆）
- 应用内部使用智能指针更安全，避免内存泄漏
- 抽象接口不能直接构造，需要通过注册函数间接创建

所以我们提供了三个层次的工厂模板来应对这些需求。

## PlainFactory - 裸指针工厂

最基本的工厂，返回 `new` 出来的裸指针，调用方负责释放。

```cpp
#include "base/factory/plain_factory.hpp"

struct Widget {
    int x, y;
    Widget(int x, int y) : x(x), y(y) {}
};

cf::PlainFactory<Widget, int, int> factory;
Widget* w = factory.make(10, 20);
// 使用 w...
delete w;  // 调用方负责删除
```

`PlainFactory` 的设计目标之一是跨 ABI 边界。动态库导出的接口通常不适合直接返回 `std::unique_ptr`（不同编译器/标准库的 `unique_ptr` 布局可能不同），但裸指针永远兼容。

### StaticPlainFactory - 单例版裸指针工厂

当你需要一个全局唯一的工厂实例时：

```cpp
#include "base/factory/plain_factory.hpp"

using WidgetFactory = cf::StaticPlainFactory<Widget, int, int>;

// 在任何地方
auto& factory = WidgetFactory::instance();
Widget* w = factory.make(10, 20);
```

`StaticPlainFactory` 继承了 `PlainFactory` 和 `SimpleSingleton`，线程安全的单例由 Meyer's Singleton 保证。

## SmartPtrPlainFactory - 智能指针工厂

和 `PlainFactory` 类似，但返回 `std::unique_ptr` 或 `std::shared_ptr`，自动管理对象生命周期。

```cpp
#include "base/factory/smartptr_plain_factory.hpp"

struct Service {
    std::string name;
    explicit Service(std::string name) : name(std::move(name)) {}
};

cf::SmartPtrPlainFactory<Service, std::string> factory;

auto unique_svc = factory.make_unique("Logger");  // std::unique_ptr<Service>
auto shared_svc = factory.make_shared("Config");  // std::shared_ptr<Service>

// 不需要手动 delete
```

### StaticSmartPtrPlainFactory - 单例版智能指针工厂

```cpp
using ServiceFactory = cf::StaticSmartPtrPlainFactory<Service, std::string>;

auto svc = ServiceFactory::instance().make_unique("MyService");
```

## RegisteredFactory - 注册式工厂

当你要创建的对象类型是抽象接口（不能直接 `new`）时，需要用注册式工厂。平台相关的实现就是在启动时注册不同的 creator 函数。

```cpp
#include "base/factory/registered_factory.hpp"

// 抽象接口
class IRenderer {
public:
    virtual ~IRenderer() = default;
    virtual void draw() = 0;
};

// 具体实现
class OpenGLRenderer : public IRenderer {
public:
    void draw() override { /* OpenGL 绘制 */ }
};

class VulkanRenderer : public IRenderer {
public:
    void draw() override { /* Vulkan 绘制 */ }
};

// 注册工厂
cf::RegisteredFactory<IRenderer> renderer_factory;

// 根据平台注册不同实现
renderer_factory.register_creator([]() -> IRenderer* {
    return new VulkanRenderer;
});

// 创建
auto renderer = renderer_factory.make_unique();
renderer->draw();
```

### 自定义删除器

`RegisteredFactory` 支持注册自定义删除器，适用于通过特殊方式分配的对象：

```cpp
renderer_factory.register_creator(
    []() -> IRenderer* { return new VulkanRenderer; },
    [](IRenderer* p) { /* 自定义清理逻辑 */ delete p; }
);
```

### StaticRegisteredFactory - 单例版注册式工厂

```cpp
using RendererFactory = cf::StaticRegisteredFactory<IRenderer>;

// 初始化时注册
RendererFactory::instance().register_creator([]() -> IRenderer* {
    return new OpenGLRenderer;
});

// 使用
auto renderer = RendererFactory::instance().make_unique();
```

### 检查注册状态

```cpp
if (RendererFactory::instance().has_creator()) {
    auto renderer = RendererFactory::instance().make_unique();
} else {
    // 没有注册任何 creator，无法创建
}
```

## 线程安全说明

| 工厂 | make 系列方法 | register_creator |
|------|--------------|------------------|
| PlainFactory | 不涉及共享状态，安全 | N/A |
| SmartPtrPlainFactory | 不涉及共享状态，安全 | N/A |
| RegisteredFactory | 读取 creator_，需要外部同步 | 写入 creator_，需要外部同步 |

`Static*` 变体的单例初始化是线程安全的（Meyer's Singleton），但工厂方法本身的线程安全性取决于是否并发调用 `register_creator` 和 `make_*`。如果注册在启动时一次性完成，之后只有 `make_*` 调用，则是安全的。

## 设计选择

为什么不用抽象工厂模式（Abstract Factory）？因为 C++ 模板已经能在编译期确定类型，不需要运行时的类型擦除。模板工厂零开销，类型安全，而且不需要虚函数。

为什么保留裸指针工厂？因为在跨 DLL/SO 边界时，智能指针的类型布局可能不一致。裸指针是最通用、最安全的跨模块传递方式。

## 相关文档

- [Singleton - 单例模式](./singleton.md)
- [PolicyChain - 策略链](./policy_chain.md)
- [基础工具类概述](./overview.md)
