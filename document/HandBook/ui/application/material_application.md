# MaterialApplication - Material 应用

`MaterialApplication` 是 `Application` 的 Material Design 3 专用派生类，自动注册亮色和暗色主题，并把 Material 动画工厂绑定上去。如果你的应用用 Material Design 3，直接用它就行，不需要手动注册主题。

## 初始化

用法很简单，替换掉 `QApplication` 就行：

```cpp
#include "ui/widget/material/application/material_application.h"

using namespace cf::ui::widget::material;

int main(int argc, char* argv[]) {
    MaterialApplication app(argc, argv);

    // 默认已经是 Material 亮色主题
    MainWindow w;
    w.show();

    return app.exec();
}
```

构造函数里会自动调用 `init()`，`init()` 按下面的顺序执行：

1. 注册 Material 动画工厂（前缀 `theme.material`）
2. 注册 Material 亮色和暗色主题到 `ThemeManager`
3. 设置默认主题为 `theme.material.light`
4. 调用基类的 `init()` 创建动画工厂

## 主题切换

Material 主题已经预注册好了，直接切换就行：

```cpp
// 切换到暗色主题
app.setTheme("theme.material.dark");

// 或者用 token 字面量（推荐）
using namespace cf::ui::core::token::literals;
app.setTheme(MATERIAL_THEME_DARK);
```

主题切换会触发 `themeChanged` 信号，动画工厂会同步重建。

## 动画工厂绑定

Material 动画工厂是和主题绑定的——当主题切换到任何 `theme.material.*` 时，都会用 `CFMaterialAnimationFactory` 创建动画。工厂的前缀匹配保证了这点：

```cpp
// 在 MaterialApplication::init() 里已经注册了
Application::registerAnimationFactoryType("theme.material",
    [](const core::ICFTheme& theme, QObject* parent) {
        return std::make_unique<CFMaterialAnimationFactory>(theme, nullptr, parent);
    });
```

所以你不需要自己管工厂创建，只要确保主题 token 以 `theme.material.` 开头就行。

## 默认主题

`MaterialApplication::DEFAULT_THEME` 定义为 `MATERIAL_THEME_LIGHT`（即 `"theme.material.light"`）。如果你想在构造后立即切换主题，可以直接用：

```cpp
MaterialApplication app(argc, argv);

// 如果想要暗色主题作为默认
app.setTheme(MATERIAL_THEME_DARK);
```

或者自己派生一个类，在 `init()` 里改默认主题：

```cpp
class DarkMaterialApplication : public MaterialApplication {
protected:
    void init() override {
        MaterialApplication::init();  // 先让基类注册完主题
        setTheme(MATERIAL_THEME_DARK);  // 然后改默认
    }
};
```

⚠️ 不要在 `MaterialApplication::init()` 里改默认主题再调用基类 `init()`——因为基类的 `init()` 会创建动画工厂，此时默认主题还是亮色的。要么在构造后改，要么在派生类的 `init()` 里先调基类 `init()` 再改。

## 和裸 Application 的区别

直接用 `Application` 需要自己注册主题和动画工厂：

```cpp
// 用裸 Application 需要这些额外步骤
Application app(argc, argv);

Application::registerAnimationFactoryType("theme.material", ...);
themeManager()->insert_one("theme.material.light", ...);
themeManager()->setThemeTo("theme.material.light", false);

app.init();  // 别忘了手动调用 init()
```

而 `MaterialApplication` 把这些都包圆了，构造完就能用。如果你用 Material Design 3，没理由不用它。

## 相关文档

- [Application - 应用基础类](./application.md)
- [Material 主题系统](../material/cfmaterial_theme.md)
- [Material 动画](../material/animation/index.md)
