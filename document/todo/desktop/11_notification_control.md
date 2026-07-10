---
title: "Phase 11: 通知与控制中心 TODO"
description: "预计周期: 2~3 周，依赖阶段: Phase 6, Phase 10"
---

# Phase 11: 通知与控制中心 TODO

> **状态**: 🚧 最小闭环已落地（2026-07-10）；完整版（持久化/分组/优先级/操作按钮/媒体卡片）仍待做
> **预计周期**: 2~3 周
> **依赖阶段**: Phase 6, Phase 10
> **目标交付物**: NotificationService 通知服务、ControlCenter 控制中心
>
> ⚠️ **关键依赖与现实核对（2026-06-29）**：
> - NotificationService 设计为**独立进程**，跨进程通信**强依赖 Phase A-A5 IPC（当前 0%，见 [06_infrastructure.md](06_infrastructure.md)）**，必须与 IPC 同批落地。
> - 勿与本仓已有的 `desktop/base/config_manager/include/cfconfig_notify_policy.h` 混淆——后者是 ConfigStore 的**配置变更回调派发策略**（Manual/Immediate），与用户通知无关。
> - 绝不引入 D-Bus；嵌入式走文件/socket IPC（可借鉴 CCIMXDesktop 的 toast 文件投递范式）。
>
> ✅ **最小闭环已落地（2026-07-10，feat/phase-f-control-center-notifications）**：`NotificationService`（**进程内单例**，内存模型，`ui/components/notification/`）+ `Notification`（id/title/message/app_id/timestamp）+ `NotificationBanner`（横幅，5s 自动消失）+ `NotificationCenterPanel`（通知中心，状态栏通知图标触发）+ `StatusBar::timeClicked/notifyIconClicked` + 矢量通知图标（铃铛 + 未读红点）+ IPC `notify` 消息（外部 app 发通知，cfipc）+ DND 持久化（ConfigStore `notification.dnd.enabled`）。
>
> ⚠️ **defer 项**：通知持久化（当前重启清空）、按应用分组、优先级分级（Critical/Normal/Low/Silent）、操作按钮、媒体控制卡片、`NotificationService` 独立进程（当前为 shell 进程内单例，跨进程经 cfipc `notify` 消息投递）、硬件后端真实接入（亮度/音量/WiFi/蓝牙，待 aels-power/network 建仓）。

---

## 一、阶段目标

### 核心目标
实现完善的通知系统和快捷控制中心，对标 iOS 通知中心 + Windows Action Center。

### 具体交付物
- [ ] `NotificationService` 独立进程服务
- [ ] 通知横幅弹窗
- [ ] 通知中心面板
- [ ] `ControlCenter` 快捷控制中心
- [ ] IPC 通信接口

---

## 二、待实现任务

### Week 1: NotificationService 通知服务

#### Day 1-2: 服务核心框架
- [ ] 创建 NotificationService 进程
  - [ ] 独立可执行程序
  - [ ] 服务启动/停止
- [ ] 实现通知 API
  - [ ] `notify(title, message, priority)` 发送通知
  - [ ] `notify(id, title, message, icon, priority)` 带ID通知
  - [ ] `cancel(id)` 取消通知
  - [ ] `clearAll()` 清除所有
  - [ ] `getNotifications()` 获取通知列表
- [ ] 定义通知结构
  - [ ] `id`（唯一标识）
  - [ ] `title`（标题）
  - [ ] `message`（内容）
  - [ ] `icon`（图标）
  - [ ] `priority`（优先级）
  - [ ] `timestamp`（时间戳）
  - [ ] `appId`（来源应用）
  - [ ] `actions`（操作按钮列表）
- [ ] 实现优先级分级
  - [ ] `Critical`（紧急，红色）
  - [ ] `Normal`（普通，蓝色）
  - [ ] `Low`（低，灰色）
  - [ ] `Silent`（静默，无横幅）
- [ ] 编写单元测试

#### Day 3: 通知持久化
- [ ] 实现通知存储
  - [ ] 基于 ConfigStore
  - [ ] 历史记录存储（最多100条）
  - [ ] 过期清理（24小时）
- [ ] 实现分组
  - [ ] 按应用分组
  - [ ] 按时间排序
- [ ] 实现勿扰模式
  - [ ] `DoNotDisturb` 开关
  - [ ] 静音时段配置
  - [ ] 例外规则（紧急联系人为例）
- [ ] 编写单元测试

#### Day 4-5: 通知横幅弹窗
- [ ] 创建 NotificationBanner 类
  - [ ] 继承 QWidget
  - [ ] 顶层窗口
- [ ] 实现滑入动画
  - [ ] iOS：顶部下拉滑入
  - [ ] Windows：右侧滑入
- [ ] 实现自动消失
  - [ ] 默认 3 秒
  - [ ] 可配置时长
  - [ ] 鼠标悬停暂停
- [ ] 实现操作按钮
  - [ ] 最多 2 个快捷操作
  - [ ] 点击触发回调
- [ ] 实现 HWTier 降级
  - [ ] Low Tier 仅文字
  - [ ] Low Tier 简化动画
- [ ] 编写单元测试

#### Day 5: 通知中心面板
- [ ] 创建 NotificationCenter 类
  - [ ] 继承 QWidget
- [ ] 实现 iOS 风格下拉
  - [ ] 从顶部下滑展开
  - [ ] 半透明背景
- [ ] 实现 Windows 风格侧滑
  - [ ] 从右侧滑出
  - [ ] 固定宽度面板
- [ ] 实现分组折叠
  - [ ] 按应用分组
  - [ ] 折叠/展开动画
- [ ] 实现清除操作
  - [ ] 单条清除
  - [ ] 全部清除
  - [ ] 按应用清除
- [ ] 实现时间分组
  - [ ] 今天
  - [ ] 昨天
  - [ ] 更早
- [ ] 编写单元测试

---

### Week 2: ControlCenter 控制中心

#### Day 1-2: 基础框架
- [ ] 创建 ControlCenter 类
  - [ ] 继承 QWidget
- [ ] 实现面板布局
  - [ ] 网格布局
  - [ ] 滑块区域
  - [ ] 瓦片区域
- [ ] 实现主题联动
  - [ ] iOS：右上角下滑展开
  - [ ] Windows：右下角点击展开
- [ ] 实现面板样式
  - [ ] 半透明背景
  - [ ] 圆角卡片
  - [ ] 模糊效果（High Tier）
- [ ] 编写单元测试

#### Day 3: 快捷滑条
- [ ] 实现亮度滑条
  - [ ] 连接系统背光 API
  - [ ] 实时反馈
  - [ ] 图标动态变化
- [ ] 实现音量滑条
  - [ ] 连接系统音量 API
  - [ ] 实时反馈
  - [ ] 输出设备切换（可选）
- [ ] 实现 ControlSlider 组件
  - [ ] 带图标的滑条
  - [ ] 数值显示
- [ ] 编写单元测试

#### Day 4: 快捷瓦片
- [ ] 实现 WiFi 开关
  - [ ] 状态显示
  - [ ] 点击切换
  - [ ] 长按进入设置
- [ ] 实现蓝牙开关
  - [ ] 状态显示
  - [ ] 点击切换
  - [ ] 设备列表（长按）
- [ ] 实现勿扰模式开关
  - [ ] 状态显示
  - [ ] 点击切换
- [ ] 实现截图瓦片
  - [ ] 点击截图
  - [ ] 截图预览
- [ ] 实现 ControlTile 组件
  - [ ] 图标+标签
  - [ ] 状态指示
  - [ ] 点击/长按事件
- [ ] 编写单元测试

#### Day 5: 媒体控制与自定义
- [ ] 实现媒体控制卡片
  - [ ] 显示当前播放
  - [ ] 播放/暂停按钮
  - [ ] 上一曲/下一曲
  - [ ] 进度条（可选）
- [ ] 实现主题切换入口
  - [ ] iOS/Windows 主题切换按钮
- [ ] 实现自定义瓦片 API
  - [ ] `registerTile(id, tile)`
  - [ ] `unregisterTile(id)`
  - [ ] 第三方应用可注册自定义瓦片
- [ ] 编写单元测试

---

### Week 3: IPC 通信与集成

> 🔴 强依赖 Phase A-A5 IPC（当前 0%）；IPC 未落地则本周无法开工，须与 [06_infrastructure.md](06_infrastructure.md) IPC 基础层同批推进。

#### Day 1-2: IPC 通信接口
- [ ] 实现通知 IPC 接口
  - [ ] 定义 IPC 消息类型
  - [ ] `NOTIFY` 消息
  - [ ] `CANCEL` 消息
  - [ ] `CLEAR_ALL` 消息
- [ ] 实现控制中心 IPC 接口
  - [ ] `TOGGLE_WIFI` 消息
  - [ ] `TOGGLE_BLUETOOTH` 消息
  - [ ] `SET_BRIGHTNESS` 消息
  - [ ] `SET_VOLUME` 消息
- [ ] 实现服务发现
  - [ ] ServiceLocator 注册
  - [ ] 健康检查
  - [ ] 重连机制
- [ ] 编写集成测试

#### Day 3: Shell 集成
- [ ] 集成到 StatusBar
  - [ ] 时间点击展开通知中心
  - [ ] 图标点击展开控制中心
- [ ] 集成手势支持
  - [ ] iOS 右上角下滑
  - [ ] Windows 右下角点击
- [ ] 实现面板协调
  - [ ] 通知中心和控制中心互斥显示
  - [ ] 与其他面板协调
- [ ] 编写集成测试

#### Day 4-5: 测试与优化
- [ ] 跨进程通知测试
  - [ ] 发送/接收测试
  - [ ] 并发测试
- [ ] 控制中心功能测试
  - [ ] 所有滑条测试
  - [ ] 所有开关测试
- [ ] 性能测试
  - [ ] 通知延迟 < 100ms
  - [ ] 面板展开 < 200ms
- [ ] 内存泄漏检查

---

## 三、验收标准

### 功能验收
- [ ] 任意进程可通过 API 发送通知并在桌面正常展示
- [ ] 通知中心可分组/清除
- [ ] 控制中心亮度/音量调节生效
- [ ] 勿扰模式正常工作
- [ ] IPC 通信稳定可靠

### 性能验收
- [ ] 通知发送到显示延迟 < 100ms
- [ ] 控制中心面板展开 < 200ms
- [ ] 服务进程内存占用 < 50MB

### 代码质量
- [ ] 单元测试覆盖率 > 80%
- [ ] 符合项目代码规范

---

## 四、文件清单（待实现）

### 头文件
- [ ] `desktop/ui/components/notification/notification_service.h`
- [ ] `desktop/ui/components/notification/notification_api.h`
- [ ] `desktop/ui/components/notification/notification_types.h`
- [ ] `desktop/ui/components/notification/notification_banner.h`
- [ ] `desktop/ui/components/notification/notification_center.h`
- [ ] `desktop/ui/widget/control_center/control_center.h`
- [ ] `desktop/ui/widget/control_center/control_slider.h`
- [ ] `desktop/ui/widget/control_center/control_tile.h`
- [ ] `desktop/ui/widget/control_center/media_control_card.h`

### 源文件
- [ ] `desktop/ui/components/notification/notification_service.cpp`
- [ ] `desktop/ui/components/notification/notification_api.cpp`
- [ ] `desktop/ui/components/notification/notification_banner.cpp`
- [ ] `desktop/ui/components/notification/notification_center.cpp`
- [ ] `desktop/ui/widget/control_center/control_center.cpp`
- [ ] `desktop/ui/widget/control_center/control_slider.cpp`
- [ ] `desktop/ui/widget/control_center/control_tile.cpp`
- [ ] `desktop/ui/widget/control_center/media_control_card.cpp`

### IPC 定义（归属基础设施层，见 [06_infrastructure.md](06_infrastructure.md)）
- [ ] `desktop/base/infrastructure/ipc_notification_messages.h`
- [ ] `desktop/base/infrastructure/ipc_control_messages.h`

### 测试文件
- [ ] `tests/unit/desktop/notification/test_notification_service.cpp`
- [ ] `tests/unit/desktop/notification/test_notification_banner.cpp`
- [ ] `tests/unit/desktop/notification/test_notification_center.cpp`
- [ ] `tests/unit/desktop/control_center/test_control_center.cpp`
- [ ] `tests/integration/desktop/test_notification_ipc.cpp`

---

## 五、相关文档

- 设计文档: [../desktop/summary.md](summary.md) Phase F 节
- 依赖: [06_infrastructure.md](06_infrastructure.md), [10_shell_navigation.md](10_shell_navigation.md)
- 参考设计: iOS Notification Center, Windows 11 Action Center

---

*最后更新: 2026-03-12*
