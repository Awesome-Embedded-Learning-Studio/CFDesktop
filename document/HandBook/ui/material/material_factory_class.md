---
title: "MaterialFactory - Material 主题工厂"
description: 是创建 Material Design 3 主题的入口。它实现了  接口，提供了按名称创建、从 JS
---

# MaterialFactory - Material 主题工厂

`MaterialFactory` 是创建 Material Design 3 主题的入口。它实现了 `ThemeFactory` 接口，提供了按名称创建、从 JSON 创建和导出 JSON 的能力。这个类存在的意义是统一管理主题的创建方式，让调用方不需要知道 `MaterialTheme` 的构造细节。

## 按名称创建

最简单的使用方式是用预定义名称创建主题：

```cpp
#include "material_factory_class.h"

MaterialFactory factory;

// 创建浅色主题
auto lightTheme = factory.fromName("theme.material.light");

// 创建深色主题
auto darkTheme = factory.fromName("theme.material.dark");

// 检查是否创建成功
if (!lightTheme) {
    qDebug() << "主题创建失败，可能是名称错误";
}
```text

如果传入无法识别的名称，`fromName` 会返回 `nullptr`。支持的名称列表：
- `theme.material.light`：Material 默认浅色主题
- `theme.material.dark`：Material 默认深色主题

## 从 JSON 创建

支持从 Material Theme Builder 导出的 JSON 创建主题：

```cpp
QByteArray json = R"({
  "colors": {
    "primary": "#6750A4",
    "onPrimary": "#FFFFFF",
    "primaryContainer": "#EADDFF",
    ...
  },
  "typography": {
    ...
  },
  "shapes": {
    ...
  }
})";

auto theme = factory.fromJson(json);

if (!theme) {
    qDebug() << "JSON 解析失败";
}
```text

JSON 格式可以只包含颜色，也可以包含完整的主题配置。如果某个部分缺失，工厂会使用默认值填充。

⚠️ JSON 格式必须符合 Material Theme Builder 的导出规范。如果格式不正确，`fromJson` 会返回 `nullptr`。目前没有详细的错误信息返回，这是个待改进点。

## 导出到 JSON

可以把已有主题导出为 JSON 格式：

```cpp
auto theme = factory.fromName("theme.material.light");
QByteArray json = factory.toJson(theme.get());

// 保存到文件
QFile file("my_theme.json");
file.open(QIODevice::WriteOnly);
file.write(json);
```text

导出的 JSON 兼容 Material Theme Builder 的导入格式，可以在在线工具中编辑后再导回。

## 作为 ThemeFactory 使用

`MaterialFactory` 继承自 `ThemeFactory`，可以配合我们的主题管理系统使用：

```cpp
std::unique_ptr<ThemeFactory> factory = std::make_unique<MaterialFactory>();
setThemeFactory(std::move(factory));

// 后续可以通过通用接口创建主题
auto theme = themeFactory()->fromName("theme.material.light");
```text

这样设计的好处是可以在运行时切换不同的主题系统（比如未来添加 Fluent Design 支持），而不需要修改业务代码。

## 错误处理

`MaterialFactory` 的错误处理比较简单——失败时返回空指针。这在某些场景下不够友好，因为调用方无法知道失败的具体原因：

```cpp
// 当前的方式
auto theme = factory.fromJson(json);
if (!theme) {
    // 不知道是 JSON 格式错误、颜色值错误、还是其他问题
}

// 更理想的方式（待实现）
auto result = factory.fromJsonWithError(json);
if (!result) {
    const auto& err = result.error();
    // 可以根据 err.kind 判断具体错误类型
}
```text

这是我们在后续版本中计划改进的地方。

## 线程安全

`MaterialFactory` 本身是无状态的，创建主题的过程不依赖共享状态。因此可以在多线程环境中安全使用：

```cpp
// 多个线程可以同时调用
MaterialFactory factory;

// 线程 1
auto t1 = factory.fromName("theme.material.light");

// 线程 2
auto t2 = factory.fromName("theme.material.dark");
```text

创建出来的 `MaterialTheme` 对象也是独立不共享的，可以在线程间传递。

## 相关文档

- [MaterialTheme - 主题实现](./cfmaterial_theme.md)
- [MaterialColorScheme - 颜色方案](./cfmaterial_scheme.md)
- [material_factory.hpp - 工厂函数](./material_factory_hpp.md)
