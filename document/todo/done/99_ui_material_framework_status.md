# Phase 6: UI Material 框架 - 状态文档

> **模块ID**: Phase 6
> **状态**: 🚧 P1 控件完成，核心架构缺失
> **总体进度**: 75%
> **最后更新**: 2026-03-27

---

## 一、架构概览

### 1.1 分层架构

```
Layer 6: Performance Profile (0%)
Layer 5: Material Widget Adapter (P0: 100%, P1: 100%, P2: 0%, P3: 0%)
Layer 4: Material Behavior Layer (100%)
Layer 3: Animation Engine Layer (90%) - 缺Rotate/Path动画
Layer 2: Theme Engine Layer (100%)
Layer 1: Core Math & Utility Layer (100%)
Layer 0: Layout System (0%) - **新增：布局系统完全缺失**
```

### 1.2 设计规则 (RULE-01 to RULE-09)

| 规则 | 描述 |
|------|------|
| **RULE-01** | Inheritance Only - 所有 Material 控件必须继承 Qt 原生控件 |
| **RULE-02** | Signal Preservation - 不移除/修改 Qt 公共信号 |
| **RULE-03** | Complete Paint Override - paintEvent 必须完全控制渲染，不使用 `style()->drawControl()` |
| **RULE-04** | No QSS - Qt StyleSheet 禁止用于 Material 视觉效果 |
| **RULE-05** | No Qt Quick - 不使用 QML/Qt Quick |
| **RULE-06** | Animation Disable - 动画系统必须支持全局禁用 |
| **RULE-07** | Theme Read-Only - 控件不能修改全局主题状态 |
| **RULE-08** | Component Injection - 渲染组件通过组合注入，而非直接 `new` |
| **RULE-09** | Token Access - 通过上下文感知接口和 token 字符串访问主题 |

### 1.3 层级依赖规则

```
ui/widget/material  -> ui/components, ui/core, ui/base
ui/components       -> ui/core, ui/base
ui/core             -> ui/base
ui/base             -> QtCore only (no QtWidgets/QtGui)
```

---

## 二、完成状态总览

| Layer | 状态 | 完成度 |
|-------|------|--------|
| Layer 1: Core Math & Utility | ✅ | 100% |
| Layer 2: Theme Engine | ✅ | 100% |
| Layer 3: Animation Engine | ⚠️ | 90% |
| Layer 4: Material Behavior | ✅ | 100% |
| Layer 5: P0 核心控件 | ✅ | 100% (7/7) |
| Layer 5: P1 常用控件 | ✅ | 100% (12/12) |
| Layer 5: P2 高级控件 | ⬜ | 0% (0/27) |
| Layer 5: P3 专业控件 | ⬜ | 0% (0/25) |
| Layer 6: Performance Profile | ⬜ | 0% |
| Layer 0: Layout System | ❌ | 0% (新增) |

---

## 三、已完成工作

### 3.1 Layer 1: Core Math & Utility (100%)

| 文件 | 路径 | 功能 |
|------|------|------|
| MathHelper | `ui/base/math_helper.h` | 插值、钳位、曲线求值 |
| Color | `ui/base/color.h` | 颜色空间转换、HCT支持 |
| ColorHelper | `ui/base/color_helper.h` | 颜色混合、对比度 |
| Easing | `ui/base/easing.h` | Material缓动曲线 |
| GeometryHelper | `ui/base/geometry_helper.h` | 圆角形状生成 |
| DevicePixel | `ui/base/device_pixel.h` | DPI单位转换 |

**核心特性:**
- 零 Qt UI 依赖 (仅 QtCore)
- 全面的动画和渲染数学函数
- Material Color System 支持
- 编译时哈希函数 (FNV-1a)

### 3.2 Layer 2: Theme Engine (100%)

#### Token 系统架构

```
Reference Token (MD3 spec)
       |
       v
System Token (cfmaterial scheme)
       |
       v
Component Token (widget-specific)
```

#### 已完成文件

**核心接口:**
- `ui/core/theme_manager.h` - 主题管理器单例
- `ui/core/color_scheme.h` - 颜色方案接口
- `ui/core/font_type.h` - 字体类型接口
- `ui/core/motion_spec.h` - 动画规范接口
- `ui/core/radius_scale.h` - 形状缩放接口

**Material 实现:**
- `ui/core/material/cfmaterial_scheme.h` - Material 颜色实现
- `ui/core/material/cfmaterial_theme.h` - Material 主题
- `ui/core/material/cfmaterial_fonttype.h` - Material 字体实现
- `ui/core/material/cfmaterial_motion.h` - Material 动画实现
- `ui/core/material/cfmaterial_radius_scale.h` - Material 形状实现
- `ui/core/material/material_factory.h` - Material 主题工厂

**Token 目录:** (参见 ui/core/token/ 源码)

### 3.3 Layer 3: Animation Engine (100%)

#### 已完成文件

**核心接口:**
- `ui/components/animation.h` - ICFAbstractAnimation 基类
- `ui/components/timing_animation.h` - ICFTimingAnimation (基于时间)
- `ui/components/animation_factory_manager.h` - 动画工厂管理器

**Material 实现:**
- `ui/components/material/cfmaterial_animation_factory.h` - Material 工厂实现
- `ui/components/material/cfmaterial_animation_strategy.h` - 动画策略模式
- `ui/components/material/cfmaterial_fade_animation.h` - 淡入淡出
- `ui/components/material/cfmaterial_scale_animation.h` - 缩放变换
- `ui/components/material/cfmaterial_slide_animation.h` - 滑动动画
- `ui/components/material/cfmaterial_property_animation.h` - Qt 属性动画包装器

**Tokens:**
- `ui/components/material/token/animation_token_literals.h` - 动画 token 定义
- `ui/components/material/token/animation_token_mapping.h` - Token 到动画映射

#### 动画状态

```
Idle -> Running -> (Paused | Finished)
         |
         v
     Stopped
```

#### 支持的 Token 类型
- `md.animation.fadeIn`
- `md.animation.fadeOut`
- `md.animation.slideUp`
- `md.animation.slideDown`
- `md.animation.scaleUp`
- `md.animation.scaleDown`
- `md.animation.rotateIn`
- `md.animation.rotateOut`

### 3.4 Layer 4: Material Behavior (100%)

| 组件 | 文件 | 功能 |
|------|------|------|
| StateMachine | `ui/widget/material/base/state_machine.h` | 视觉状态管理 |
| PainterLayer | `ui/widget/material/base/painter_layer.h` | 颜色叠加层绘制 |
| RippleHelper | `ui/widget/material/base/ripple_helper.h` | 涟漪效果控制器 |
| ElevationController | `ui/widget/material/base/elevation_controller.h` | 阴影/海拔系统 |
| FocusRing | `ui/widget/material/base/focus_ring.h` | 焦点环渲染器 |

#### 状态机状态与透明度

| 状态 | 透明度 | 描述 |
|------|:------:|------|
| Normal | 0.00 | 默认状态 |
| Hovered | 0.08 | 鼠标悬停 |
| Pressed | 0.12 | 鼠标按下 |
| Focused | 0.12 | 键盘焦点 |
| Dragged | 0.16 | 正在拖拽 |
| Checked | 0.08 | 选中状态 |
| Disabled | 0.00 | 禁用状态 |

**优先级:** Disabled > Pressed > Dragged > Focused > Hovered > Normal

#### 海拔级别

| 级别 | DP | 阴影 (浅色) | 色调叠加 (深色) |
|------:|---:|-------------|------------------|
| 0 | 0dp | 无 | 无 |
| 1 | 1dp | blur=2px, offset=1px | overlay 1 |
| 2 | 3dp | blur=4px, offset=2px | overlay 2 |
| 3 | 6dp | blur=8px, offset=4px | overlay 3 |
| 4 | 8dp | blur=12px, offset=6px | overlay 4 |
| 5 | 12dp | blur=16px, offset=8px | overlay 5 |

### 3.5 Layer 5: P0 核心控件 (100%)

| 控件 | 文件 | Qt 基类 | 功能 |
|------|------|---------|------|
| **Button** | `ui/widget/material/widget/button/` | QPushButton | 5种变体、阴影、涟漪 |
| **TextField** | `ui/widget/material/widget/textfield/` | QLineEdit | 浮空标签、前后图标、计数器 |
| **TextArea** | `ui/widget/material/widget/textarea/` | QTextEdit | 多行文本输入 |
| **Label** | `ui/widget/material/widget/label/` | QLabel | 文本显示 |
| **CheckBox** | `ui/widget/material/widget/checkbox/` | QCheckBox | 复选框 |
| **RadioButton** | `ui/widget/material/widget/radiobutton/` | QRadioButton | 单选按钮 |
| **GroupBox** | `ui/widget/material/widget/groupbox/` | QGroupBox | 分组容器 |

#### Button 变体
- Filled (primary) - 填充主按钮
- Outlined (secondary) - 轮廓次按钮
- Text (tertiary) - 文本三级按钮
- Tonal - 色调按钮
- Elevated - 抬升按钮

#### TextField 功能
- 2种变体：Filled, Outlined
- 浮空标签
- 前后图标支持
- 清除按钮
- 字符计数器
- 帮助文本/错误文本
- 密码模式

---

## 四、待完成工作

### 4.1 Layer 5: P1 常用控件 (12/12) ✅

| 控件 | Qt 基类 | 路径 | Demo |
|------|---------|------|------|
| **Button** | QPushButton | `button/` | ButtonDemo |
| **CheckBox** | QCheckBox | `checkbox/` | CheckBoxDemo |
| **ComboBox** | QComboBox | `comboBox/` | ComboBoxDemo |
| **DoubleSpinBox** | QDoubleSpinBox | `doublespinbox/` | DoubleSpinBoxDemo |
| **GroupBox** | QGroupBox | `groupbox/` | GroupBoxDemo |
| **Label** | QLabel | `label/` | LabelDemo |
| **ListView** | QListView | `listview/` | ListViewDemo |
| **ProgressBar** | QProgressBar | `progressbar/` | ProgressBarDemo |
| **RadioButton** | QRadioButton | `radiobutton/` | RadioButtonDemo |
| **ScrollView** | QScrollArea | `scrollview/` | ScrollViewDemo |
| **Separator** | QFrame | `separator/` | SeparatorDemo |
| **Slider** | QSlider | `slider/` | SliderDemo |
| **SpinBox** | QSpinBox | `spinbox/` | SpinBoxDemo |
| **Switch** | QCheckBox | `switch/` | SwitchDemo |
| **TabView** | QTabWidget | `tabview/` | TabViewDemo |
| **TableView** | QTableView | `tableview/` | TableViewDemo |
| **TextArea** | QTextEdit | `textarea/` | TextAreaDemo |
| **TextField** | QLineEdit | `textfield/` | TextFieldDemo |
| **TreeView** | QTreeView | `treeview/` | TreeViewDemo |

### 4.2 Layer 5: P2 高级控件 (0/27)

| 控件 | Qt 基类 | 优先级 |
|------|---------|--------|
| DatePicker | QCalendarWidget | P2 |
| TimePicker | QTimeEdit | P2 |
| ColorPicker | QColorDialog | P2 |
| MenuBar | QMenuBar | P2 |
| ContextMenu | QMenu | P2 |
| ToolBar | QToolBar | P2 |
| StatusBar | QStatusBar | P2 |
| Dialog | QDialog | P2 |
| Snackbar | QWidget (custom) | P2 |
| Card | QFrame (custom) | P2 |
| FloatingActionButton | QPushButton (custom) | P2 |
| BottomNavigation | QWidget (custom) | P2 |
| Drawer | QWidget (custom) | P2 |
| SearchBar | QLineEdit (custom) | P2 |
| Dial | QDial | P2 |
| Stepper | QWidget (custom) | P2 |
| Chip | QWidget (custom) | P2 |
| Badge | QWidget (custom) | P2 |
| Tooltip | QToolTip | P2 |
| Popover | QWidget (custom) | P2 |
| CircularProgressBar | QWidget (custom) | P2 |
| NavigationRail | QWidget (custom) | P2 |
| NavigationDrawer | QWidget (custom) | P2 |
| TopAppBar | QWidget (custom) | P2 |
| BottomSheet | QWidget (custom) | P2 |
| AlertDialog | QWidget (custom) | P2 |
| Menu | QWidget (custom) | P2 |

### 4.3 Layer 5: P3 专业控件 (0/25)

| 控件 | 类型 |
|------|------|
| SplitView | 专业分割视图 |
| ChartView | 图表视图 |
| DrawingArea | 绘图区域 |
| Canvas | 画布控件 |
| FileList | 文件列表 |
| FileTree | 文件树 |
| CodeEditor | 代码编辑器 |
| Calendar | 日历控件 |
| Wizard | 向导对话框 |
| PropertyEditor | 属性编辑器 |
| TreeTable | 树表控件 |
| Gauge | 仪表盘 |
| Timeline | 时间轴 |
| GridView | 网格视图 |
| SwipeableItem | 可滑动项 |
| NotificationList | 通知列表 |
| Breadcrumbs | 面包屑导航 |
| Pagination | 分页控件 |
| RatingBar | 评分条 |
| RangeSlider | 范围滑块 |
| SliderCaptcha | 滑块验证码 |
| InputValidator | 输入验证器 |
| AutoComplete | 自动完成 |
| DropZone | 拖放区域 |
| RichTextToolBar | 富文本工具栏 |

### 4.4 Layer 6: 性能配置 (0%)

#### PerformanceProfile 枚举

```cpp
enum class PerformanceProfile {
    Desktop,   // 完整: 60fps, 阴影, 涟漪, 全部动画
    Embedded,  // 降级: 30fps, 无阴影, 无涟漪, 仅状态动画
    UltraLow   // 最小: 无动画, 无阴影, 仅颜色
};
```

#### 降级策略

| 特性 | Desktop | Embedded | UltraLow |
|------|:-------:|:--------:|:--------:|
| 涟漪效果 | 是 | 否 | 否 |
| 阴影 | 是 | 否 | 否 |
| 状态动画 | 是 | 是 | 否 |
| 焦点动画 | 是 | 是 | 否 |
| 目标 FPS | 60 | 30 | N/A |

---

## 五、实现规范

### 5.1 Widget 实现模板

所有控件遵循 Button 实现模式:

```cpp
class Button : public QPushButton {
    Q_OBJECT

public:
    explicit Button(QWidget* parent = nullptr);
    void paintEvent(QPaintEvent* event) override;
    QSize sizeHint() const override;

protected:
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    StateMachine* m_stateMachine;
    RippleHelper* m_ripple;
    MdElevationController* m_elevation;
    MdFocusIndicator* m_focusIndicator;
};
```

### 5.2 事件处理模式

```cpp
void Widget::enterEvent(QEnterEvent* event) {
    QPushButton::enterEvent(event);  // 始终先调用父类
    m_stateMachine->onHoverEnter();
    update();
}
```

### 5.3 标准绘制模式

```cpp
void Widget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 1. 背景
    painter.fillRect(rect(), backgroundColor());

    // 2. 状态层
    m_stateMachine->paintStateLayer(&painter, rect());

    // 3. 内容
    paintContent(&painter);

    // 4. 海拔阴影
    m_elevation->paintShadow(&painter, rect());

    // 5. 焦点环
    if (hasFocus()) {
        m_focusIndicator->paint(&painter, rect());
    }
}
```

---

## 六、测试覆盖情况

### 已实现的测试

**ui/base 测试:**
- `math_helper_test.cpp`
- `color_test.cpp`
- `color_helper_test.cpp`
- `device_pixel_test.cpp`
- `geometry_helper_test.cpp`
- `easing_test.cpp`

**ui/components 测试:**
- `state_machine_test.cpp`
- `ripple_helper_test.cpp`
- `elevation_controller_test.cpp`
- `focus_ring_test.cpp`
- `painter_layer_test.cpp`

**ui/core 测试:**
- `token_test.cpp`

### 关键缺口: UI控件测试

| 测试类型 | 状态 | 覆盖率 |
|---------|------|-------|
| UI Base 测试 | ✅ | 60% |
| UI Components 测试 | ✅ | 60% |
| UI Widgets 测试 | ❌ | **0%** |
| Widget 单元测试 | ❌ | **0%** |

**急需补充**: 为19个P0/P1控件添加单元测试
- button_test.cpp, textfield_test.cpp, textarea_test.cpp
- checkbox_test.cpp, radiobutton_test.cpp, groupbox_test.cpp
- label_test.cpp, slider_test.cpp, progressbar_test.cpp
- switch_test.cpp, combobox_test.cpp, spinbox_test.cpp
- doublespinbox_test.cpp, listview_test.cpp, treeview_test.cpp
- tableview_test.cpp, tabview_test.cpp, scrollview_test.cpp
- separator_test.cpp

---

## 七、关键文件路径

### 已实现文件

**基础层 (Layer 1):**
```
d:\ProjectHome\CFDesktop\ui\base\
├── math_helper.h/cpp
├── color.h/cpp
├── color_helper.h/cpp
├── easing.h/cpp
├── geometry_helper.h/cpp
└── device_pixel.h/cpp
```

**核心层 (Layer 2):**
```
d:\ProjectHome\CFDesktop\ui\core\
├── theme_manager.h/cpp
├── color_scheme.h
├── material/
│   ├── cfmaterial_scheme.h/cpp
│   ├── cfmaterial_theme.h/cpp
│   ├── cfmaterial_fonttype.h/cpp
│   ├── cfmaterial_motion.h/cpp
│   ├── cfmaterial_radius_scale.h/cpp
│   └── material_factory.h/cpp
└── token/
    ├── material_scheme/
    ├── motion/
    ├── radius_scale/
    ├── typography/
    └── theme_name/
```

**动画层 (Layer 3):**
```
d:\ProjectHome\CFDesktop\ui\components\
├── animation.h/cpp
├── timing_animation.h/cpp
├── animation_factory_manager.h/cpp
└── material/
    ├── cfmaterial_animation_factory.h/cpp
    ├── cfmaterial_animation_strategy.h/cpp
    ├── cfmaterial_fade_animation.h/cpp
    ├── cfmaterial_scale_animation.h/cpp
    ├── cfmaterial_slide_animation.h/cpp
    ├── cfmaterial_property_animation.h/cpp
    └── token/
```

**行为层 (Layer 4):**
```
d:\ProjectHome\CFDesktop\ui\widget\material\base\
├── state_machine.h/cpp
├── painter_layer.h/cpp
├── ripple_helper.h/cpp
├── elevation_controller.h/cpp
└── focus_ring.h/cpp
```

**控件层 (Layer 5 - P0 + P1):**
```
/home/charliechen/CFDesktop/ui/widget/material/widget/
├── button/button.h/cpp ✅
├── textfield/textfield.h/cpp ✅
├── textarea/textarea.h/cpp ✅
├── label/label.h/cpp ✅
├── checkbox/checkbox.h/cpp ✅
├── radiobutton/radiobutton.h/cpp ✅
├── groupbox/groupbox.h/cpp ✅
├── combobox/combobox.h/cpp ✅ (P1)
├── slider/slider.h/cpp ✅ (P1)
├── progressbar/progressbar.h/cpp ✅ (P1)
├── switch/switch.h/cpp ✅ (P1)
├── listview/listview.h/cpp ✅ (P1)
├── treeview/treeview.h/cpp ✅ (P1)
├── tableview/tableview.h/cpp ✅ (P1)
├── tabview/tabview.h/cpp ✅ (P1)
├── scrollview/scrollview.h/cpp ✅ (P1)
├── separator/separator.h/cpp ✅ (P1)
├── spinbox/spinbox.h/cpp ✅ (P1)
└── doublespinbox/doublespinbox.h/cpp ✅ (P1)
```

### 待创建文件

**Layer 5 - P2 高级控件 (27个):**
```
ui/widget/material/widget/
├── datepicker/          # 日期选择器
├── timepicker/          # 时间选择器
├── colorpicker/         # 颜色选择器
├── menubar/             # 菜单栏
├── contextmenu/         # 右键菜单
├── toolbar/             # 工具栏
├── statusbar/           # 状态栏
├── dialog/              # 对话框
├── snackbar/            # 消息条
├── card/                # 卡片
├── floatingactionbutton/ # 浮动操作按钮
├── bottomnavigation/    # 底部导航
├── drawer/              # 抽屉
├── searchbar/           # 搜索栏
├── dial/                # 旋钮
├── stepper/             # 步进器
├── chip/                # 芯片
├── badge/               # 徽章
├── tooltip/             # 工具提示
├── popover/             # 弹出框
├── circularprogressbar/ # 圆形进度条
├── navigationrail/      # 侧边导航
├── navigationdrawer/    # 导航抽屉
├── topappbar/           # 顶栏
├── bottomsheet/         # 底部面板
├── alertdialog/         # 警告对话框
└── menu/                # 菜单
```

**Layer 6 - 性能配置:**
```
d:\ProjectHome\CFDesktop\ui\core\
└── performance_profile.h/cpp
```

---

## 八、P1 控件完成里程碑 (2026-03-19)

### 完成概览
- **完成日期**: 2026-03-18
- **组件总数**: 19 个 (P0: 7 + P1: 12)
- **Demo 示例**: 19 个完整示例
- **代码行数**: ~15000 行 (估算)

### P1 新增组件详情

| 组件 | 主要功能 | 特性 |
|------|---------|------|
| **Slider** | 滑块输入 | 水平/垂直、刻度标记、轨道激活态 |
| **Switch** | 开关切换 | 动画滑块、52x32dp 尺寸、轨道颜色过渡 |
| **ProgressBar** | 进度显示 | 确定/不确定模式、平滑动画 |
| **ComboBox** | 下拉选择 | Filled/Outlined 变体、自定义弹出窗口 |
| **ListView** | 列表显示 | 单/双/三行项、自定义委托、分割线 |
| **TableView** | 表格显示 | Material 表头、网格线、行选择涟漪 |
| **TreeView** | 树形显示 | 展开/折叠动画、树连接线、56dp/级缩进 |
| **TabView** | 标签页 | 滑动指示器动画、关闭按钮、选项卡滚动 |
| **ScrollView** | 滚动视图 | 12dp 滚动条、淡入淡出、悬停扩展 |
| **Separator** | 分隔线 | 水平/垂直、FullBleed/Inset/MiddleInset |
| **SpinBox** | 整数输入 | 增量/减量按钮、轮廓样式、焦点指示器 |
| **DoubleSpinBox** | 浮点输入 | 同 SpinBox，支持浮点数 |

### 共同特性实现
- ✅ Material Design 3 色彩系统 (CFColor + MD3 tokens)
- ✅ 状态机管理 (hover/press/focus/disabled)
- ✅ 涟漪效果 (RippleHelper)
- ✅ 焦点指示器 (FocusRing)
- ✅ 动画支持 (CFMaterialAnimationFactory)
- ✅ 7步绘制流水线

### 构建集成
所有组件已集成到 CMake 构建系统：
- `ui/widget/CMakeLists.txt` 已更新
- `example/ui/widget/material_example/CMakeLists.txt` 已更新
- MaterialGalleryWindow 已注册所有 Demo

### 下一步工作
1. **P2 高级控件** (27个) - 菜单、对话框、工具栏等
2. **单元测试补充** - 提高 P0/P1 控件测试覆盖率
3. **Layer 6: 性能配置** - HWTier 联动降级策略

---

## 九、相关文档

- 原始TODO: [../base/99_ui_material_framework.md](../base/99_ui_material_framework.md)

---

*文档版本: v1.1*
*生成时间: 2026-03-19*
