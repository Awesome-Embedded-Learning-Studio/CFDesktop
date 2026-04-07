# Application - 应用基础类

`Application` 是 Qt 的 `QApplication` 替代品，在标准应用框架之上集成了主题管理和动画工厂。我们用它替换掉裸的 `QApplication`，这样在任何地方都能通过静态方法访问当前主题和动画资源，不需要到处传递单例指针。

## 初始化流程

`Application` 的构造函数只做最基础的设置，真正的初始化在 `init()` 方法里完成。这是有意为之的——派生类需要先注册自己的主题，然后才能调用基类的 `init()`，否则动画工厂创建时会找不到对应的主题。

```cpp
#include "ui/widget/application_support/application.h"

using namespace cf::ui::widget::application_support;

int main(int argc, char* argv[]) {
    Application app(argc, argv);

    // 如果是派生类，在 init() 里注册主题
    app.setTheme("theme.material.light");

    MainWindow w;
    w.show();

    return app.exec();
}
```

注意：如果你直接用 `Application` 而不是它的派生类，需要手动注册主题并调用 `init()`，否则 `currentTheme()` 会抛异常。实际上更推荐用 `MaterialApplication`，下面会说。

## 主题系统集成

`Application` 内部持有一个指向 `ThemeManager` 单例的连接。当主题切换时，`ThemeManager` 会发出信号，`Application` 收到后会重建整个动画工厂——因为工厂内部持有对主题的引用，主题换了就必须重建。

```cpp
// 设置主题
app.setTheme("theme.material.dark");

// 主题切换会触发这个信号
connect(&app, &Application::themeChanged, [](const core::ICFTheme& newTheme) {
    // 响应主题切换，更新 UI
});
```

这里有个细节：动画工厂的重建会保留之前的启用状态。比如你禁用了动画，切换主题后动画仍然是禁用状态，不会突然恢复。

## 动画访问

通过 token 获取动画是最常见的用法：

```cpp
// 从任何地方获取动画
auto fadeIn = Application::animation("md.animation.fadeIn");
if (fadeIn) {
    fadeIn->setTargetWidget(myWidget);
    fadeIn->start();
}

// 全局禁用动画（比如在批量操作时）
Application::setAnimationsEnabled(false);
```

返回的是 `WeakPtr`，因为动画工厂归 `Application` 所有，外部只持有弱引用。如果 `Application` 被销毁或者主题切换导致工厂重建，原来的 `WeakPtr` 就会失效——所以拿到指针后要尽快用，不要长期持有。

⚠️ `setAnimationsEnabled(false)` 只影响新创建的动画，已经正在运行的动画不会受影响。如果你需要立即停止所有动画，需要自己遍历并停止它们。

## 自定义动画工厂

如果你的应用不用 Material Design，或者想用自己的动画实现，可以注册一个自定义工厂：

```cpp
// 在 main() 之前，或者在派生类的 init() 里
Application::registerAnimationFactoryType("theme.fluent",
    [](const core::ICFTheme& theme, QObject* parent) {
        return std::make_unique<FluentAnimationFactory>(theme, parent);
    });

// 现在切换到 "theme.fluent.*" 主题时会用你的工厂
app.setTheme("theme.fluent.dark");
```

匹配逻辑是按前缀最长匹配：`theme.fluent.dark` 会匹配 `theme.fluent`，而不是 `theme`。这样你可以同时注册多个工厂，每个负责一个主题家族。

## 派生类注意事项

如果你需要继承 `Application`，要记住 `init()` 的调用顺序：

```cpp
class MyApplication : public Application {
protected:
    void init() override {
        // 1. 先注册动画工厂（如果需要）
        registerAnimationFactoryType("theme.myapp", ...);

        // 2. 再注册主题
        themeManager()->insert_one("theme.myapp.light", ...);
        themeManager()->setThemeTo("theme.myapp.light", false);

        // 3. 最后调用基类 init
        Application::init();
    }
};
```

如果顺序搞反了，基类的 `init()` 会尝试创建动画工厂，但此时主题还没注册，工厂创建会失败。

## 相关文档

- [MaterialApplication - Material 应用](./material_application.md)
- [ThemeManager - 主题管理](../core/theme_manager.md)
- [动画系统概述](../components/animation.md)
