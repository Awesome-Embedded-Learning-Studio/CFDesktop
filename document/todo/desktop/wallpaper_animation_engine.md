---
title: "壁纸动画轮播引擎——接手实施计划"
description: "移植 CCIMXDesktop 的 WallPaperEngine + WallPaperAnimationHandler 到 CFDesktop,后端无关重表达。自洽接手文档。"
---

# 壁纸动画轮播引擎——接手实施计划

> **状态**: ⬜ 待开始(下一批)
> **来源**: 资源包发现(PR #13)已合入,`status/current.md` 标注「动画轮播引擎留待下一批」;本文为该批的详细实施计划,供下一位 AI 直接接手。
> **所属 Phase**: Phase 13([13_widget_apps.md](13_widget_apps.md) 壁纸段「只需补轮播」)。
> **预计规模**: 中等(≈2 个新文件 + 1 个扩展 + 配置 + 测试)。
> **最后更新**: 2026-06-30

---

## 一、Context(为什么做)

CFDesktop 壁纸的**数据/token 层**(`ImageWallPaperLayer` / `WallPaperAccessStorage` / `WallPaperToken`)与**运行时资源包发现**(`Wallpapers` PathType + `filter_target_recursive` + `make_layer()` 兜底)已落地。当前壁纸切换是**瞬时换图**:`WallPaperLayer` 的 `ImageChangedCallback` → `WallpaperShellLayerStrategy::onWallpaperChanged()` → `rescaleImage()` → `requestRepaint()`,无过渡动画、无定时轮播。

本批目标:移植 CCIMXDesktop 的 `WallPaperEngine` + `WallPaperAnimationHandler`,实现:
- **三种模式**:`Fixed`(不切)/ `Gradient`(交叉淡入)/ `Movement`(平移滑入)。
- **QTimer 定时自动轮播**(默认 20s)+ 缓动曲线(默认 InOutCubic)+ 随机/顺序选择器。
- 配置驱动(`wallpaper.json` 新 key)。

---

## 二、关键约束:必须「重表达」,不能照抄

⚠️ CCIMXDesktop 用 **QLabel 双缓冲 + `QGraphicsOpacityEffect` + `QPropertyAnimation` 改 QLabel `pos`** 做动画——它耦合 QWidget 渲染。

CFDesktop 的 `WallPaperLayer` 是**后端无关**的纯数据层(只吐 `QImage`),`WallpaperShellLayerStrategy` 通过 `currentBackgroundImage()` 返回**单张预缩放 `QImage`** 供 shell layer 的 `paintEvent` 消费(后端是 QPainter 或 RHI,都吃这张 `QImage`)。

**因此动画必须重表达为**:
> 过渡期间,把 `cached_scaled_image` 变成**新旧两图的逐帧合成 QImage**;`QPropertyAnimation` 驱动 `t∈[0,1]`,每个 `valueChanged` 重算合成 + `requestRepaint()`;过渡结束丢弃旧图,回到单图。

这套对 QPainter/RHI 都成立(它们都消费 `QImage`),不引入 QWidget 耦合,保持后端无关。

---

## 三、源参考(CCIMXDesktop,只读参照)

> 路径在 `~/CCIMXDesktop/`(本机存在)。**仅参照语义,不要照抄 QLabel/QWidget 实现**。

| 文件 | 行数 | 要点 |
|---|---|---|
| `core/wallpaper/WallPaperEngine.{h,cpp}` | ~146 | `SwitchingMode{Fixed,Gradient,Movement}`、QTimer(默认 `SWITCH_INTERVAL=20000`)、`ANIMATION_DURATION=2000`、`QEasingCurve(InOutCubic)`、`DEF_MODE=Movement`、随机选择器、`friend WallPaperAnimationHandler` |
| `ui/wallpaperanimationhandler.{h,cpp}` | ~154 | `process_opacity_switch`(双缓冲 QLabel + opacity effect 1→0 淡出旧图)、`process_movement_switch`(`QParallelAnimationGroup`:旧图左移出 + 新图右移入) |

注意 CCIMX 的边界:`image_list.size() <= 1` 时直接 return(不切)——照搬这个守卫。

---

## 四、目标架构(CFDesktop)

### 4.1 渲染缝(已存在,动画插在这里)

`desktop/ui/components/shell_layer_impl/WallpaperShellLayerStrategy.cpp`:
- `currentBackgroundImage()`(L72)返回 `d->cached_scaled_image`——paintEvent 消费它。
- `rescaleImage()`(L83)从 `d->wallpaper_layer->currentImage()` 按 `ScalingMode`(Fill/Fit/Stretch)预缩放进 `cached_scaled_image`。
- `onWallpaperChanged()`(L132,layer 回调)→ `rescaleImage()` + `requestRepaint()`——**当前是瞬时,这是动画改造点**。

### 4.2 职责划分(推荐)

| 组件 | 职责 | 改动 |
|---|---|---|
| `WallPaperLayer`(接口)+ `ImageWallPaperLayer` | 数据层:`currentImage()` / `showNextOne/Prev/Target` / `ImageChangedCallback` / `ScalingMode` | **不改**(已具备手动切换) |
| `WallPaperEngine`(**新**) | 配置(mode/interval/duration/easing)、QTimer 定时、选下一张、发 `requestTransition()` 给 strategy | 新增 |
| `WallpaperShellLayerStrategy`(**扩展**) | 过渡状态机:捕获旧图、启动 `QPropertyAnimation`、每帧合成、帧结束清理 | 扩展 |

> **为什么过渡状态机放 strategy 而非 engine**:strategy 拥有 `cached_scaled_image` 和 `rescaleImage()`,合成必须在这里;engine 只管「何时切、切到哪、用什么模式」。

### 4.3 过渡状态机(strategy 侧)

新增私有态:`QImage previous_scaled`、`bool transitioning`、`qreal transition_progress`、`SwitchingMode transition_mode`、`std::unique_ptr<QPropertyAnimation>`。

过渡流程(timer 触发或手动 next):
1. `engine` 决定切下一张 → 调 `strategy.beginTransition(mode)`。
2. strategy:`previous_scaled = cached_scaled_image`(捕获当前)。
3. strategy 调 `wallpaper_layer->showNextOne()` → layer 换 `currentImage()` + fire `ImageChangedCallback`。
4. strategy 在 `onWallpaperChanged()` 里检测 `transitioning==true` → 不走瞬时路径,改为 `rescaleImage()` 把**新**图缩放进 `cached_scaled_image`(作为 current),保留 `previous_scaled`,启动 `QPropertyAnimation(0→1, duration, easing)`。
5. 每个 `valueChanged(t)`:`cached_scaled_image = composite(previous_scaled, current_scaled, t, mode)` → `requestRepaint()`。
6. `finished`:丢弃 `previous_scaled`、`transitioning=false`、`cached_scaled_image` = 纯 current。

合成函数 `composite(prev, cur, t, mode)`(用 `QPainter` 画到目标尺寸 `QImage`):
- **Gradient**:先画 `cur`(opacity=1),再画 `prev`(opacity=`1-t`)——淡出新、露出新。或 prev opacity `1-t` + cur opacity `t`(双向淡入淡出)。选定一种并注释。
- **Movement**:`prev` 在 `x = lerp(0, -W, t)`、`cur` 在 `x = lerp(W, 0, t)`(沿用 CCIMX 方向:旧左出 / 新右入),均 opacity=1。

> `transition_progress` 作为 `Q_PROPERTY` 挂在 strategy 上,`QPropertyAnimation` 绑 `progress` 属性;setter 触发合成 + `requestRepaint()`。

### 4.4 `WallPaperEngine`(新,`desktop/ui/components/wallpaper/`)

```
namespace cf::desktop::wallpaper {
enum class SwitchingMode { Fixed, Gradient, Movement };   // 对齐 CCIMX

class WallPaperEngine : public QObject {
  Q_OBJECT
public:
  // 读 ConfigStore "wallpaper" 域,绑定到 layer(非 owning)与 strategy 回调
  WallPaperEngine(WallPaperLayer* layer, std::function<void(SwitchingMode)> request_transition, QObject* parent);
  void start();   // activate 时:mode!=Fixed 且 layer 张数>1 才 startTimer
  void stop();    // deactivate 时
  // 可选:配置热更 setMode/setInterval/setDuration
private:
  void onTimerTick();   // 选下一张(随机/顺序)→ request_transition(mode)
};
}
```

选择器:默认随机(对齐 CCIMX `ImagePoolEngine::default_index`);可后续加顺序模式。

### 4.5 配置(`wallpaper.json` 新 key)

经 ConfigStore `wallpaper` 域,`KeyView{.group=..., .key=...}`(动态 key,无需注册,见 `cfconfig.hpp`):
- `switch_mode`:`"fixed" | "gradient" | "movement"`(默认 `"movement"`,对齐 CCIMX `DEF_MODE`)。
- `switch_interval_ms`:默认 `20000`。
- `animation_duration_ms`:默认 `2000`。
- (可选/后续)`switch_easing`:默认 `InOutCubic`。

在 [desktop_config_init.cpp](../../../desktop/main/init/desktop_config_init/desktop_config_init.cpp) 的 `WALLPAPER_CONFIG_TEMPLATE` 补默认值。

### 4.6 生命周期接线

[wallpaper_setup.cpp](../../../desktop/ui/components/shell_layer_impl/wallpaper_setup.cpp) 的 `create_wallpaper_strategy()` 现为 `make_unique<WallpaperShellLayerStrategy>(make_layer())`:
- strategy 内部构造 `WallPaperEngine`(传 layer 原始指针 + `request_transition` 回调)。
- `activate()` 末尾 `engine->start()`;`deactivate()` 里 `engine->stop()`。
- `mode==Fixed` 或 `storage->size()<=1`:`start()` 不启动 timer(照搬 CCIMX 守卫)。

### 4.7 CMake

新文件加入 [desktop/ui/components/wallpaper/CMakeLists.txt](../../../desktop/ui/components/wallpaper/CMakeLists.txt) 的 `cfdesktop_wallpaper` STATIC 目标(`WallPaperEngine.cpp`),或 shell_layer_impl 目标(看 engine 最终落点)。链接 `Qt6::Core`(QTimer/QPropertyAnimation 已由 Gui/Core 覆盖)。

---

## 五、待办步骤(接手 AI 执行)

- [ ] 0. 读本文件 + `WallPaperLayer.h` + `WallpaperShellLayerStrategy.{h,cpp}` + `wallpaper_setup.cpp` + `filter_target.h` + CCIMX 两源文件。
- [ ] 1. **先商量拉分支**(纪律见下),开 `feat/wallpaper-animation-engine`。
- [ ] 2. 配置先行:`WALLPAPER_CONFIG_TEMPLATE` 补 key + 一个读取 helper(后续 engine/strategy 都依赖)。
- [ ] 3. 实现 `WallPaperEngine.{h,cpp}` + 注册 CMake。
- [ ] 4. 扩展 `WallpaperShellLayerStrategy`:过渡状态机 + `Q_PROPERTY(progress)` + `composite()` + `beginTransition()`;改造 `onWallpaperChanged()` 区分瞬时/过渡路径。
- [ ] 5. 接线:`create_wallpaper_strategy` / `activate` / `deactivate` 构造与启停 engine。
- [ ] 6. 单元测试(`add_gtest_executable`,标 `"desktop;unit;wallpaper"`):engine 模式/定时/`>1` 守卫(用假 layer);strategy `composite()` 给定两张图 + t 验证输出尺寸与非空;Qt 动画用 `QSignalSpy`。
- [ ] 7. 文档:HandBook 壁纸页 + `status/current.md` + 本文件勾选 + `13_widget_apps.md` 壁纸段状态。
- [ ] 8. 验证(见第七节)。

---

## 六、约束(铁律)

- **三层依赖**:engine/strategy 在 desktop 层,不得 `#include` 反向。验证:`grep -r '#include.*\(ui/\|desktop/\)' base/` 须空。
- **Doxygen**:新公共类/函数按 [`document/DOXYGEN_REQUEST.md`](../../DOXYGEN_REQUEST.md);过 `python3 scripts/doxygen/lint.py`。
- **Ownership**:`WallPaperEngine` 用 `std::unique_ptr` 持有(由 strategy 持有);engine 对 `WallPaperLayer` 只持**非 owning 指针**(strategy 拥有 layer)。避免裸 `new`(除 Qt parent-ownership)。
- **No silent fallbacks**:配置 `switch_mode` 非法 → 显式 `log::warn` + 回退默认 `movement`,不要静默吞。
- **性能平衡**(见 [[balance-perf-even-when-told-to-ignore]]):过渡期 2000ms/60fps ≈ 120 次全屏 QImage 合成,可接受,但:① 仅过渡窗口内跑,非过渡空闲零开销(仅 timer);② Movement 合成用 `drawImage` 简单 blit,不走 `SmoothTransformation`;③ geometry 变化时若正在过渡,按既有 `onGeometryChanged` 重缩放逻辑处理(注意过渡中要同步重缩 `previous_scaled`)。
- **分支/提交纪律**:分支上 `add+commit` 可以;**绝不 push**(用户自己 push);commit message **英文**且**不加 `Co-Authored-By`**(见记忆 `no-co-authored-by-in-commits`、`branch-commit-allowed-push-never`)。

---

## 七、验证(端到端)

1. **Fixed**:不轮播,壁纸静止(行为同今天)。
2. **Gradient**:定时到点 → 交叉淡入,过渡平滑、结束显示新图;日志见 transition start/end。
3. **Movement**:平移滑入(旧左出 / 新右入),方向同 CCIMX。
4. **边界**:壁纸库 0 张/1 张 → 不崩溃、不启动 timer(`size()<=1` 守卫);`mode` 非法 → 回退 movement + warn。
5. **配置**:改 `switch_interval_ms`/`switch_mode`(重启或热更,注明支持哪种)→ 生效。
6. **回归**:`linux_run_tests.sh` 全绿(含新增动画测试);既有壁纸/发现行为不变(Fixed 模式 ≈ 今天)。
7. **门禁**:三层依赖 grep + `python3 scripts/doxygen/lint.py` 通过;`linux_fast_develop_build.sh` 通过。

---

## 八、待定决策(接手 AI 与用户确认)

1. **手动切换是否也走动画**:现有 `showNext/Prev/Target` 是否复用过渡路径?建议:是(timer 自动 + 手动都动画,体验一致)。
2. **缓动曲线是否配置化**:先固定 `InOutCubic`,随主题/设置面板(Phase 12)再暴露。
3. **低性能板降级**:6ULL 等是否默认 Fixed / 关动画?见 `desktop-buildout-handoff` 记忆的 6ULL 铁律;建议加一个 HWTier 判断或配置项。
4. **资源包 manifest 解析**:CF-Gallery `scripts/manifest.json`(带 photographer/width/height)是否本批接入,用于壁纸选择器 UI 的署名/尺寸?建议**本批不做**,随壁纸选择器 UI(Phase 12/13 后续)。

---

## 九、关键文件速查

| 文件 | 角色 |
|---|---|
| [WallPaperLayer.h](../../../desktop/ui/components/wallpaper/WallPaperLayer.h) | 数据层接口(不改) |
| [ImageWallPaperLayer.h](../../../desktop/ui/components/wallpaper/ImageWallPaperLayer.h) / [.cpp](../../../desktop/ui/components/wallpaper/ImageWallPaperLayer.cpp) | 数据层实现(不改) |
| [WallpaperShellLayerStrategy.h](../../../desktop/ui/components/shell_layer_impl/WallpaperShellLayerStrategy.h) / [.cpp](../../../desktop/ui/components/shell_layer_impl/WallpaperShellLayerStrategy.cpp) | **渲染缝,扩展过渡状态机** |
| [wallpaper_setup.cpp](../../../desktop/ui/components/shell_layer_impl/wallpaper_setup.cpp) | 装配点(`create_wallpaper_strategy`),接线 engine |
| [wallpaper/CMakeLists.txt](../../../desktop/ui/components/wallpaper/CMakeLists.txt) | 注册新 `WallPaperEngine.cpp` |
| [desktop_config_init.cpp](../../../desktop/main/init/desktop_config_init/desktop_config_init.cpp) | `WALLPAPER_CONFIG_TEMPLATE` 补 key |
| `~/CCIMXDesktop/core/wallpaper/WallPaperEngine.{h,cpp}` | 源参照(engine 语义) |
| `~/CCIMXDesktop/ui/wallpaperanimationhandler.{h,cpp}` | 源参照(过渡语义,勿抄 QLabel) |
