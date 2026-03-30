# Phase 2: Base 库核心详细设计文档

## 文档信息
| 项目 | 内容 |
|------|------|
| 文档版本 | v1.0 |
| 创建日期 | 2026-02-20 |
| 阶段代号 | Phase 2 - Base 库核心 |
| 预计周期 | 3~4 周 |
| 依赖阶段 | Phase 0, Phase 1 |

---

## 一、阶段目标

### 1.1 核心目标
建立所有上层模块依赖的基础设施，包括主题引擎、动画管理、分辨率适配、配置中心和日志系统。

### 1.2 具体交付物
- [x] `ThemeEngine` 主题引擎模块
- [x] `AnimationManager` 动画管理器
- [ ] `DPIManager` 分辨率适配管理器
- [x] `ConfigStore` 配置中心
- [x] `Logger` 日志系统
- [x] 各模块单元测试

---

## 二、模块架构设计

### 2.1 整体依赖关系

```
┌──────────────────────────────────────────────────────────┐
│                   Application Layer                       │
│                  (Shell / Third-party Apps)               │
├──────────────────────────────────────────────────────────┤
│                         SDK Layer                         │
├──────────────────────────────────────────────────────────┤
│                      Base Library                         │
│  ┌──────────────┐ ┌──────────────┐ ┌──────────────┐      │
│  │ ThemeEngine  │ │AnimationMgr  │ │  DPIManager  │      │
│  └──────────────┘ └──────────────┘ └──────────────┘      │
│  ┌──────────────┐ ┌──────────────┐ ┌──────────────┐      │
│  │ ConfigStore  │ │   Logger     │ │HardwareProbe │      │
│  └──────────────┘ └──────────────┘ └──────────────┘      │
├──────────────────────────────────────────────────────────┤
│                      Qt6 Framework                        │
│  Core / Gui / Widgets / Multimedia / Network             │
└──────────────────────────────────────────────────────────┘
```

### 2.2 Base 库目录结构

```
src/base/
├── include/CFDesktop/Base/
│   ├── ThemeEngine/
│   │   ├── ThemeEngine.h          # 主题引擎主类
│   │   ├── Theme.h                # 主题数据结构
│   │   ├── ThemeLoader.h          # 主题加载器
│   │   └── ThemeVariables.h       # 主题变量系统
│   │
│   ├── AnimationManager/
│   │   ├── AnimationManager.h     # 动画管理器
│   │   ├── Animation.h            # 动画基类
│   │   ├── PropertyAnimation.h    # 属性动画
│   │   ├── GroupAnimation.h       # 组动画
│   │   └── AnimationPolicy.h      # 动画策略（来自 Phase 1）
│   │
│   ├── DPIManager/
│   │   ├── DPIManager.h           # DPI 管理器
│   │   ├── DisplayInfo.h          # 显示信息
│   │   └── DPIConverter.h         # 单位转换工具
│   │
│   ├── ConfigStore/
│   │   ├── ConfigStore.h          # 配置存储主类
│   │   ├── ConfigNode.h           # 配置节点
│   │   └── ConfigWatcher.h        # 配置监视器
│   │
│   └── Logger/
│       ├── Logger.h               # 日志主类
│       ├── LogMessage.h           # 日志消息
│       ├── LogSink.h              # 日志输出抽象
│       ├── FileSink.h             # 文件输出
│       ├── ConsoleSink.h          # 控制台输出
│       └── NetworkSink.h          # 网络输出
│
└── src/
    ├── theme/
    │   ├── ThemeEngine.cpp
    │   ├── ThemeLoader.cpp
    │   ├── QSSProcessor.cpp       # QSS 处理器
    │   └── VariableResolver.cpp   # 变量解析器
    │
    ├── animation/
    │   ├── AnimationManager.cpp
    │   ├── PropertyAnimation.cpp
    │   └── GroupAnimation.cpp
    │
    ├── dpi/
    │   ├── DPIManager.cpp
    │   └── DPIConverter.cpp
    │
    ├── config/
    │   ├── ConfigStore.cpp
    │   └── ConfigWatcher.cpp
    │
    └── logging/
        ├── Logger.cpp
        ├── FileSink.cpp
        ├── ConsoleSink.cpp
        └── NetworkSink.cpp
```

---

## 三、主题引擎 (ThemeEngine)

### 3.1 主题包结构

```
assets/themes/
└── default/
    ├── theme.json                 # 主题元数据
    ├── variables/
    │   ├── colors.json            # 颜色变量
    │   ├── sizes.json             # 尺寸变量
    │   └── fonts.json             # 字体变量
    ├── styles/
    │   ├── base.qss               # 基础样式
    │   ├── widgets.qss            # 控件样式
    │   └── animations.qss         # 动画样式
    └── icons/
        ├── actions/               # 动作图标
        ├── status/                # 状态图标
        └── devices/               # 设备图标
```

### 3.2 主题元数据格式

**文件**: `assets/themes/default/theme.json`

```json
{
    "meta": {
        "name": "Default Light",
        "version": "1.0.0",
        "author": "CFDesktop Team",
        "description": "Default light theme",
        "hwTier": "any"
    },
    "inherit": null,
    "settings": {
        "windowOpacity": 0.95,
        "enableBlur": true,
        "enableShadows": true,
        "animationPolicy": "full"
    }
}
```

### 3.3 颜色变量定义

**文件**: `assets/themes/default/variables/colors.json`

```json
{
    "primary": "#2196F3",
    "primaryDark": "#1976D2",
    "primaryLight": "#BBDEFB",
    "secondary": "#FF9800",
    "accent": "#FF4081",
    "background": "#FFFFFF",
    "surface": "#F5F5F5",
    "error": "#F44336",
    "success": "#4CAF50",
    "warning": "#FFC107",
    "info": "#2196F3",
    "text": {
        "primary": "#212121",
        "secondary": "#757575",
        "disabled": "#BDBDBD",
        "inverse": "#FFFFFF"
    },
    "divider": "#E0E0E0",
    "shadow": "rgba(0, 0, 0, 0.12)"
}
```

### 3.4 QSS 模板语法

```qss
/* assets/themes/default/styles/base.qss */
QWidget {
    background-color: @background;
    color: @text.primary;
}

QPushButton {
    background-color: @primary;
    color: @text.inverse;
    border: none;
    border-radius: @radius.medium;
    padding: @padding.normal;
    font-size: @font.size.medium;
}

QPushButton:hover {
    background-color: @primaryLight;
}

QPushButton:pressed {
    background-color: @primaryDark;
}

QPushButton:disabled {
    background-color: @divider;
    color: @text.disabled;
}
```

### 3.5 ThemeEngine 类接口

**文件**: `include/CFDesktop/Base/ThemeEngine/ThemeEngine.h`

```cpp
#pragma once
#include <QObject>
#include <QString>
#include <QColor>
#include <QFont>
#include <QMap>
#include "Theme.h"

namespace CFDesktop::Base {

/**
 * @brief 主题引擎主类
 *
 * 负责加载、切换和管理主题，提供变量查询接口。
 * 单例模式，进程内唯一实例。
 */
class ThemeEngine : public QObject {
    Q_OBJECT

public:
    static ThemeEngine* instance();

    /**
     * @brief 加载主题
     * @param themePath 主题路径
     * @return 是否成功
     */
    bool loadTheme(const QString& themePath);

    /**
     * @brief 切换主题
     * @param themeName 主题名称
     */
    void switchTheme(const QString& themeName);

    /**
     * @brief 获取当前主题
     */
    Theme currentTheme() const;

    /**
     * @brief 获取颜色变量
     * @param key 变量名 (支持点分隔路径: "text.primary")
     */
    QColor color(const QString& key) const;

    /**
     * @brief 获取尺寸变量
     * @param key 变量名
     */
    int size(const QString& key) const;

    /**
     * @brief 获取字体变量
     * @param key 变量名
     */
    QFont font(const QString& key) const;

    /**
     * @brief 获取任意变量
     */
    QVariant variable(const QString& key) const;

    /**
     * @brief 应用主题到应用
     * @param app 目标应用
     */
    void applyToApplication(QApplication* app);

    /**
     * @brief 处理 QSS 中的变量引用
     * @param qss 原始 QSS
     * @return 处理后的 QSS
     */
    QString processQSS(const QString& qss) const;

    /**
     * @brief 获取当前主题样式表
     */
    QString styleSheet() const;

    /**
     * @brief 重新加载主题（热更新）
     */
    void reload();

signals:
    /**
     * @brief 主题变化信号
     */
    void themeChanged(const Theme& theme);

    /**
     * @brief 颜色变量变化信号
     */
    void colorChanged(const QString& key, const QColor& color);

private:
    ThemeEngine(QObject* parent = nullptr);
    ~ThemeEngine() = default;

    void loadVariables(const QString& path);
    void loadStyles(const QString& path);
    void resolveThemeInheritance(Theme& theme);
    QString resolveVariables(const QString& input) const;

private:
    static ThemeEngine* s_instance;
    Theme m_currentTheme;
    QMap<QString, QColor> m_colors;
    QMap<QString, int> m_sizes;
    QMap<QString, QFont> m_fonts;
    QMap<QString, QString> m_styleSheets;
    QString m_themePath;
};

} // namespace CFDesktop::Base
```

### 3.6 主题降级策略

根据 HWTier 自动应用不同效果：

| 特性 | Low 档 | Mid 档 | High 档 |
|------|--------|--------|---------|
| 阴影效果 | 禁用 | 简单阴影 | 完整阴影 |
| 圆角 | 小圆角 (2px) | 中圆角 (4px) | 大圆角 (8px) |
| 模糊背景 | 禁用 | 半透明模糊 | 全模糊 |
| 渐变 | 禁用 | 简单渐变 | 复杂渐变 |
| 动画 | 禁用 | 基础动画 | 完整动画 |

---

## 四、动画管理器 (AnimationManager)

### 4.1 AnimationManager 类接口

**文件**: `include/CFDesktop/Base/AnimationManager/AnimationManager.h`

```cpp
#pragma once
#include <QObject>
#include <QPointer>
#include <QVariant>
#include <QPropertyAnimation>
#include "Animation.h"

namespace CFDesktop::Base {

/**
 * @brief 动画管理器
 *
 * 统一管理所有动画，根据 HWTier 自动调整动画参数。
 * Low 档位时所有动画时长归零，实现透明降级。
 */
class AnimationManager : public QObject {
    Q_OBJECT

public:
    static AnimationManager* instance();

    /**
     * @brief 创建属性动画
     * @param target 目标对象
     * @param propertyName 属性名
     * @param startValue 起始值
     * @param endValue 结束值
     * @param duration 动画时长 (毫秒)，Low 档会自动归零
     * @return 动画指针
     */
    QPropertyAnimation* createPropertyAnimation(
        QObject* target,
        const QByteArray& propertyName,
        const QVariant& startValue,
        const QVariant& endValue,
        int duration = 300
    );

    /**
     * @brief 创建淡入动画
     */
    QPropertyAnimation* createFadeIn(
        QWidget* widget,
        int duration = 300
    );

    /**
     * @brief 创建淡出动画
     */
    QPropertyAnimation* createFadeOut(
        QWidget* widget,
        int duration = 300
    );

    /**
     * @brief 创建滑入动画
     */
    QPropertyAnimation* createSlideIn(
        QWidget* widget,
        Qt::Direction direction,
        int distance = 100,
        int duration = 300
    );

    /**
     * @brief 创建缩放动画
     */
    QPropertyAnimation* createScale(
        QWidget* widget,
        qreal fromScale,
        qreal toScale,
        int duration = 300
    );

    /**
     * @brief 创建旋转动画
     */
    QPropertyAnimation* createRotation(
        QWidget* widget,
        qreal fromAngle,
        qreal toAngle,
        int duration = 300
    );

    /**
     * @brief 创建并行动画组
     */
    QParallelAnimationGroup* createParallelGroup(
        const QList<QAbstractAnimation*>& animations
    );

    /**
     * @brief 创建串行动画组
     */
    QSequentialAnimationGroup* createSequentialGroup(
        const QList<QAbstractAnimation*>& animations
    );

    /**
     * @brief 启动动画
     */
    void start(QAbstractAnimation* animation);

    /**
     * @brief 停止动画
     */
    void stop(QAbstractAnimation* animation);

    /**
     * @brief 停止所有动画
     */
    void stopAll();

    /**
     * @brief 调整动画时长（根据 HWTier）
     * @param originalDuration 原始时长
     * @return 调整后的时长
     */
    int adjustDuration(int originalDuration) const;

    /**
     * @brief 是否启用动画
     */
    bool isAnimationEnabled() const;

signals:
    /**
     * @brief 动画策略变化信号
     */
    void animationPolicyChanged(const AnimationPolicy& policy);

private:
    AnimationManager(QObject* parent = nullptr);
    ~AnimationManager() = default;

    void updatePolicy();
    QPropertyAnimation* createBaseAnimation(
        QObject* target,
        const QByteArray& propertyName
    );

private:
    static AnimationManager* s_instance;
    AnimationPolicy m_policy;
    QList<QPointer<QAbstractAnimation>> m_runningAnimations;
};

} // namespace CFDesktop::Base
```

### 4.2 预定义动画类型

```cpp
namespace CFDesktop::Base {

/**
 * @brief 预定义动画类型
 */
enum class AnimationType {
    None,           // 无动画
    Fade,           // 淡入淡出
    Slide,          // 滑动
    Scale,          // 缩放
    Rotate,         // 旋转
    Bounce,         // 弹跳
    Elastic,        // 弹性
    Complex         // 复杂组合
};

/**
 * @brief 缓动曲线类型
 */
enum class EasingType {
    Linear,
    InQuad, OutQuad, InOutQuad,
    InCubic, OutCubic, InOutCubic,
    InQuart, OutQuart, InOutQuart,
    InQuint, OutQuint, InOutQuint,
    InBack, OutBack, InOutBack,
    InBounce, OutBounce, InOutBounce
};

/**
 * @brief 获取 Qt 缓动曲线
 */
QEasingCurve easingCurve(EasingType type);

} // namespace CFDesktop::Base
```

---

## 五、DPI 管理器 (DPIManager)

### 5.1 DPIManager 类接口

**文件**: `include/CFDesktop/Base/DPIManager/DPIManager.h`

```cpp
#pragma once
#include <QObject>
#include <QRect>
#include "DisplayInfo.h"

namespace CFDesktop::Base {

/**
 * @brief DPI 管理器
 *
 * 提供分辨率无关的尺寸单位，支持多屏幕和热插拔。
 */
class DPIManager : public QObject {
    Q_OBJECT

public:
    static DPIManager* instance();

    /**
     * @brief 密度无关像素 (dp) 转换为物理像素
     *
     * 类 Android 的 dp 单位，在 160dpi 屏幕上 1dp = 1px
     */
    int dp(int value) const;

    /**
     * @brief 缩放无关像素 (sp) 转换为物理像素
     *
     * 用于字体大小，会跟随用户字体缩放设置
     */
    int sp(int value) const;

    /**
     * @brief 物理像素转换为 dp
     */
    int pxToDp(int pixels) const;

    /**
     * @brief 获取屏幕 DPI
     */
    int screenDPI() const;

    /**
     * @brief 获取设备像素比
     */
    qreal devicePixelRatio() const;

    /**
     * @brief 获取屏幕尺寸 (英寸)
     */
    QSizeF screenSizeInches() const;

    /**
     * @brief 获取屏幕逻辑分辨率
     */
    QSize logicalSize() const;

    /**
     * @brief 获取屏幕物理分辨率
     */
    QSize physicalSize() const;

    /**
     * @brief 是否为高 DPI 屏幕
     */
    bool isHighDPI() const;

    /**
     * @brief 注入屏幕参数（用于模拟器）
     */
    void injectScreenParameters(
        int dpi,
        qreal devicePixelRatio,
        const QSize& size
    );

    /**
     * @brief 清除注入的参数
     */
    void clearInjectedParameters();

signals:
    /**
     * @brief DPI 变化信号
     */
    void dpiChanged(int oldDpi, int newDpi);

    /**
     * @brief 屏幕变化信号
     */
    void screenChanged(const QRect& geometry);

private:
    DPIManager(QObject* parent = nullptr);
    ~DPIManager() = default;

    void detectScreenParameters();
    void updateDpi();

private:
    static DPIManager* s_instance;
    int m_dpi = 96;
    qreal m_devicePixelRatio = 1.0;
    QSize m_logicalSize;
    QSize m_physicalSize;
    bool m_hasInjectedParams = false;
};

/**
 * @brief 便捷函数
 */
inline int dp(int value) { return DPIManager::instance()->dp(value); }
inline int sp(int value) { return DPIManager::instance()->sp(value); }

} // namespace CFDesktop::Base
```

### 5.2 DPI 计算公式

```cpp
int DPIManager::dp(int value) const {
    // 基准 DPI 为 160
    const qreal BASE_DPI = 160.0;
    return qRound(value * m_dpi / BASE_DPI * m_devicePixelRatio);
}

int DPIManager::sp(int value) const {
    // sp 考虑字体缩放因子
    const qreal BASE_DPI = 160.0;
    qreal fontScale = QApplication::font().pointSizeF() / 10.0;
    return qRound(value * m_dpi / BASE_DPI * m_devicePixelRatio * fontScale);
}
```

### 5.3 常用尺寸定义

```cpp
namespace CFDesktop::Base::Sizes {
    // 间距 (使用 dp)
    inline int tiny() { return dp(2); }
    inline int small() { return dp(4); }
    inline int normal() { return dp(8); }
    inline int medium() { return dp(16); }
    inline int large() { return dp(24); }
    inline int xlarge() { return dp(32); }
    inline int xxlarge() { return dp(48); }

    // 圆角
    inline int radiusSmall() { return dp(2); }
    inline int radiusMedium() { return dp(4); }
    inline int radiusLarge() { return dp(8); }
    inline int radiusXLarge() { return dp(16); }

    // 控件尺寸
    inline int buttonHeight() { return dp(40); }
    inline int inputHeight() { return dp(36); }
    inline int iconSizeSmall() { return dp(16); }
    inline int iconSizeMedium() { return dp(24); }
    inline int iconSizeLarge() { return dp(48); }
}
```

---

## 六、配置中心 (ConfigStore)

### 6.1 ConfigStore 类接口

**文件**: `include/CFDesktop/Base/ConfigStore/ConfigStore.h`

```cpp
#pragma once
#include <QObject>
#include <QString>
#include <QVariant>
#include <QSettings>
#include "ConfigNode.h"

namespace CFDesktop::Base {

/**
 * @brief 配置类型
 */
enum class ConfigType {
    System,     // 系统配置 (/etc/CFDesktop/config.conf)
    User,       // 用户配置 (~/.config/CFDesktop/config.conf)
    App,        // 应用配置
    Temp        // 临时配置 (内存)
};

/**
 * @brief 配置中心
 *
 * 提供层级化的配置存储，支持命名空间和变更监听。
 */
class ConfigStore : public QObject {
    Q_OBJECT

public:
    static ConfigStore* instance();

    /**
     * @brief 获取配置值
     * @param key 配置键，支持点分隔命名空间 (如 "ui.theme.name")
     * @param defaultValue 默认值
     */
    QVariant get(
        const QString& key,
        const QVariant& defaultValue = QVariant()
    ) const;

    /**
     * @brief 设置配置值
     * @param key 配置键
     * @param value 配置值
     * @param type 配置类型
     */
    void set(
        const QString& key,
        const QVariant& value,
        ConfigType type = ConfigType::User
    );

    /**
     * @brief 删除配置
     */
    void remove(const QString& key, ConfigType type = ConfigType::User);

    /**
     * @brief 检查配置是否存在
     */
    bool contains(const QString& key) const;

    /**
     * @brief 获取配置节点
     */
    ConfigNode node(const QString& path) const;

    /**
     * @brief 保存配置
     */
    void save(ConfigType type = ConfigType::User);

    /**
     * @brief 重新加载配置
     */
    void reload(ConfigType type = ConfigType::User);

    /**
     * @brief 设置配置变更监听
     * @param key 监听的键
     * @param receiver 接收对象
     * @param slot 槽函数
     */
    void watch(
        const QString& key,
        QObject* receiver,
        const char* slot
    );

signals:
    /**
     * @brief 配置变化信号
     */
    void configChanged(const QString& key, const QVariant& value);

private:
    ConfigStore(QObject* parent = nullptr);
    ~ConfigStore();

    void initialize();
    QString normalizeKey(const QString& key) const;
    QSettings* settingsForType(ConfigType type) const;

private:
    static ConfigStore* s_instance;
    QScopedPointer<QSettings> m_systemSettings;
    QScopedPointer<QSettings> m_userSettings;
    QHash<QString, QVariant> m_tempSettings;
    QMultiHash<QString, QPointer<QObject>> m_watchers;
};

/**
 * @brief 便捷访问函数
 */
template<typename T>
T getConfig(const QString& key, const T& defaultValue = T()) {
    return ConfigStore::instance()->get(key, QVariant::fromValue(defaultValue)).value<T>();
}

template<typename T>
void setConfig(const QString& key, const T& value) {
    ConfigStore::instance()->set(key, QVariant::fromValue(value));
}

} // namespace CFDesktop::Base
```

### 6.2 配置文件格式

**系统配置**: `/etc/CFDesktop/config.conf`

```ini
[General]
Version=1.0

[Hardware]
Tier=auto
MaxMemoryMB=512

[UI]
Theme=default
EnableAnimations=auto
RenderingBackend=auto

[Network]
AutoConnect=true
UpdateCheck=true

[Logging]
Level=Info
MaxFileSizeMB=10
MaxFiles=5
```

**用户配置**: `~/.config/CFDesktop/config.conf`

```ini
[General]
Language=zh_CN

[UI]
Theme=dark
EnableAnimations=true
StatusBarPosition=top

[User]
Name=User
AutoLogin=false
```

---

## 七、日志系统 (Logger)

### 7.1 Logger 类接口

**文件**: `include/CFDesktop/Base/Logger/Logger.h`

```cpp
#pragma once
#include <QObject>
#include <QString>
#include "LogMessage.h"

namespace CFDesktop::Base {

/**
 * @brief 日志等级
 */
enum class LogLevel {
    Trace = 0,
    Debug,
    Info,
    Warning,
    Error,
    Fatal
};

/**
 * @brief 日志输出抽象基类
 */
class LogSink {
public:
    virtual ~LogSink() = default;
    virtual void write(const LogMessage& message) = 0;
    virtual void flush() = 0;
};

/**
 * @brief 日志主类
 *
 * 提供统一的日志接口，支持多个输出目标。
 */
class Logger : public QObject {
    Q_OBJECT

public:
    static Logger* instance();

    /**
     * @brief 记录日志
     */
    void log(LogLevel level, const QString& tag, const QString& message);

    /**
     * @brief 添加日志输出
     */
    void addSink(LogSink* sink);

    /**
     * @brief 移除日志输出
     */
    void removeSink(LogSink* sink);

    /**
     * @brief 设置最低日志等级
     */
    void setMinLevel(LogLevel level);

    /**
     * @brief 获取最低日志等级
     */
    LogLevel minLevel() const;

    /**
     * @brief 设置日志标签过滤器
     *
     * 只有标签在列表中的日志才会被记录。
     * 空列表表示不过滤。
     */
    void setTagFilter(const QStringList& tags);

    /**
     * @brief 刷新所有日志输出
     */
    void flush();

    // 便捷方法
    void trace(const QString& tag, const QString& message);
    void debug(const QString& tag, const QString& message);
    void info(const QString& tag, const QString& message);
    void warning(const QString& tag, const QString& message);
    void error(const QString& tag, const QString& message);
    void fatal(const QString& tag, const QString& message);

signals:
    /**
     * @brief 日志写入信号
     */
    void messageLogged(const LogMessage& message);

private:
    Logger(QObject* parent = nullptr);
    ~Logger() = default;

    QString formatMessage(const LogMessage& message) const;
    QString levelToString(LogLevel level) const;

private:
    static Logger* s_instance;
    QList<LogSink*> m_sinks;
    LogLevel m_minLevel = LogLevel::Info;
    QStringList m_tagFilters;
    QMutex m_mutex;
};

// 便捷宏
#define LOG_TRACE(tag, msg) Logger::instance()->trace(tag, msg)
#define LOG_DEBUG(tag, msg) Logger::instance()->debug(tag, msg)
#define LOG_INFO(tag, msg)  Logger::instance()->info(tag, msg)
#define LOG_WARNING(tag, msg) Logger::instance()->warning(tag, msg)
#define LOG_ERROR(tag, msg) Logger::instance()->error(tag, msg)
#define LOG_FATAL(tag, msg) Logger::instance()->fatal(tag, msg)

} // namespace CFDesktop::Base
```

### 7.2 LogMessage 结构

```cpp
namespace CFDesktop::Base {

/**
 * @brief 日志消息结构
 */
struct LogMessage {
    LogLevel level;              // 日志等级
    QString tag;                 // 标签
    QString message;             // 消息内容
    QDateTime timestamp;         // 时间戳
    QString threadId;            // 线程 ID
    QString sourceFile;          // 源文件
    int sourceLine;              // 源文件行号
    QString function;            // 函数名

    QString toString() const;
};

} // namespace CFDesktop::Base
```

### 7.3 LogSink 实现

#### FileSink

```cpp
namespace CFDesktop::Base {

/**
 * @brief 文件日志输出
 */
class FileSink : public LogSink {
public:
    explicit FileSink(const QString& filePath);
    ~FileSink() override;

    void write(const LogMessage& message) override;
    void flush() override;

    void setMaxFileSize(int bytes);
    void setMaxFiles(int count);

private:
    void checkRotation();
    void rotate();

private:
    QFile m_file;
    QTextStream m_stream;
    int m_maxFileSize = 10 * 1024 * 1024;  // 10MB
    int m_maxFiles = 5;
    QMutex m_mutex;
};

} // namespace CFDesktop::Base
```

#### ConsoleSink

```cpp
namespace CFDesktop::Base {

/**
 * @brief 控制台日志输出
 *
 * 支持彩色输出。
 */
class ConsoleSink : public LogSink {
public:
    explicit ConsoleSink(bool enableColor = true);
    ~ConsoleSink() override = default;

    void write(const LogMessage& message) override;
    void flush() override;

private:
    QString colorize(const QString& text, LogLevel level) const;

private:
    bool m_enableColor;
};

} // namespace CFDesktop::Base
```

#### NetworkSink

```cpp
namespace CFDesktop::Base {

/**
 * @brief 网络日志输出
 *
 * 通过 UDP 发送日志到远程服务器。
 */
class NetworkSink : public LogSink {
public:
    explicit NetworkSink(const QString& host, quint16 port);
    ~NetworkSink() override;

    void write(const LogMessage& message) override;
    void flush() override;

private:
    QUdpSocket m_socket;
    QString m_host;
    quint16 m_port;
};

} // namespace CFDesktop::Base
```

### 7.4 日志配置

**文件**: `configs/logging.conf`

```ini
[General]
Level=Debug
# 过滤标签，空表示不过滤
TagFilters=HardwareProbe,ThemeEngine,AnimationManager

[Console]
Enabled=true
Color=true

[File]
Enabled=true
Path=/var/log/CFDesktop/desktop.log
MaxFileSizeMB=10
MaxFiles=5

[Network]
Enabled=false
Host=192.168.1.100
Port=514
```

---

## 八、详细任务清单

### 8.1 Week 1: 主题引擎

#### Day 1-2: 基础结构
- [ ] 创建 ThemeEngine 类框架
- [ ] 定义主题数据结构
- [ ] 实现主题加载器
- [ ] 创建默认主题包

#### Day 3: 变量系统
- [ ] 实现变量解析器
- [ ] 支持点分隔路径
- [ ] 实现变量继承

#### Day 4: QSS 处理
- [ ] 实现 QSS 变量替换
- [ ] 实现 QSS 合并
- [ ] 支持主题继承

#### Day 5: 集成与测试
- [ ] 集成到应用
- [ ] 编写单元测试
- [ ] 性能优化

### 8.2 Week 2: 动画管理器

#### Day 1-2: 核心功能
- [ ] 创建 AnimationManager 类
- [ ] 实现基础动画创建
- [ ] 实现 HWTier 降级逻辑

#### Day 3: 预定义动画
- [ ] 实现淡入淡出
- [ ] 实现滑动动画
- [ ] 实现缩放动画
- [ ] 实现旋转动画

#### Day 4: 组动画
- [ ] 实现并行动画组
- [ ] 实现串行动画组
- [ ] 实现动画生命周期管理

#### Day 5: 测试与优化
- [ ] 编写单元测试
- [ ] 性能测试
- [ ] 内存泄漏检查

### 8.3 Week 3: DPI 管理器与配置中心

#### Day 1-2: DPI 管理器
- [ ] 创建 DPIManager 类
- [ ] 实现屏幕检测
- [ ] 实现 dp/sp 转换
- [ ] 实现模拟器注入接口

#### Day 3-4: 配置中心
- [ ] 创建 ConfigStore 类
- [ ] 实现层级存储
- [ ] 实现变更监听
- [ ] 实现配置持久化

#### Day 5: 测试
- [ ] 跨平台测试
- [ ] 单元测试
- [ ] 集成测试

### 8.4 Week 4: 日志系统与集成

#### Day 1-2: 日志系统
- [ ] 创建 Logger 类
- [ ] 实现 FileSink
- [ ] 实现 ConsoleSink
- [ ] 实现 NetworkSink

#### Day 3: 日志增强
- [ ] 实现日志轮转
- [ ] 实现标签过滤
- [ ] 实现彩色输出

#### Day 4: 集成测试
- [ ] 全模块集成
- [ ] 端到端测试
- [ ] 性能测试

#### Day 5: 文档与发布
- [ ] API 文档
- [ ] 使用示例
- [ ] Code Review

---

## 九、验收标准

### 9.1 主题引擎
- [ ] 支持动态切换主题
- [ ] 支持变量继承
- [ ] Low 档位自动禁用阴影/圆角/渐变
- [ ] 热重载时间 < 100ms

### 9.2 动画管理器
- [ ] Low 档位动画时长归零
- [ ] 所有预定义动画正常工作
- [ ] 无内存泄漏
- [ ] 动画不阻塞主线程

### 9.3 DPI 管理器
- [ ] dp/sp 转换准确
- [ ] 支持模拟器注入
- [ ] 热插拔屏幕自动适配

### 9.4 配置中心
- [ ] 层级存储正常
- [ ] 变更监听触发
- [ ] 持久化正常

### 9.5 日志系统
- [ ] 多 Sink 并发安全
- [ ] 日志轮转正常
- [ ] 性能影响 < 1%

---

## 十、下一步行动

完成 Phase 2 后，进入 **Phase 3: 输入抽象层**。
