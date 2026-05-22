---
title: "Phase 10: Shell 导航系统 TODO"
description: "依赖阶段: Phase 6, Phase 9"
---

# Phase 10: Shell 导航系统 TODO

> **状态**: ⬜ 待开始
> **预计周期**: 3 周
> **依赖阶段**: Phase 6, Phase 9
> **目标交付物**: StatusBar 状态栏、导航组件（双风格）、应用启动器

---

## 一、阶段目标

### 核心目标
实现桌面 Shell 的导航系统，支持 iOS 和 Windows 11 两种主题风格的动态切换。

### 具体交付物
- [ ] `ShellStatusBar` 系统状态栏
- [ ] `NavigationPolicy` 导航策略接口
- [ ] iOS 风格导航（BottomGestureBar / BottomTabBar / IOSAppLauncher）
- [ ] Windows 风格导航（CenteredTaskbar / SystemTray / WindowsAppLauncher）
- [ ] 运行时主题切换

---

## 二、待实现任务

### Week 1: 状态栏与导航策略

#### Day 1-2: ShellStatusBar 状态栏
- [ ] 创建 ShellStatusBar 类
  - [ ] 继承 QWidget
  - [ ] 固定在屏幕顶部
- [ ] 实现时间/日期显示
  - [ ] 格式化显示（HH:MM）
  - [ ] 每秒更新
  - [ ] 点击展开通知中心
- [ ] 实现系统图标
  - [ ] 网络状态图标（WiFi/以太网）
  - [ ] 电量图标（如硬件支持）
  - [ ] 通知角标（未读数量）
  - [ ] 音量图标（可选）
- [ ] 实现主题联动
  - [ ] iOS 风格：居中时间显示
  - [ ] Windows 风格：左侧时间+右侧系统托盘
- [ ] 实现信号槽
  - [ ] `timeClicked()`
  - [ ] `iconClicked(iconType)`
- [ ] 编写单元测试

#### Day 3-4: NavigationPolicy 导航策略
- [ ] 定义 NavigationPolicy 抽象接口
  - [ ] `createStatusBar()` → ShellStatusBar*
  - [ ] `createNavigation()` → QWidget*
  - [ ] `getGestureRegions()` → QList<QRect>
  - [ ] `getLauncherPosition()` → QRect
- [ ] 实现 IOSNavigationPolicy
  - [ ] 创建底部手势条
  - [ ] 创建底部 Tab 栏
  - [ ] 定义手势区域（屏幕底部）
- [ ] 实现 WindowsNavigationPolicy
  - [ ] 创建居中任务栏
  - [ ] 创建系统托盘区
  - [ ] 定义手势区域（无/可选）
- [ ] 实现 Policy 单例
  - [ ] `setCurrentPolicy(ThemeStyle)`
  - [ ] `getCurrentPolicy()`
  - [ ] 策略切换通知
- [ ] 编写单元测试

#### Day 5: NavigationContainer 导航容器
- [ ] 创建 NavigationContainer 类
  - [ ] 继承 QWidget
- [ ] 实现策略注入
  - [ ] 动态加载导航组件
  - [ ] 布局协调
- [ ] 实现主题切换协调
  - [ ] 渐进式切换（避免视觉撕裂）
  - [ ] 状态保持（保留当前状态）
- [ ] 实现信号转发
  - [ ] 应用启动请求
  - [ ] 设置打开请求
- [ ] 编写单元测试

---

### Week 2: iOS 风格导航

#### Day 1-2: BottomGestureBar 底部手势条
- [ ] 创建 BottomGestureBar 类
  - [ ] 继承 QWidget
- [ ] 实现手势条绘制
  - [ ] 圆角矩形
  - [ ] 半透明效果
  - [ ] 高度自适应（基于 DPI）
- [ ] 实现手势识别
  - [ ] 上滑回主屏
  - [ ] 上滑停留进 App Switcher
  - [ ] 左右滑切换应用
- [ ] 实现视觉反馈
  - [ ] 触摸高亮
  - [ ] 手势进度提示
- [ ] 实现 HWTier 联动
  - [ ] Low Tier 可禁用动画
- [ ] 编写单元测试

#### Day 3: BottomTabBar 底部标签栏
- [ ] 创建 BottomTabBar 类
  - [ ] 继承 QWidget
- [ ] 实现 Tab 项
  - [ ] 图标显示
  - [ ] 图标+文字（可选）
  - [ ] 运行指示器（下方圆点）
- [ ] 实现交互
  - [ ] 点击启动/切换应用
  - [ ] 长按显示菜单（可选）
  - [ ] 滑动手势
- [ ] 实现编辑模式（可选）
  - [ ] 图标抖动
  - [ ] 拖拽重排
  - [ ] 删除图标
- [ ] 编写单元测试

#### Day 4-5: IOSAppLauncher iOS 应用启动器
- [ ] 创建 IOSAppLauncher 类
- [ ] 实现网格图标布局
  - [ ] 自适应行列数
  - [ ] 图标间距自适应
- [ ] 实现图标显示
  - [ ] 应用图标
  - [ ] 应用名称
  - [ ] 角标（通知数量）
- [ ] 实现交互
  - [ ] 点击启动应用
  - [ ] 长按进入编辑模式
- [ ] 实现文件夹支持（可选）
  - [ ] 拖拽创建文件夹
  - [ ] 文件夹展开/收起
- [ ] 实现多页滑动
  - [ ] 左右滑动切换页面
  - [ ] 页面指示器
- [ ] 编写单元测试

---

### Week 3: Windows 风格导航

#### Day 1-2: CenteredTaskbar 居中任务栏
- [ ] 创建 CenteredTaskbar 类
  - [ ] 继承 QWidget
  - [ ] 固定在屏幕底部
- [ ] 实现居中布局
  - [ ] 应用图标居中
  - [ ] 系统托盘在右侧
- [ ] 实现应用图标
  - [ ] 固定应用图标
  - [ ] 运行中应用图标
  - [ ] 运行指示器（下方横线）
- [ ] 实现悬停预览
  - [ ] 鼠标悬停显示缩略图
  - [ ] 延迟显示（可配置）
- [ ] 实现交互
  - [ ] 点击启动/切换应用
  - [ ] 右键显示跳转列表
- [ ] 实现开始按钮
  - [ ] 触发 WindowsAppLauncher
- [ ] 编写单元测试

#### Day 3: SystemTray 系统托盘
- [ ] 创建 SystemTray 类
  - [ ] 继承 QWidget
- [ ] 实现时间显示
  - [ ] HH:MM 格式
  - [ ] 点击展开日历（可选）
- [ ] 实现系统图标区
  - [ ] 网络图标
  - [ ] 音量图标
  - [ ] 电池图标
  - [ ] 通知图标
  - [ ] 语言/输入法（可选）
- [ ] 实现点击展开
  - [ ] 控制中心
  - [ ] 通知中心
- [ ] 实现系统托盘图标
  - [ ] 最小化到托盘
- [ ] 编写单元测试

#### Day 4: WindowsAppLauncher 开始菜单
- [ ] 创建 WindowsAppLauncher 类
  - [ ] 继承 QWidget
  - [ ] 弹出窗口
- [ ] 实现布局
  - [ ] 左侧：固定应用
  - [ ] 中间：推荐/最近使用
  - [ ] 右侧/底部：全部应用列表
- [ ] 实现搜索框
  - [ ] 全局搜索（应用/文件/设置）
  - [ ] 实时过滤
- [ ] 实现应用列表
  - [ ] 字母分组
  - [ ] 滚动支持
- [ ] 实现用户信息（可选）
  - [ ] 用户名/头像
  - [ ] 电源菜单
- [ ] 编写单元测试

#### Day 5: 测试与集成
- [ ] 双主题切换测试
  - [ ] iOS → Windows 切换
  - [ ] Windows → iOS 切换
  - [ ] 状态保持验证
- [ ] 导航功能测试
  - [ ] 应用启动测试
  - [ ] 手势识别测试
- [ ] 集成测试
  - [ ] 与 WindowManager 集成
  - [ ] 与 AppSwitcher 集成

---

## 三、验收标准

### 功能验收
- [ ] 两种主题导航体系均可独立运作
- [ ] 主题包热切换无需重启
- [ ] 应用启动器可浏览和启动应用
- [ ] 手势识别准确率 > 95%

### 性能验收
- [ ] 导航切换延迟 < 100ms
- [ ] 主题切换 < 500ms
- [ ] 手势响应延迟 < 16ms

### 代码质量
- [ ] 单元测试覆盖率 > 80%
- [ ] 符合项目代码规范

---

## 四、文件清单（待实现）

### 头文件
- [ ] `ui/desktop/shell/status_bar.h`
- [ ] `ui/desktop/shell/navigation_policy.h`
- [ ] `ui/desktop/shell/ios_navigation_policy.h`
- [ ] `ui/desktop/shell/windows_navigation_policy.h`
- [ ] `ui/desktop/shell/navigation_container.h`
- [ ] `ui/desktop/shell/bottom_gesture_bar.h`
- [ ] `ui/desktop/shell/bottom_tab_bar.h`
- [ ] `ui/desktop/shell/centered_taskbar.h`
- [ ] `ui/desktop/shell/system_tray.h`
- [ ] `ui/desktop/shell/app_launcher.h`
- [ ] `ui/desktop/shell/ios_app_launcher.h`
- [ ] `ui/desktop/shell/windows_app_launcher.h`

### 源文件
- [ ] `src/desktop/shell/status_bar.cpp`
- [ ] `src/desktop/shell/navigation_policy.cpp`
- [ ] `src/desktop/shell/ios_navigation_policy.cpp`
- [ ] `src/desktop/shell/windows_navigation_policy.cpp`
- [ ] `src/desktop/shell/navigation_container.cpp`
- [ ] `src/desktop/shell/bottom_gesture_bar.cpp`
- [ ] `src/desktop/shell/bottom_tab_bar.cpp`
- [ ] `src/desktop/shell/centered_taskbar.cpp`
- [ ] `src/desktop/shell/system_tray.cpp`
- [ ] `src/desktop/shell/app_launcher.cpp`
- [ ] `src/desktop/shell/ios_app_launcher.cpp`
- [ ] `src/desktop/shell/windows_app_launcher.cpp`

### 测试文件
- [ ] `tests/unit/desktop/shell/test_status_bar.cpp`
- [ ] `tests/unit/desktop/shell/test_navigation_policy.cpp`
- [ ] `tests/unit/desktop/shell/test_bottom_gesture_bar.cpp`
- [ ] `tests/unit/desktop/shell/test_bottom_tab_bar.cpp`
- [ ] `tests/unit/desktop/shell/test_centered_taskbar.cpp`
- [ ] `tests/unit/desktop/shell/test_system_tray.cpp`
- [ ] `tests/unit/desktop/shell/test_ios_launcher.cpp`
- [ ] `tests/unit/desktop/shell/test_windows_launcher.cpp`
- [ ] `tests/integration/desktop/test_navigation_integration.cpp`

---

## 五、相关文档

- 设计文档: [../desktop/summary.md](summary.md) Phase E 节
- 依赖: [09_window_manager.md](09_window_manager.md), [06_infrastructure.md](06_infrastructure.md)
- 参考设计: iOS Human Interface Guidelines, Windows 11 Design Guidelines

---

*最后更新: 2026-03-12*
