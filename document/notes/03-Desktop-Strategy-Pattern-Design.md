---
title: 桌面策略系统设计（Strategy Pattern 实战）
description: 1. 为什么使用 Strategy 模式
---

# 桌面策略系统设计（Strategy Pattern 实战）

## 目录

1. [为什么使用 Strategy 模式](#为什么使用-strategy-模式)
    - [1.1 跨平台差异处理](#11-跨平台差异处理)
    - [1.2 行为解耦与单一职责](#12-行为解耦与单一职责)
    - [1.3 插件化架构支持](#13-插件化架构支持)
    - [1.4 测试友好性](#14-测试友好性)
2. [Strategy Pattern 理论基础](#strategy-pattern-理论基础)
    - [2.1 设计模式定义](#21-设计模式定义)
    - [2.2 UML 结构图](#22-uml-结构图)
    - [2.3 C++ 实现要点](#23-c-实现要点)
3. [接口设计：IDesktopDisplaySizeStrategy](#接口设计idesktopdisplaysizestrategy)
    - [3.1 基础接口定义](#31-基础接口定义)
    - [3.2 策略类型枚举](#32-策略类型枚举)
    - [3.3 ABI 友好设计](#33-abi-友好设计)
4. [Action vs Query：核心思想对比](#action-vs-query核心思想对比)
    - [4.1 CQRS 原则](#41-cqrs-原则)
    - [4.2 Action 方法设计](#42-action-方法设计)
    - [4.3 Query 方法设计](#43-query-方法设计)
    - [4.4 分离的好处](#44-分离的好处)
5. [示例策略实现](#示例策略实现)
    - [5.1 FullscreenStrategy](#51-fullscreenstrategy)
    - [5.2 FramelessStrategy](#52-framelessstrategy)
    - [5.3 WSL 平台策略](#53-wsl-平台策略)
6. [多策略组合实现](#多策略组合实现)
    - [6.1 策略组合模式](#61-策略组合模式)
    - [6.2 CompositeStrategy](#62-compositestrategy)
    - [6.3 策略链模式](#63-策略链模式)
7. [冲突检测机制](#冲突检测机制)
    - [7.1 行为冲突定义](#71-行为冲突定义)
    - [7.2 冲突检测接口](#72-冲突检测接口)
    - [7.3 冲突解决策略](#73-冲突解决策略)
8. [工厂模式集成](#工厂模式集成)
    - [8.1 PlatformFactory 设计](#81-platformfactory-设计)
    - [8.2 平台特定工厂](#82-平台特定工厂)
    - [8.3 插件化工厂](#83-插件化工厂)
9. [最佳实践与设计原则](#最佳实践与设计原则)

---

## 为什么使用 Strategy 模式

### 1.1 跨平台差异处理

在桌面应用开发中，不同操作系统对窗口行为有截然不同的实现方式：

| 平台特性 | Windows | macOS | Linux (X11) | Linux (Wayland) | WSL |
|---------|---------|-------|------------|----------------|-----|
| 全屏实现 | `WM_SYSCOMMAND` | `NSFullScreenMode` | `_NET_WM_STATE_FULLSCREEN` | xdg-shell 全屏 | 代理实现 |
| 无边框窗口 | `WS_POPUP` | `NSWindowStyleMaskBorderless` | `_NET_WM_WINDOW_TYPE` | xdg-shell popup | 限制支持 |
| 窗口置顶 | `WS_EX_TOPMOST` | `NSWindowLevel` | `_NET_WM_STATE_ABOVE` | 不支持 | 不支持 |
| 窗口层级 | Z-Order | Window Level | EWMH 层级 | Wayland 协议 | 限制支持 |

这种差异使得单一实现难以应对所有场景，而 Strategy 模式提供了一种优雅的解决方案：

```cpp
// ❌ 不使用 Strategy：条件分支地狱
void applyWindowFlags(QWidget* widget) {
    #ifdef Q_OS_WIN
        // Windows 特定代码
    #elif defined(Q_OS_MACOS)
        // macOS 特定代码
    #elif defined(Q_OS_LINUX)
        if (QGuiApplication::platformName() == "xcb") {
            // X11 特定代码
        } else if (QGuiApplication::platformName() == "wayland") {
            // Wayland 特定代码
        }
    #endif
    // 代码维护困难，难以扩展
}

// ✅ 使用 Strategy：平台无关接口
void applyWindowFlags(QWidget* widget, IDesktopDisplaySizeStrategy* strategy) {
    strategy->action(widget);  // 平台特定实现被封装
}
```text

### 1.2 行为解耦与单一职责

Strategy 模式遵循**单一职责原则**（Single Responsibility Principle），将窗口行为的具体实现从主业务逻辑中分离：

```cpp
// 职责分离示例
class CFDesktop : public QWidget {
    // CFDesktop 专注于：
    // 1. 管理桌面组件（PanelManager、ShellLayer）
    // 2. 提供代理访问（CFDesktopProxy）
    // 3. 协调组件生命周期

    // 不负责：
    // ✗ 平台特定的窗口行为实现
    // ✗ 不同显示模式的具体逻辑
};

// 策略专注于：
// 1. 实现特定平台的窗口行为
// 2. 提供行为查询接口
// 3. 管理平台相关的状态
```text

这种分离带来的好处：

1. **代码可读性**：每个策略类职责明确，易于理解
2. **可维护性**：修改平台特定逻辑不影响其他代码
3. **可测试性**：可以独立测试每个策略

### 1.3 插件化架构支持

Strategy 模式为插件化架构提供了天然支持。通过工厂模式 + 策略模式，可以实现运行时动态加载平台实现：

```cpp
// 插件化架构示例
namespace cf::desktop::platform_strategy {

// 平台工厂 API
struct PlatformFactoryAPI {
    using CreateFunc = IDesktopPropertyStrategy*(StrategyType);
    using ReleaseFunc = void(IDesktopPropertyStrategy*);

    CreateFunc creator_func;
    ReleaseFunc release_func;
};

// 本地实现
PlatformFactoryAPI native() noexcept;

// TODO: 从 DLL 加载
// PlatformFactoryAPI* remote() noexcept;

} // namespace
```text

这支持以下场景：

1. **运行时平台检测**：根据运行环境自动选择策略
2. **动态插件加载**：从共享库加载平台实现
3. **A/B 测试**：可以同时测试不同的策略实现

### 1.4 测试友好性

Strategy 模式使得单元测试变得简单：

```cpp
// 测试用例示例
class MockDisplaySizeStrategy : public IDesktopDisplaySizeStrategy {
public:
    const char* name() const noexcept override {
        return "Mock Strategy";
    }

    bool action(QWidget* widget) override {
        action_called = true;
        last_widget = widget;
        return true;
    }

    DesktopBehaviors query() const override {
        return DesktopBehaviorFlag::Fullscreen | DesktopBehaviorFlag::Frameless;
    }

    // 测试辅助
    bool action_called = false;
    QWidget* last_widget = nullptr;
};

// 使用 mock 进行测试
TEST(DesktopTest, ApplyStrategy) {
    MockDisplaySizeStrategy mock_strategy;
    CFDesktop desktop;

    // 应用策略
    mock_strategy.action(&desktop);

    // 验证
    EXPECT_TRUE(mock_strategy.action_called);
    EXPECT_EQ(mock_strategy.last_widget, &desktop);
}
```yaml

参考资料：
- [Strategy in C++ / Design Patterns - Refactoring.Guru](https://refactoring.guru/design-patterns/strategy/cpp/example)
- [The Strategy Pattern – MC++ BLOG - Modernes C++](https://www.modernescpp.com/index.php/the-strategy-pattern/)
- [Design Patterns: Elements of Reusable Object-Oriented Software - GoF](https://en.wikipedia.org/wiki/Design_Patterns)

---

## Strategy Pattern 理论基础

### 2.1 设计模式定义

根据 GoF（Gang of Four）的经典定义：

> **Strategy Pattern**：定义一系列算法，把它们一个个封装起来，并且使它们可相互替换。本模式使得算法可独立于使用它的客户而变化。

在桌面行为管理场景中：

- **算法**：窗口行为的具体实现（全屏、无边框、置顶等）
- **客户**：CFDesktop 或其他需要应用窗口行为的组件
- **变化**：不同平台、不同配置下的行为差异

### 2.2 UML 结构图

```text
┌─────────────────────────────────────────────────────────────────────┐
│                        Context (CFDesktop)                          │
│                                                                     │
│  - strategy: IDesktopDisplaySizeStrategy                           │
│  + setStrategy(strategy: IDesktopDisplaySizeStrategy)               │
│  + applyBehavior()                                                  │
└─────────────────────────────┬───────────────────────────────────────┘
                              │
                              │ uses
                              ▼
┌─────────────────────────────────────────────────────────────────────┐
│           <<interface>> IDesktopDisplaySizeStrategy                │
│                                                                     │
│  + action(widget: QWidget*): bool                                  │
│  + query(): DesktopBehaviors                                       │
│  + name(): const char*                                              │
└─────────────────────────────┬───────────────────────────────────────┘
                              │
                              │ implements
          ┌───────────────────┼───────────────────┐
          ▼                   ▼                   ▼
┌─────────────────┐ ┌─────────────────┐ ┌─────────────────┐
│FullscreenStrategy│ │FramelessStrategy│ │  WSLDisplaySize  │
│                 │ │                 │ │     Strategy     │
│+ action()       │ │+ action()       │ │+ action()        │
│+ query()        │ │+ query()        │ │+ query()         │
└─────────────────┘ └─────────────────┘ └─────────────────┘
```text

### 2.3 C++ 实现要点

#### 虚析构函数

策略接口必须声明虚析构函数以确保正确的资源清理：

```cpp
class IDesktopDisplaySizeStrategy : public IDesktopPropertyStrategy {
public:
    virtual ~IDesktopDisplaySizeStrategy() = default;
    // ...
};
```text

#### 智能指针支持

使用智能指针管理策略生命周期：

```cpp
class PlatformFactory {
public:
    using StrategyDeleter = void (*)(IDesktopPropertyStrategy*);

    // 使用 unique_ptr，自定义删除器
    std::unique_ptr<IDesktopPropertyStrategy, StrategyDeleter>
    factorize_unique(const IDesktopPropertyStrategy::StrategyType t);

    // 使用 shared_ptr，支持共享所有权
    std::shared_ptr<IDesktopPropertyStrategy>
    factorize_shared(const IDesktopPropertyStrategy::StrategyType t);
};
```text

#### WeakPtr 集成

策略支持 WeakPtr，避免循环引用：

```cpp
class IDesktopDisplaySizeStrategy : public IDesktopPropertyStrategy {
public:
    WeakPtr<IDesktopDisplaySizeStrategy> GetOne() {
        return weak_factory_ptr_.GetWeakPtr();
    }

private:
    WeakPtrFactory<IDesktopDisplaySizeStrategy> weak_factory_ptr_;
};
```yaml

参考资料：
- [Strategy Design Pattern - GeeksforGeeks](https://www.geeksforgeeks.org/system-design/strategy-pattern-set-1/)
- [C++ Core Guidelines: C.129 - When designing a class hierarchy, distinguish between implementation inheritance and interface inheritance](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rc-interface)

---

## 接口设计：IDesktopDisplaySizeStrategy

### 3.1 基础接口定义

```cpp
/**
 * @file IDesktopPropertyStrategy.h
 * @brief 桌面属性策略基础接口
 *
 * 所有桌面策略的抽象基类，定义了策略的基本契约。
 */

namespace cf::desktop::platform_strategy {

/**
 * @brief 桌面属性策略基类
 *
 * @details 提供所有策略共享的类型标识和接口契约
 */
class IDesktopPropertyStrategy {
public:
    /**
     * @brief 策略类型枚举
     *
     * 用于策略工厂的类型标识，确保类型安全的策略创建
     */
    enum class StrategyType {
        Unavailable,      ///< 无效/不可用策略
        DisplaySizePolicy, ///< 显示尺寸策略（全屏、无边框等）
        Extensions,       ///< 扩展策略（输入法、系统托盘等）
    };

    /**
     * @brief 构造函数
     * @param t 策略类型
     */
    IDesktopPropertyStrategy(const StrategyType t);

    /**
     * @brief 虚析构函数
     *
     * 确保通过基类指针删除派生类对象时正确调用派生类析构函数
     */
    virtual ~IDesktopPropertyStrategy() = default;

    /**
     * @brief 获取策略名称
     *
     * @return 策略的名称字符串，用于调试和日志
     *
     * @note 必须由派生类实现，返回有效的字符串字面量
     * @note 使用 noexcept 确保不会抛出异常
     */
    virtual const char* name() const noexcept = 0;

protected:
    const StrategyType type_;  ///< 策略类型，运行时类型标识
};

} // namespace cf::desktop::platform_strategy
```text

### 3.2 策略类型枚举

策略类型枚举提供了类型安全的策略创建机制：

```cpp
/**
 * @brief 策略类型到字符串的转换（用于日志）
 */
inline const char* strategyTypeToString(IDesktopPropertyStrategy::StrategyType type) {
    switch (type) {
        case IDesktopPropertyStrategy::StrategyType::Unavailable:
            return "Unavailable";
        case IDesktopPropertyStrategy::StrategyType::DisplaySizePolicy:
            return "DisplaySizePolicy";
        case IDesktopPropertyStrategy::StrategyType::Extensions:
            return "Extensions";
        default:
            return "Unknown";
    }
}
```text

### 3.3 ABI 友好设计

为了确保 ABI（Application Binary Interface）兼容性，接口设计遵循以下原则：

#### 1. 避免虚模板方法

```cpp
// ❌ 不推荐：模板虚方法破坏 ABI
class IDesktopPropertyStrategy {
public:
    template<typename T>
    virtual T get() = 0;  // 编译错误或 ABI 问题
};

// ✅ 推荐：使用显式类型方法
class IDesktopPropertyStrategy {
public:
    virtual const char* name() const noexcept = 0;
    virtual StrategyType type() const noexcept { return type_; }
};
```text

#### 2. 使用 Pimpl 模式隐藏实现

```cpp
// 头文件：稳定 ABI
class IDesktopPropertyStrategy {
public:
    // 公共接口...
private:
    class Impl;
    std::unique_ptr<Impl> impl_;  // 实现细节
};

// 实现文件：可变实现
class IDesktopPropertyStrategy::Impl {
    // 实现细节可以随意修改而不影响 ABI
};
```text

#### 3. 自定义删除器

```cpp
// 使用自定义删除器确保跨 DLL 边界的正确释放
class PlatformFactory {
public:
    using StrategyDeleter = void (*)(IDesktopPropertyStrategy*);

    std::unique_ptr<IDesktopPropertyStrategy, StrategyDeleter>
    factorize_unique(const IDesktopPropertyStrategy::StrategyType t) {
        // 创建策略并使用工厂的删除器
        return { createImpl(t), [](IDesktopPropertyStrategy* p) {
            releaseImpl(p);  // 使用工厂的释放函数
        }};
    }
};
```text

### 3.4 显示策略接口

```cpp
/**
 * @file IDesktopDisplaySizeStrategy.h
 * @brief 桌面显示尺寸策略接口
 *
 * 定义了控制窗口显示行为（全屏、无边框等）的策略接口。
 */

namespace cf::desktop::platform_strategy {

/**
 * @brief 桌面行为标志枚举
 *
 * 使用位标志表示窗口的各种行为特性。
 * 多个标志可以使用位运算符组合。
 */
enum class DesktopBehaviorFlag {
    Fullscreen     = 0x1,  ///< 全屏模式
    Frameless      = 0x2,  ///< 无边框窗口
    StayOnBottom   = 0x4,  ///< 置底窗口
    AllowResize    = 0x8,  ///< 允许调整大小
    AvoidSystemUI  = 0x10, ///< 避开系统 UI
};

Q_DECLARE_FLAGS(DesktopBehaviors, DesktopBehaviorFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(DesktopBehaviors)

/**
 * @brief 桌面显示尺寸策略接口
 *
 * @details 定义了窗口显示行为的操作接口：
 *          - action(): 应用行为到窗口
 *          - query(): 查询当前行为状态
 */
class IDesktopDisplaySizeStrategy : public IDesktopPropertyStrategy {
public:
    virtual ~IDesktopDisplaySizeStrategy() = default;

    /**
     * @brief 获取策略的 WeakPtr
     *
     * @return WeakPtr 指向当前策略实例
     *
     * @note 用于避免循环引用
     */
    WeakPtr<IDesktopDisplaySizeStrategy> GetOne() {
        return weak_factory_ptr_.GetWeakPtr();
    }

    /**
     * @brief 应用窗口行为
     *
     * @param widget_data 目标 QWidget 指针
     * @return 成功返回 true，失败返回 false
     *
     * @note 默认实现不做任何操作，返回 true
     * @note 派生类应重写此方法以实现特定行为
     */
    virtual bool action(QWidget* widget_data) {
        return true;  // 默认：不做任何操作
    }

    /**
     * @brief 查询当前窗口行为
     *
     * @return 当前行为的标志组合
     *
     * @note 默认实现返回 None
     * @note 派生类应重写此方法以查询实际状态
     */
    virtual DesktopBehaviors query() const;

private:
    WeakPtrFactory<IDesktopDisplaySizeStrategy> weak_factory_ptr_;
};

} // namespace cf::desktop::platform_strategy
```bash

参考资料：
- [Qt 6.10.2 QFlags 官方文档](https://doc.qt.io/qt-6/qflags.html)
- [C++ ABI Compatibility - Itanium C++ ABI](https://itanium-cxx-abi.github.io/cxx-abi/abi.html)
- [Pimpl Idiom in C++ - Wikipedia](https://en.cppreference.com/w/cpp/language/pimpl)

---

## Action vs Query：核心思想对比

### 4.1 CQRS 原则

**CQRS**（Command Query Responsibility Segregation）是 Bertrand Meyer 提出的设计原则，在 Martin Fowler 的文章中得到详细阐述：

> "CQRS stands for Command Query Responsibility Segregation. It's a pattern that separates operations that read data (queries) from operations that modify data (commands)."

在我们的策略系统中：

| 类型 | 方法 | 职责 | 副作用 | 返回值 |
|------|------|------|--------|--------|
| **Action** | `action()` | 修改系统状态 | 有 | bool（成功/失败） |
| **Query** | `query()` | 读取系统状态 | 无 | DesktopBehaviors（状态） |

### 4.2 Action 方法设计

Action 方法遵循以下设计原则：

#### 1. 明确的副作用

```cpp
/**
 * @brief 应用窗口行为
 *
 * @details 此方法会：
 *          1. 修改 widget 的窗口标志（Qt::WindowFlags）
 *          2. 可能改变 widget 的显示状态
 *          3. 可能影响 widget 的位置和大小
 *
 * @param widget_data 目标 QWidget
 * @return true 表示操作成功，false 表示失败
 *
 * @note 调用此方法会改变 widget 的状态
 * @warning 不应在持有锁的状态下调用此方法（可能触发 Qt 事件）
 */
virtual bool action(QWidget* widget_data);
```text

#### 2. 返回值表示操作结果

```cpp
// ✅ 正确：使用返回值表示成功/失败
bool result = strategy->action(widget);
if (!result) {
    // 处理失败情况
    qWarning() << "Failed to apply strategy:" << strategy->name();
}

// ❌ 错误：在 action 中抛出异常（Qt 约定不使用异常）
virtual bool action(QWidget* widget_data) {
    if (!widget_data) {
        throw std::invalid_argument("widget is null");  // 不推荐
    }
}
```text

#### 3. 幂等性

Action 方法应该是幂等的，多次调用应该产生相同的结果：

```cpp
class FullscreenStrategy : public IDesktopDisplaySizeStrategy {
public:
    bool action(QWidget* widget) override {
        if (!widget) return false;

        // 幂等性检查：如果已经是全屏，不需要重复操作
        if (widget->isFullScreen()) {
            return true;
        }

        widget->showFullScreen();
        return true;
    }
};
```text

### 4.3 Query 方法设计

Query 方法遵循以下设计原则：

#### 1. 无副作用

```cpp
/**
 * @brief 查询当前窗口行为
 *
 * @details 此方法：
 *          1. 不修改任何状态
 *          2. 不触发任何事件
 *          3. 可以安全地在任何上下文中调用
 *
 * @return 当前行为的标志组合
 *
 * @note 此方法是线程安全的（假设 DesktopBehaviors 是值类型）
 * @note 可以多次调用而不影响系统状态
 */
virtual DesktopBehaviors query() const;
```text

#### 2. 返回完整信息

```cpp
// ✅ 正确：返回完整的状态信息
DesktopBehaviors behaviors = strategy->query();
if (behaviors.testFlag(DesktopBehaviorFlag::Fullscreen)) {
    // 处理全屏状态
}

// ❌ 错误：提供多个查询方法
bool isFullscreen() const;  // 应该使用 query().testFlag()
bool isFrameless() const;   // 应该使用 query().testFlag()
```text

#### 3. const 正确性

```cpp
// Query 方法必须是 const 的
virtual DesktopBehaviors query() const;

// 允许在 const 上下文中调用
void monitorDesktop(const IDesktopDisplaySizeStrategy& strategy) {
    DesktopBehaviors current = strategy.query();  // OK
}
```text

### 4.4 分离的好处

#### 1. 缓存优化

Query 方法可以被安全地缓存：

```cpp
class CachedDesktopDisplaySizeStrategy : public IDesktopDisplaySizeStrategy {
public:
    DesktopBehaviors query() const override {
        // 缓存结果，避免重复查询
        if (!cache_valid_) {
            cached_behaviors_ = doQuery();
            cache_valid_ = true;
        }
        return cached_behaviors_;
    }

    bool action(QWidget* widget) override {
        // Action 操作后使缓存失效
        bool result = doAction(widget);
        cache_valid_ = false;
        return result;
    }

private:
    mutable DesktopBehaviors cached_behaviors_;
    mutable bool cache_valid_ = false;
};
```text

#### 2. 并发访问

Query 方法可以被多个线程同时调用：

```cpp
// 线程 1：读取状态
DesktopBehaviors state1 = strategy.query();

// 线程 2：同时读取状态（无竞态条件）
DesktopBehaviors state2 = strategy.query();

// 线程 3：修改状态（需要同步）
std::lock_guard<std::mutex> lock(mtx);
strategy.action(widget);
```text

#### 3. 前置条件检查

在执行 Action 之前检查状态：

```cpp
// 检查当前状态后再决定是否执行 action
DesktopBehaviors current = strategy.query();
if (!current.testFlag(DesktopBehaviorFlag::Fullscreen)) {
    // 只有在非全屏状态下才执行全屏操作
    strategy.action(widget);
}
```yaml

参考资料：
- [CQRS - Martin Fowler](https://martinfowler.com/bliki/CQRS.html)
- [Command-Query Separation - Wikipedia](https://en.wikipedia.org/wiki/Command%E2%80%93query_separation)
- [CQRS Pattern - Microsoft Azure Architecture Center](https://learn.microsoft.com/en-us/azure/architecture/patterns/cqrs)

---

## 示例策略实现

### 5.1 FullscreenStrategy

全屏策略负责将窗口设置为全屏模式：

```cpp
/**
 * @file FullscreenStrategy.h
 * @brief 全屏策略实现
 */

namespace cf::desktop::platform_strategy {

/**
 * @brief 全屏策略
 *
 * @details 将窗口设置为全屏模式，隐藏标题栏和边框
 */
class FullscreenStrategy : public IDesktopDisplaySizeStrategy {
public:
    FullscreenStrategy() = default;
    ~FullscreenStrategy() override = default;

    /**
     * @brief 获取策略名称
     */
    const char* name() const noexcept override {
        return "Fullscreen Strategy";
    }

    /**
     * @brief 应用全屏模式
     *
     * @param widget 目标窗口
     * @return 成功返回 true
     */
    bool action(QWidget* widget) override {
        if (!widget) {
            qWarning() << "FullscreenStrategy: null widget";
            return false;
        }

        // 幂等性检查
        if (widget->isFullScreen()) {
            qDebug() << "Widget already in fullscreen mode";
            return true;
        }

        // 应用全屏
        widget->showFullScreen();

        qDebug() << "Applied fullscreen to" << widget;
        return true;
    }

    /**
     * @brief 查询当前状态
     */
    DesktopBehaviors query() const override {
        return DesktopBehaviorFlag::Fullscreen |
               DesktopBehaviorFlag::Frameless;
    }
};

} // namespace
```text

### 5.2 FramelessStrategy

无边框策略负责移除窗口的标题栏和边框：

```cpp
/**
 * @file FramelessStrategy.h
 * @brief 无边框策略实现
 */

namespace cf::desktop::platform_strategy {

/**
 * @brief 无边框策略
 *
 * @details 移除窗口的标题栏和边框，通常配合自定义标题栏使用
 */
class FramelessStrategy : public IDesktopDisplaySizeStrategy {
public:
    FramelessStrategy() = default;
    ~FramelessStrategy() override = default;

    const char* name() const noexcept override {
        return "Frameless Strategy";
    }

    bool action(QWidget* widget) override {
        if (!widget) {
            qWarning() << "FramelessStrategy: null widget";
            return false;
        }

        // 获取当前窗口标志
        Qt::WindowFlags flags = widget->windowFlags();

        // 幂等性检查
        if (flags & Qt::FramelessWindowHint) {
            qDebug() << "Widget already frameless";
            return true;
        }

        // 添加无边框标志
        flags |= Qt::FramelessWindowHint;

        // 设置窗口标志（会触发窗口重建）
        widget->setWindowFlags(flags);

        // 显示窗口（setWindowFlags 会隐藏窗口）
        widget->show();

        qDebug() << "Applied frameless to" << widget;
        return true;
    }

    DesktopBehaviors query() const override {
        return DesktopBehaviorFlag::Frameless;
    }
};

} // namespace
```text

### 5.3 WSL 平台策略

WSL（Windows Subsystem for Linux）平台的特殊实现：

```cpp
/**
 * @file linux_wsl_display_size_policy.h
 * @brief WSL 平台显示策略
 */

namespace cf::desktop::platform_strategy::wsl {

/**
 * @brief WSL 显示尺寸策略
 *
 * @details WSL 环境下的特殊实现，需要处理 Windows/WSL 边界
 */
class DisplaySizePolicyMaker : public IDesktopDisplaySizeStrategy {
public:
    DisplaySizePolicyMaker();
    ~DisplaySizePolicyMaker();

    const char* name() const noexcept override {
        return "WSL Desktop Size Policy";
    }

    /**
     * @brief 应用 WSL 特定的窗口行为
     *
     * @details WSL 环境下：
     *          1. 需要检测是否在 WSL 内运行
     *          2. 某些窗口标志可能不可用
     *          3. 可能需要通过 WSLg (WSL GUI) 处理
     */
    bool action(QWidget* widget_data) override {
        if (!widget_data) {
            return false;
        }

        // WSL 特定逻辑
        #ifdef Q_OS_WIN
        // 运行在 Windows 上，可能需要特殊处理
        #elif defined(Q_OS_LINUX)
        // 检测 WSL 环境
        if (isWSLEnvironment()) {
            return applyWSLBehavior(widget_data);
        }
        #endif

        // 默认行为
        return true;
    }

    /**
     * @brief 查询 WSL 支持的行为
     */
    DesktopBehaviors query() const override {
        DesktopBehaviors behaviors = DesktopBehaviorFlag::None;

        #ifdef Q_OS_WIN
        // Windows 上支持全屏
        behaviors |= DesktopBehaviorFlag::Fullscreen;
        #elif defined(Q_OS_LINUX)
        if (isWSLEnvironment()) {
            // WSL 限制支持
            behaviors |= DesktopBehaviorFlag::Frameless;
            behaviors |= DesktopBehaviorFlag::AllowResize;
        }
        #endif

        return behaviors;
    }

private:
    /**
     * @brief 检测是否在 WSL 环境中运行
     */
    bool isWSLEnvironment() const {
        // 检查 /proc/version 是否包含 "Microsoft"
        QFile versionFile("/proc/version");
        if (versionFile.open(QIODevice::ReadOnly)) {
            QByteArray content = versionFile.readAll();
            return content.contains("Microsoft");
        }
        return false;
    }

    /**
     * @brief 应用 WSL 特定的窗口行为
     */
    bool applyWSLBehavior(QWidget* widget) {
        // WSLg 特定实现
        // ...

        return true;
    }
};

} // namespace cf::desktop::platform_strategy::wsl
```text

### 5.4 组合行为策略

一个策略可以组合多个行为标志：

```cpp
/**
 * @brief 桌面覆盖层策略
 *
 * @details 实现类似 Windows 小部件的桌面覆盖层：
 *          - 无边框
 *          - 置底
 *          - 避开系统 UI
 *          - 不接受键盘输入
 */
class DesktopOverlayStrategy : public IDesktopDisplaySizeStrategy {
public:
    const char* name() const noexcept override {
        return "Desktop Overlay Strategy";
    }

    bool action(QWidget* widget) override {
        if (!widget) {
            return false;
        }

        Qt::WindowFlags flags = widget->windowFlags();

        // 移除冲突标志
        flags &= ~Qt::WindowStaysOnTopHint;

        // 添加所需标志
        flags |= Qt::FramelessWindowHint;
        flags |= Qt::WindowStaysOnBottomHint;
        flags |= Qt::WindowDoesNotAcceptFocus;

        widget->setWindowFlags(flags);
        widget->show();

        return true;
    }

    DesktopBehaviors query() const override {
        return DesktopBehaviorFlag::Frameless |
               DesktopBehaviorFlag::StayOnBottom |
               DesktopBehaviorFlag::AvoidSystemUI;
    }
};
```yaml

参考资料：
- [QWidget::setWindowFlags - Qt Documentation](https://doc.qt.io/qt-6/qwidget.html#windowFlags-prop)
- [QWidget::showFullScreen - Qt Documentation](https://doc.qt.io/qt-6/qwidget.html#showFullScreen)
- [WSL GUI (WSLg) Documentation - Microsoft Learn](https://learn.microsoft.com/en-us/windows/wsl/tutorials/gui-apps)

---

## 多策略组合实现

### 6.1 策略组合模式

单个策略往往不足以应对复杂的场景，需要组合多个策略：

```cpp
/**
 * @brief 策略组合接口
 *
 * @details 允许将多个策略组合成一个复合策略
 */
class ICompositeStrategy : public IDesktopDisplaySizeStrategy {
public:
    /**
     * @brief 添加子策略
     *
     * @param strategy 要添加的策略
     * @return 成功返回 true
     */
    virtual bool addStrategy(std::shared_ptr<IDesktopDisplaySizeStrategy> strategy) = 0;

    /**
     * @brief 移除子策略
     *
     * @param strategy 要移除的策略
     * @return 成功返回 true
     */
    virtual bool removeStrategy(IDesktopDisplaySizeStrategy* strategy) = 0;

    /**
     * @brief 获取子策略数量
     */
    virtual size_t strategyCount() const = 0;

    /**
     * @brief 清空所有子策略
     */
    virtual void clearStrategies() = 0;
};
```text

### 6.2 CompositeStrategy

复合策略的实现：

```cpp
/**
 * @file CompositeStrategy.h
 * @brief 复合策略实现
 */

namespace cf::desktop::platform_strategy {

/**
 * @brief 复合策略
 *
 * @details 将多个策略组合成一个，按顺序执行所有策略
 */
class CompositeStrategy : public ICompositeStrategy {
public:
    CompositeStrategy() = default;
    ~CompositeStrategy() override = default;

    const char* name() const noexcept override {
        return "Composite Strategy";
    }

    bool addStrategy(std::shared_ptr<IDesktopDisplaySizeStrategy> strategy) override {
        if (!strategy) {
            return false;
        }

        strategies_.push_back(strategy);
        return true;
    }

    bool removeStrategy(IDesktopDisplaySizeStrategy* strategy) override {
        auto it = std::remove_if(strategies_.begin(), strategies_.end(),
            [strategy](const auto& ptr) {
                return ptr.get() == strategy;
            });

        if (it != strategies_.end()) {
            strategies_.erase(it, strategies_.end());
            return true;
        }
        return false;
    }

    size_t strategyCount() const override {
        return strategies_.size();
    }

    void clearStrategies() override {
        strategies_.clear();
    }

    /**
     * @brief 应用所有子策略
     *
     * @details 按添加顺序执行所有策略
     *          如果任一策略失败，继续执行后续策略
     */
    bool action(QWidget* widget) override {
        if (!widget) {
            return false;
        }

        bool all_success = true;

        for (auto& strategy : strategies_) {
            if (!strategy->action(widget)) {
                qWarning() << "Strategy" << strategy->name() << "failed";
                all_success = false;
            }
        }

        return all_success;
    }

    /**
     * @brief 查询所有子策略的行为组合
     *
     * @details 将所有子策略的行为标志组合在一起
     */
    DesktopBehaviors query() const override {
        DesktopBehaviors combined = DesktopBehaviorFlag::None;

        for (const auto& strategy : strategies_) {
            combined |= strategy->query();
        }

        return combined;
    }

private:
    std::vector<std::shared_ptr<IDesktopDisplaySizeStrategy>> strategies_;
};

} // namespace
```text

### 6.3 策略链模式

策略链模式允许策略按链式执行，每个策略可以决定是否继续传递：

```cpp
/**
 * @brief 策略链上下文
 *
 * @details 包含策略执行过程中的状态信息
 */
struct StrategyChainContext {
    QWidget* widget;              ///< 目标窗口
    bool stop_processing;         ///< 是否停止后续策略
    DesktopBehaviors applied;     ///< 已应用的行为
    QString error_message;        ///< 错误消息

    StrategyChainContext(QWidget* w)
        : widget(w), stop_processing(false), applied(DesktopBehaviorFlag::None) {}
};

/**
 * @brief 链式策略接口
 *
 * @details 每个策略可以决定是否继续执行后续策略
 */
class IChainStrategy : public IDesktopDisplaySizeStrategy {
public:
    /**
     * @brief 执行策略（链式）
     *
     * @param widget 目标窗口
     * @param context 链上下文
     * @return 如果设置 context.stop_processing，停止后续策略
     */
    virtual bool process(QWidget* widget, StrategyChainContext& context) = 0;
};

/**
 * @brief 策略链
 *
 * @details 按顺序执行链中的策略，支持提前终止
 */
class StrategyChain : public IDesktopDisplaySizeStrategy {
public:
    const char* name() const noexcept override {
        return "Strategy Chain";
    }

    void addStrategy(std::shared_ptr<IChainStrategy> strategy) {
        chain_.push_back(strategy);
    }

    bool action(QWidget* widget) override {
        if (!widget) {
            return false;
        }

        StrategyChainContext context(widget);

        for (auto& strategy : chain_) {
            if (!strategy->process(widget, context)) {
                // 策略失败
                qWarning() << "Chain strategy" << strategy->name() << "failed";
            }

            // 检查是否需要停止
            if (context.stop_processing) {
                qDebug() << "Chain stopped at" << strategy->name()
                         << "reason:" << context.error_message;
                break;
            }
        }

        return true;
    }

    DesktopBehaviors query() const override {
        DesktopBehaviors combined = DesktopBehaviorFlag::None;
        for (const auto& strategy : chain_) {
            combined |= strategy->query();
        }
        return combined;
    }

private:
    std::vector<std::shared_ptr<IChainStrategy>> chain_;
};
```text

### 6.4 条件策略

根据条件选择不同的策略：

```cpp
/**
 * @brief 条件策略
 *
 * @details 根据运行时条件选择不同的策略
 */
class ConditionalStrategy : public IDesktopDisplaySizeStrategy {
public:
    using ConditionFunc = std::function<bool()>;

    ConditionalStrategy(
        ConditionFunc condition,
        std::shared_ptr<IDesktopDisplaySizeStrategy> true_strategy,
        std::shared_ptr<IDesktopDisplaySizeStrategy> false_strategy
    ) : condition_(condition),
        true_strategy_(true_strategy),
        false_strategy_(false_strategy) {}

    const char* name() const noexcept override {
        return "Conditional Strategy";
    }

    bool action(QWidget* widget) override {
        auto strategy = condition_() ? true_strategy_ : false_strategy_;
        if (!strategy) {
            return false;
        }
        return strategy->action(widget);
    }

    DesktopBehaviors query() const override {
        auto strategy = condition_() ? true_strategy_ : false_strategy_;
        if (!strategy) {
            return DesktopBehaviorFlag::None;
        }
        return strategy->query();
    }

private:
    ConditionFunc condition_;
    std::shared_ptr<IDesktopDisplaySizeStrategy> true_strategy_;
    std::shared_ptr<IDesktopDisplaySizeStrategy> false_strategy_;
};

// 使用示例
auto createConditionalStrategy() {
    return std::make_shared<ConditionalStrategy>(
        []() {
            // 条件：检测是否在 Wayland 上运行
            return QGuiApplication::platformName() == "wayland";
        },
        std::make_shared<WaylandStrategy>(),      // true 分支
        std::make_shared<X11Strategy>()           // false 分支
    );
}
```yaml

参考资料：
- [Composite Pattern - Refactoring.Guru](https://refactoring.guru/design-patterns/composite)
- [Chain of Responsibility Pattern - Refactoring.Guru](https://refactoring.guru/design-patterns/chain-of-responsibility)

---

## 冲突检测机制

### 7.1 行为冲突定义

某些行为标志之间存在互斥关系，不能同时应用：

```cpp
/**
 * @brief 行为冲突规则
 *
 * @details 定义了行为标志之间的冲突关系
 */
namespace BehaviorConflictRules {

/**
 * @brief 冲突规则定义
 */
struct ConflictRule {
    DesktopBehaviors flags;              ///< 要检查的标志组合
    DesktopBehaviors conflicts_with;     ///< 与之冲突的标志
    const char* reason;                  ///< 冲突原因说明
};

/**
 * @brief 定义的所有冲突规则
 */
inline const std::vector<ConflictRule> kConflictRules = {
    // 置顶和置底冲突
    {
        DesktopBehaviorFlag::StayOnTop,
        DesktopBehaviorFlag::StayOnBottom,
        "Window cannot stay on both top and bottom"
    },
    // 全屏模式下通常不允许调整大小
    {
        DesktopBehaviorFlag::Fullscreen,
        DesktopBehaviorFlag::AllowResize,
        "Fullscreen windows typically don't allow resize"
    },
    // 避开系统 UI 通常需要无边框
    {
        DesktopBehaviorFlag::AvoidSystemUI,
        DesktopBehaviorFlag::None,  // 不与其他标志冲突
        "AvoidSystemUI is typically used with Frameless"
    },
};

} // namespace BehaviorConflictRules
```text

### 7.2 冲突检测接口

```cpp
/**
 * @brief 冲突检测结果
 */
struct ConflictDetectionResult {
    bool has_conflicts;           ///< 是否存在冲突
    DesktopBehaviors conflicts;   ///< 冲突的标志
    std::vector<QString> reasons; ///< 冲突原因列表

    ConflictDetectionResult()
        : has_conflicts(false), conflicts(DesktopBehaviorFlag::None) {}

    void addConflict(DesktopBehaviorFlag flag, const char* reason) {
        has_conflicts = true;
        conflicts |= flag;
        reasons.push_back(reason);
    }

    QString toString() const {
        if (!has_conflicts) {
            return "No conflicts";
        }

        QString result = "Conflicts detected:\n";
        for (const auto& reason : reasons) {
            result += "  - ";
            result += reason;
            result += "\n";
        }
        return result;
    }
};

/**
 * @brief 冲突检测器
 */
class StrategyConflictDetector {
public:
    /**
     * @brief 检测行为冲突
     *
     * @param behaviors 要检测的行为组合
     * @return 冲突检测结果
     */
    static ConflictDetectionResult detect(DesktopBehaviors behaviors) {
        ConflictDetectionResult result;

        for (const auto& rule : BehaviorConflictRules::kConflictRules) {
            // 检查是否包含规则中的标志
            if ((behaviors & rule.flags).operator bool()) {
                // 检查是否同时包含冲突的标志
                if ((behaviors & rule.conflicts_with).operator bool()) {
                    result.addConflict(rule.conflicts_with, rule.reason);
                }
            }
        }

        return result;
    }

    /**
     * @brief 检测策略列表冲突
     *
     * @param strategies 策略列表
     * @return 冲突检测结果
     */
    static ConflictDetectionResult detect(
        const std::vector<std::shared_ptr<IDesktopDisplaySizeStrategy>>& strategies
    ) {
        DesktopBehaviors combined = DesktopBehaviorFlag::None;

        for (const auto& strategy : strategies) {
            combined |= strategy->query();
        }

        return detect(combined);
    }
};
```text

### 7.3 冲突解决策略

```cpp
/**
 * @brief 冲突解决策略枚举
 */
enum class ConflictResolution {
    Fail,           ///< 失败，不应用任何行为
    Warning,        ///< 警告，应用行为但记录警告
    AutoResolve,    ///< 自动解决，移除冲突的行为
    Prioritize      ///< 优先级，使用预定义的优先级
};

/**
 * @brief 冲突解决器
 */
class StrategyConflictResolver {
public:
    /**
     * @brief 解决冲突
     *
     * @param behaviors 原始行为
     * @param resolution 解决策略
     * @return 解决后的行为
     */
    static DesktopBehaviors resolve(
        DesktopBehaviors behaviors,
        ConflictResolution resolution = ConflictResolution::Warning
    ) {
        auto detection = StrategyConflictDetector::detect(behaviors);

        if (!detection.has_conflicts) {
            return behaviors;
        }

        switch (resolution) {
            case ConflictResolution::Fail:
                qWarning() << "Conflict detection failed:" << detection.toString();
                return DesktopBehaviorFlag::None;

            case ConflictResolution::Warning:
                qWarning() << "Conflicts detected (applying anyway):"
                           << detection.toString();
                return behaviors;

            case ConflictResolution::AutoResolve:
                return autoResolve(behaviors, detection);

            case ConflictResolution::Prioritize:
                return prioritizeResolve(behaviors);
        }

        return behaviors;
    }

private:
    /**
     * @brief 自动解决冲突
     *
     * @details 移除冲突的行为标志
     */
    static DesktopBehaviors autoResolve(
        DesktopBehaviors behaviors,
        const ConflictDetectionResult& detection
    ) {
        DesktopBehaviors resolved = behaviors;

        // 移除所有冲突的标志
        resolved &= ~detection.conflicts;

        qInfo() << "Auto-resolved conflicts, removed:" << detection.toString();

        return resolved;
    }

    /**
     * @brief 优先级解决
     *
     * @details 根据预定义的优先级解决冲突
     */
    static DesktopBehaviors prioritizeResolve(DesktopBehaviors behaviors) {
        // 定义优先级（高到低）
        const std::vector<DesktopBehaviorFlag> priority = {
            DesktopBehaviorFlag::Fullscreen,      // 最高优先级
            DesktopBehaviorFlag::Frameless,
            DesktopBehaviorFlag::StayOnTop,
            DesktopBehaviorFlag::StayOnBottom,
            DesktopBehaviorFlag::AvoidSystemUI,
            DesktopBehaviorFlag::AllowResize,     // 最低优先级
        };

        // 按优先级检查冲突，保留高优先级的行为
        for (size_t i = 0; i < priority.size(); ++i) {
            for (size_t j = i + 1; j < priority.size(); ++j) {
                // 检查 i 和 j 是否冲突
                if (behaviors.testFlag(priority[i]) &&
                    behaviors.testFlag(priority[j])) {

                    // 检查是否定义了冲突规则
                    auto rules = BehaviorConflictRules::kConflictRules;
                    for (const auto& rule : rules) {
                        if ((rule.flags & priority[i]).operator bool() ||
                            (rule.conflicts_with & priority[j]).operator bool()) {
                            // 移除低优先级的行为
                            behaviors &= ~priority[j];
                            qInfo() << "Prioritized" << static_cast<int>(priority[i])
                                    << "over" << static_cast<int>(priority[j]);
                        }
                    }
                }
            }
        }

        return behaviors;
    }
};
```text

### 7.4 集成到策略应用

```cpp
/**
 * @brief 带冲突检测的策略应用器
 */
class SafeStrategyApplier {
public:
    /**
     * @brief 应用策略（带冲突检测）
     *
     * @param widget 目标窗口
     * @param strategy 要应用的策略
     * @param resolution 冲突解决策略
     * @return 成功返回 true
     */
    static bool apply(
        QWidget* widget,
        IDesktopDisplaySizeStrategy* strategy,
        ConflictResolution resolution = ConflictResolution::Warning
    ) {
        if (!widget || !strategy) {
            return false;
        }

        // 查询行为
        DesktopBehaviors behaviors = strategy->query();

        // 检测冲突
        auto detection = StrategyConflictDetector::detect(behaviors);

        if (detection.has_conflicts) {
            qWarning() << "Strategy" << strategy->name()
                       << "has conflicts:" << detection.toString();

            // 解决冲突
            behaviors = StrategyConflictResolver::resolve(behaviors, resolution);

            if (behaviors == DesktopBehaviorFlag::None &&
                resolution == ConflictResolution::Fail) {
                return false;
            }
        }

        // 应用策略
        return strategy->action(widget);
    }
};
```yaml

参考资料：
- [Conflict-Free Replicated Data Types (CRDTs) - Wikipedia](https://en.wikipedia.org/wiki/Conflict-free_replicated_data_type)
- [Semantic Conflict Resolution - Microsoft Research](https://www.microsoft.com/en-us/research/publication/semantic-conflict-resolution/)

---

## 工厂模式集成

### 8.1 PlatformFactory 设计

工厂模式与策略模式结合，提供策略的创建和管理：

```cpp
/**
 * @file DesktopPropertyStrategyFactory.h
 * @brief 桌面属性策略工厂
 */

namespace cf::desktop::platform_strategy {

/**
 * @brief 平台工厂
 *
 * @details 负责创建和管理平台特定的策略实例
 */
class PlatformFactory {
public:
    PlatformFactory();
    ~PlatformFactory();

    /**
     * @brief 策略删除器类型
     *
     * @details 使用自定义删除器确保正确的内存管理
     */
    using StrategyDeleter = void (*)(IDesktopPropertyStrategy*);

    /**
     * @brief 创建策略（unique_ptr 版本）
     *
     * @param t 策略类型
     * @return 策略的 unique_ptr，使用自定义删除器
     *
     * @note 返回的策略调用者拥有所有权
     * @note 使用工厂的删除器确保跨 DLL 边界的安全释放
     */
    std::unique_ptr<IDesktopPropertyStrategy, StrategyDeleter>
    factorize_unique(const IDesktopPropertyStrategy::StrategyType t);

    /**
     * @brief 创建策略（shared_ptr 版本）
     *
     * @param t 策略类型
     * @return 策略的 shared_ptr
     *
     * @note 允许多个调用者共享策略所有权
     * @note 使用工厂的删除器确保正确的释放
     */
    std::shared_ptr<IDesktopPropertyStrategy>
    factorize_shared(const IDesktopPropertyStrategy::StrategyType t);

    /**
     * @brief 检查策略类型是否可用
     *
     * @param t 策略类型
     * @return 可用返回 true
     */
    bool isAvailable(const IDesktopPropertyStrategy::StrategyType t) const;

private:
    class PlatformImpl;
    std::unique_ptr<PlatformImpl> impl_;
};

} // namespace
```text

### 8.2 平台特定工厂

每个平台可以提供自己的工厂实现：

```cpp
/**
 * @file linux_wsl_factory.h
 * @brief WSL 平台特定工厂
 */

namespace cf::desktop::platform_strategy::wsl {

/**
 * @brief WSL 桌面属性策略工厂
 *
 * @details WSL 平台的策略工厂实现
 */
class WSLDeskProStrategyFactory : public SimpleSingleton<WSLDeskProStrategyFactory> {
public:
    WSLDeskProStrategyFactory();
    ~WSLDeskProStrategyFactory();

    /**
     * @brief 创建策略
     *
     * @param t 策略类型
     * @return 策略指针，由工厂管理生命周期
     *
     * @note 调用者不应手动删除返回的策略
     * @note 使用 release() 释放策略
     */
    IDesktopPropertyStrategy* create(IDesktopPropertyStrategy::StrategyType t);

    /**
     * @brief 释放策略
     *
     * @param ptr 要释放的策略指针
     *
     * @note 只有通过 create() 创建的策略才能通过此方法释放
     */
    void release(IDesktopPropertyStrategy* ptr);

private:
    std::unique_ptr<DisplaySizePolicyMaker> sz_policy;
};

} // namespace
```text

### 8.3 插件化工厂

支持从动态库加载策略：

```cpp
/**
 * @brief 插件策略工厂
 *
 * @details 从动态库加载策略实现
 */
class PluginStrategyFactory {
public:
    /**
     * @brief 加载插件
     *
     * @param plugin_path 插件路径
     * @return 成功返回 true
     *
     * @note 插件必须实现 PluginStrategyInterface
     */
    bool loadPlugin(const QString& plugin_path);

    /**
     * @brief 卸载插件
     *
     * @param plugin_path 插件路径
     */
    void unloadPlugin(const QString& plugin_path);

    /**
     * @brief 从插件创建策略
     *
     * @param plugin_path 插件路径
     * @param strategy_name 策略名称
     * @return 策略指针
     */
    std::shared_ptr<IDesktopPropertyStrategy>
    createFromPlugin(const QString& plugin_path, const QString& strategy_name);

    /**
     * @brief 获取所有可用的插件
     */
    QStringList availablePlugins() const;

private:
    struct PluginInfo {
        QPluginLoader* loader;
        QString name;
        QString version;
        QStringList strategies;
    };

    QHash<QString, PluginInfo> plugins_;
};

/**
 * @brief 插件策略接口
 *
 * @details 所有插件策略必须实现此接口
 */
class PluginStrategyInterface {
public:
    virtual ~PluginStrategyInterface() = default;

    /**
     * @brief 插件名称
     */
    virtual QString pluginName() const = 0;

    /**
     * @brief 插件版本
     */
    virtual QString pluginVersion() const = 0;

    /**
     * @brief 创建策略
     *
     * @param strategy_name 策略名称
     * @return 策略指针
     */
    virtual IDesktopPropertyStrategy* createStrategy(const QString& strategy_name) = 0;

    /**
     * @brief 获取支持的策略列表
     */
    virtual QStringList supportedStrategies() const = 0;
};
```yaml

参考资料：
- [How to Create Qt Plugins - Qt Documentation](https://doc.qt.io/qt-6/plugins-howto.html)
- [Factory Method Pattern - Refactoring.Guru](https://refactoring.guru/design-patterns/factory-method)
- [Abstract Factory Pattern - Refactoring.Guru](https://refactoring.guru/design-patterns/abstract-factory)

---

## 最佳实践与设计原则

### 9.1 策略设计原则

#### 1. 单一职责

每个策略应该只负责一个明确的行为：

```cpp
// ✅ 正确：单一职责
class FullscreenStrategy : public IDesktopDisplaySizeStrategy {
    // 只负责全屏行为
};

// ❌ 错误：多重职责
class WindowStrategy : public IDesktopDisplaySizeStrategy {
    // 同时负责全屏、无边框、置顶等多个行为
    bool action(QWidget* widget) override {
        widget->showFullScreen();
        widget->setWindowFlags(widget->windowFlags() | Qt::FramelessWindowHint);
        // ... 更多行为
    }
};
```text

#### 2. 开闭原则

对扩展开放，对修改封闭：

```cpp
// ✅ 正确：通过添加新策略扩展功能
class NewBehaviorStrategy : public IDesktopDisplaySizeStrategy {
    // 新功能通过新策略实现，不修改现有代码
};

// ❌ 错误：修改现有策略添加新功能
class FullscreenStrategy : public IDesktopDisplaySizeStrategy {
    bool action(QWidget* widget) override {
        widget->showFullScreen();
        // 添加了与全屏无关的新功能
        addNewFeature(widget);  // 违反开闭原则
    }
};
```text

#### 3. 依赖倒置

依赖抽象而非具体实现：

```cpp
// ✅ 正确：依赖抽象接口
class CFDesktop {
private:
    IDesktopDisplaySizeStrategy* display_strategy_;  // 抽象接口
};

// ❌ 错误：依赖具体实现
class CFDesktop {
private:
    FullscreenStrategy* display_strategy_;  // 具体实现
};
```text

### 9.2 命名约定

```cpp
// 策略接口命名：I + 功能 + Strategy
class IDesktopDisplaySizeStrategy;

// 具体策略命名：功能 + Strategy
class FullscreenStrategy;
class FramelessStrategy;

// 平台特定策略：平台 + 功能 + Strategy
class WSLDisplaySizePolicy;
class WindowsDisplaySizePolicy;

// 工厂命名：功能 + Factory
class PlatformFactory;
class StrategyFactory;

// 组合策略：Composite + 功能
class CompositeStrategy;
class StrategyChain;
```text

### 9.3 错误处理

```cpp
/**
 * @brief 策略错误类型
 */
enum class StrategyError {
    None,           ///< 无错误
    NullWidget,     ///< 空指针
    NotSupported,   ///< 不支持的操作
    Conflict,       ///< 行为冲突
    Failed          ///< 操作失败
};

/**
 * @brief 策略结果
 */
struct StrategyResult {
    bool success;           ///< 成功标志
    StrategyError error;    ///< 错误类型
    QString message;        ///< 错误消息

    static StrategyResult ok() {
        return {true, StrategyError::None, QString()};
    }

    static StrategyResult fail(StrategyError error, const QString& msg) {
        return {false, error, msg};
    }
};

// 在策略中使用
class FullscreenStrategy : public IDesktopDisplaySizeStrategy {
public:
    bool action(QWidget* widget) override {
        if (!widget) {
            last_result_ = StrategyResult::fail(
                StrategyError::NullWidget,
                "Widget pointer is null"
            );
            return false;
        }

        // ... 执行操作

        last_result_ = StrategyResult::ok();
        return true;
    }

    StrategyResult getLastResult() const {
        return last_result_;
    }

private:
    StrategyResult last_result_;
};
```text

### 9.4 性能考虑

```cpp
/**
 * @brief 缓存策略结果
 */
class CachedDisplaySizeStrategy : public IDesktopDisplaySizeStrategy {
public:
    CachedDisplaySizeStrategy(std::shared_ptr<IDesktopDisplaySizeStrategy> impl)
        : impl_(impl), cache_valid_(false) {}

    bool action(QWidget* widget) override {
        bool result = impl_->action(widget);

        // action 后使缓存失效
        cache_valid_ = false;

        return result;
    }

    DesktopBehaviors query() const override {
        if (cache_valid_) {
            return cached_behaviors_;
        }

        cached_behaviors_ = impl_->query();
        cache_valid_ = true;

        return cached_behaviors_;
    }

private:
    std::shared_ptr<IDesktopDisplaySizeStrategy> impl_;
    mutable DesktopBehaviors cached_behaviors_;
    mutable bool cache_valid_;
};
```text

### 9.5 线程安全

```cpp
/**
 * @brief 线程安全的策略
 */
class ThreadSafeDisplaySizeStrategy : public IDesktopDisplaySizeStrategy {
public:
    ThreadSafeDisplaySizeStrategy(std::shared_ptr<IDesktopDisplaySizeStrategy> impl)
        : impl_(impl) {}

    bool action(QWidget* widget) override {
        std::lock_guard<std::mutex> lock(mutex_);
        return impl_->action(widget);
    }

    DesktopBehaviors query() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return impl_->query();
    }

private:
    std::shared_ptr<IDesktopDisplaySizeStrategy> impl_;
    mutable std::mutex mutex_;
};
```text

### 9.6 测试策略

```cpp
// 测试辅助类
class StrategyTestHelper {
public:
    /**
     * @brief 创建测试窗口
     */
    static std::unique_ptr<QWidget> createTestWidget() {
        auto widget = std::make_unique<QWidget>();
        widget->resize(800, 600);
        return widget;
    }

    /**
     * @brief 验证策略结果
     */
    static bool verifyStrategyResult(
        IDesktopDisplaySizeStrategy* strategy,
        const DesktopBehaviors& expected
    ) {
        DesktopBehaviors actual = strategy->query();
        return actual == expected;
    }
};

// 测试用例
TEST(FullscreenStrategyTest, ApplyFullscreen) {
    FullscreenStrategy strategy;
    auto widget = StrategyTestHelper::createTestWidget();

    // 应用策略
    bool result = strategy.action(widget.get());
    EXPECT_TRUE(result);

    // 验证结果
    EXPECT_TRUE(StrategyTestHelper::verifyStrategyResult(
        &strategy,
        DesktopBehaviorFlag::Fullscreen | DesktopBehaviorFlag::Frameless
    ));
}
```yaml

---

## 总结

### 核心要点

1. **Strategy Pattern 适用场景**：
   - 跨平台差异处理
   - 行为解耦与单一职责
   - 插件化架构支持
   - 测试友好设计

2. **Action vs Query 分离**：
   - Action：修改状态，有副作用
   - Query：读取状态，无副作用
   - 遵循 CQRS 原则

3. **接口设计**：
   - 虚析构函数
   - ABI 友好设计
   - WeakPtr 支持
   - 自定义删除器

4. **多策略组合**：
   - Composite Pattern
   - Chain of Responsibility
   - 条件策略

5. **冲突检测**：
   - 定义冲突规则
   - 自动检测冲突
   - 多种解决策略

### 参考资源

#### 官方文档
- [Qt 6.10.2 QFlags 官方文档](https://doc.qt.io/qt-6/qflags.html)
- [How to Create Qt Plugins - Qt Documentation](https://doc.qt.io/qt-6/plugins-howto.html)
- [QWidget::setWindowFlags - Qt Documentation](https://doc.qt.io/qt-6/qwidget.html#windowFlags-prop)

#### 设计模式参考
- [Strategy in C++ / Design Patterns - Refactoring.Guru](https://refactoring.guru/design-patterns/strategy/cpp/example)
- [The Strategy Pattern – MC++ BLOG - Modernes C++](https://www.modernescpp.com/index.php/the-strategy-pattern/)
- [Design Patterns: Elements of Reusable Object-Oriented Software - GoF](https://en.wikipedia.org/wiki/Design_Patterns)

#### CQRS 原则
- [CQRS - Martin Fowler](https://martinfowler.com/bliki/CQRS.html)
- [Command-Query Separation - Wikipedia](https://en.wikipedia.org/wiki/Command%E2%80%93query_separation)
- [CQRS Pattern - Microsoft Azure Architecture Center](https://learn.microsoft.com/en-us/azure/architecture/patterns/cqrs)

#### 跨平台开发
- [Best Practices for "Cross-Platform" Development with Qt - Stack Overflow](https://stackoverflow.com/questions/4839350/best-practices-for-cross-platform-development-with-qt)
- [Wayland and Qt - Qt 6.11 文档](https://doc.qt.io/qt-6/wayland-and-qt.html)
- [WSL GUI (WSLg) Documentation - Microsoft Learn](https://learn.microsoft.com/en-us/windows/wsl/tutorials/gui-apps)

---

**文档版本**: 1.0
**最后更新**: 2026-03-27
**作者**: CFDesktop Team
**许可**: MIT License
