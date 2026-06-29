---
title: "Phase 12: 主题、设置与锁屏 TODO"
description: "预计周期: 4~5 周，依赖阶段: Phase 6, Phase 10, Phase 11"
---

# Phase 12: 主题、设置与锁屏 TODO

> **状态**: ⬜ 待开始
> **预计周期**: 4~5 周
> **依赖阶段**: Phase 6, Phase 10, Phase 11
> **目标交付物**: ThemeStyleManager、iOS/Windows 主题包、系统设置 App、锁屏模块
>
> 📌 **现实核对与补漏（2026-06-29）**：
> - **壁纸引擎已落地**（本仓 `desktop/ui/components/wallpaper/`：`ImageWallPaperLayer` / `WallPaperAccessStorage` / `WallPaperToken` / `WallpaperShellLayerStrategy` + `wallpaper_src_chain`，多图集切换已跑通）。本 Phase 只需**产出壁纸/主题资源**并接线，引擎不必重做。
> - **frosted_backdrop 已是 Mica/Acrylic 雏形**（`desktop/ui/components/frosted_backdrop/`），需接线 HWTier 分档（Low Tier 降级纯色/关闭模糊）。
> - **路径已订正**：bare `ui/...`（Material 控件 / ThemeManager）迁移至 QuarkWidgets 子模块；本仓自有落 `desktop/ui/...`。
> - **本 Phase 补充立项（原计划未覆盖）**：
>   - **i18n 国际化 + CJK 字体子集化** → 独立 AELS 仓 `aels-i18n`（见 [aels_cross_repo_deps.md](aels_cross_repo_deps.md)）；桌面侧只做设置入口接线。CJK 字体必须子集化省闪存。
>   - **icon-theme 图标主题包**（可切换图标包 + 加载机制，依赖 .desktop 扫描）。
>   - **ISecretStore 加密键值原语** → 锁屏凭证 / 未来密钥环 / 账户密码共用，别在锁屏重造（归 utils/aex 或独立小库）。
>   - **多屏显示配置面板（写侧）**：分辨率 / DPI / 排列 / 主屏，归设置 App display 页（只读枚举已在 `IDisplayServerBackend::outputs()`，缺写侧）。
>   - **会话电源动作**：注销 / 重启 / 关机 + 会话保存恢复。i.MX6ULL 走 `reboot(2)`/`poweroff(2)` syscall + sync，**不依赖 systemd**。
>   - **a11y reduce-motion**：与 HWTier Low Tier 关动效同源开关，显式立项。

---

## 一、阶段目标

### 核心目标
实现双主题风格系统的完整实现，以及依赖主题系统的设置 App 和锁屏模块。

### 具体交付物
- [ ] `ThemeStyleManager` 主题管理器
- [ ] iOS 主题包
- [ ] Windows 11 主题包
- [ ] 系统设置 App
- [ ] 锁屏模块

---

## 二、待实现任务

### Week 1-2: ThemeStyleManager

#### Week 1, Day 1-2: ThemePack 数据结构
- [ ] 定义 ThemePack 结构
  - [ ] `id`（主题标识符）
  - [ ] `name`（主题名称）
  - [ ] `version`（版本号）
  - [ ] `author`（作者）
- [ ] 定义颜色 Token 集
  - [ ] 主色调（Primary/OnPrimary/PrimaryContainer）
  - [ ] 次色调（Secondary/OnSecondary/SecondaryContainer）
  - [ ] 背景色（Background/OnBackground/Surface/OnSurface）
  - [ ] 错误色（Error/OnError）
  - [ ] 自定义颜色
- [ ] 定义排版 Token
  - [ ] 字体家族
  - [ ] 字体大小（Display/Headline/Title/Body/Label）
  - [ ] 行高
  - [ ] 字重
- [ ] 定义形状 Token
  - [ ] 圆角半径（Small/Medium/Large/ExtraLarge）
  - [ ] 边框宽度
- [ ] 定义间距 Token
  - [ ] 基础间距单位
  - [ ] 内边距
  - [ ] 外边距
- [ ] 定义动效策略引用
  - [ ] `animationPolicy`（Spring/Ease）
- [ ] 定义布局策略引用
  - [ ] `navigationPolicy`（iOS/Windows）
  - [ ] `windowPolicy`（Fullscreen/Floating）
- [ ] 编写单元测试

#### Week 1, Day 3-5: 主题包加载与缓存
- [ ] 实现主题包加载
  - [ ] JSON 格式解析
  - [ ] 资源文件加载（图标/壁纸/字体）
  - [ ] 验证主题包完整性
- [ ] 实现主题包存储
  - [ ] 系统主题目录 `/usr/share/CFDesktop/themes/`
  - [ ] 用户主题目录 `~/.local/share/CFDesktop/themes/`
- [ ] 实现主题包缓存
  - [ ] 解析后缓存
  - [ ] 资源预加载
  - [ ] 缓存失效检测
- [ ] 实现 ThemePackRegistry
  - [ ] `registerTheme(pack)`
  - [ ] `unregisterTheme(id)`
  - [ ] `getTheme(id)`
  - [ ] `getAllThemes()`
- [ ] 编写单元测试

#### Week 2, Day 1-3: 热切换机制
- [ ] 创建 ThemeStyleManager 单例
- [ ] 实现主题应用
  - [ ] `applyTheme(themeId)` 应用主题
  - [ ] `getCurrentTheme()` 获取当前主题
  - [ ] `getDefaultTheme()` 获取默认主题
- [ ] 实现切换协调器
  - [ ] 通知所有订阅方
  - [ ] 渐进式切换（避免视觉撕裂）
  - [ ] 切换动画
- [ ] 实现订阅机制
  - [ ] `subscribe(callback)` 订阅变更
  - [ ] `unsubscribe(callback)` 取消订阅
- [ ] 实现主题持久化
  - [ ] 保存用户选择
  - [ ] 启动时加载
- [ ] 编写单元测试

#### Week 2, Day 4-5: 组件级微调 API
- [ ] 实现组件样式覆盖
  - [ ] `setComponentStyle(componentId, styleOverrides)`
  - [ ] `getComponentStyle(componentId)`
- [ ] 实现微调配置持久化
  - [ ] 基于 ConfigStore
  - [ ] 与主题包关联
- [ ] 实现样式重置
  - [ ] `resetComponentStyle(componentId)`
  - [ ] `resetAllStyles()`
- [ ] 编写单元测试

---

### Week 3: iOS 主题包

#### Day 1-2: iOS 颜色 Token
- [ ] 实现 iOS 颜色方案
  - [ ] 对标 iOS 17 Human Interface Guidelines
  - [ ] 定义主色调（iOS Blue）
  - [ ] 定义背景色（浅色/深色模式）
  - [ ] 定义表面色
- [ ] 实现深色模式支持
  - [ ] `light` 颜色集
  - [ ] `dark` 颜色集
  - [ ] 自动切换（跟随系统）
- [ ] 准备 iOS 风格图标
  - [ ] SF Pro 风格图标
  - [ ] 圆角矩形图标
  - [ ] 图标阴影

#### Day 3: iOS 布局策略
- [ ] 实现 IOSNavigationPolicy（已在 Phase 10 实现）
- [ ] 实现 IOSWindowPolicy（全屏模式）
- [ ] 定义 iOS 间距规范
  - [ ] 8pt 基础网格
  - [ ] 标准间距（8/16/24/32pt）

#### Day 4-5: iOS 动效与壁纸
- [ ] 实现 iOS 动效策略
  - [ ] Spring Animation 参数集
  - [ ] `mass: 1.0`
  - [ ] `stiffness: 300`
  - [ ] `damping: 20`
- [ ] 准备 iOS 默认壁纸
  - [ ] 3-5 张高质量壁纸
  - [ ] 支持深色模式壁纸
- [ ] 编写 iOS 主题包配置文件

---

### Week 4: Windows 11 主题包

#### Day 1-2: Windows 颜色 Token
- [ ] 实现 Windows 颜色方案
  - [ ] 对标 Windows 11 Fluent Design
  - [ ] 定义主色调（Windows Blue）
  - [ ] 定义 Mica 效果颜色
  - [ ] 定义 Acrylic 效果颜色
- [ ] 实现深色模式支持
  - [ ] `light` 颜色集
  - [ ] `dark` 颜色集
- [ ] 准备 Windows 风格图标
  - [ ] Segoe UI 风格图标
  - [ ] 中等圆角
  - [ ] Fluent 风格

#### Day 3: Windows 布局策略
- [ ] 实现 WindowsNavigationPolicy（已在 Phase 10 实现）
- [ ] 实现 WindowsWindowPolicy（浮动模式）
- [ ] 定义 Windows 间距规范
  - [ ] 4px 基础网格
  - [ ] 标准间距（4/8/12/16/20px）

#### Day 4: Windows 动效与特效
- [ ] 实现 Windows 动效策略
  - [ ] Ease 曲线参数集
  - [ ] 缓入缓出
  - [ ] 标准时长（200ms）
- [ ] 实现 Mica 效果（High Tier）
  - [ ] 半透明背景
  - [ ] 模糊效果
- [ ] 实现 Acrylic 效果（High Tier）
  - [ ] 毛玻璃效果
- [ ] 编写 Windows 主题包配置文件

#### Day 5: 主题包验证与测试
- [ ] 编写主题包验证工具
- [ ] 测试主题切换
  - [ ] iOS → Windows
  - [ ] Windows → iOS
- [ ] 测试深色模式切换
- [ ] 性能测试

---

### Week 4-5: 系统设置 App

#### Week 4, Day 1-2: 设置框架
- [ ] 创建 SettingsApp（独立进程）
- [ ] 实现设置分类导航
  - [ ] 侧边栏导航
  - [ ] 分类图标+名称
- [ ] 实现设置项通用组件
  - [ ] `SettingItem`（标题+描述+控件）
  - [ ] `SettingGroup`（分组标题+子项）
  - [ ] `SettingSwitch`（开关）
  - [ ] `SettingSlider`（滑条）
  - [ ] `SettingComboBox`（下拉选择）
- [ ] 实现搜索功能
  - [ ] 所有设置项可搜索
  - [ ] 实时过滤
- [ ] 编写单元测试

#### Week 4, Day 3-5: 设置页面
- [ ] 显示设置
  - [ ] 分辨率选择
  - [ ] 亮度滑条
  - [ ] 夜间模式开关
  - [ ] 缩放比例
- [ ] 声音设置
  - [ ] 音量滑条
  - [ ] 输出设备选择
  - [ ] 提示音开关
- [ ] 网络设置
  - [ ] WiFi 列表
  - [ ] IP 配置
  - [ ] 代理设置
- [ ] 蓝牙设置
  - [ ] 设备列表
  - [ ] 配对管理
- [ ] 桌面设置
  - [ ] 主题包切换
  - [ ] 壁纸选择
  - [ ] 动效开关
  - [ ] 深色模式切换

#### Week 5, Day 1-2: 更多设置页面
- [ ] 输入设置
  - [ ] 触摸灵敏度
  - [ ] 鼠标速度
  - [ ] 键盘布局
- [ ] 应用设置
  - [ ] 已安装应用列表
  - [ ] 默认应用
  - [ ] 权限管理
- [ ] 语言设置
  - [ ] 语言列表（预留）
  - [ ] 地区设置
  - [ ] 时区设置
- [ ] 辅助功能
  - [ ] 字体大小缩放
  - [ ] 高对比度（预留）
  - [ ] 减弱动效 reduce-motion（与 HWTier Low Tier 关动效同源开关）
- [ ] 系统设置
  - [ ] 关于本机
  - [ ] 硬件信息（HWTier 展示）
  - [ ] 日志查看

---

### Week 5: 锁屏模块

#### Day 1-3: 锁屏核心
- [ ] 创建 LockScreen 类
  - [ ] 最高 Z-order 层
  - [ ] 覆盖所有窗口
- [ ] 实现解锁方式
  - [ ] PIN 码解锁
  - [ ] 密码解锁
  - [ ] 图案解锁（可选）
- [ ] 实现安全凭证存储
  - [ ] 加密存储
  - [ ] 与系统分离
- [ ] 实现锁屏界面
  - [ ] 时间显示
  - [ ] 日期显示
  - [ ] 解锁控件
  - [ ] 通知提示（可选）
- [ ] 编写单元测试

#### Day 4-5: 锁屏增强
- [ ] 实现锁屏壁纸
  - [ ] 独立于桌面壁纸
  - [ ] 可配置
- [ ] 实现锁屏通知
  - [ ] 仅 Normal 及以上级别
  - [ ] 点击查看详情（需先解锁）
- [ ] 实现息屏策略
  - [ ] 超时自动锁屏（可配置）
  - [ ] HWTier 联动
    - [ ] Low Tier: 30 秒
    - [ ] Mid Tier: 1 分钟
    - [ ] High Tier: 2 分钟
- [ ] 实现锁屏媒体控制卡片
  - [ ] 正在播放时显示
  - [ ] 播放/暂停控制
- [ ] 编写集成测试

#### Day 6: 会话与电源动作（补漏：原 Phase 12 未覆盖）
- [ ] 注销 / 重启 / 关机 / 休眠
  - [ ] Windows: `ExitWindowsEx`
  - [ ] Linux/嵌入式: `reboot(2)` / `poweroff(2)` syscall + `sync`（**不依赖 systemd**）
- [ ] 会话保存与恢复
  - [ ] 序列化：打开的应用 / 窗口几何 / panel 状态 → ConfigStore
  - [ ] 下次启动 `init_chain` 恢复（内存吃紧时仅恢复应用列表）
- [ ] 优雅退出协调（PanelManager / WindowManager / 各服务 flush 落盘）

> 注：电源动作**执行**下沉 `aels-power-thermal`（见 [aels_cross_repo_deps.md](aels_cross_repo_deps.md)）；会话序列化归本仓 ConfigStore。

---

## 三、验收标准

### 功能验收
- [ ] iOS/Windows 两套主题包均完整可用
- [ ] 运行时热切换无崩溃、无视觉撕裂
- [ ] 动效在各 HWTier 按策略正确降级
- [ ] 设置 App 所有项可配置
- [ ] 锁屏安全可靠

### 性能验收
- [ ] 主题切换 < 500ms
- [ ] 设置 App 启动 < 1s
- [ ] 锁屏解锁响应 < 200ms

### 代码质量
- [ ] 单元测试覆盖率 > 80%
- [ ] 符合项目代码规范

---

## 四、文件清单（待实现）

### 头文件
- [ ] `desktop/ui/theme/theme_style_manager.h`
- [ ] `desktop/ui/theme/theme_pack.h`
- [ ] `desktop/ui/theme/theme_colors.h`
- [ ] `desktop/ui/theme/theme_typography.h`
- [ ] `desktop/ui/theme/theme_shapes.h`
- [ ] `desktop/ui/theme/theme_spacing.h`
- [ ] `desktop/ui/theme/ios_theme_pack.h`
- [ ] `desktop/ui/theme/windows_theme_pack.h`
- [ ] `desktop/ui/app/settings_app.h`
- [ ] `desktop/ui/app/setting_item.h`
- [ ] `desktop/ui/app/setting_group.h`
- [ ] `desktop/ui/components/lockscreen/lock_screen.h`

### 源文件
- [ ] `desktop/ui/theme/theme_style_manager.cpp`
- [ ] `desktop/ui/theme/theme_pack.cpp`
- [ ] `desktop/ui/theme/ios_theme_pack.cpp`
- [ ] `desktop/ui/theme/windows_theme_pack.cpp`
- [ ] `desktop/ui/app/settings_app.cpp`
- [ ] `desktop/ui/app/setting_item.cpp`
- [ ] `desktop/ui/components/lockscreen/lock_screen.cpp`

### 资源文件
- [ ] `assets/themes/ios/theme.json`
- [ ] `assets/themes/ios/colors.json`
- [ ] `assets/themes/ios/typography.json`
- [ ] `assets/themes/ios/wallpapers/`
  - [ ] `ios_light_01.jpg`
  - [ ] `ios_dark_01.jpg`
  - [ ] ...
- [ ] `assets/themes/windows11/theme.json`
- [ ] `assets/themes/windows11/colors.json`
- [ ] `assets/themes/windows11/typography.json`
- [ ] `assets/themes/windows11/wallpapers/`
  - [ ] `win_bloom.jpg`
  - [ ] `win_flow.jpg`
  - [ ] ...

### 测试文件
- [ ] `tests/unit/desktop/theme/test_theme_style_manager.cpp`
- [ ] `tests/unit/desktop/theme/test_theme_pack.cpp`
- [ ] `tests/unit/desktop/theme/test_ios_theme.cpp`
- [ ] `tests/unit/desktop/theme/test_windows_theme.cpp`
- [ ] `tests/unit/desktop/app/test_settings_app.cpp`
- [ ] `tests/unit/desktop/shell/test_lock_screen.cpp`

---

## 五、相关文档

- 设计文档: [../desktop/summary.md](summary.md) Phase G/H/I 节
- 依赖: [06_infrastructure.md](06_infrastructure.md), [10_shell_navigation.md](10_shell_navigation.md), [11_notification_control.md](11_notification_control.md)
- 参考设计: iOS 17 HIG, Windows 11 Fluent Design

---

*最后更新: 2026-03-12*
