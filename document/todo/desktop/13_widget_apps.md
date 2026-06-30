---
title: "Phase 13: Widget 与桌面应用 TODO"
description: "预计周期: 4~5 周，依赖阶段: Phase 9, Phase 12"
---

# Phase 13: Widget 与桌面应用 TODO

> **状态**: ⬜ 待开始
> **预计周期**: 4~5 周
> **依赖阶段**: Phase 9, Phase 12
> **目标交付物**: Widget 系统、壁纸管理、文件管理器 App、媒体控制服务
>
> 📌 **现实核对与补漏（2026-06-29）**：
> - **壁纸引擎已落地**（`desktop/ui/components/wallpaper/`：`ImageWallPaperLayer`/`WallPaperAccessStorage`/`WallPaperToken`/`WallpaperShellLayerStrategy`），本 Phase 壁纸部分只需补轮播/生成式/资源，引擎不必重做。
>   - **资源包运行时发现已合入**(PR #13,2026-06):第三方壁纸包安装到 `<desktop_active_root>/Wallpapers/<pack>/` 即被递归发现;CF-Gallery 提供 `install-to-cfdesktop.sh`。
>   - **轮播/动画引擎待做**:定时轮播 + Gradient 交叉淡入 / Movement 平移,后端无关重表达——**实施计划见 [wallpaper_animation_engine.md](wallpaper_animation_engine.md)**(自洽接手文档)。
> - **Widget 框架尺度调和**：本 Phase（Phase J）画的是**全量愿景**（沙箱/进程隔离/API）；近期落地以 [milestone_06](milestone_06_widget_control_center.md) **最小版**（ClockWidget + ControlCenter）为准。沙箱依赖 IPC(0%)+CrashHandler；低 RAM 退化同进程崩溃隔离。
> - **FileManagerApp 现实**：① 依赖 P2 控件（ToolBar/ContextMenu/TreeView 等，当前 0%）；② 本仓 `desktop/base/file_operations/file_op.h` **只是路径拼接微工具**（copy/move/rename/delete/trash 全无），**不能当文件管理器底座**，需新建；③ 建议参照 CCIMX `FileRamber`（QtConcurrent 异步刷新）移植。
> - **数据源前置**：WeatherWidget 依赖 `aels-network`；ResourceMonitorWidget 依赖 `base/system` 探针（已 90%）；均见 [aels_cross_repo_deps.md](aels_cross_repo_deps.md)。
> - **路径已订正**：bare `ui/desktop/...` / `src/desktop/...` 统一为实际 `desktop/ui/...` 根。
> - **补漏（原 Phase 未列）**：系统监控 App、串口工具 App（均为 extern，见文末新增小节）。

## 一、阶段目标

### 核心目标
实现桌面 Widget 系统、壁纸管理、文件管理器 App 和媒体控制服务，完善桌面应用层。

### 具体交付物
- [ ] `WidgetFramework` Widget 框架
- [ ] 内置 Widget（时钟/日历/天气/资源监控）
- [ ] `WallpaperManager` 壁纸管理器
- [ ] `FileManagerApp` 文件管理器
- [ ] `FilePickerDialog` 文件选择器
- [ ] `MediaService` 媒体控制服务

---

## 二、待实现任务

### Week 1-2: Widget 框架

#### Week 1, Day 1-2: Widget 框架核心
- [ ] 创建 Widget 基类
  - [ ] `WidgetBase` 抽象类
  - [ ] 生命周期管理（init/update/pause/resume/destroy）
  - [ ] 尺寸配置（最小/最大/首选）
  - [ ] 刷新频率配置
- [ ] 创建 Widget 容器
  - [ ] `WidgetContainer` 类
  - [ ] 布局管理（网格布局）
  - [ ] Widget 注册/注销
  - [ ] Widget 重排
- [ ] 实现 Widget 沙箱
  - [ ] 进程隔离（可选）
  - [ ] 资源限制（CPU/内存）
  - [ ] 权限控制
- [ ] 编写单元测试

#### Week 1, Day 3-5: Widget API
- [ ] 实现 Widget 注册 API
  - [ ] `registerWidget(widgetId, widget)`
  - [ ] `unregisterWidget(widgetId)`
- [ ] 实现 Widget 配置
  - [ ] 配置编辑界面
  - [ ] 配置持久化
- [ ] 实现 Widget 通信
  - [ ] 事件分发
  - [ ] 数据共享（可选）
- [ ] 编写单元测试

#### Week 2: 编辑模式
- [ ] 实现编辑模式
  - [ ] 长按进入编辑模式
  - [ ] 编辑模式视觉反馈
- [ ] 实现拖拽重排
  - [ ] 拖拽手柄
  - [ ] 放置指示
- [ ] 实现缩放
  - [ ] 调整 Widget 尺寸
  - [ ] 尺寸约束
- [ ] 实现删除
  - [ ] 删除按钮
  - [ ] 确认对话框
- [ ] 编写单元测试

---

### Week 2: 内置 Widget

#### Day 1-2: 时钟 Widget
- [ ] 创建 ClockWidget 类
  - [ ] 继承 WidgetBase
- [ ] 实现显示
  - [ ] 数字时钟
  - [ ] 模拟时钟（可选）
  - [ ] 日期显示
- [ ] 实现配置
  - [ ] 12/24 小时制
  - [ ] 显示秒
  - [ ] 字体大小
- [ ] 编写单元测试

#### Day 3: 日历 Widget
- [ ] 创建 CalendarWidget 类
- [ ] 实现月历显示
  - [ ] 月份导航
  - [ ] 今天高亮
  - [ ] 事件标记（可选）
- [ ] 实现配置
  - [ ] 周起始日
  - [ ] 显示周数
- [ ] 编写单元测试

#### Day 4: 天气 Widget
- [ ] 创建 WeatherWidget 类
- [ ] 实现天气显示
  - [ ] 当前温度
  - [ ] 天气图标
  - [ ] 城市名称
- [ ] 实现数据获取
  - [ ] API 接口（可配置）
  - [ ] 缓存机制
  - [ ] 定时刷新
- [ ] 实现配置
  - [ ] 城市选择
  - [ ] 温度单位（°C/°F）
- [ ] 编写单元测试

#### Day 5: 资源监控 Widget
- [ ] 创建 ResourceMonitorWidget 类
- [ ] 实现 CPU 监控
  - [ ] 实时使用率
  - [ ] 进度条/图表
- [ ] 实现内存监控
  - [ ] 实时使用率
  - [ ] 可用/总量
- [ ] 实现配置
  - [ ] 更新频率
  - [ ] 显示样式
- [ ] 编写单元测试

---

### Week 3: 壁纸系统

#### Day 1-2: WallpaperManager 核心
- [ ] 创建 WallpaperManager 类
  - [ ] 单例模式
- [ ] 实现壁纸设置
  - [ ] `setWallpaper(imagePath)` 设置壁纸
  - [ ] `getWallpaper()` 获取当前壁纸
  - [ ] 恢复默认壁纸
- [ ] 实现壁纸存储
  - [ ] 系统壁纸目录
  - [ ] 用户壁纸目录
- [ ] 实现壁纸列表
  - [ ] `getAvailableWallpapers()` 获取列表
  - [ ] 壁纸缩略图
- [ ] 编写单元测试

#### Day 3: 静态壁纸
- [ ] 实现静态壁纸支持
  - [ ] 图片格式（JPG/PNG/WebP）
  - [ ] 缩放模式（填充/适应/拉伸）
  - [ ] 背景色填充

#### Day 4: 动态壁纸（High Tier）
- [ ] 实现动态壁纸支持
  - [ ] 视频壁纸（MP4）
  - [ ] Lottie 动画壁纸（可选）
- [ ] 实现性能监控
  - [ ] 检测帧率
  - [ ] Low/Mid Tier 自动降级
- [ ] 实现配置
  - [ ] 动画速度
  - [ ] 暂停策略（省电）

#### Day 5: 主题联动
- [ ] 实现主题壁纸推荐
  - [ ] 主题包切换时推荐壁纸
  - [ ] 自动应用（可选）
- [ ] 实现定时切换
  - [ ] 按时间间隔
  - [ ] 按时间段（早/中/晚）

---

### Week 3-4: 文件管理器

#### Week 3, Day 1-2: FileManagerApp 核心
- [ ] 创建 FileManagerApp（独立进程）
- [ ] 实现主窗口布局
  - [ ] 左侧：目录树
  - [ ] 中间：文件列表
  - [ ] 底部：状态栏
- [ ] 实现目录树导航
  - [ ] 树形结构
  - [ ] 展开/收起
  - [ ] 隐藏文件（可选）
- [ ] 实现面包屑导航
  - [ ] 路径显示
  - [ ] 点击跳转
- [ ] 编写单元测试

#### Week 3, Day 3-4: 文件列表视图
- [ ] 实现视图模式
  - [ ] 图标视图
  - [ ] 列表视图
  - [ ] 详情视图
- [ ] 实现排序
  - [ ] 按名称
  - [ ] 按大小
  - [ ] 按日期
  - [ ] 按类型
- [ ] 实现选择
  - [ ] 单选
  - [ ] 多选（Ctrl/Shift）
  - [ ] 全选
- [ ] 编写单元测试

#### Week 3, Day 5: 文件操作
- [ ] 实现基础操作
  - [ ] 复制（Ctrl+C / Ctrl+V）
  - [ ] 剪切（Ctrl+X / Ctrl+V）
  - [ ] 粘贴
  - [ ] 删除（Delete）
  - [ ] 重命名（F2）
- [ ] 实现拖拽操作
  - [ ] 拖拽移动
  - [ ] Ctrl+拖拽复制
- [ ] 实现右键菜单
  - [ ] 新建文件夹
  - [ ] 属性
- [ ] 编写单元测试

#### Week 4, Day 1: 文件搜索
- [ ] 实现搜索框
  - [ ] 实时搜索
  - [ ] 通配符支持
- [ ] 实现搜索结果
  - [ ] 高亮匹配
  - [ ] 按相关性排序
- [ ] 编写单元测试

#### Week 4, Day 2-3: FilePickerDialog 文件选择器
- [ ] 创建 FilePickerDialog 类
  - [ ] 系统级 API
  - [ ] 所有 App 可调用
- [ ] 实现打开文件对话框
  - [ ] 文件选择
  - [ ] 过滤器（文件类型）
  - [ ] 多选支持
- [ ] 实现保存文件对话框
  - [ ] 文件名输入
  - [ ] 覆盖确认
- [ ] 实现 IPC 接口
  - [ ] 跨进程调用
- [ ] 编写单元测试

---

### Week 4-5: MediaService 媒体服务

#### Week 4, Day 1-2: 媒体服务核心
- [ ] 创建 MediaService（独立进程）
- [ ] 实现媒体会话管理
  - [ ] `MediaSession` 类
  - [ ] 会话生命周期
  - [ ] 多会话管理（优先级队列）
- [ ] 实现媒体元数据
  - [ ] 标题/艺术家/专辑
  - [ ] 封面图
  - [ ] 时长/位置
- [ ] 实现 IPC 接口
  - [ ] `play()` / `pause()` / `stop()`
  - [ ] `next()` / `previous()`
  - [ ] `seek(position)`
  - [ ] `setVolume(volume)`
- [ ] 编写单元测试

#### Week 4, Day 3-4: 媒体控制集成
- [ ] 状态栏媒体卡片
  - [ ] 显示当前播放
  - [ ] 播放/暂停按钮
- [ ] 锁屏媒体卡片
  - [ ] 显示当前播放
  - [ ] 完整控制（上一曲/下一曲）
- [ ] 控制中心媒体区块
  - [ ] 与 Phase 11 集成
  - [ ] 进度条
  - [ ] 音量控制
- [ ] 编写集成测试

#### Week 4, Day 5: 音频设备集成
- [ ] 实现输出设备切换
  - [ ] 设备列表
  - [ ] 默认设备
- [ ] 实现音量控制
  - [ ] 系统音量
  - [ ] 媒体音量
- [ ] 编写单元测试

#### Week 5, Day 1-3: 测试与优化
- [ ] Widget 沙箱测试
  - [ ] 崩溃隔离
  - [ ] 资源限制
- [ ] 文件管理器测试
  - [ ] 大文件处理
  - [ ] 特殊字符处理
- [ ] 媒体服务测试
  - [ ] 多应用并发
  - [ ] 会话切换
- [ ] 性能测试
- [ ] 内存泄漏检查

#### Week 5, Day 4-5: 文档与发布
- [ ] API 文档
  - [ ] Widget API
  - [ ] FilePicker API
  - [ ] MediaService API
- [ ] 使用示例
- [ ] 代码审查

### Week 5+（补漏）: extern 桌面应用

> 以下为补漏项，原 Phase 13 未列。均为**独立 extern 应用**（非 shell 内核），可参照 CCIMXDesktop extern_app 模式移植。

#### 系统监控 App（SystemMonitor，extern）
- [ ] 进程表（ProcessBrowser：读 `/proc`）
- [ ] CPU/内存/温度曲线（读 `/proc`、`/sys/class/thermal/`，采样节流 1-2Hz 避免拖死单核）
- [ ] 关于页（硬件信息 / HWTier 展示）
- [ ] 日志查看（读 CFDesktop file_sink 落盘日志；6ULL 无 systemd-journal）

> 数据源：`base/system` 探针（已 90%）+ `/proc` + `/sys`；可移植 CCIMX `SystemState`。

#### 串口工具 App（SerialTool，extern）
- [ ] 串口枚举/打开/配置（波特率/数据位/停止位/校验）
- [ ] 收发数据面板 + 十六进制视图
- [ ] 走 termios `/dev/ttymxc`（NXP UART 已移植）

> **职责边界**：`ISerialPort` **接口**归 core（`base/` 硬件统一接口，见硬件集成域）；**工具本体**归本 extern App。注意 CCIMX `CCommunicator` 实为 TCP 端口通信，**非串口**，不可直接复用。

---

## 三、验收标准

### 功能验收
- [ ] Widget 可正常加载和交互
- [ ] 文件管理器可管理文件
- [ ] 媒体服务可控制播放
- [ ] 壁纸系统正常工作
- [ ] FilePicker 可被其他应用调用

### 性能验收
- [ ] Widget 刷新率可配置
- [ ] 文件操作响应 < 100ms
- [ ] 媒体控制延迟 < 50ms

### 代码质量
- [ ] 单元测试覆盖率 > 80%
- [ ] 符合项目代码规范

---

## 四、文件清单（待实现）

### 头文件
- [ ] `desktop/ui/widget/widget_framework.h`
- [ ] `desktop/ui/widget/widget_base.h`
- [ ] `desktop/ui/widget/widget_container.h`
- [ ] `desktop/ui/widget/widget_api.h`
- [ ] `desktop/ui/widget/widget_config.h`
- [ ] `desktop/ui/widget/builtin/clock_widget.h`
- [ ] `desktop/ui/widget/builtin/calendar_widget.h`
- [ ] `desktop/ui/widget/builtin/weather_widget.h`
- [ ] `desktop/ui/widget/builtin/resource_monitor_widget.h`
- [ ] `desktop/ui/widget/wallpaper_manager.h`
- [ ] `desktop/ui/app/file_manager_app.h`
- [ ] `desktop/ui/app/file_picker_dialog.h`
- [ ] `desktop/ui/app/file_tree_view.h`
- [ ] `desktop/ui/app/file_list_view.h`
- [ ] `desktop/ui/components/media/media_service.h`
- [ ] `desktop/ui/components/media/media_session.h`

### 源文件
- [ ] `desktop/ui/widget/widget_framework.cpp`
- [ ] `desktop/ui/widget/widget_base.cpp`
- [ ] `desktop/ui/widget/widget_container.cpp`
- [ ] `desktop/ui/widget/widget_api.cpp`
- [ ] `desktop/ui/widget/builtin/clock_widget.cpp`
- [ ] `desktop/ui/widget/builtin/calendar_widget.cpp`
- [ ] `desktop/ui/widget/builtin/weather_widget.cpp`
- [ ] `desktop/ui/widget/builtin/resource_monitor_widget.cpp`
- [ ] `desktop/ui/widget/wallpaper_manager.cpp`
- [ ] `desktop/ui/app/file_manager_app.cpp`
- [ ] `desktop/ui/app/file_picker_dialog.cpp`
- [ ] `desktop/ui/app/file_tree_view.cpp`
- [ ] `desktop/ui/app/file_list_view.cpp`
- [ ] `desktop/ui/components/media/media_service.cpp`
- [ ] `desktop/ui/components/media/media_session.cpp`

### 测试文件
- [ ] `tests/unit/desktop/widget/test_widget_framework.cpp`
- [ ] `tests/unit/desktop/widget/test_widget_container.cpp`
- [ ] `tests/unit/desktop/widget/test_clock_widget.cpp`
- [ ] `tests/unit/desktop/widget/test_calendar_widget.cpp`
- [ ] `tests/unit/desktop/widget/test_weather_widget.cpp`
- [ ] `tests/unit/desktop/widget/test_resource_monitor.cpp`
- [ ] `tests/unit/desktop/widget/test_wallpaper_manager.cpp`
- [ ] `tests/unit/desktop/app/test_file_manager.cpp`
- [ ] `tests/unit/desktop/app/test_file_picker.cpp`
- [ ] `tests/unit/desktop/service/test_media_service.cpp`
- [ ] `tests/integration/desktop/test_widget_integration.cpp`
- [ ] `tests/integration/desktop/test_file_operations.cpp`
- [ ] `tests/integration/desktop/test_media_integration.cpp`

### 资源文件
- [ ] `assets/widgets/clock/`
- [ ] `assets/widgets/calendar/`
- [ ] `assets/widgets/weather/`
- [ ] `assets/widgets/resource_monitor/`
- [ ] `assets/wallpapers/`
- [ ] `apps/filemanager/icons/`

---

## 五、相关文档

- 设计文档: [../desktop/summary.md](summary.md) Phase J/K/L 节
- 依赖: [09_window_manager.md](09_window_manager.md), [12_theme_settings_lockscreen.md](12_theme_settings_lockscreen.md)
- 参考设计: iOS Widgets, Windows 11 Widgets, Nautilus File Manager

---

*最后更新: 2026-03-21*
