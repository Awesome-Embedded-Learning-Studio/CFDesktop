---
title: 桌面行为系统设计：从策略到Window Manager抽象
description: 桌面行为系统设计：从策略到Window Manager抽象 的详细文档
---

# 桌面行为系统设计：从策略到Window Manager抽象

## 目录

1. [系统架构概览](#系统架构概览)
2. [分层架构设计](#分层架构设计)
3. [行为流转流程](#行为流转流程)
4. [冲突解决机制](#冲突解决机制)
5. [插件系统集成](#插件系统集成)
6. [未来扩展方向](#未来扩展方向)
7. [最佳实践总结](#最佳实践总结)

---

## 系统架构概览

### 设计目标

桌面行为系统的核心设计目标是构建一个**可扩展、跨平台、插件化**的窗口行为管理框架。该框架需要：

1. **抽象统一**：将不同平台的具体行为抽象为统一的行为模型
2. **策略可插拔**：支持通过插件方式添加新的行为策略
3. **冲突可控**：提供明确的冲突检测和解决机制
4. **平台无关**：应用层代码不依赖于特定平台 API

### 架构原则

```cpp
// 核心架构原则
namespace desktop::architecture {

// 1. 依赖倒置：高层模块不依赖低层模块，都依赖抽象
// 2. 开闭原则：对扩展开放，对修改关闭
// 3. 单一职责：每个组件只负责一个明确的功能
// 4. 接口隔离：客户端不应该依赖它不需要的接口

} // namespace desktop::architecture
```yaml

---

## 分层架构设计

### 整体分层视图

```text
┌─────────────────────────────────────────────────────────────────┐
│                        Application Layer                        │
│                   (用户代码 / 业务逻辑层)                          │
└─────────────────────────────────────────────────────────────────┘
                                  ↓
┌─────────────────────────────────────────────────────────────────┐
│                      Behavior Abstraction                       │
│                      (行为抽象层 - Domain)                       │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │  DesktopBehaviors (QFlags<DesktopBehaviorFlag>)        │   │
│  │  - 行为查询接口 (IDesktopBehaviorQuery)                 │   │
│  │  - 行为修改接口 (IDesktopBehaviorModifier)              │   │
│  └─────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────┘
                                  ↓
┌─────────────────────────────────────────────────────────────────┐
│                        Strategy Layer                           │
│                       (策略层 - Business)                        │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐         │
│  │  Fullscreen  │  │  Frameless   │  │  StayOnTop   │  ...    │
│  │  Strategy    │  │  Strategy    │  │  Strategy    │         │
│  └──────────────┘  └──────────────┘  └──────────────┘         │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │         Strategy Factory / Registry                    │   │
│  └─────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────┘
                                  ↓
┌─────────────────────────────────────────────────────────────────┐
│                      Qt Integration Layer                       │
│                    (Qt 集成层 - Implementation)                  │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │  Qt WindowFlags / WindowState                          │   │
│  │  QWidget / QWindow 行为适配                             │   │
│  └─────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────┘
                                  ↓
┌─────────────────────────────────────────────────────────────────┐
│                      Platform Abstraction                       │
│                    (平台抽象层 - Infrastructure)                 │
│  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐           │
│  │ Windows │  │   X11   │  │ Wayland │  │ Embedded│  ...     │
│  └─────────┘  └─────────┘  └─────────┘  └─────────┘           │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │     Qt Platform Abstraction (QPA)                      │   │
│  └─────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────┘
                                  ↓
┌─────────────────────────────────────────────────────────────────┐
│                      Window Manager / OS                         │
│                   (窗口管理器 / 操作系统层)                       │
└─────────────────────────────────────────────────────────────────┘
```text

### 各层职责详解

#### 1. Application Layer（应用层）

**职责**：业务逻辑和用户交互

```cpp
// 应用层代码示例
namespace desktop::app {

class MainWindow : public QWidget {
public:
    void enableFullscreenMode() {
        // 直接使用行为抽象，不关心具体实现
        auto behaviors = m_behaviorQuery->behaviors();
        behaviors |= DesktopBehaviorFlag::Fullscreen;
        m_behaviorModifier->setBehaviors(behaviors);
    }

private:
    IDesktopBehaviorQuery* m_behaviorQuery;
    IDesktopBehaviorModifier* m_behaviorModifier;
};

} // namespace desktop::app
```text

**特点**：
- 完全不依赖 Qt 具体类
- 不包含平台相关代码
- 易于测试和模拟

#### 2. Behavior Abstraction Layer（行为抽象层）

**职责**：定义行为模型和操作接口

```cpp
// DesktopBehaviorAbstraction.h
#pragma once
#include <QFlags>

namespace desktop::abstraction {

// 行为标志定义
enum class DesktopBehaviorFlag {
    None           = 0,
    Fullscreen     = 1 << 0,
    Frameless      = 1 << 1,
    StayOnTop      = 1 << 2,
    StayOnBottom   = 1 << 3,
    AllowResize    = 1 << 4,
    AvoidSystemUI  = 1 << 5,
    Transparent    = 1 << 6,
    ClickThrough   = 1 << 7,
    Modal          = 1 << 8,
    Tool           = 1 << 9,
};

Q_DECLARE_FLAGS(DesktopBehaviors, DesktopBehaviorFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(DesktopBehaviors)

// 行为查询接口
class IDesktopBehaviorQuery {
public:
    virtual ~IDesktopBehaviorQuery() = default;

    virtual DesktopBehaviors behaviors() const = 0;
    virtual bool hasBehavior(DesktopBehaviorFlag flag) const = 0;
    virtual bool hasAnyBehavior(DesktopBehaviors flags) const = 0;
    virtual bool hasAllBehaviors(DesktopBehaviors flags) const = 0;
};

// 行为修改接口
class IDesktopBehaviorModifier {
public:
    virtual ~IDesktopBehaviorModifier() = default;

    virtual void setBehaviors(DesktopBehaviors flags) = 0;
    virtual void addBehaviors(DesktopBehaviors flags) = 0;
    virtual void removeBehaviors(DesktopBehaviors flags) = 0;
    virtual void clearBehaviors() = 0;
};

} // namespace desktop::abstraction
```text

#### 3. Strategy Layer（策略层）

**职责**：实现具体的行为策略

```cpp
// StrategyLayer.h
#pragma once
#include "DesktopBehaviorAbstraction.h"

namespace desktop::strategy {

// 策略接口
class IDesktopBehaviorStrategy {
public:
    virtual ~IDesktopBehaviorStrategy() = default;

    // Query：查询策略能提供的行为
    virtual DesktopBehaviors query() const = 0;

    // Action：应用策略到目标
    virtual bool apply(IDesktopBehaviorModifier* modifier) = 0;

    // 优先级（用于冲突解决）
    virtual int priority() const = 0;

    // 策略名称
    virtual QString name() const = 0;
};

// 策略工厂
class IStrategyFactory {
public:
    virtual ~IStrategyFactory() = default;

    virtual IDesktopBehaviorStrategy* create(const QString& strategyName) = 0;
    virtual QStringList availableStrategies() const = 0;
};

// 策略注册表
class StrategyRegistry {
public:
    static StrategyRegistry& instance();

    void registerFactory(const QString& name, IStrategyFactory* factory);
    IDesktopBehaviorStrategy* createStrategy(const QString& name);

private:
    QMap<QString, IStrategyFactory*> m_factories;
};

} // namespace desktop::strategy
```text

#### 4. Qt Integration Layer（Qt 集成层）

**职责**：将行为抽象转换为 Qt API 调用

```cpp
// QtIntegrationLayer.h
#pragma once
#include "DesktopBehaviorAbstraction.h"
#include <QWidget>

namespace desktop::qtintegration {

// Qt 行为修改器实现
class QtBehaviorModifier : public abstraction::IDesktopBehaviorModifier {
public:
    explicit QtBehaviorModifier(QWidget* target)
        : m_target(target) {}

    void setBehaviors(DesktopBehaviors flags) override {
        if (!m_target) return;

        Qt::WindowFlags windowFlags = m_target->windowFlags();

        // 清除相关标志
        windowFlags &= ~(Qt::FramelessWindowHint |
                        Qt::WindowStaysOnTopHint |
                        Qt::WindowStaysOnBottomHint |
                        Qt::ToolTip |
                        Qt::SplashScreen);

        // 应用新标志
        if (flags.testFlag(DesktopBehaviorFlag::Frameless))
            windowFlags |= Qt::FramelessWindowHint;
        if (flags.testFlag(DesktopBehaviorFlag::StayOnTop))
            windowFlags |= Qt::WindowStaysOnTopHint;
        if (flags.testFlag(DesktopBehaviorFlag::StayOnBottom))
            windowFlags |= Qt::WindowStaysOnBottomHint;
        if (flags.testFlag(DesktopBehaviorFlag::Tool))
            windowFlags |= Qt::Tool;
        if (flags.testFlag(DesktopBehaviorFlag::Splash))
            windowFlags |= Qt::SplashScreen;

        m_target->setWindowFlags(windowFlags);

        // 处理全屏
        if (flags.testFlag(DesktopBehaviorFlag::Fullscreen)) {
            m_target->showFullScreen();
        } else if (m_target->isFullScreen()) {
            m_target->showNormal();
        }

        // 处理大小调整
        if (!flags.testFlag(DesktopBehaviorFlag::AllowResize)) {
            m_target->setFixedSize(m_target->size());
        }
    }

    void addBehaviors(DesktopBehaviors flags) override {
        auto current = queryCurrent();
        setBehaviors(current | flags);
    }

    void removeBehaviors(DesktopBehaviors flags) override {
        auto current = queryCurrent();
        setBehaviors(current & ~flags);
    }

    void clearBehaviors() override {
        setBehaviors(DesktopBehaviorFlag::None);
    }

private:
    DesktopBehaviors queryCurrent() const {
        DesktopBehaviors result = DesktopBehaviorFlag::None;
        if (m_target->isFullScreen())
            result |= DesktopBehaviorFlag::Fullscreen;
        if (m_target->windowFlags() & Qt::FramelessWindowHint)
            result |= DesktopBehaviorFlag::Frameless;
        if (m_target->windowFlags() & Qt::WindowStaysOnTopHint)
            result |= DesktopBehaviorFlag::StayOnTop;
        if (m_target->windowFlags() & Qt::WindowStaysOnBottomHint)
            result |= DesktopBehaviorFlag::StayOnBottom;
        if (m_target->windowFlags() & Qt::Tool)
            result |= DesktopBehaviorFlag::Tool;
        return result;
    }

    QWidget* m_target;
};

// Qt 行为查询器实现
class QtBehaviorQuery : public abstraction::IDesktopBehaviorQuery {
public:
    explicit QtBehaviorQuery(QWidget* target)
        : m_target(target) {}

    DesktopBehaviors behaviors() const override {
        DesktopBehaviors result = DesktopBehaviorFlag::None;

        if (m_target->isFullScreen())
            result |= DesktopBehaviorFlag::Fullscreen;

        Qt::WindowFlags flags = m_target->windowFlags();
        if (flags & Qt::FramelessWindowHint)
            result |= DesktopBehaviorFlag::Frameless;
        if (flags & Qt::WindowStaysOnTopHint)
            result |= DesktopBehaviorFlag::StayOnTop;
        if (flags & Qt::WindowStaysOnBottomHint)
            result |= DesktopBehaviorFlag::StayOnBottom;
        if (flags & Qt::Tool)
            result |= DesktopBehaviorFlag::Tool;
        if (flags & Qt::SplashScreen)
            result |= DesktopBehaviorFlag::Splash;

        // 推断行为
        if (canResize())
            result |= DesktopBehaviorFlag::AllowResize;

        return result;
    }

    bool hasBehavior(DesktopBehaviorFlag flag) const override {
        return behaviors().testFlag(flag);
    }

    bool hasAnyBehavior(DesktopBehaviors flags) const override {
        return (behaviors() & flags) != DesktopBehaviorFlag::None;
    }

    bool hasAllBehaviors(DesktopBehaviors flags) const override {
        return (behaviors() & flags) == flags;
    }

private:
    bool canResize() const {
        return m_target->minimumSize().isEmpty() &&
               m_target->maximumSize().isEmpty();
    }

    QWidget* m_target;
};

} // namespace desktop::qtintegration
```text

#### 5. Platform Abstraction Layer（平台抽象层）

**职责**：处理平台特定的行为差异

Qt 本身提供了 **Qt Platform Abstraction (QPA)** 层来处理平台差异。根据 [Qt Platform Abstraction 官方文档](https://doc.qt.io/qt-6/qpa.html)：

> "The Qt Platform Abstraction (QPA) is the main platform abstraction layer in Qt.
> The API can be identified by the QPlatform* class prefix."

在我们的系统中，可以利用 QPA 来处理平台特定的行为：

```cpp
// PlatformAbstraction.h
#pragma once
#include <QGuiApplication>

namespace desktop::platform {

enum class PlatformType {
    Windows,
    macOS,
    X11,
    Wayland,
    Embedded,
    Unknown
};

class PlatformInfo {
public:
    static PlatformType currentPlatform() {
        QString platformName = QGuiApplication::platformName();

        if (platformName == "windows")
            return PlatformType::Windows;
        if (platformName == "cocoa" || platformName == "macos")
            return PlatformType::macOS;
        if (platformName == "xcb")
            return PlatformType::X11;
        if (platformName == "wayland")
            return PlatformType::Wayland;
        if (platformName.startsWith("eglfs") ||
            platformName == "linuxfb" ||
            platformName == "offscreen")
            return PlatformType::Embedded;

        return PlatformType::Unknown;
    }

    static bool supportsAlwaysOnTop(PlatformType platform) {
        switch (platform) {
            case PlatformType::Windows:
            case PlatformType::macOS:
            case PlatformType::X11:
                return true;
            case PlatformType::Wayland:
                return false;  // Wayland 不支持
            case PlatformType::Embedded:
                return false;
            default:
                return false;
        }
    }

    static bool supportsFrameless(PlatformType platform) {
        // 所有平台都支持无边框
        return true;
    }

    static bool supportsClickThrough(PlatformType platform) {
        switch (platform) {
            case PlatformType::Windows:
                return true;
            case PlatformType::X11:
                return true;
            default:
                return false;
        }
    }
};

// 平台行为过滤器
class PlatformBehaviorFilter {
public:
    static DesktopBehaviors filter(DesktopBehaviors behaviors) {
        PlatformType platform = PlatformInfo::currentPlatform();

        // 移除不支持的标志
        if (!PlatformInfo::supportsAlwaysOnTop(platform)) {
            behaviors &= ~DesktopBehaviorFlag::StayOnTop;
            behaviors &= ~DesktopBehaviorFlag::StayOnBottom;
        }

        if (!PlatformInfo::supportsClickThrough(platform)) {
            behaviors &= ~DesktopBehaviorFlag::ClickThrough;
        }

        return behaviors;
    }
};

} // namespace desktop::platform
```text

### 层间通信协议

```cpp
// LayerCommunication.h
#pragma once
#include "DesktopBehaviorAbstraction.h"
#include "QtIntegrationLayer.h"
#include "PlatformAbstraction.h"

namespace desktop::communication {

// 行为请求
struct BehaviorRequest {
    DesktopBehaviors desired;
    DesktopBehaviors current;
    QString context;  // 请求上下文（如 "user_action", "plugin_request"）
};

// 行为响应
struct BehaviorResponse {
    bool success;
    DesktopBehaviors applied;
    QStringList warnings;
    QStringList errors;
};

// 行为协调器（连接各层）
class BehaviorCoordinator {
public:
    BehaviorResponse apply(const BehaviorRequest& request) {
        BehaviorResponse response;

        // 1. 平台过滤
        DesktopBehaviors filtered =
            platform::PlatformBehaviorFilter::filter(request.desired);

        // 2. 冲突解决
        DesktopBehaviors resolved =
            m_conflictResolver.resolve(filtered, request.current);

        // 3. 应用到 Qt
        response.success = m_qtModifier->setBehaviors(resolved);
        response.applied = resolved;

        // 4. 检查警告
        if (filtered != request.desired) {
            response.warnings << "Some behaviors were filtered due to platform limitations";
        }

        return response;
    }

private:
    qtintegration::QtBehaviorModifier* m_qtModifier;
    ConflictResolver m_conflictResolver;
};

} // namespace desktop::communication
```yaml

---

## 行为流转流程

### 完整流程图

```text
┌─────────────────────────────────────────────────────────────────┐
│                        行为变更请求                              │
│                    (User / Plugin / System)                     │
└─────────────────────────────────────────────────────────────────┘
                                  ↓
┌─────────────────────────────────────────────────────────────────┐
│                        1. Strategy 阶段                          │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │  - 调用相关策略的 query() 获取行为                     │   │
│  │  - 策略可能来自多个来源 (User, Plugin, System)        │   │
│  └─────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────┘
                                  ↓
┌─────────────────────────────────────────────────────────────────┐
│                        2. Merge 阶段                             │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │  - 合并来自不同策略的行为请求                          │   │
│  │  - 使用位或运算: final = s1 | s2 | s3                 │   │
│  └─────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────┘
                                  ↓
┌─────────────────────────────────────────────────────────────────┐
│                        3. Resolve 阶段                           │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │  - 检测行为冲突                                        │   │
│  │  - 应用优先级规则                                      │   │
│  │  - 执行冲突解决策略                                    │   │
│  └─────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────┘
                                  ↓
┌─────────────────────────────────────────────────────────────────┐
│                        4. Filter 阶段                            │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │  - 平台能力过滤                                        │   │
│  │  - 移除当前平台不支持的行为                            │   │
│  └─────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────┘
                                  ↓
┌─────────────────────────────────────────────────────────────────┐
│                        5. Action 阶段                            │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │  - 调用策略的 apply() 方法                             │   │
│  │  - 将行为转换为 Qt API 调用                            │   │
│  │  - 更新窗口状态                                        │   │
│  └─────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────┘
                                  ↓
┌─────────────────────────────────────────────────────────────────┐
│                        6. Validate 阶段                          │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │  - 验证行为是否正确应用                                │   │
│  │  - 检测实际状态与期望状态的差异                        │   │
│  │  - 记录任何不一致问题                                  │   │
│  └─────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────┘
                                  ↓
┌─────────────────────────────────────────────────────────────────┐
│                        行为变更完成                              │
│                    (通知观察者 / 触发事件)                       │
└─────────────────────────────────────────────────────────────────┘
```text

### 流程代码实现

```cpp
// BehaviorFlow.h
#pragma once
#include "DesktopBehaviorAbstraction.h"
#include "StrategyLayer.h"
#include "PlatformAbstraction.h"
#include <QVector>

namespace desktop::flow {

// 行为变更请求
struct BehaviorChangeRequest {
    DesktopBehaviors desired;
    QString source;  // "user", "plugin_xxx", "system"
    int priority;    // 0-100, 数值越大优先级越高
};

// 行为变更结果
struct BehaviorChangeResult {
    bool success;
    DesktopBehaviors final;
    DesktopBehaviors applied;
    QStringList warnings;
    QStringList errors;
};

// 行为流程引擎
class BehaviorFlowEngine {
public:
    BehaviorChangeResult apply(const QVector<BehaviorChangeRequest>& requests) {
        BehaviorChangeResult result;

        // 阶段 1: Strategy - 收集所有策略的请求
        QVector<StrategyRequest> strategyRequests = collectStrategyRequests(requests);

        // 阶段 2: Merge - 合并请求
        DesktopBehaviors merged = mergeRequests(strategyRequests);

        // 获取当前行为
        DesktopBehaviors current = queryCurrentBehaviors();

        // 阶段 3: Resolve - 解决冲突
        DesktopBehaviors resolved = m_conflictResolver.resolve(merged, current);

        // 阶段 4: Filter - 平台过滤
        DesktopBehaviors filtered = platform::PlatformBehaviorFilter::filter(resolved);

        if (filtered != resolved) {
            result.warnings << "Some behaviors were filtered due to platform limitations";
        }

        // 阶段 5: Action - 应用行为
        if (m_modifier) {
            m_modifier->setBehaviors(filtered);
        }

        // 阶段 6: Validate - 验证结果
        DesktopBehaviors actual = queryCurrentBehaviors();
        result.success = (actual == filtered);
        result.final = actual;
        result.applied = filtered;

        if (!result.success) {
            result.errors << "Applied behaviors do not match expected";
        }

        return result;
    }

    void setModifier(IDesktopBehaviorModifier* modifier) {
        m_modifier = modifier;
    }

    void setQuery(IDesktopBehaviorQuery* query) {
        m_query = m_query;
    }

private:
    struct StrategyRequest {
        DesktopBehaviors behaviors;
        QString source;
        int priority;
    };

    QVector<StrategyRequest> collectStrategyRequests(
        const QVector<BehaviorChangeRequest>& requests) {

        QVector<StrategyRequest> strategyRequests;
        for (const auto& req : requests) {
            strategyRequests.append({
                req.desired,
                req.source,
                req.priority
            });
        }

        // 添加系统策略
        strategyRequests.append({
            m_systemStrategy->query(),
            "system",
            50
        });

        return strategyRequests;
    }

    DesktopBehaviors mergeRequests(const QVector<StrategyRequest>& requests) {
        DesktopBehaviors result = DesktopBehaviorFlag::None;

        // 按优先级排序
        auto sorted = requests;
        std::sort(sorted.begin(), sorted.end(),
            [](const StrategyRequest& a, const StrategyRequest& b) {
                return a.priority > b.priority;
            });

        // 合并行为（高优先级的覆盖低优先级的）
        for (const auto& req : sorted) {
            result |= req.behaviors;
        }

        return result;
    }

    DesktopBehaviors queryCurrentBehaviors() const {
        return m_query ? m_query->behaviors() : DesktopBehaviorFlag::None;
    }

    IDesktopBehaviorModifier* m_modifier = nullptr;
    IDesktopBehaviorQuery* m_query = nullptr;
    IDesktopBehaviorStrategy* m_systemStrategy = nullptr;
    ConflictResolver m_conflictResolver;
};

} // namespace desktop::flow
```text

### 流程监控与调试

```cpp
// FlowMonitoring.h
#pragma once
#include "BehaviorFlow.h"

namespace desktop::flow {

// 流程跟踪器
class FlowTracer {
public:
    void traceRequest(const BehaviorChangeRequest& request) {
        qDebug() << "[FlowTracer] Request from" << request.source
                 << ":" << formatBehaviors(request.desired);
    }

    void traceMerge(const DesktopBehaviors& merged) {
        qDebug() << "[FlowTracer] Merged:" << formatBehaviors(merged);
    }

    void traceConflict(const DesktopBehaviors& conflict) {
        qDebug() << "[FlowTracer] Conflict detected:" << formatBehaviors(conflict);
    }

    void traceResolution(const DesktopBehaviors& resolved,
                        const DesktopBehaviors& removed) {
        qDebug() << "[FlowTracer] Resolved to:" << formatBehaviors(resolved)
                 << "Removed:" << formatBehaviors(removed);
    }

    void traceFilter(const DesktopBehaviors& before,
                     const DesktopBehaviors& after) {
        qDebug() << "[FlowTracer] Filtered:" << formatBehaviors(before)
                 << "->" << formatBehaviors(after);
    }

    void traceResult(const BehaviorChangeResult& result) {
        qDebug() << "[FlowTracer] Result: success=" << result.success;
        if (!result.warnings.isEmpty()) {
            qDebug() << "[FlowTracer] Warnings:" << result.warnings;
        }
        if (!result.errors.isEmpty()) {
            qDebug() << "[FlowTracer] Errors:" << result.errors;
        }
    }

private:
    QString formatBehaviors(DesktopBehaviors behaviors) {
        QStringList flags;
        if (behaviors.testFlag(DesktopBehaviorFlag::Fullscreen))
            flags << "Fullscreen";
        if (behaviors.testFlag(DesktopBehaviorFlag::Frameless))
            flags << "Frameless";
        if (behaviors.testFlag(DesktopBehaviorFlag::StayOnTop))
            flags << "StayOnTop";
        if (behaviors.testFlag(DesktopBehaviorFlag::StayOnBottom))
            flags << "StayOnBottom";
        if (behaviors.testFlag(DesktopBehaviorFlag::AllowResize))
            flags << "AllowResize";
        return flags.join("|");
    }
};

} // namespace desktop::flow
```yaml

---

## 冲突解决机制

### 冲突类型定义

```cpp
// ConflictResolution.h
#pragma once
#include "DesktopBehaviorAbstraction.h"
#include <QPair>

namespace desktop::conflict {

// 冲突类型
enum class ConflictType {
    MutuallyExclusive,    // 互斥冲突（如 StayOnTop vs StayOnBottom）
    Incompatible,         // 不兼容冲突（如 Fullscreen vs AllowResize）
    PlatformUnsupported,  // 平台不支持
    PriorityOverride,     // 优先级覆盖
};

// 冲突定义
struct ConflictDefinition {
    DesktopBehaviorFlag flag1;
    DesktopBehaviorFlag flag2;
    ConflictType type;
    QString description;
    DesktopBehaviorFlag preferred;  // 优先保留的标志
};

// 冲突规则
class ConflictRules {
public:
    static QVector<ConflictDefinition> defaultRules() {
        return {
            // 互斥冲突
            {
                DesktopBehaviorFlag::StayOnTop,
                DesktopBehaviorFlag::StayOnBottom,
                ConflictType::MutuallyExclusive,
                "Window cannot be both on top and on bottom",
                DesktopBehaviorFlag::StayOnTop  // 优先保留 StayOnTop
            },
            // 不兼容冲突
            {
                DesktopBehaviorFlag::Fullscreen,
                DesktopBehaviorFlag::AllowResize,
                ConflictType::Incompatible,
                "Fullscreen window should not be resizable",
                DesktopBehaviorFlag::Fullscreen
            },
            {
                DesktopBehaviorFlag::Modal,
                DesktopBehaviorFlag::Tool,
                ConflictType::Incompatible,
                "Modal window cannot be a tool window",
                DesktopBehaviorFlag::Modal
            },
        };
    }
};

// 解决策略
enum class ResolutionStrategy {
    Fail,           // 失败，不应用任何行为
    Warning,        // 警告，自动解决冲突
    AutoResolve,    // 自动解决（使用优先级）
    Prioritize,     // 使用预定义优先级
    RemoveBoth,     // 移除冲突的两个标志
};

// 冲突解决器
class ConflictResolver {
public:
    ConflictResolver()
        : m_resolutionStrategy(ResolutionStrategy::Prioritize) {
        m_rules = ConflictRules::defaultRules();
    }

    void setResolutionStrategy(ResolutionStrategy strategy) {
        m_resolutionStrategy = strategy;
    }

    DesktopBehaviors resolve(const DesktopBehaviors& desired,
                            const DesktopBehaviors& current) {
        DesktopBehaviors result = desired;
        QVector<Conflict> conflicts = detectConflicts(desired);

        if (!conflicts.isEmpty()) {
            switch (m_resolutionStrategy) {
                case ResolutionStrategy::Fail:
                    return current;  // 不做任何修改

                case ResolutionStrategy::Warning:
                    emitWarning(conflicts);
                    result = autoResolve(result, conflicts);
                    break;

                case ResolutionStrategy::AutoResolve:
                    result = autoResolve(result, conflicts);
                    break;

                case ResolutionStrategy::Prioritize:
                    result = prioritizeResolve(result, conflicts);
                    break;

                case ResolutionStrategy::RemoveBoth:
                    result = removeBothResolve(result, conflicts);
                    break;
            }
        }

        return result;
    }

    QVector<Conflict> detectConflicts(const DesktopBehaviors& behaviors) {
        QVector<Conflict> conflicts;

        for (const auto& rule : m_rules) {
            if (behaviors.testFlag(rule.flag1) &&
                behaviors.testFlag(rule.flag2)) {
                conflicts.append({
                    rule.flag1,
                    rule.flag2,
                    rule.type,
                    rule.description
                });
            }
        }

        return conflicts;
    }

private:
    struct Conflict {
        DesktopBehaviorFlag flag1;
        DesktopBehaviorFlag flag2;
        ConflictType type;
        QString description;
    };

    void emitWarning(const QVector<Conflict>& conflicts) {
        for (const auto& conflict : conflicts) {
            qWarning() << "[ConflictResolver]" << conflict.description;
        }
    }

    DesktopBehaviors autoResolve(const DesktopBehaviors& behaviors,
                                 const QVector<Conflict>& conflicts) {
        DesktopBehaviors result = behaviors;

        for (const auto& conflict : conflicts) {
            // 查找对应的规则
            for (const auto& rule : m_rules) {
                if (conflict.flag1 == rule.flag1 &&
                    conflict.flag2 == rule.flag2) {
                    // 移除非优先标志
                    result &= ~rule.preferred;
                    result |= rule.preferred;
                    break;
                }
            }
        }

        return result;
    }

    DesktopBehaviors prioritizeResolve(const DesktopBehaviors& behaviors,
                                      const QVector<Conflict>& conflicts) {
        DesktopBehaviors result = behaviors;

        // 使用预定义优先级
        QMap<DesktopBehaviorFlag, int> priorities = {
            {DesktopBehaviorFlag::Fullscreen, 100},
            {DesktopBehaviorFlag::Modal, 90},
            {DesktopBehaviorFlag::StayOnTop, 80},
            {DesktopBehaviorFlag::Frameless, 70},
            {DesktopBehaviorFlag::StayOnBottom, 60},
            {DesktopBehaviorFlag::Tool, 50},
            {DesktopBehaviorFlag::AllowResize, 40},
            {DesktopBehaviorFlag::AvoidSystemUI, 30},
            {DesktopBehaviorFlag::Transparent, 20},
            {DesktopBehaviorFlag::ClickThrough, 10},
        };

        for (const auto& conflict : conflicts) {
            int priority1 = priorities.value(conflict.flag1, 0);
            int priority2 = priorities.value(conflict.flag2, 0);

            if (priority1 > priority2) {
                result &= ~conflict.flag2;  // 移除低优先级的
            } else {
                result &= ~conflict.flag1;
            }
        }

        return result;
    }

    DesktopBehaviors removeBothResolve(const DesktopBehaviors& behaviors,
                                      const QVector<Conflict>& conflicts) {
        DesktopBehaviors result = behaviors;

        for (const auto& conflict : conflicts) {
            result &= ~conflict.flag1;
            result &= ~conflict.flag2;
        }

        return result;
    }

    ResolutionStrategy m_resolutionStrategy;
    QVector<ConflictDefinition> m_rules;
};

// 冲突解决结果
struct ResolutionResult {
    bool hasConflicts;
    DesktopBehaviors resolved;
    QStringList warnings;
    QVector<Conflict> conflicts;
};

} // namespace desktop::conflict
```text

### 优先级系统

```cpp
// PrioritySystem.h
#pragma once
#include "DesktopBehaviorAbstraction.h"

namespace desktop::priority {

// 行为优先级
enum class BehaviorPriority {
    Critical = 100,      // 关键行为（如 Modal）
    High = 80,           // 高优先级（如 StayOnTop）
    Medium = 50,         // 中优先级
    Low = 20,            // 低优先级
    Optional = 0         // 可选行为
};

// 优先级管理器
class PriorityManager {
public:
    static BehaviorPriority getPriority(DesktopBehaviorFlag flag) {
        static QMap<DesktopBehaviorFlag, BehaviorPriority> priorities = {
            {DesktopBehaviorFlag::Modal, BehaviorPriority::Critical},
            {DesktopBehaviorFlag::Fullscreen, BehaviorPriority::High},
            {DesktopBehaviorFlag::StayOnTop, BehaviorPriority::High},
            {DesktopBehaviorFlag::Frameless, BehaviorPriority::Medium},
            {DesktopBehaviorFlag::StayOnBottom, BehaviorPriority::Medium},
            {DesktopBehaviorFlag::Tool, BehaviorPriority::Medium},
            {DesktopBehaviorFlag::Splash, BehaviorPriority::Medium},
            {DesktopBehaviorFlag::AllowResize, BehaviorPriority::Low},
            {DesktopBehaviorFlag::AvoidSystemUI, BehaviorPriority::Low},
            {DesktopBehaviorFlag::Transparent, BehaviorPriority::Optional},
            {DesktopBehaviorFlag::ClickThrough, BehaviorPriority::Optional},
        };
        return priorities.value(flag, BehaviorPriority::Optional);
    }

    // 按优先级排序行为
    static QVector<DesktopBehaviorFlag> sortByPriority(
        const QVector<DesktopBehaviorFlag>& flags) {

        QVector<DesktopBehaviorFlag> sorted = flags;
        std::sort(sorted.begin(), sorted.end(),
            [](DesktopBehaviorFlag a, DesktopBehaviorFlag b) {
                return static_cast<int>(getPriority(a)) >
                       static_cast<int>(getPriority(b));
            });
        return sorted;
    }
};

} // namespace desktop::priority
```yaml

---

## 插件系统集成

### Qt 插件架构概述

Qt 提供了强大的插件系统，允许应用程序在运行时动态加载扩展。根据 [How to Create Qt Plugins - Qt Documentation](https://doc.qt.io/qt-6/plugins-howto.html)：

> "Writing a plugin that extends Qt itself is achieved by subclassing the appropriate plugin base class, implementing a few functions, and adding a macro."

### 插件接口定义

```cpp
// DesktopBehaviorPlugin.h
#pragma once
#include <QObject>
#include "StrategyLayer.h"

namespace desktop::plugin {

// 插件接口
class IDesktopBehaviorPlugin {
public:
    virtual ~IDesktopBehaviorPlugin() = default;

    // 插件信息
    virtual QString pluginName() const = 0;
    virtual QString pluginVersion() const = 0;
    virtual QString pluginDescription() const = 0;

    // 策略提供
    virtual QVector<QString> providedStrategies() const = 0;
    virtual IDesktopBehaviorStrategy* createStrategy(
        const QString& strategyName) = 0;

    // 初始化/清理
    virtual bool initialize() = 0;
    virtual void cleanup() = 0;
};

// Qt 插件接口（用于 QPluginLoader）
#define DesktopBehaviorPlugin_iid "com.desktop.behavior.plugin/1.0"

} // namespace desktop::plugin

Q_DECLARE_INTERFACE(desktop::plugin::IDesktopBehaviorPlugin,
                    desktop::plugin::DesktopBehaviorPlugin_iid)
```text

### 插件管理器

```cpp
// PluginManager.h
#pragma once
#include "DesktopBehaviorPlugin.h"
#include <QPluginLoader>
#include <QVector>

namespace desktop::plugin {

// 插件信息
struct PluginInfo {
    QString name;
    QString version;
    QString description;
    QString filePath;
    IDesktopBehaviorPlugin* plugin;
    bool isLoaded;
};

// 插件管理器
class PluginManager : public QObject {
    Q_OBJECT

public:
    static PluginManager& instance();

    // 加载插件
    bool loadPlugin(const QString& pluginPath);

    // 加载插件目录
    int loadPluginsFromDirectory(const QString& directory);

    // 卸载插件
    bool unloadPlugin(const QString& pluginName);

    // 获取所有插件
    QVector<PluginInfo> loadedPlugins() const;

    // 创建策略
    IDesktopBehaviorStrategy* createStrategy(
        const QString& pluginName,
        const QString& strategyName);

    // 获取所有可用策略
    QVector<QString> availableStrategies() const;

signals:
    void pluginLoaded(const QString& pluginName);
    void pluginUnloaded(const QString& pluginName);
    void pluginLoadFailed(const QString& pluginName, const QString& error);

private:
    PluginManager() = default;

    QMap<QString, PluginInfo> m_plugins;
};

// 单例实现
PluginManager& PluginManager::instance() {
    static PluginManager manager;
    return manager;
}

} // namespace desktop::plugin
```text

### 插件实现示例

```cpp
// ExamplePlugin.h
#pragma once
#include "DesktopBehaviorPlugin.h"

namespace desktop::plugin::example {

// 示例策略：全屏策略
class ExampleFullscreenStrategy : public strategy::IDesktopBehaviorStrategy {
public:
    DesktopBehaviors query() const override {
        return DesktopBehaviorFlag::Fullscreen;
    }

    bool apply(IDesktopBehaviorModifier* modifier) override {
        if (!modifier) return false;

        DesktopBehaviors behaviors = modifier->behaviors();
        behaviors |= DesktopBehaviorFlag::Fullscreen;
        behaviors &= ~DesktopBehaviorFlag::AllowResize;  // 全屏时不允许调整大小

        modifier->setBehaviors(behaviors);
        return true;
    }

    int priority() const override {
        return 90;
    }

    QString name() const override {
        return "ExampleFullscreen";
    }
};

// 示例插件
class ExamplePlugin : public QObject,
                      public IDesktopBehaviorPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID DesktopBehaviorPlugin_iid FILE "example.json")
    Q_INTERFACES(desktop::plugin::IDesktopBehaviorPlugin)

public:
    QString pluginName() const override {
        return "ExamplePlugin";
    }

    QString pluginVersion() const override {
        return "1.0.0";
    }

    QString pluginDescription() const override {
        return "Example behavior plugin with fullscreen strategy";
    }

    QVector<QString> providedStrategies() const override {
        return {"ExampleFullscreen"};
    }

    IDesktopBehaviorStrategy* createStrategy(
        const QString& strategyName) override {

        if (strategyName == "ExampleFullscreen") {
            return new ExampleFullscreenStrategy();
        }
        return nullptr;
    }

    bool initialize() override {
        qDebug() << "ExamplePlugin initialized";
        return true;
    }

    void cleanup() override {
        qDebug() << "ExamplePlugin cleaned up";
    }
};

} // namespace desktop::plugin::example
```text

### 插件元数据文件

```json
// example.json
{
    "Keys": ["ExampleFullscreen"],
    "ClassName": "desktop::plugin::example::ExamplePlugin",
    "IID": "com.desktop.behavior.plugin/1.0",
    "MetaData": {
        "Author": "Desktop Team",
        "Date": "2025-01-01",
        "Description": "Example behavior plugin",
        "Version": "1.0.0",
        "MinQtVersion": "6.5.0",
        "RequiredFeatures": []
    }
}
```text

### 动态加载策略

根据 [QPluginLoader Class | Qt Core | Qt 6.10.2](https://doc.qt.io/qt-6/qpluginloader.html)：

```cpp
// DynamicStrategyLoader.h
#pragma once
#include "PluginManager.h"

namespace desktop::plugin {

// 动态策略加载器
class DynamicStrategyLoader {
public:
    // 从插件加载策略
    static IDesktopBehaviorStrategy* loadStrategy(
        const QString& pluginName,
        const QString& strategyName) {

        PluginManager& manager = PluginManager::instance();
        return manager.createStrategy(pluginName, strategyName);
    }

    // 按能力查找策略
    static QVector<IDesktopBehaviorStrategy*> findStrategiesByCapability(
        DesktopBehaviorFlag capability) {

        QVector<IDesktopBehaviorStrategy*> result;
        PluginManager& manager = PluginManager::instance();

        for (const auto& pluginInfo : manager.loadedPlugins()) {
            if (!pluginInfo.isLoaded) continue;

            for (const auto& strategyName : pluginInfo.plugin->providedStrategies()) {
                auto* strategy = pluginInfo.plugin->createStrategy(strategyName);
                if (strategy && strategy->query().testFlag(capability)) {
                    result.append(strategy);
                }
            }
        }

        return result;
    }
};

} // namespace desktop::plugin
```yaml

---

## 未来扩展方向

### 新增行为特性

```cpp
// FutureBehaviors.h
#pragma once
#include "DesktopBehaviorAbstraction.h"

namespace desktop::future {

// 未来可能的行为标志
enum class FutureBehaviorFlag {
    // 现有行为...
    None           = 0,
    Fullscreen     = 1 << 0,
    Frameless      = 1 << 1,
    StayOnTop      = 1 << 2,
    StayOnBottom   = 1 << 3,
    AllowResize    = 1 << 4,
    AvoidSystemUI  = 1 << 5,
    Transparent    = 1 << 6,
    ClickThrough   = 1 << 7,

    // 新增行为
    AlwaysOnTop        = 1 << 10,  ///< 始终置顶（与 StayOnTop 类似但语义不同）
    AlwaysOnBottom     = 1 << 11,  ///< 始终置底
    VirtualDesktop     = 1 << 12,  ///< 虚拟桌面支持
    SkipTaskbar        = 1 << 13,  ///< 不显示在任务栏
    SkipPager          = 1 << 14,  ///< 不显示在页面切换器
    SkipSwitcher       = 1 << 15,  ///< 不显示在窗口切换器
    Above              = 1 << 16,  ///< 位于特定窗口之上
    Below              = 1 << 17,  ///< 位于特定窗口之下
    DemandsAttention   = 1 << 18,  ///< 需要注意（闪烁等）
    Focused            = 1 << 19,  ///< 获取焦点
    AcceptFocus        = 1 << 20,  ///< 接受焦点
    FocusOnButtonClick = 1 << 21,  ///< 点击时获取焦点
    NoFocus            = 1 << 22,  ///< 不获取焦点
    GroupLeader        = 1 << 23,  ///< 组领导者
    Independent        = 1 << 24,  ///< 独立窗口
    BypassWindowManager = 1 << 25, ///< 绕过窗口管理器
    Maximized          = 1 << 26,  ///< 最大化
    Minimized          = 1 << 27,  ///< 最小化
    Active             = 1 << 28,  ///< 活动窗口
    Dock               = 1 << 29,  ///< 停靠窗口
    Desktop            = 1 << 30,  ///< 桌面窗口类型
};

Q_DECLARE_FLAGS(FutureBehaviors, FutureBehaviorFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(FutureBehaviors)

// 扩展的行为接口
class IFutureDesktopBehaviorQuery {
public:
    virtual ~IFutureDesktopBehaviorQuery() = default;

    virtual FutureBehaviors futureBehaviors() const = 0;
    virtual bool hasFutureBehavior(FutureBehaviorFlag flag) const = 0;

    // 虚拟桌面相关
    virtual int currentVirtualDesktop() const = 0;
    virtual void setVirtualDesktop(int desktop) = 0;
    virtual QVector<int> availableVirtualDesktops() const = 0;

    // 窗口层级相关
    virtual void setWindowLevel(WindowLevel level) = 0;
    virtual WindowLevel windowLevel() const = 0;

    // 注意力相关
    virtual void setDemandsAttention(bool demand) = 0;
    virtual bool demandsAttention() const = 0;
};

// 窗口层级定义
enum class WindowLevel {
    Normal,
    Above,
    Below,
    TopMost,
    BottomMost
};

} // namespace desktop::future
```text

### 平台特定扩展

```cpp
// PlatformExtensions.h
#pragma once
#include "DesktopBehaviorAbstraction.h"

namespace desktop::platform {

// Windows 特定扩展
#ifdef Q_OS_WIN
struct WindowsSpecificBehaviors {
    bool appWindow = false;           // 应用程序窗口
    bool toolWindow = false;          // 工具窗口
    bool noActivate = false;          // 不激活
    bool topMost = false;             // 最顶层
};
#endif

// macOS 特定扩展
#ifdef Q_OS_MACOS
struct MacOSSpecificBehaviors {
    bool floatingPanel = false;       // 浮动面板
    bool hudWindow = false;           // HUD 窗口
    bool utilityWindow = false;       // 实用工具窗口
    bool docModal = false;            // 文档模态
};
#endif

// Linux 特定扩展
#ifdef Q_OS_LINUX
struct LinuxSpecificBehaviors {
    bool dockWindow = false;          // 停靠窗口
    bool desktopWidget = false;       // 桌面部件
    bool notification = false;        // 通知窗口
    bool comboBoxPopup = false;       // 组合框弹出
    bool dndPopup = false;            // 拖放弹出
    bool tooltip = false;             // 工具提示
};

// X11 特定
struct X11SpecificBehaviors {
    bool bypassWindowManager = false; // 绕过窗口管理器
    bool x11BypassWM = false;         // X11 绕过 WM
    uint32_t x11WindowType = 0;       // X11 窗口类型
};

// Wayland 特定
struct WaylandSpecificBehaviors {
    bool layerShell = false;          // 层级外壳
    bool xdgShell = false;            // XDG 外壳
    bool subsurface = false;          // 子表面
};
#endif

} // namespace desktop::platform
```yaml

---

## 最佳实践总结

### 架构设计原则

1. **单一职责原则（SRP）**
   - 每个类只负责一个明确的功能
   - Strategy 只负责单一行为的实现
   - Coordinator 只负责流程协调

2. **开闭原则（OCP）**
   - 对扩展开放：通过插件添加新策略
   - 对修改关闭：核心代码不需要修改

3. **依赖倒置原则（DIP）**
   - 高层模块不依赖低层模块
   - 都依赖于抽象接口

4. **接口隔离原则（ISP）**
   - 接口细粒度，客户端不依赖不需要的方法
   - Query 和 Action 分离

### 性能优化建议

```cpp
// PerformanceOptimizations.h
#pragma once
#include "BehaviorFlow.h"

namespace desktop::performance {

// 行为缓存
class BehaviorCache {
public:
    void cache(const QString& key, const DesktopBehaviors& behaviors) {
        m_cache[key] = behaviors;
    }

    DesktopBehaviors get(const QString& key) const {
        return m_cache.value(key, DesktopBehaviorFlag::None);
    }

    bool contains(const QString& key) const {
        return m_cache.contains(key);
    }

    void invalidate(const QString& key) {
        m_cache.remove(key);
    }

    void clear() {
        m_cache.clear();
    }

private:
    QMap<QString, DesktopBehaviors> m_cache;
};

// 批量操作优化
class BatchBehaviorProcessor {
public:
    void addRequest(const BehaviorChangeRequest& request) {
        m_requests.append(request);
    }

    QVector<BehaviorChangeResult> processAll() {
        QVector<BehaviorChangeResult> results;

        // 批量合并请求
        DesktopBehaviors merged = mergeBatchRequests(m_requests);

        // 一次性应用
        BehaviorChangeRequest combined;
        combined.desired = merged;
        combined.source = "batch";
        combined.priority = 0;

        BehaviorChangeResult result = m_engine.apply({combined});
        results.append(result);

        m_requests.clear();
        return results;
    }

private:
    QVector<BehaviorChangeRequest> m_requests;
    BehaviorFlowEngine m_engine;

    DesktopBehaviors mergeBatchRequests(
        const QVector<BehaviorChangeRequest>& requests) {

        DesktopBehaviors result = DesktopBehaviorFlag::None;
        for (const auto& req : requests) {
            result |= req.desired;
        }
        return result;
    }
};

} // namespace desktop::performance
```text

### 测试策略

```cpp
// TestingSupport.h
#pragma once
#include "DesktopBehaviorAbstraction.h"

namespace desktop::testing {

// Mock 查询器
class MockBehaviorQuery : public IDesktopBehaviorQuery {
public:
    void setBehaviors(DesktopBehaviors behaviors) {
        m_behaviors = behaviors;
    }

    DesktopBehaviors behaviors() const override {
        return m_behaviors;
    }

    bool hasBehavior(DesktopBehaviorFlag flag) const override {
        return m_behaviors.testFlag(flag);
    }

    bool hasAnyBehavior(DesktopBehaviors flags) const override {
        return (m_behaviors & flags) != DesktopBehaviorFlag::None;
    }

    bool hasAllBehaviors(DesktopBehaviors flags) const override {
        return (m_behaviors & flags) == flags;
    }

private:
    DesktopBehaviors m_behaviors = DesktopBehaviorFlag::None;
};

// Mock 修改器
class MockBehaviorModifier : public IDesktopBehaviorModifier {
public:
    DesktopBehaviors lastSetBehaviors() const {
        return m_lastBehaviors;
    }

    int setBehaviorsCallCount() const {
        return m_callCount;
    }

    void setBehaviors(DesktopBehaviors flags) override {
        m_lastBehaviors = flags;
        m_callCount++;
    }

    void addBehaviors(DesktopBehaviors flags) override {
        m_lastBehaviors |= flags;
        m_callCount++;
    }

    void removeBehaviors(DesktopBehaviors flags) override {
        m_lastBehaviors &= ~flags;
        m_callCount++;
    }

    void clearBehaviors() override {
        m_lastBehaviors = DesktopBehaviorFlag::None;
        m_callCount++;
    }

private:
    DesktopBehaviors m_lastBehaviors = DesktopBehaviorFlag::None;
    int m_callCount = 0;
};

} // namespace desktop::testing
```yaml

---

## 参考资源

### Qt 官方文档

- [Qt Platform Abstraction | Platform Integration | Qt 6.11.0](https://doc.qt.io/qt-6/qpa.html)
- [QPluginLoader Class | Qt Core | Qt 6.10.2](https://doc.qt.io/qt-6/qpluginloader.html)
- [How to Create Qt Plugins - Qt Documentation](https://doc.qt.io/qt-6/plugins-howto.html)
- [QWidget Class | Qt Widgets | Qt 6.11.0](https://doc.qt.io/qt-6/qwidget.html)
- [Wayland and Qt | Qt 6.11](https://doc.qt.io/qt-6/wayland-and-qt.html)

### 架构设计参考

- [Layered Architecture and Abstraction Layers | by Patryk Rogala](https://medium.com/@patrykrogedu/layered-architecture-and-abstraction-layers-167438dd1a8b)
- [Must-Know Software Architecture Patterns - ByteByteGo Newsletter](https://blog.bytebytego.com/p/must-know-software-architecture-patterns)
- [14 software architecture design patterns to know - Red Hat](https://www.redhat.com/en/blog/14-software-architecture-patterns)
- [Abstraction layer - Wikipedia](https://en.wikipedia.org/wiki/wiki/Abstraction_layer)

### 插件开发参考

- [Adapting Embedded Devices with Qt 6 Plugins | ICS](https://www.ics.com/blog/adapting-embedded-devices-qt-6-plugins-coffee-machine-case-study)
- [Loading and initialising a Qt plug-in dynamically - Qt Wiki](https://qt.shoutwiki.com/wiki/Loading_and_initialising_a_Qt_plug-in_dynamically)

---

## 总结

本文档详细介绍了桌面行为系统的完整架构设计，从分层架构到行为流转流程，从冲突解决机制到插件系统集成。这套架构的核心价值在于：

1. **可扩展性**：通过插件系统轻松添加新的行为策略
2. **跨平台性**：通过平台抽象层处理不同平台的差异
3. **可维护性**：清晰的分层架构使代码易于理解和维护
4. **可测试性**：依赖抽象接口便于编写单元测试

希望这套架构能够为您的桌面应用开发提供有力支持！
