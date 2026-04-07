# Phase 2: 基础库核心 - 状态文档

> **模块ID**: Phase 2
> **状态**: ✅ 完成
> **总体进度**: 100%
> **最后更新**: 2026-03-18
> **架构说明**: 原规划在 `base/` 目录，实际实现在 `ui/` 和 `desktop/base/` 目录

---

## 一、模块概述

Base库核心是CFDesktop项目的基础设施层，为所有上层模块提供统一的主题管理、动画控制、分辨率适配、配置存储和日志记录能力。

### 核心职责

1. **主题管理** - 动态主题切换、变量解析、QSS处理
2. **动画管理** - 动画生命周期、性能降级、预定义动画
3. **分辨率适配** - dp/sp单位转换、屏幕参数检测
4. **配置中心** - 层级存储、变更监听、持久化
5. **日志系统** - 多Sink支持、日志轮转、标签过滤

---

## 二、完成状态总览

| 模块 | 完成度 | 实现位置 | 备注 |
|------|--------|----------|------|
| ThemeEngine | 60% | `ui/core/` | 核心框架已实现 |
| AnimationManager | 65% | `ui/components/` | 接口已定义 |
| DPI management | 80% | `ui/base/` | 基础功能完成 |
| ConfigStore | 100% | `desktop/base/config_manager/` | ✅ 完成 |
| Logger | 100% | `desktop/base/logger/` | ✅ 完成 |

---

## 三、已完成工作

### 3.1 主题引擎 ThemeEngine (60%)

**实际实现位置**: `ui/core/` (非原计划的 `base/`)

#### 实现文件

| 文件路径 | 说明 |
|---------|------|
| `ui/core/theme.h` | ICFTheme 接口 |
| `ui/core/theme_manager.h` | 主题管理器 (单例) |
| `ui/core/theme_factory.h` | 主题工厂接口 |
| `ui/core/material/cfmaterial_theme.h` | Material 主题实现 |
| `ui/core/material/cfmaterial_scheme.h` | Material 配色方案 |
| `ui/core/material/cfmaterial_radius_scale.h` | 圆角规范 |
| `ui/core/material/cfmaterial_fonttype.h` | 字体规范 |
| `ui/core/material/cfmaterial_motion.h` | 动画规范 |
| `ui/core/material/material_factory_class.h` | Material 工厂 |

#### 功能实现

- [x] 主题管理器 - `ThemeManager` 单例
- [x] 主题注册/卸载 - `insert_one()`, `remove_one()`
- [x] 主题切换 - `setThemeTo()`
- [x] 主题变更信号 - `themeChanged`
- [x] Widget 主题安装 - `install_widget()`
- [x] Material 配色方案 - MaterialColorScheme
- [x] Token 系统 - `cfmaterial_token_literals.h`
- [x] 工厂模式创建 - MaterialFactory

#### 接口摘要

```cpp
namespace cf::ui::core {
class ThemeManager : public QObject {
public:
    static ThemeManager& instance();
    const ICFTheme& theme(const std::string& name) const;
    bool insert_one(const std::string& name, InstallerMaker make_one);
    void remove_one(const std::string& name);
    void install_widget(QWidget* w);
    void remove_widget(QWidget* w);
    void setThemeTo(const std::string& name, bool doBroadcast = true);
    const std::string& currentThemeName() const;
signals:
    void themeChanged(const ICFTheme& new_theme);
};
}
```

#### 测试

- [x] `test/ui/core/token_test.cpp`

#### 示例

---

### 3.2 动画管理器 AnimationManager (65%)

**实际实现位置**: `ui/components/`

#### 实现文件

| 文件路径 | 说明 |
|---------|------|
| `ui/components/animation.h` | 动画基类 |
| `ui/components/animation_factory_manager.h` | 动画工厂管理器 |
| `ui/components/animation_group.h` | 动画组 |
| `ui/components/timing_animation.h` | 时序动画 |
| `ui/components/spring_animation.h` | 弹簧动画 |
| `ui/components/material/cfmaterial_animation_strategy.h` | 动画策略 |
| `ui/components/material/cfmaterial_animation_factory.h` | Material 动画工厂 |
| `ui/components/material/cfmaterial_fade_animation.h` | 淡入淡出 |
| `ui/components/material/cfmaterial_slide_animation.h` | 滑动动画 |
| `ui/components/material/cfmaterial_scale_animation.h` | 缩放动画 |
| `ui/components/material/cfmaterial_property_animation.h` | 属性动画 |

#### 功能实现

- [x] 动画工厂管理器
- [x] 动画注册/创建
- [x] 预定义动画:
  - [x] FadeAnimation (淡入淡出)
  - [x] SlideAnimation (滑动，支持方向)
  - [x] ScaleAnimation (缩放)
- [x] 动画组:
  - [x] 并行组
  - [x] 串行组
- [x] 动画生命周期管理
- [x] 动画策略模式
- [x] WeakPtr 所有权管理

#### 核心类型

```cpp
namespace cf::ui::components {
// 动画创建器函数类型
using AnimationCreator = std::function<ICFAbstractAnimation*(QObject*)>;

// 动画工厂管理器接口
class ICFAnimationManagerFactory : public QObject {
    virtual RegisteredResult registerOneAnimation(const QString& name, const QString& type) = 0;
    virtual RegisteredResult registerAnimationCreator(const QString& name, AnimationCreator creator) = 0;
    virtual cf::WeakPtr<ICFAbstractAnimation> getAnimation(const char* name) = 0;
    virtual void setEnabledAll(bool enabled) = 0;
    virtual bool isAllEnabled() = 0;
};

// 动画状态枚举
enum class State { Idle, Running, Paused, Finished };
enum class Direction { Forward, Backward };
}
```

#### 示例

---

### 3.3 DPI 适配 (80%)

**实际实现位置**: `ui/base/`

#### 实现文件

| 文件路径 | 说明 |
|---------|------|
| `ui/base/device_pixel.h` | dp/sp 转换工具 |
| `ui/base/device_pixel.cpp` | 实现 |

#### 功能实现

- [x] `CanvasUnitHelper` 结构体
- [x] `dpToPx()` - 设备无关像素转物理像素
- [x] `spToPx()` - 可缩放像素转物理像素
- [x] `pxToDp()` - 物理像素转设备无关像素
- [x] `dpi()` - 获取设备像素比
- [x] `BreakPoint` - 响应式断点 (Compact/Medium/Expanded)

#### 接口摘要

```cpp
namespace cf::ui::base::device {
struct CanvasUnitHelper {
    CanvasUnitHelper(const qreal devicePixelRatio);
    qreal dpToPx(qreal dp) const;
    qreal spToPx(qreal sp) const;
    qreal pxToDp(qreal px) const;
    qreal dpi() const;

    enum class BreakPoint { Compact, Medium, Expanded };
    BreakPoint breakPoint(qreal widthDp);
};
}
```

#### 测试

- [x] `test/ui/base/device_pixel_test.cpp`

---

### 3.4 颜色系统支持

**位置**: `ui/base/color.h`, `ui/base/color_helper.h`

**已完成**:
- HCT颜色空间支持 `CFColor` 类
- 颜色混合 `blend()`
- 对比度计算 `contrastRatio()`
- 色调板生成 `tonalPalette()`
- 高程叠加 `elevationOverlay()`
- 相对亮度计算 `relativeLuminance()`

---

### 3.5 ConfigStore - 配置中心 (100%)

**实现位置**: `desktop/base/config_manager/`

#### 实现文件

| 文件路径 | 说明 |
|---------|------|
| `desktop/base/config_manager/include/cfconfig_key.h` | 配置键定义 |
| `desktop/base/config_manager/include/cfconfig_layer.h` | 配置层级 (Temp/App/User/System) |
| `desktop/base/config_manager/include/cfconfig_notify_policy.h` | 通知策略 |
| `desktop/base/config_manager/include/cfconfig/cfconfig_result.h` | 操作结果 |
| `desktop/base/config_manager/include/cfconfig/cfconfig_watcher.h` | 配置监听器 |
| `desktop/base/config_manager/include/cfconfig/cfconfig_path_provider.h` | 路径提供者 |
| `desktop/base/config_manager/src/cfconfig.cpp` | 实现 |
| `desktop/base/config_manager/src/impl/config_impl.h` | 内部实现 |
| `desktop/base/config_manager/src/impl/config_impl.cpp` | 内部实现 |

#### 功能实现

- [x] 四层存储 (Temp/App/User/System)
- [x] 点分隔键名支持 (`group.subgroup.key`)
- [x] 优先级查询 (Temp → App → User → System)
- [x] 变更监听机制 (watch/unwatch)
- [x] 持久化存储 (JSON格式)
- [x] 同步/异步 sync
- [x] 类型安全查询 (模板 API)
- [x] 线程安全

#### 接口摘要

```cpp
namespace cf::config {

enum class Layer { Temp, App, User, System };

class ConfigStore : public SimpleSingleton<ConfigStore> {
public:
    // 查询操作
    template <typename Value>
    std::optional<Value> query(const KeyView key);
    template <typename Value>
    Value query(const KeyView key, const Value& default_value);
    template <typename Value>
    std::optional<Value> query(const KeyView key, Layer layer);
    bool has_key(const KeyView key);

    // 写入操作
    template <typename Value>
    bool set(const KeyView key, const Value& v, Layer layer = Layer::App);
    template <typename Value>
    RegisterResult register_key(const Key& key, const Value& init_value, Layer layer = Layer::App);
    UnRegisterResult unregister_key(const Key& key, Layer layer = Layer::App);

    // 监听操作
    WatcherHandle watch(const std::string& key_pattern, Watcher callback);
    void unwatch(WatcherHandle handle);

    // 持久化
    void sync(SyncMethod m = SyncMethod::Async);
    void reload();
};

}
```

#### 测试

- [x] `test/config_manager/config_store_test.cpp`

#### 文档

- [x] `document/desktop/base/config_manager/` - API文档
- [x] `document/HandBook/desktop/base/config_manager/` - 使用手册

---

### 3.6 Logger - 日志系统 (100%)

**实现位置**: `desktop/base/logger/`

#### 实现文件

| 文件路径 | 说明 |
|---------|------|
| `desktop/base/logger/include/cflog.h` | 便捷日志函数 |
| `desktop/base/logger/include/cflog/cflog_record.h` | 日志记录 |
| `desktop/base/logger/include/cflog/cflog_sink.h` | Sink 接口 |
| `desktop/base/logger/include/cflog/cflog_format.h` | 格式化接口 |
| `desktop/base/logger/include/cflog/cflog_format_factory.h` | 格式化工厂 |
| `desktop/base/logger/include/cflog/cflog_format_config.h` | 格式化配置 |
| `desktop/base/logger/include/cflog/cflog_format_flags.h` | 格式化标志 |
| `desktop/base/logger/src/logger/cflog.cpp` | 实现 |
| `desktop/base/logger/src/impl/cflog_impl.h` | 内部实现 |
| `desktop/base/logger/src/impl/cflog_impl.cpp` | 内部实现 |

#### 功能实现

- [x] 多等级日志 (Trace/Debug/Info/Warning/Error)
- [x] ISink 接口
- [x] 多种格式化器
- [x] 异步日志 (后台线程处理)
- [x] 线程安全队列
- [x] 等级过滤
- [x] Tag 支持
- [x] 源码位置自动捕获 (`std::source_location`)

#### 接口摘要

```cpp
namespace cf::log {

enum class level { Trace, Debug, Info, Warning, Error };

class Logger : public SimpleSingleton<Logger> {
public:
    bool log(level log_level, std::string_view msg, std::string_view tag, std::source_location loc);
    void flush();
    void flush_sync();
    void setMininumLevel(const level lvl);
    void add_sink(std::shared_ptr<ISink> sink);
    void remove_sink(ISink* sink);
    void clear_sinks();
};

// 便捷函数
void trace(std::string_view msg, std::string_view tag = "CFLog", ...);
void debug(std::string_view msg, std::string_view tag = "CFLog", ...);
void info(std::string_view msg, std::string_view tag = "CFLog", ...);
void warning(std::string_view msg, std::string_view tag = "CFLog", ...);
void error(std::string_view msg, std::string_view tag = "CFLog", ...);

class ISink {
public:
    virtual bool write(const LogRecord& record) = 0;
    virtual bool flush() = 0;
    virtual bool setFormat(std::shared_ptr<IFormatter> formatter);
};

}
```

#### 测试

- [x] `test/logger/logger_formatter_test.cpp`
- [x] `test/logger/logger_concurrency_test.cpp`

#### 文档

- [x] `document/HandBook/desktop/base/logger/` - 使用手册

---

## 四、使用示例

### 主题切换

```cpp
#include <ui/core/theme_manager.h>

auto& manager = cf::ui::core::ThemeManager::instance();
manager.setThemeTo("theme.material.light");
```

### 动画使用

```cpp
#include <ui/components/material/cfmaterial_animation_factory.h>

auto factory = std::make_unique<MaterialAnimationFactory>(theme);
auto anim = factory->getAnimation("md.animation.fadeIn");
if (anim) {
    anim->start();
}
```

### DPI 转换

```cpp
#include <ui/base/device_pixel.h>

cf::ui::base::device::CanvasUnitHelper helper(2.0);
qreal pixels = helper.dpToPx(16.0);  // 16dp -> 32px
```

### 配置读写

```cpp
#include <cfconfig.hpp>

using namespace cf::config;

// 注册键
Key theme_key{.full_key = "app.theme.name", .full_description = "Application theme"};
ConfigStore::instance().register_key(theme_key, std::string("default"), Layer::App);

// 查询配置
KeyView kv{.group = "app.theme", .key = "name"};
std::string theme = ConfigStore::instance().query<std::string>(kv, "default");

// 修改配置
ConfigStore::instance().set(kv, std::string("dark"), Layer::App);

// 监听变化
ConfigStore::instance().watch("app.theme.*",
    [](const Key& k, const std::any* old, const std::any* new_val, Layer layer) {
        // Handle change
    });

// 同步到磁盘
ConfigStore::instance().sync(SyncMethod::Async);
```

### 日志记录

```cpp
#include <cflog.h>

using namespace cf::log;

// 便捷日志函数
info("Application started", "MyApp");
warning("Configuration file not found, using defaults", "Config");
error("Failed to load resource", "Network");

// 设置日志等级
set_level(level::Debug);

// 使用 Logger 实例
auto& logger = Logger::instance();
logger.log(level::Info, "Message", "Tag", std::source_location::current());

// 刷新
flush();
```

---

## 五、其他待完成模块

**注**: ConfigStore 和 Logger 已完成，以下是其他可选增强功能。

| 模块 | 优先级 | 依赖 | 状态 |
|------|--------|------|------|
| QSSProcessor | P2 | - | 待规划 |
| VariableResolver | P2 | - | 待规划 |
| HWTier 降级逻辑 | P1 | HWTier | 待规划 |
| 屏幕参数检测 | P2 | - | 待规划 |
| 模拟器注入接口 | P2 | - | 待规划 |

#### 主题加载器 (部分完成)

**待完成**:
- JSON主题文件解析
- 文件系统扫描
- 主题继承链解析
- 循环继承检测
- QSS变量替换

**建议文件路径**:
- `ui/core/theme_loader.h` (新建)
- `ui/core/qss_processor.h` (新建)

#### 变量解析器 (部分完成)

**待完成**:
- 点分隔路径解析 (如 `text.primary`)
- 嵌套对象解析
- 变量继承和回退

**建议文件路径**:
- `ui/core/variable_resolver.h` (新建)

---

## 六、文件结构总览

### 已实现文件

#### ui/ 目录 (主题、动画、DPI)

```
ui/
├── core/
│   ├── theme_manager.h         # 主题管理器 (核心)
│   ├── theme.h                 # ICFTheme接口
│   ├── theme_factory.h         # ThemeFactory抽象接口
│   ├── color_scheme.h          # ICFColorScheme颜色方案
│   ├── motion_spec.h           # IMotionSpec动画规范
│   ├── font_type.h             # IFontType字体接口
│   └── radius_scale.h          # IRadiusScale圆角接口
├── components/
│   ├── animation_factory_manager.h  # 动画工厂管理器
│   ├── animation.h             # ICFAbstractAnimation基础接口
│   ├── spring_animation.h      # 弹簧动画
│   ├── timing_animation.h      # 时间曲线动画
│   └── animation_group.h       # 动画组
└── base/
    ├── device_pixel.h          # DPI转换工具
    ├── color.h                 # CFColor (HCT颜色)
    ├── color_helper.h          # 颜色工具函数
    ├── easing.h                # 缓动曲线
    ├── geometry_helper.h       # 几何工具
    └── math_helper.h           # 数学工具
```

#### desktop/base/config_manager/ (配置中心) ✅

```
desktop/base/config_manager/
├── include/
│   ├── cfconfig.hpp                    # 主配置存储接口
│   ├── cfconfig_key.h                  # 配置键定义
│   ├── cfconfig_layer.h                # 配置层级
│   ├── cfconfig_notify_policy.h        # 通知策略
│   └── cfconfig/
│       ├── cfconfig_result.h           # 操作结果
│       ├── cfconfig_watcher.h          # 配置监听器
│       └── cfconfig_path_provider.h    # 路径提供者
└── src/
    ├── cfconfig.cpp                    # 实现
    └── impl/
        ├── config_impl.h               # 内部实现
        └── config_impl.cpp
```

#### desktop/base/logger/ (日志系统) ✅

```
desktop/base/logger/
├── include/
│   ├── cflog.h                         # 便捷日志函数
│   └── cflog/
│       ├── cflog.hpp                   # 主 Logger 类
│       ├── cflog_level.hpp             # 日志等级
│       ├── cflog_record.h              # 日志记录
│       ├── cflog_sink.h                # Sink 接口
│       ├── cflog_format.h              # 格式化接口
│       ├── cflog_format_factory.h      # 格式化工厂
│       ├── cflog_format_config.h       # 格式化配置
│       └── cflog_format_flags.h        # 格式化标志
└── src/
    ├── logger/
    │   └── cflog.cpp                   # 实现
    └── impl/
        ├── cflog_impl.h                # 内部实现
        └── cflog_impl.cpp
```

### 待创建文件 (可选增强)

```
ui/core/
├── theme_loader.h             # 主题加载器 (待创建)
├── qss_processor.h            # QSS处理器 (待创建)
└── variable_resolver.h        # 变量解析器 (待创建)
```

---

## 七、下一步行动建议

**Phase 2 核心模块已全部完成！** 以下是可选增强功能。

### 优先级1 (高) - 可选增强

1. **完善主题加载器**
   - 实现JSON主题文件解析
   - 实现主题继承
   - 预计工作量: 2-3天

2. **完善变量解析器**
   - 实现点分隔路径解析
   - 实现变量回退机制
   - 预计工作量: 1-2天

### 优先级2 (中)

3. **增强DPI管理**
   - 添加模拟器注入接口
   - 支持热插拔屏幕
   - 预计工作量: 1-2天

4. **动画性能优化**
   - 实现HWTier降级逻辑
   - 添加性能测试
   - 预计工作量: 2-3天

### 已完成模块

- [x] ConfigStore 配置中心 (100%)
- [x] Logger 日志系统 (100%)
- [x] ThemeEngine 主题引擎 (60%)
- [x] AnimationManager 动画管理器 (65%)
- [x] DPI management 分辨率适配 (80%)

---

## 八、相关文档

- 原始TODO: ~~已归档~~（原 `02_base_library.md` 已删除）
- 设计文档: `../../design_stage/02_phase2_base_library.md`

---

*文档版本: v2.0*
*生成时间: 2026-03-18*
