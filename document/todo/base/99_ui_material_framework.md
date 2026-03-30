# UI Material Framework TODO

> **状态**: ⬜ 高级控件待实现
> **预计周期**: 持续迭代
> **依赖阶段**: Phase 0-3
> **参考文档**: [../../ui/MaterialRules.md](../../ui/MaterialRules.md)

**完成进度**:
- ✅ Layer 1-4: 基础框架 (100%) - 已归档
- ✅ P0核心控件 (7/7) - 已归档
- ✅ P1控件 (12/12) - 已归档
- ⬜ P2控件 (0/27)
- ⬜ P3控件 (0/25)

**已完成归档**:
- [Layer 1-4 及 P0 控件](done/99_ui_layer1-4_and_p0_widgets_done.md)
- [P1 控件](done/08_p1_controls_status.md)

---

## 一、架构概览

### 分层架构
```
Layer 6: Qt Native Widgets (QPushButton, QLineEdit, ...)
Layer 5: Material Widget Adapter (Button, TextField, ...)
Layer 4: Material Behavior Layer (StateMachine, Ripple, ...)
Layer 3: Animation Engine Layer (TimingAnimation, SpringAnimation, ...)
Layer 2: Theme Engine Layer (ThemeManager, ICFColorScheme, ...)
Layer 1: Core Math & Utility Layer (math_helper, color, geometry, ...)
```

### 核心约束（RULE-01 至 RULE-09）
- [ ] RULE-01: 所有 Material 控件必须继承 Qt 原生控件
- [ ] RULE-02: 不得移除、覆盖、修改任何 Qt 原生 public 信号
- [ ] RULE-03: paintEvent 必须完全接管绘制
- [ ] RULE-04: 禁止使用 QSS 实现任何 Material 视觉
- [ ] RULE-05: 禁止依赖 Qt Quick / QML
- [ ] RULE-06: 动画系统必须可整体关闭
- [ ] RULE-07: 主题数据只读访问
- [ ] RULE-08: 渲染组件通过组合注入
- [ ] RULE-09: 通过 Token 访问主题

---

## 二、P2 - 高级功能控件 (Medium Priority)

- [ ] **DatePicker** - 日期选择器
  - [ ] 文件: `ui/widget/material/widget/date_picker/`
  - [ ] 基类: QCalendarWidget（控件化）
  - [ ] 日期选择，支持日历弹出
- [ ] **TimePicker** - 时间选择器
  - [ ] 文件: `ui/widget/material/widget/time_picker/`
  - [ ] 基类: QTimeEdit
  - [ ] 时间选择，支持时钟盘
- [ ] **ColorPicker** - 颜色选择器
  - [ ] 文件: `ui/widget/material/widget/color_picker/`
  - [ ] 基类: QColorDialog（控件化）
  - [ ] 颜色选择，支持色盘、吸管
- [ ] **MenuBar** - 菜单栏
  - [ ] 文件: `ui/widget/material/widget/menu_bar/`
  - [ ] 基类: QMenuBar
  - [ ] 顶部菜单栏，支持下拉菜单
- [ ] **ContextMenu** - 上下文菜单
  - [ ] 文件: `ui/widget/material/widget/context_menu/`
  - [ ] 基类: QMenu
  - [ ] 右键菜单，支持动态生成
- [ ] **ToolBar** - 工具栏
  - [ ] 文件: `ui/widget/material/widget/tool_bar/`
  - [ ] 基类: QToolBar
  - [ ] 快速访问常用操作
- [ ] **StatusBar** - 状态栏
  - [ ] 文件: `ui/widget/material/widget/status_bar/`
  - [ ] 基类: QStatusBar
  - [ ] 显示状态信息和进度
- [ ] **Dialog** - 对话框基类
  - [ ] 文件: `ui/widget/material/widget/dialog/`
  - [ ] 基类: QDialog
  - [ ] 模态/非模态对话框
- [ ] **Snackbar** - 消息提示
  - [ ] 文件: `ui/widget/material/widget/snackbar/`
  - [ ] 基类: QWidget（自定义）
  - [ ] Material 风格轻量级提示
- [ ] **Card** - 卡片
  - [ ] 文件: `ui/widget/material/widget/card/`
  - [ ] 基类: QFrame（自定义）
  - [ ] 带圆角、阴影的内容容器
- [ ] **FloatingActionButton** - 悬浮操作按钮
  - [ ] 文件: `ui/widget/material/widget/fab/`
  - [ ] 基类: QPushButton（自定义）
  - [ ] 圆形悬浮按钮
- [ ] **BottomNavigation** - 底部导航栏
  - [ ] 文件: `ui/widget/material/widget/bottom_navigation/`
  - [ ] 基类: QWidget（自定义）
  - [ ] 底部导航标签
- [ ] **Drawer** - 侧边抽屉
  - [ ] 文件: `ui/widget/material/widget/drawer/`
  - [ ] 基类: QWidget（自定义）
  - [ ] 从屏幕边缘滑出
- [ ] **SearchBar** - 搜索框
  - [ ] 文件: `ui/widget/material/widget/search_bar/`
  - [ ] 基类: QLineEdit（自定义）
  - [ ] 带搜索图标和清除按钮
- [ ] **Dial** - 拨盘
  - [ ] 文件: `ui/widget/material/widget/dial/`
  - [ ] 基类: QDial
  - [ ] 旋转式数值选择
- [ ] **Stepper** - 步进器
  - [ ] 文件: `ui/widget/material/widget/stepper/`
  - [ ] 基类: QWidget（自定义）
  - [ ] 步骤指示器
- [ ] **Chip** - 芯片/标签
  - [ ] 文件: `ui/widget/material/widget/chip/`
  - [ ] 基类: QWidget（自定义）
  - [ ] 小型交互元素
- [ ] **Badge** - 徽章
  - [ ] 文件: `ui/widget/material/widget/badge/`
  - [ ] 基类: QWidget（自定义）
  - [ ] 数字或状态指示器
- [ ] **Tooltip** - 工具提示
  - [ ] 文件: `ui/widget/material/widget/tooltip/`
  - [ ] 基类: QToolTip
  - [ ] 鼠标悬停显示提示
- [ ] **Popover** - 气泡弹出框
  - [ ] 文件: `ui/widget/material/widget/popover/`
  - [ ] 基类: QWidget（自定义）
  - [ ] 锚定到其他控件的弹出内容
- [ ] **CircularProgressBar** - 环形进度条
  - [ ] 文件: `ui/widget/material/widget/circular_progress_bar/`
  - [ ] 基类: QWidget（自定义）
  - [ ] 圆形进度指示器

---

## 三、P3 - 专业/高级控件 (Low Priority)

- [ ] **SplitView** - 分割窗口
- [ ] **ChartView** - 图表视图
- [ ] **DrawingArea** - 绘图区域
- [ ] **Canvas** - 画布
- [ ] **FileList** - 文件列表
- [ ] **FileTree** - 文件树
- [ ] **CodeEditor** - 代码编辑器
- [ ] **Calendar** - 日历
- [ ] **Wizard** - 向导对话框
- [ ] **PropertyEditor** - 属性编辑器
- [ ] **TreeTable** - 树形表格
- [ ] **Gauge** - 仪表盘
- [ ] **Timeline** - 时间轴
- [ ] **GridView** - 网格视图
- [ ] **SwipeableItem** - 滑动删除
- [ ] **BottomSheet** - 底部表单
- [ ] **NotificationList** - 通知列表
- [ ] **Breadcrumbs** - 面包屑导航
- [ ] **Pagination** - 分页控件
- [ ] **RatingBar** - 评分控件
- [ ] **RangeSlider** - 范围滑块
- [ ] **SliderCaptcha** - 滑动验证
- [ ] **InputValidator** - 输入校验器
- [ ] **AutoComplete** - 自动完成
- [ ] **DropZone** - 拖放区域
- [ ] **RichTextToolBar** - 富文本工具栏

---

## 四、性能与嵌入式支持

### PerformanceProfile
- [ ] 实现性能档位枚举
  ```cpp
  enum class PerformanceProfile {
      Desktop,     // 全功能：60fps，阴影，Ripple
      Embedded,    // 精简：30fps，无阴影，无 Ripple
      UltraLow     // 极简：无动画，无阴影
  };
  ```
- [ ] ThemeManager 支持性能档位
  - [ ] `setPerformanceProfile(PerformanceProfile)`
  - [ ] `performanceProfile()`

### 组件降级配置
- [ ] Desktop 模式
- [ ] Embedded 模式
- [ ] UltraLow 模式

---

## 五、相关文档

- 架构规范: [../../ui/MaterialRules.md](../../ui/MaterialRules.md)
- 依赖: [00_project_skeleton.md](../00_project_skeleton.md), [02_input_layer.md](02_input_layer.md)

---

*最后更新: 2026-03-30*
