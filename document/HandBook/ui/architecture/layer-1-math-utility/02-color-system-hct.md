---
title: HCT 色彩空间实战——Material 动态主题的数学基础
description: 在上一篇文章里，我们聊了为什么需要自己实现一套数学工具。现在我们来深入其中一个核心问题：颜色。
---

# HCT 色彩空间实战——Material 动态主题的数学基础

在上一篇文章里，我们聊了为什么需要自己实现一套数学工具。现在我们来深入其中一个核心问题：颜色。

假设你正在做一个支持"动态主题"的应用——用户可以选择自己的品牌色，然后应用自动生成一套完整的配色方案。这听起来很简单，但真的动手做的时候，你会发现这事儿比想象中麻烦得多。

## 问题：给定品牌色，生成整套主题

我们用一个具体的例子来说明问题。假设用户的品牌色是 `#6200EE`（Material Design 的经典紫色），现在需要生成以下颜色：

- Primary：品牌色本身
- OnPrimary：在 Primary 上显示的文本颜色
- PrimaryContainer：使用 Primary 的容器背景
- OnPrimaryContainer：在 PrimaryContainer 上显示的文本颜色
- Secondary：辅助色
- ...（还有 20+ 个角色）

如果用 RGB 空间直接操作，你很快就会遇到问题。

## RGB 空间的"不直观"

首先，我们想生成一个"比 Primary 浅一点"的颜色用于 hover 状态。在 RGB 空间里，怎么做？

最直观的想法是：直接增加 RGB 三个分量。比如 `RGB(98, 0, 238)` 乘以 1.2 得到 `RGB(117, 0, 255)`。看起来没问题？

但问题来了：

1. 当某个分量超过 255 时，你需要钳位。这意味着三个分量的相对比例会发生变化，颜色会"跑偏"。
2. 不同颜色的"变亮"效果不一致。深紫色增加 50 和深红色增加 50，视觉效果完全不同。
3. 最要命的是：RGB 空间的"亮度"和人眼感知的亮度不一致。同样是增加 50，绿色看起来比蓝色亮得多。

这就是为什么我们需要一个更"符合人类感知"的色彩空间。

## HCT 色彩空间的三个维度

HCT 代表 Hue-Chroma-Tone，分别对应：

- **Hue（色相）**：0-360 度，决定颜色是什么（红、橙、黄、绿...）
- **Chroma（色度）**：0-150，决定颜色的"鲜艳程度"
- **Tone（色调/亮度）**：0-100，决定颜色有多亮

这三个维度是（近似）正交的：调整 Tone 不会影响 Hue 和 Chroma，调整 Chroma 不会影响 Hue。这正是我们需要的——我们可以保持色相和色度不变，只调整亮度来生成一套配色。

## 从 RGB 到 HCT 的转换

现在问题来了：我们拿到的是 RGB 值（比如 `#6200EE`），怎么转换成 HCT？

完整的 CAM16 色彩模型算法非常复杂，涉及到复杂的矩阵运算和迭代求解。我们这里用的是简化版本，通过 HSL 做中转。

```cpp
// RGB → HSL
HSL hsl = rgbToHsl(r, g, b);

// HSL → HCT（近似）
outH = hsl.h;  // 色相直接沿用
outT = perceivedLightness * 100.0f;  // 感知亮度
outC = (hsl.s / toneFactor) * 100.0f;  // 色度从饱和度推导
```text

这里有几个细节需要注意。首先是"感知亮度"，它不是简单的 (R+G+B)/3，而是加权平均：

```cpp
float perceivedLightness = 0.299f * r + 0.587f * g + 0.114f * b;
```text

这个权重来源于人眼对不同颜色的敏感度——绿色看起来最亮，蓝色最暗。

然后是色度的计算。在极端亮度（非常亮或非常暗）的情况下，同样的饱和度产生的"鲜艳感"会变弱，所以我们需要一个 toneFactor 来补偿：

```cpp
float toneFactor = 1.0f - std::abs(hsl.l - 0.5f) * 1.5f;
toneFactor = math::clamp(toneFactor, 0.2f, 1.0f);
outC = (hsl.s / toneFactor) * 100.0f;
```text

## 从 HCT 到 RGB 的回转

生成配色的时候，我们是在 HCT 空间操作的（比如固定 Hue 和 Chroma，改变 Tone）。最终显示的时候，需要转回 RGB。

```cpp
// HCT → HSL（近似）
float t = tone / 100.0f;
float chromaFactor = math::clamp(chroma / 100.0f, 0.0f, 1.5f);

// 饱和度在中色调时达到峰值
float toneSaturationFactor = 1.0f - std::abs(t - 0.5f) * 2.0f;
toneSaturationFactor = toneSaturationFactor * toneSaturationFactor;

outS = chromaFactor * (0.3f + 0.7f * toneSaturationFactor);
outL = t - chromaFactor * 0.05f;  // 高色度会让颜色看起来更暗
```text

然后再用标准的 HSL → RGB 算法转回 RGB。

## tonalPalette 算法

有了 HCT，生成 tonal palette 就很简单了：

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

就这么简单。你固定 Hue 和 Chroma，只变 Tone，就能得到 13 个颜色，它们的"颜色感"是一致的，只有亮度不同。

## 相对亮度和对比度计算

有了颜色，我们还需要计算对比度——比如确保文本和背景之间的对比度达到 WCAG AA 标准（4.5:1）。

对比度计算需要先算出"相对亮度"（Relative Luminance），这里有个关键点：需要先做 gamma 校正。

```cpp
// sRGB → linear RGB（gamma 校正）
float toLinear(float c) {
    if (c <= 0.04045f) {
        return c / 12.92f;
    }
    return std::pow((c + 0.055f) / 1.055f, 2.4f);
}

// 相对亮度
float relativeLuminance() const {
    float r = toLinear(internal_color.redF());
    float g = toLinear(internal_color.greenF());
    float b = toLinear(internal_color.blueF());

    return 0.2126f * r + 0.7152f * g + 0.0722f * b;
}
```text

为什么需要 gamma 校正？因为 sRGB 是非线性编码的，直接用 RGB 值计算亮度会得到错误的结果。gamma 校正后，RGB 值才与实际的物理光强成正比。

对比度计算很简单：

```cpp
float contrastRatio(CFColor& a, CFColor& b) {
    float lumA = a.relativeLuminance();
    float lumB = b.relativeLuminance();

    float lighter = std::max(lumA, lumB);
    float darker = std::min(lumA, lumB);

    return (lighter + 0.05f) / (darker + 0.05f);
}
```yaml

加 0.05 是 WCAG 标准的规定，用于避免极端情况的数值问题。

## 色域裁剪的问题

这里有个潜在的坑：某些 HCT 组合在 sRGB 色域里是无法表示的。比如非常高的 Chroma + 中等 Tone，可能会超出 sRGB 的范围。

我们在 HCT → RGB 转换时做了钳位处理，但钳位会导致颜色"失真"。一个更完善的方案是在检测到超出色域时，降低 Chroma 值直到颜色可表示。不过这个会增加计算复杂度，我们目前采用简化处理。

## 下一步

到这里，我们就有了完整的颜色系统：HCT 色彩空间支持、tonal palette 生成、对比度计算。但颜色只是主题系统的一部分，一个完整的主题还需要字体、圆角、动画规范等组件。

更重要的是，我们需要一个系统来管理这些组件，让它们能够动态切换、能够被控件方便地访问。

接下来，我们进入 Layer 2：Theme Engine Layer，看看如何把颜色、字体、形状等组件整合成一个完整的主题系统。

---

**相关文档**

- [为什么我们需要自己的数学层](./01-why-we-need-own-math-layer.md)——基础层设计动机
- [几何与设备无关](./03-geometry-and-device-pixel.md)——跨 DPI 适配的完整方案
- [颜色方案实现](../layer-2-theme-engine/03-color-scheme.md)——主题系统中的颜色应用
