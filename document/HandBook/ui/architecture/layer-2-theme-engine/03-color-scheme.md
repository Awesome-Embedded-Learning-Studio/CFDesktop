---
title: 颜色方案实现——从种子颜色到完整主题
description: 在之前的文章里，我们聊了 HCT 色彩空间的数学原理，也讲了 Token 系统的设计。现在我们把两者
---

# 颜色方案实现——从种子颜色到完整主题

在之前的文章里，我们聊了 HCT 色彩空间的数学原理，也讲了 Token 系统的设计。现在我们把两者结合起来：如何从一个"种子颜色"生成完整的 Material Design 3 主题。

## Material Design 3 的颜色角色

Material Design 3 定义了 26 个标准颜色角色，分为几组：

- **Primary（4个）**：primary、onPrimary、primaryContainer、onPrimaryContainer
- **Secondary（4个）**：secondary、onSecondary、secondaryContainer、onSecondaryContainer
- **Tertiary（4个）**：tertiary、onTertiary、tertiaryContainer、onTertiaryContainer
- **Error（4个）**：error、onError、errorContainer、onErrorContainer
- **Surface（8个）**：background、onBackground、surface、onSurface、surfaceVariant、onSurfaceVariant、outline、outlineVariant
- **Utility（5个）**：shadow、scrim、inverseSurface、inverseOnSurface、inversePrimary

这些角色不是随意定义的，它们之间有明确的语义关系。比如 `onX` 表示"在 X 颜色上显示的文本/图标颜色"，`XContainer` 表示"使用 X 颜色的容器背景"。

## tonalPalette 算法

生成主题的核心是 tonalPalette 算法——从一个种子颜色生成 13 个亮度等级。

MaterialColorScheme 内部使用了 EmbeddedTokenRegistry 来存储颜色：

```cpp
class MaterialColorScheme : public ICFColorScheme {
private:
    EmbeddedTokenRegistry registry_;
    mutable std::unordered_map<std::string, QColor> color_cache_;
};
```text

注意这里有两个存储：`registry_` 存储原始的 CFColor（带 HCT 信息），`color_cache_` 存储转换为 QColor 的结果（用于查询缓存）。

生成 tonal palette 的过程：

```cpp
QList<CFColor> tonalPalette(CFColor keyColor) {
    float hue = keyColor.hue();      // 保持色相
    float chroma = keyColor.chroma(); // 保持色度

    // 13 个标准 Tone 值
    constexpr float TONAL_VALUES[] = {
        0.0f, 10.0f, 20.0f, 30.0f, 40.0f,
        50.0f, 60.0f, 70.0f, 80.0f, 90.0f,
        95.0f, 99.0f, 100.0f
    };

    QList<CFColor> palette;
    for (int i = 0; i < 13; ++i) {
        palette.append(CFColor(hue, chroma, TONAL_VALUES[i]));
    }
    return palette;
}
```text

这个算法我们在 Layer 1 里讲过，核心思想是固定 Hue 和 Chroma，只变 Tone。这样生成的 13 个颜色在视觉上是"同一个颜色的不同亮度版本"。

## Primary 组的生成

Primary 组的颜色来自主种子颜色：

```cpp
// 生成 Primary tonal palette
QList<CFColor> primaryPalette = tonalPalette(seedColor);

// 从 tonal palette 中选择特定的 Tone 值
primary = primaryPalette[Tone 40];           // md.primary
onPrimary = primaryPalette[Tone 100];        // md.onPrimary（白色）
primaryContainer = primaryPalette[Tone 90];  // md.primaryContainer
onPrimaryContainer = primaryPalette[Tone 10]; // md.onPrimaryContainer
```text

这里有个设计选择：为什么 Primary 选 Tone 40，而 PrimaryContainer 选 Tone 90？

原因是 Primary 用作"主色"，通常是中等亮度；而 PrimaryContainer 用作"主色容器"，需要更亮一些，形成对比。具体的 Tone 值选择是 Material Design 3 规范的一部分，经过了大量视觉测试。

## Secondary 和 Tertiary 组

Secondary 和 Tertiary 组也用同样的算法，但种子颜色不同：

```cpp
// Secondary：从主种子颜色衍生
CFColor secondarySeed = deriveSecondary(seedColor);
QList<CFColor> secondaryPalette = tonalPalette(secondarySeed);

// Tertiary：从主种子颜色衍生（不同的衍生规则）
CFColor tertiarySeed = deriveTertiary(seedColor);
QList<CFColor> tertiaryPalette = tonalPalette(tertiarySeed);
```text

衍生算法会调整 HCT 值，让 Secondary 和 Tertiary 与 Primary 形成视觉和谐。比如 Secondary 可能旋转色相 30 度，Tertiary 可能旋转 60 度。

## Surface 组和 Light/Dark 差异

Surface 组（background、surface 等）的处理方式不同。在 Light 主题中，background 通常是接近白色的高亮度颜色；而在 Dark 主题中，background 是接近黑色的低亮度颜色。

```cpp
// Light 主题
background = CFColor(hue, chroma, Tone 98);   // 接近白色
onBackground = CFColor(hue, chroma, Tone 10);  // 深色文本

// Dark 主题
background = CFColor(hue, chroma, Tone 10);    // 接近黑色
onBackground = CFColor(hue, chroma, Tone 90);  // 浅色文本
```text

注意这里虽然用了相同的 `hue` 和 `chroma`，但 Tone 值差异很大。实际上，Surface 颜色通常会使用很低的 chroma（接近中性灰），以避免与内容颜色冲突。

## Error 组

Error 组使用固定的种子颜色（通常是红色），不随主题变化：

```cpp
CFColor errorSeed("#B00020");  // Material 标准错误红
QList<CFColor> errorPalette = tonalPalette(errorSeed);
```text

## onX 颜色的对比度要求

Material Design 3 要求 `onX` 颜色与 X 颜色之间满足 WCAG AA 对比度标准（4.5:1）。这意味着：

```cpp
float ratio = contrastRatio(primary, onPrimary);
// ratio >= 4.5 必须成立
```text

如果 tonalPalette 生成的颜色不满足对比度要求，需要调整。通常的做法是：

1. 先用 tonalPalette 生成候选颜色
2. 计算对比度
3. 如果不满足，向黑色或白色方向调整 Tone 值
4. 重新计算对比度，直到满足要求

## 查询接口

MaterialColorScheme 实现了 `queryExpectedColor` 方法：

```cpp
QColor& MaterialColorScheme::queryExpectedColor(const char* name) {
    // 先查缓存
    auto it = color_cache_.find(name);
    if (it != color_cache_.end()) {
        return it->second;
    }

    // 从 registry 获取 CFColor
    uint64_t hash = cf::hash::fnv1a64(name);
    auto result = registry_.get_dynamic<CFColor>(name);
    if (!result) {
        // 返回默认颜色
        static QColor defaultColor(Qt::black);
        return defaultColor;
    }

    // 转换为 QColor 并缓存
    QColor color = (*result)->native_color();
    color_cache_[name] = color;
    return color_cache_[name];
}
```text

注意这里返回的是引用，意味着调用者不应该修改返回的颜色（否则会影响缓存）。如果需要修改，应该用 `queryColor` 返回副本。

## EmbeddedTokenRegistry 的使用

MaterialColorScheme 使用 `EmbeddedTokenRegistry` 而不是全局的 `TokenRegistry`，原因是：

1. **独立性**：每个颜色方案有自己独立的存储，不会互相干扰
2. **可移动**：EmbeddedTokenRegistry 支持移动语义，整个颜色方案可以被高效地移动
3. **生命周期管理**：颜色方案销毁时，EmbeddedTokenRegistry 也会自动销毁，无需手动清理

## 验证一下

创建一个完整的 Material 主题：

```cpp
// 从品牌色生成 Light 主题
CFColor brandColor("#6200EE");
auto lightScheme = material::light(brandColor);

// 查询颜色
QColor primary = lightScheme.queryColor("md.primary");
QColor onPrimary = lightScheme.queryColor("md.onPrimary");

// 验证对比度
float ratio = contrastRatio(primary, onPrimary);
// ratio 应该 >= 4.5
```yaml

## 总结

MaterialColorScheme 完整实现了 Material Design 3 的颜色系统，从一个种子颜色生成 26 个标准角色。它使用 tonalPalette 算法生成颜色，用 EmbeddedTokenRegistry 存储颜色，用 color_cache_ 加速查询。

但颜色只是主题的一部分。一个完整的主题还需要字体系统、圆角规范、动画参数等组件。

接下来，我们聊聊字体、形状与动效。

---

**相关文档**

- [Token 系统设计](./02-token-system.md)——字符串字面量的编译时魔法
- [HCT 色彩空间实战](../layer-1-math-utility/02-color-system-hct.md)——Layer 1 的颜色数学基础
- [字体、形状与动效](./04-typography-shape-motion.md)——主题的其余组件
