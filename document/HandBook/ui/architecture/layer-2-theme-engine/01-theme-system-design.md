---
title: 主题系统架构设计——从单例到工厂的完整方案
description: 在 Layer 1 里，我们搭建了基础的数学工具层。有了颜色系统、缓动曲线、几何工具，现在的问题是：
---

# 主题系统架构设计——从单例到工厂的完整方案

在 Layer 1 里，我们搭建了基础的数学工具层。有了颜色系统、缓动曲线、几何工具，现在的问题是：怎么把这些东西组织成一个完整的主题，让所有控件都能方便地访问？

这篇文章聊聊主题系统的架构设计。

## 架构演进的故事

说实话，我们的主题系统经历了好几次"推倒重来"。

第一版方案非常简单粗暴：全局变量。一个全局的 `QColor g_primaryColor`，一个全局的 `QFont g_defaultFont`，控件直接访问这些变量。这方案的缺点显而易见：不支持多主题、不支持动态切换、全局污染、命名空间冲突……

第二版方案我们探索了 QSS（Qt Style Sheets）。QSS 确实能解决一些问题，但它对于 Material Design 这种复杂的设计规范来说力不从心。QSS 不支持状态层的半透明叠加，不支持涟漪动画，不支持运行时的精确属性控制。更重要的是，QSS 的重新加载性能很差，不适合动态主题切换。

最终我们确立了现在的方案：接口 + 工厂 + 单例管理器的组合。

## 核心架构

整个主题系统由三个核心部分组成：

1. **ICFTheme 接口**：主题的抽象定义，包含颜色、字体、形状、动效四个组件
2. **ThemeFactory 接口**：创建主题的工厂，支持从名称或 JSON 创建
3. **ThemeManager 单例**：管理多个主题工厂，处理主题切换，广播变更信号

### ICFTheme：主题的抽象定义

ICFTheme 是一个纯接口（所有方法都是 virtual），它定义了主题应该包含什么：

```cpp
struct ICFTheme {
    ICFColorScheme& color_scheme() const;
    IMotionSpec& motion_spec() const;
    IRadiusScale& radius_scale() const;
    IFontType& font_type() const;

protected:
    std::unique_ptr<ICFColorScheme> color_scheme_;
    std::unique_ptr<IMotionSpec> motion_spec_;
    std::unique_ptr<IRadiusScale> radius_scale_;
    std::unique_ptr<IFontType> font_type_;
};
```text

设计成接口的原因是：我们可能有多种不同的主题实现（Material、Cupertino、Fluent），但它们都应该遵循同一个接口。控件只需要依赖 ICFTheme 接口，不需要知道具体是哪种主题。

这里有个细节：ICFTheme 的构造函数是 protected 的，只有 ThemeFactory（被声明为 friend）可以创建实例。这确保了主题只能通过工厂创建，避免用户直接构造导致的不一致。

### ThemeFactory：创建主题的工厂

ThemeFactory 也是一个接口，定义了创建主题的三种方式：

```cpp
class ThemeFactory {
public:
    virtual std::unique_ptr<ICFTheme> fromName(const char* name) = 0;
    virtual std::unique_ptr<ICFTheme> fromJson(const QByteArray& json) = 0;
    virtual QByteArray toJson(ICFTheme* raw_theme) = 0;
};
```text

`fromName()` 用于创建预定义的主题（比如 "light"、"dark"），`fromJson()` 用于从 Material Theme Builder 导出的 JSON 创建主题，`toJson()` 用于序列化。

工厂模式的好处是：可以在运行时注册新的主题类型，而不需要修改核心代码。比如你想添加一个"自定义主题"功能，只需要实现一个新的 ThemeFactory，然后注册到 ThemeManager。

### ThemeManager：单例管理器

ThemeManager 是整个主题系统的入口，它是一个单例：

```cpp
class ThemeManager : public QObject {
public:
    static ThemeManager& instance();

    // 注册/移除主题工厂
    bool insert_one(const std::string& name, InstallerMaker make_one);
    void remove_one(const std::string& name);

    // 获取主题
    const ICFTheme& theme(const std::string& name) const;

    // 切换主题
    void setThemeTo(const std::string& name, bool doBroadcast = true);

    // 控件订阅主题更新
    void install_widget(QWidget* w);
    void remove_widget(QWidget* w);

signals:
    void themeChanged(const ICFTheme& new_theme);
};
```text

使用方式很直观：

```cpp
// 注册主题
ThemeManager::instance().insert_one("material.light", []() {
    return std::make_unique<MaterialFactory>();
});

// 让控件订阅主题更新
ThemeManager::instance().install_widget(myButton);

// 切换主题
ThemeManager::instance().setThemeTo("material.light");
```text

## 为什么用接口 + 工厂？

你可能会问：为什么不直接让 ThemeManager 管理 ICFTheme 实例，非要中间加一个 ThemeFactory？

原因有几个：

1. **延迟创建**：主题可能包含大量数据（颜色表、字体缓存），如果不使用就创建会浪费内存。工厂模式下，主题只在第一次访问时创建。
2. **支持动态注册**：你可以在运行时注册新的主题类型，而不需要修改 ThemeManager 的代码。
3. **序列化支持**：fromJson/toJson 方法让主题可以被持久化和传输。

## 线程安全设计

ThemeManager 的单例实现使用了 C++11 的"魔术静态变量"：

```cpp
static ThemeManager& instance() {
    static ThemeManager manager;
    return manager;
}
```text

C++11 保证局部静态变量的初始化是线程安全的，所以这个实现不需要额外的锁。

但 `install_widget/remove_widget` 和 `setThemeTo` 不是线程安全的。如果需要在多线程环境使用，需要外部同步。不过一般来说，主题操作都在主线程进行，这不是问题。

## 主题切换的广播机制

当主题切换时，ThemeManager 会发出 `themeChanged` 信号，所有订阅的控件都会收到通知：

```cpp
// 在控件的构造函数中
connect(&ThemeManager::instance(), &ThemeManager::themeChanged,
        this, [this](const ICFTheme&) { update(); });
```text

这里有个设计细节：为什么用 `install_widget` 而不是让控件直接连接信号？

原因是为了生命周期管理。如果控件直接连接信号，控件销毁时需要手动断开连接，否则会访问野指针。而 `install_widget` 会跟踪控件的生命周期，当控件销毁时自动从订阅列表中移除。

实际上我们并没有用 QObject::destroyed 信号，因为那会有循环引用的风险。我们选择在控件析构时手动调用 `remove_widget`，或者在 ThemeManager 里定期清理失效的指针。

## 生命周期管理

主题实例由 ThemeManager 持有（通过 `theme_cache_`），而主题内的各个组件（颜色方案、字体等）由主题持有。所有权链条是清晰的：

```text
ThemeManager (owner)
  └── unordered_map<string, unique_ptr<ICFTheme>> theme_cache_
       └── ICFTheme (owner)
            ├── unique_ptr<ICFColorScheme> color_scheme_
            ├── unique_ptr<IMotionSpec> motion_spec_
            ├── unique_ptr<IRadiusScale> radius_scale_
            └── unique_ptr<IFontType> font_type_
```yaml

控件只持有引用（通过 `themeChanged` 信号的参数），不拥有主题的所有权。这样当主题被销毁时，不会有 dangling pointer 的问题。

## 下一步

到这里，主题系统的骨架就搭好了。但我们还没有讨论具体的实现细节：颜色是怎么存储的？字体是怎么缓存的？Token 系统是怎么工作的？

在下一篇文章里，我们会深入 Token 系统——那个让 Material Design 3 的颜色访问变得类型安全又高性能的魔法。

---

**相关文档**

- [几何与设备无关](../layer-1-math-utility/03-geometry-and-device-pixel.md)——基础层的最后一篇
- [Token 系统设计](./02-token-system.md)——字符串字面量的编译时魔法
- [颜色方案实现](./03-color-scheme.md)——从种子颜色到完整主题
