# CFDesktop 桌面本体规划文档

---

## 一、项目背景与现状

### 1.1 项目定性

CFDesktop 是一个基于 **Qt 6.8.3+ / C++23** 开发的**嵌入式桌面 UI 框架**，遵循 Material Design 3 规范。当前版本（0.13.1）本质上是一个**UI 组件库 + 底层架构**，尚不构成完整的桌面环境。

本文档的目标是在现有基础上，规划**桌面本体（Desktop Shell）**的完整建设路径。

### 1.2 当前已完成的基础（可复用）

| 模块 | 完成度 | 可复用价值 |
|------|--------|-----------|
| 工程骨架（CMake/CI/CD） | 100% | ✅ 直接复用 |
| Material Design 3 Token 系统 | 100% | ✅ 主题系统基础 |
| ThemeEngine 主题管理 | 100% | ✅ 桌面主题切换核心 |
| AnimationManager 动画引擎 | 100% | ✅ 桌面过渡动效基础 |
| DPI 管理 | 100% | ✅ 多分辨率屏幕适配 |
| 19个 P0+P1 控件 | 100% | ✅ 桌面 Shell UI 基础 |
| Google Test 测试框架 | 100% | ✅ 持续集成基础 |
| CPU/内存/GPU/网络检测器 | 90% | ✅ 硬件探针基本完成 |
| ConfigStore 配置中心 | 100% | ✅ 四层存储，INI持久化 |
| Logger 日志系统 | 100% | ✅ 异步日志，多Sink |
| Windows 显示后端 | 100% | ✅ Win32 DWM + HWND |
| WSL X11 显示后端 | 100% | ✅ XCB + XWayland |
| 显示服务器抽象 (IDisplayServerBackend) | 100% | ✅ 三模式抽象 |
| 窗口管理基础 (WindowManager/PanelManager) | 80% | ✅ 基础注册/查询/弱引用 |

### 1.3 当前关键缺口（桌面本体的前置依赖）

| 缺口模块 | 当前进度 | 对桌面本体的影响 |
|----------|--------|----------------|
| 输入抽象层（InputManager） | 0% | 🔴 所有交互的根基，必须最先完成 |
| HWTier 硬件分级系统 | 0% | 🔴 性能自适应的核心判据 |
| CrashHandler 崩溃处理 | 0% | 🟡 稳定性保障 |
| IPC 进程间通信 | 0% | 🟡 多进程架构依赖 |
| P2 高级控件（27个） | 0% | 🟡 文件管理器等应用依赖 |

---

## 二、需求定义

### 2.1 目标用户与场景

**主要面向**：
- 开源社区与个人开发者

**典型使用场景**：
- 嵌入式 Linux 设备上运行一个完整的桌面环境
- 开发者在 Windows 上开发调试，部署到 Linux 嵌入式设备
- 设备厂商基于 CFDesktop 定制自己品牌的桌面风格

### 2.2 目标硬件规格

| 档位 | 硬件规格 | 行为策略 |
|------|---------|---------|
| Low Tier | 无独立 GPU，≤512MB RAM，ARM Cortex-A5/A7 | 关闭动效、简化阴影、降低合成层数 |
| Mid Tier | 弱 GPU，1-2GB RAM，ARM Cortex-A53/A55 | 基础动效开启，限制并发动画数量 |
| High Tier | 独立 GPU，≥4GB RAM，ARM Cortex-A72/A76+ | 全量动效、模糊/透明效果全开 |

> **HWTier 系统是整个性能自适应的核心判据**，必须在桌面本体启动前完成探测并注入全局配置。

### 2.3 屏幕与输入规格

- **屏幕尺寸**：5.5 ～ 10.1 英寸矩形触摸屏
- **分辨率**：变化范围较大，依赖现有 DPI 管理模块动态适配
- **输入方式**：触摸（单点/多点手势）+ 鼠标键盘（开发调试/PC风格主题）
- **操作系统**：Linux 主力部署；Windows 作为开发调试等价环境

### 2.4 渲染后端策略

采用**多后端抽象层**，运行时按环境选择：

```
渲染后端抽象层（RenderBackend Interface）
    ├── EGLFS / LinuxFB   → 嵌入式 Linux 直驱（主要部署环境）
    ├── Wayland Client    → 跑在现有 Wayland 合成器之上
    ├── X11               → 旧版 Linux 桌面兼容
    └── Windows (Win32)   → 开发调试等价环境
```

> 不自研 Wayland Compositor，优先保证嵌入式 EGLFS 直驱路径的稳定性。

### 2.5 应用模型

- **多应用自由切换**（类手机/PC 体验）为主，同时支持 Kiosk 全屏模式配置
- **进程隔离**：每个 App 独立进程，桌面 Shell 作为宿主进程（Compositor Process）
- **系统服务**：核心服务单进程（通知/窗口管理/输入路由），可选服务（媒体控制/设置服务）独立进程，服务以**插件式**形式挂载
- **IPC 机制**：优先使用 Qt 自带机制（QLocalSocket / Shared Memory），如复杂度不可控再迁移 D-Bus

### 2.6 商业模式

**MIT 开源协议**，无商业授权限制。

---

## 三、桌面本体功能定义

### 3.1 核心子系统总览

```
CFDesktop Shell
├── 主题风格系统（Theme Style System）
│   ├── iOS 风格主题包
│   └── Windows 11 风格主题包
├── 窗口管理器（Window Manager）
├── 任务栏 / 导航系统（Shell Navigation）
├── 应用启动器（App Launcher）
├── 通知系统（Notification System）
├── 快捷控制中心（Control Center）
├── 系统设置（System Settings App）
├── 锁屏模块（Lock Screen）
├── 桌面壁纸与 Widget 系统（Desktop Layer）
├── 文件管理器（File Manager App）
├── 媒体控制服务（Media Control Service）
└── 硬件性能自适应引擎（HWTier Adaptive Engine）
```

### 3.2 双主题风格系统（核心特色）

CFDesktop 的核心差异化特性：**支持整体主题包切换 + 组件级微调**。

#### iOS 风格主题包

| 特性 | 描述 |
|------|------|
| 导航范式 | 底部手势条（类 iOS Home Indicator）+ 底部 Tab Bar |
| 启动器 | 网格图标桌面（类 iOS 主屏），支持图标拖拽排列 |
| 窗口过渡 | 全屏滑入/滑出（类 iOS App 打开/关闭动效） |
| 任务切换 | 上滑调出卡片式任务切换器（类 iOS App Switcher） |
| 控制中心 | 右上角下滑展开圆角磁贴面板 |
| 通知 | 顶部下滑展开通知列表 |
| 字体/圆角 | 大圆角卡片，SF Pro 风格字体层级 |
| 动效风格 | 弹簧动效（Spring Animation），响应灵敏 |

#### Windows 11 风格主题包

| 特性 | 描述 |
|------|------|
| 导航范式 | 底部居中任务栏（类 Win11 Centered Taskbar） |
| 启动器 | 点击任务栏图标展开开始菜单（类 Win11 Start Menu） |
| 窗口过渡 | 缩放淡入淡出（类 Win11 窗口动效） |
| 任务切换 | 任务栏悬停预览缩略图 + Task View 全览 |
| 控制中心 | 右下角系统托盘区点击展开快捷面板（类 Win11 Quick Settings） |
| 通知 | 右侧滑出通知中心面板（类 Win11 Action Center） |
| 字体/圆角 | 中圆角，Segoe UI 风格字体层级 |
| 动效风格 | 时间曲线动效（Ease In/Out），流畅稳重 |

#### 主题切换架构

```
ThemeStyleManager
    ├── loadThemePack(ThemePack::iOS / ThemePack::Windows)
    ├── 注入 ThemeEngine（颜色 Token）
    ├── 注入 NavigationPolicy（导航范式）
    ├── 注入 AnimationPolicy（动效策略）
    ├── 注入 LayoutPolicy（圆角/间距/字体）
    └── 触发 Shell 重新布局
```

> 主题包切换是**运行时热切换**，无需重启桌面进程。

### 3.3 窗口管理器

- **浮动窗口模式**（可拖拽/缩放，Windows 风格主题默认）
- **全屏平铺模式**（iOS 风格主题默认）
- **分屏模式**：左右/上下固定分割
- **窗口层级管理**：Z-order 管理，支持 Always On Top
- **窗口动效**：开启/关闭/最小化/恢复均有主题对应动效
- **多显示器**：基础支持（后期扩展）

### 3.4 Shell 导航系统

导航组件根据当前主题包**动态切换形态**，不硬编码：

```
NavigationPolicy Interface
    ├── iOS Policy   → BottomGestureBar + BottomTabBar
    └── Windows Policy → CenteredTaskbar + SystemTray
```

公共元素（跨主题）：
- 顶部状态栏（时间、网络、电量、通知角标）
- 应用标题区（返回按钮/窗口控制按钮随主题变化）

### 3.5 通知系统

对标 iOS 通知中心 + Windows Action Center，要求**完善**：

```
NotificationService（独立进程）
    ├── 通知接收 API（App 调用）
    ├── 通知持久化存储（ConfigStore 依赖）
    ├── 分组/折叠（按 App 归组）
    ├── 优先级分级（Critical / Normal / Silent）
    ├── 横幅弹窗（Banner，自动消失）
    ├── 通知中心面板（下拉/侧滑展开，可清除）
    ├── 角标计数（状态栏图标角标）
    └── 勿扰模式（Do Not Disturb）
```

### 3.6 快捷控制中心

对标 iOS Control Center + Windows Quick Settings：

- 亮度滑条
- 音量滑条
- WiFi / 蓝牙 快捷开关
- 截图
- 勿扰模式切换
- 主题风格快速切换入口
- 自定义快捷瓦片（开发者可注册）

### 3.7 系统设置 App

对标 KDE System Settings + Windows 设置，内置完整设置 App：

| 分类 | 设置项 |
|------|--------|
| 显示 | 分辨率、亮度、夜间模式、缩放比例 |
| 声音 | 音量、输出设备、提示音 |
| 网络 | WiFi 列表、IP 配置、代理 |
| 蓝牙 | 设备配对管理 |
| 桌面 | 主题包切换、壁纸、动效开关 |
| 输入 | 触摸灵敏度、鼠标速度、键盘布局 |
| 应用 | 已安装应用列表、权限管理、默认应用 |
| 语言 | 语言/地区/时区（框架预留，后期实现） |
| 辅助 | 字体大小缩放、高对比度（后期实现） |
| 系统 | 关于本机、硬件信息（HWTier 展示）、日志查看 |

### 3.8 锁屏模块

- **PIN / 密码 / 图案**三种解锁方式
- 锁屏壁纸（独立于桌面壁纸，或继承）
- 锁屏通知展示（仅 Normal 及以上级别）
- 息屏超时自动锁屏（可配置时长）
- 锁屏时媒体控制卡片（正在播放时展示）

### 3.9 桌面壁纸 + Widget 系统

**壁纸**：
- 静态壁纸（Low Tier 及以上）
- 动态壁纸（High Tier，视频/Lottie 动画，Mid/Low 自动降级为静态）
- 主题联动：主题包切换时自动推荐配套壁纸

**桌面 Widget（小组件）**：
- 内置 Widget：时钟、日历、天气、系统资源监控
- Widget 框架：开发者可注册自定义 Widget（提供 Widget API）
- Widget 编辑模式（长按进入，支持拖拽/缩放）
- iOS 风格：Widget 与图标共存于网格桌面
- Windows 风格：Widget 区域独立（类 Win11 Widgets Panel）

### 3.10 文件管理器 App

对标 Windows 资源管理器，内置完整文件管理器：

- 目录树导航（左侧面板）
- 文件列表视图（图标/列表/详情切换）
- 基础文件操作（复制/剪切/粘贴/删除/重命名）
- 文件搜索
- 压缩包浏览（后期）
- 文件选择器对话框（供应用调用的系统级 API）

### 3.11 媒体控制服务

- 系统级媒体会话管理（跨 App 统一控制）
- 状态栏/锁屏媒体卡片（播放/暂停/上下曲）
- 快捷控制中心媒体区块
- 音量控制与输出设备切换
- 蓝牙音频设备快速切换（依赖蓝牙模块）

---

## 四、架构分层设计

```
┌─────────────────────────────────────────────────────────┐
│                   Layer 7: Apps Layer                    │
│         文件管理器 / 系统设置 / 内置 Widget App           │
├─────────────────────────────────────────────────────────┤
│                   Layer 6: Shell Layer                   │
│   任务栏 / 启动器 / 通知中心 / 控制中心 / 锁屏 / 桌面层   │
├─────────────────────────────────────────────────────────┤
│               Layer 5: Theme Style System                │
│         iOS主题包 / Windows主题包 / ThemeStyleManager     │
├─────────────────────────────────────────────────────────┤
│               Layer 4: Window Manager                    │
│          浮动/全屏/分屏 / Z-order / 窗口动效              │
├─────────────────────────────────────────────────────────┤
│               Layer 3: System Services                   │
│   NotificationService / MediaService / InputRouter       │
├─────────────────────────────────────────────────────────┤
│               Layer 2: Base Infrastructure               │
│   InputManager / ConfigStore / Logger / IPC              │
├─────────────────────────────────────────────────────────┤
│               Layer 1: Hardware Abstraction              │
│      HardwareProbe / HWTier / RenderBackend              │
├─────────────────────────────────────────────────────────┤
│               Layer 0: Already Completed                 │
│   ThemeEngine / AnimationManager / DPI / P0 Controls     │
└─────────────────────────────────────────────────────────┘
```

**设计原则**：
- 每层只依赖下层，严禁跨层调用
- Layer 5（主题风格系统）横切 Layer 4-6，通过 Policy 注入而非直接调用
- 系统服务通过 IPC 与 Shell 通信，Shell 不直接持有服务实现

---

## 五、开发路线图

> **节奏定义**：不赶时间，架构分层优先，质量优先，每个 Phase 完成后需通过完整测试才可进入下一 Phase。

---

### Phase A：基础设施补全（前置必做）
> 对应现有 Phase 1/2 的未完成部分，是桌面本体的地基

**A1 — 硬件探针完善**
- GPU 检测器（DRM 设备枚举、OpenGL 上下文探测）
- HWTier 枚举定义（Low / Mid / High）
- HardwareProbe 主类（整合 CPU + 内存 + GPU 探测）
- CapabilityPolicy 策略引擎（各档位默认配置集）
- 单元测试覆盖

**A2 — ConfigStore 配置中心**
- 三层存储模型：系统默认 → 用户配置 → 运行时覆写
- 变更监听机制（Observer 模式）
- 持久化序列化（JSON / QSettings 后端）
- 线程安全访问
- 单元测试覆盖

**A3 — Logger 日志系统**
- 多 Sink 支持（文件 / 终端 / 系统日志）
- 日志级别（Debug / Info / Warning / Error / Fatal）
- 日志轮转（大小/时间）
- 结构化日志（JSON 格式可选）
- 单元测试覆盖

**A4 — 输入抽象层**
- InputManager 统一分发层
- TouchInputHandler（单点 + 多点手势识别）
- KeyInputHandler（键盘/快捷键）
- MouseInputHandler（鼠标 + 滚轮）
- GestureRecognizer（Tap / LongPress / Swipe / Pinch / Pan）
- FocusNavigator（焦点导航系统）
- 单元测试 + UI 自动化测试框架搭建

**A5 — IPC 基础层**
- QLocalSocket 封装（进程间消息信道）
- 消息序列化协议定义
- 服务注册/发现机制（ServiceLocator）
- 基础测试覆盖

**里程碑验收标准**：
- HWTier 可在目标设备上正确探测并输出档位
- ConfigStore 可持久化读写并触发变更通知
- 触摸/鼠标/键盘输入均可通过 InputManager 统一路由
- 两个进程可通过 IPC 层互相发送消息

---

### Phase B：渲染后端抽象层
> 解决"同一代码跑在 EGLFS / Wayland / Windows"的问题

**B1 — RenderBackend 接口定义**
- RenderBackend 抽象接口（初始化/交换缓冲/截图/VSync）
- 后端注册机制（运行时按环境选择）

**B2 — EGLFS 后端**
- Qt EGLFS 封装
- HWTier 联动：High Tier 开启 OpenGL ES 合成，Low Tier 降级 LinuxFB

**B3 — Windows 等价后端**
- Qt Windows 后端封装
- 开发调试辅助工具：屏幕尺寸模拟（5.5" / 8" / 10.1" 三档快速切换）

**B4 — Wayland/X11 后端**
- 跑在现有 Wayland/X11 合成器之上的支持（次优先）

**里程碑验收标准**：
- 同一 Shell 代码在 Linux EGLFS 和 Windows 上均可启动
- 屏幕尺寸模拟器可在 Windows 上精确还原嵌入式屏幕效果

---

### Phase C：P1 控件补全 ✅ 已完成 (2026-03-18)
> Shell UI 所需的基础控件，与 Phase A/B 并行推进

**已实现控件**（12个）：

| 优先级 | 控件 | 桌面依赖场景 | 状态 |
|--------|------|------------|------|
| P1-1 | Slider | 亮度/音量控制 | ✅ |
| P1-2 | Switch | 设置开关项 | ✅ |
| P1-3 | ProgressBar | 下载/加载状态 | ✅ |
| P1-4 | TabView | 设置分类导航 | ✅ |
| P1-5 | ComboBox | 设置下拉选项 | ✅ |
| P1-6 | ListView | 列表显示 | ✅ |
| P1-7 | TableView | 表格显示 | ✅ |
| P1-8 | TreeView | 树形显示 | ✅ |
| P1-9 | ScrollView | 滚动视图 | ✅ |
| P1-10 | Separator | 分隔线 | ✅ |
| P1-11 | SpinBox | 整数输入 | ✅ |
| P1-12 | DoubleSpinBox | 浮点输入 | ✅ |

**待实现 P2 控件**（文件管理器等依赖）：
- ToolBar / ToolButton
- MenuBar / ContextMenu
- StatusBar
- Dialog / Card / Snackbar
- Tooltip / Popover
- ...

---

### Phase D：窗口管理器
> Shell 核心，建立在 Phase A/B 完成之后

**D1 — 窗口模型**
- WindowInfo（标题/图标/PID/状态/几何/层级）
- 窗口状态机（Normal / Maximized / Minimized / Fullscreen / Closing）
- Z-order 管理（层级排序）

**D2 — 布局策略**
- FullscreenPolicy（iOS 风格默认）
- FloatingPolicy（Windows 风格默认，支持拖拽/缩放）
- SplitPolicy（左右/上下分屏）
- 策略运行时切换（随主题包切换自动切换默认策略）

**D3 — 窗口动效**
- 开启动效（按主题包：iOS 弹入 / Win 缩放淡入）
- 关闭动效
- 最小化/恢复动效
- HWTier 联动：Low Tier 关闭窗口动效

**D4 — 任务切换**
- iOS 风格：上滑卡片式 App Switcher（缩略图预览）
- Windows 风格：Task View 全览 + 任务栏悬停预览

**里程碑验收标准**：
- 可在桌面上打开/关闭/切换至少 3 个并发应用窗口
- iOS/Windows 两种布局策略均可正常运作
- 窗口动效在 Mid/High Tier 流畅，Low Tier 自动关闭

---

### Phase E：Shell 导航 + 任务栏
> 桌面的"骨架"，用户每次使用都会接触

**E1 — 状态栏（Status Bar）**
- 时间/日期显示
- 网络状态图标
- 电量图标（如有硬件支持）
- 通知角标区
- 主题联动（iOS：居中时间；Windows：左侧时间 + 右侧系统托盘）

**E2 — iOS 风格导航**
- 底部手势条（Home Indicator）
- 手势识别：上滑回主屏 / 上滑停留进 App Switcher / 左右滑切换应用
- 底部 Tab Bar（可配置应用快捷入口）

**E3 — Windows 风格导航**
- 居中任务栏（固定应用图标 + 运行中应用）
- 系统托盘区（时间/通知图标/媒体控制入口）
- 开始按钮（触发 App Launcher）

**E4 — 应用启动器**
- iOS 风格：网格图标桌面（图标拖拽/文件夹/多页滑动）
- Windows 风格：开始菜单（最近使用 + 固定应用 + 全部应用列表）
- 全局搜索（应用/文件/设置 统一搜索入口）

**里程碑验收标准**：
- 两种主题的导航体系均可独立运作
- 主题包切换后导航形态热切换无需重启
- 应用启动器可浏览和启动所有已安装应用

---

### Phase F：通知系统 + 控制中心
> 完善程度对标 iOS / Windows 11

**F1 — NotificationService（独立进程）**
- 通知接收 API（Qt IPC）
- 通知持久化（ConfigStore 依赖）
- 优先级分级（Critical / Normal / Silent）
- 勿扰模式

**F2 — 横幅弹窗（Banner）**
- 顶部滑入展示，3秒自动消失（可配置）
- 可展开操作按钮（最多2个快捷操作）
- HWTier：Low Tier 仅显示文字，关闭展开动效

**F3 — 通知中心面板**
- iOS 风格：顶部下拉展开
- Windows 风格：右侧滑出面板
- 分组折叠（按 App）
- 全部清除 / 单条清除

**F4 — 快捷控制中心**
- 亮度/音量滑条
- WiFi / 蓝牙 / 勿扰 / 截图 快捷开关
- 媒体控制卡片（当有音乐播放时展示）
- 主题切换入口
- 自定义瓦片注册 API

**里程碑验收标准**：
- 任意进程可通过 API 发送通知并在桌面正常展示
- 通知中心可展示、分组、清除通知
- 控制中心亮度/音量调节生效

---

### Phase G：主题风格系统完整实现
> 双主题包的完整实现与热切换

**G1 — ThemeStyleManager 核心**
- ThemePack 数据结构定义（颜色/圆角/间距/动效/布局策略 完整集）
- 主题包加载/卸载机制
- 热切换协调器（通知所有订阅方）
- 主题包文件格式定义（JSON + 资源文件）

**G2 — iOS 主题包**
- 完整颜色 Token 集（对标 iOS 17 Human Interface Guidelines）
- 布局策略注入（NavigationPolicy / WindowPolicy）
- 动效策略注入（Spring Animation 参数集）
- 图标风格（圆角矩形图标，带阴影）
- 壁纸默认集（3-5 张高质量默认壁纸）

**G3 — Windows 11 主题包**
- 完整颜色 Token 集（对标 Windows 11 Fluent Design）
- 布局策略注入
- 动效策略注入（Ease 曲线参数集）
- Mica/Acrylic 效果（High Tier 开启，模糊半透明）
- 壁纸默认集（3-5 张）

**G4 — 组件级微调 API**
- 每个 P0/P1 控件暴露 StyleOverride 接口
- 用户可在主题包基础上微调单个控件样式
- 微调配置持久化（ConfigStore）

**里程碑验收标准**：
- iOS / Windows 两套主题包均完整可用
- 运行时热切换主题无崩溃、无视觉撕裂
- 动效在各 HWTier 按策略正确降级

---

### Phase H：系统设置 App

对标 KDE System Settings + Windows 设置（参见 3.7 节功能定义）

**开发要点**：
- 作为独立进程启动，通过 IPC 调用各系统服务
- 设置项变更实时生效（不需要重启）
- 设置界面本身也遵循当前主题风格
- 搜索功能：可在所有设置项中模糊搜索

---

### Phase I：锁屏模块

参见 3.8 节功能定义

**开发要点**：
- 锁屏运行在最高 Z-order 层，不可被其他窗口覆盖
- 解锁验证逻辑与凭证存储安全隔离
- 锁屏时输入路由只处理解锁相关输入
- 息屏策略与 HWTier 联动（Low Tier 更激进的息屏超时）

---

### Phase J：桌面 Widget + 壁纸系统

参见 3.9 节功能定义

**开发要点**：
- Widget 沙箱机制（Widget 崩溃不影响 Shell）
- Widget API 设计需兼顾未来第三方开发者接入
- 动态壁纸严格限制 High Tier 才启用，防止低端设备性能问题

---

### Phase K：文件管理器 App

参见 3.10 节功能定义

**开发要点**：
- 依赖 P1 控件：ToolBar / ToolButton / StatusBar / ContextMenu
- 文件选择器对话框作为**系统级 API** 独立封装，所有 App 均可调用
- 支持主题联动（iOS/Windows 风格自动切换界面布局）

---

### Phase L：媒体控制服务

参见 3.11 节功能定义

**开发要点**：
- MediaService 作为独立插件进程，可选挂载
- 与 NotificationService 协作（媒体通知类型）
- 锁屏媒体卡片与锁屏模块联动

---

### Phase M：P2 高级控件 + SDK 初稿

> 此阶段桌面本体已基本完整，开始向外暴露开发者接口

- P2 控件按需实现（Dialog / Card / Snackbar / Tooltip 等，按使用频率排序）
- C++ SDK 头文件整理与文档编写
- QML 接口层（低门槛开发者接口，后期扩展）
- 开发者文档网站更新（MkDocs）

---

## 六、依赖关系图

```
Phase A（基础设施）
    ├──→ Phase B（渲染后端）
    ├──→ Phase C（P1 控件）[可并行]
    └──→ Phase D（窗口管理器）
              └──→ Phase E（Shell 导航）
                        └──→ Phase F（通知 + 控制中心）
                                  └──→ Phase G（主题系统完整实现）
                                            ├──→ Phase H（系统设置）
                                            ├──→ Phase I（锁屏）
                                            ├──→ Phase J（Widget + 壁纸）
                                            ├──→ Phase K（文件管理器）
                                            └──→ Phase L（媒体服务）
                                                      └──→ Phase M（SDK + P2 控件）
```

**可并行开发的模块**：
- Phase C（P1 控件）可与 Phase A/B 全程并行
- Phase H / I / J / K / L 在 Phase G 完成后可并行推进
- 文档编写、测试补全可贯穿全程

---

## 七、测试策略

### 7.1 测试分层

| 层次 | 内容 | 工具 |
|------|------|------|
| 单元测试 | 每个类/模块的独立测试 | Google Test / GMock |
| 集成测试 | 跨模块协作测试（如输入→窗口→通知） | Google Test |
| UI 自动化测试 | 鼠标/触摸/键盘模拟操作流程 | Qt Test / 自研 |
| 性能基准测试 | 帧率/内存/CPU 占用基线 | 自研 Benchmark 框架 |
| HWTier 回归测试 | 模拟三档硬件环境验证降级策略 | 硬件 Mock + Docker |

### 7.2 性能指标目标

| 指标 | Low Tier 目标 | Mid Tier 目标 | High Tier 目标 |
|------|--------------|--------------|---------------|
| Shell 启动时间 | < 3s | < 1.5s | < 0.8s |
| 应用启动时间 | < 2s | < 1s | < 0.5s |
| 桌面帧率 | 30 FPS（静态） | 45 FPS | 60 FPS |
| 通知弹窗延迟 | < 200ms | < 100ms | < 50ms |
| 主题切换时间 | < 500ms | < 300ms | < 200ms |
| Shell 内存占用 | < 80MB | < 150MB | < 300MB |

---

## 八、关键设计决策记录

| 决策 | 选择 | 理由 |
|------|------|------|
| IPC 机制 | Qt IPC（QLocalSocket）优先，必要时迁移 D-Bus | 减少依赖，嵌入式环境 D-Bus 不一定可用 |
| 渲染后端 | 多后端抽象，不自研 Wayland Compositor | 降低复杂度，嵌入式 EGLFS 直驱更稳定 |
| 主题切换 | 运行时热切换，Policy 注入而非硬编码 | 用户体验好，架构扩展性强 |
| 应用模型 | 进程隔离，Shell 作为宿主 | 稳定性更好，一个 App 崩溃不影响 Shell |
| 动效策略 | HWTier 联动，可配置不可强制关闭 | 低端设备保证性能，高端设备保证体验 |
| SDK 时机 | Phase M（桌面本体基本完整后）再开放 | 先把自己的桌面做好，API 稳定后再开放 |
| 商业协议 | MIT 开源 | 最大化社区采用，无授权障碍 |

---

## 九、附录：参考项目

| 参考对象 | 学习要点 |
|---------|---------|
| iOS / iPadOS | 手势体系、弹簧动效、控制中心交互、通知分组 |
| Windows 11 | Fluent Design、任务栏居中、快捷设置面板、Action Center |
| KDE Plasma | 高度可定制架构、系统设置结构、Wayland 集成方式 |
| Android | 通知优先级体系、快捷设置瓦片自定义、Widget 沙箱机制 |

---
