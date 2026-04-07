# Phase 5: 测试系统 - 状态文档

> **模块ID**: Phase 5
> **状态**: 🚧 基础框架完成，UI控件测试缺失
> **总体进度**: ~55%
> **最后更新**: 2026-03-27

---

## 一、模块概述

测试系统负责为 CFDesktop 项目提供完整的单元测试、集成测试和性能测试框架。

**模块职责:**
- 提供核心工具库的单元测试框架
- 支持 UI 组件测试
- 支持系统信息查询测试
- 支持启动序列测试

---

## 二、完成状态总览

| 模块 | 测试文件数 | 覆盖率 | 状态 |
|------|-----------:|-------:|------|
| Base Utilities | 6 | 90% | ✅ Good |
| UI Base | 6 | 60% | ⚠️ Fair |
| UI Components | 5 | 60% | ⚠️ Fair |
| UI Core | 1 | 30% | ❌ Poor |
| UI Widgets | 0 | **0%** | ❌ **Critical** |
| System Queries | 2 | 70% | ⚠️ Fair |
| Logger | 4 | 85% | ✅ Good |
| ConfigManager | 1 | 80% | ✅ Good |
| Desktop | 1 | 5% | ❌ Poor |
| **Overall** | **26** | **55%** | ⚠️ **Fair** |

---

## 三、已完成工作

### 3.1 测试基础框架 (100%)

#### Google Test 配置
- Google Test v1.14.0 集成
- CMake `setup_third_party` 自动下载
- Windows 特殊配置 (gtest_force_shared_crt ON)
- CTest 集成
- 自定义 `add_gtest_executable` 辅助函数

#### 测试运行脚本
- `run_all_tests.ps1` (Windows PowerShell)
- `run_all_tests.sh` (Linux/macOS Bash)

---

### 3.2 测试目录结构

```
test/
├── CMakeLists.txt                    # 测试构建配置
├── base/                             # 基础工具库测试
│   ├── weak_ptr/
│   │   └── weak_ptr_test.cpp        # WeakPtr 实现测试
│   ├── expected/
│   │   └── expected_test.cpp        # Expected/Result 类型测试
│   ├── hash/
│   │   └── constexpr_fnv1a_test.cpp # 编译时哈希测试
│   └── scope_guard/
│       └── scope_guard_test.cpp     # RAII 作用域守卫测试
├── ui/                               # UI 层测试
│   ├── base/
│   │   ├── device_pixel_test.cpp    # 设备像素转换测试
│   │   ├── easing_test.cpp          # 缓动曲线测试
│   │   ├── math_helper_test.cpp     # 数学助手测试 (全面)
│   │   ├── color_test.cpp           # 颜色工具测试
│   │   ├── color_helper_test.cpp    # 颜色助手测试
│   │   └── geometry_helper_test.cpp # 几何助手测试
│   ├── components/
│   │   ├── elevation_controller_test.cpp  # 高度阴影测试
│   │   ├── focus_ring_test.cpp            # 焦点指示器测试
│   │   ├── painter_layer_test.cpp         # 绘制层测试
│   │   ├── ripple_helper_test.cpp         # 涟漪效果测试
│   │   └── state_machine_test.cpp         # 状态机测试
│   └── core/
│       └── token_test.cpp            # 主题 Token 测试
├── system/                           # 系统查询测试
│   ├── test_memory_info_query.cpp   # 内存信息查询测试
│   └── test_cpu_info_query.cpp      # CPU 信息查询测试
└── boot_test/                        # 启动序列测试
    ├── boot_detect.cpp               # 启动检测
    ├── boot_test_core.cpp            # 核心启动测试
    ├── boot_test_gui.cpp             # GUI 启动测试
    ├── CMakeLists.txt
    └── README.md                     # 启动测试文档
```

---

### 3.3 已实现测试清单

#### Base Utilities 测试

| 测试文件 | 路径 | 覆盖内容 |
|---------|------|---------|
| **expected_test.cpp** | `test/base/expected/expected_test.cpp` | `cf::expected<T,E>` Result 类型：构造/销毁、值和错误访问、赋值、单子操作 (and_then, or_else, transform)、比较、交换、异常处理、移动语义、const 正确性 |
| **scope_guard_test.cpp** | `test/base/scope_guard/scope_guard_test.cpp` | RAII 作用域守卫基本功能 |
| **constexpr_fnv1a_test.cpp** | `test/base/hash/constexpr_fnv1a_test.cpp` | FNV1a 编译时哈希、抗冲突性 |
| **weak_ptr_test.cpp** | `test/base/weak_ptr/weak_ptr_test.cpp` | `cf::WeakPtr` 所有权语义、生命周期管理 |

#### UI Base 测试

| 测试文件 | 路径 | 覆盖内容 |
|---------|------|---------|
| **math_helper_test.cpp** | `test/ui/base/math_helper_test.cpp` | `lerp` (7), `clamp` (6), `remap` (8), `cubicBezier` (7), `springStep` (7), `lerpAngle` (10) 测试用例 |
| **device_pixel_test.cpp** | `test/ui/base/device_pixel_test.cpp` | 设备像素比转换、DP 到像素转换 |
| **easing_test.cpp** | `test/ui/base/easing_test.cpp` | QEasingCurve 包装器、Material 缓动曲线验证 |
| **color_test.cpp** | `test/ui/base/color_test.cpp` | 颜色空间转换、Material 颜色系统、对比度计算 |
| **color_helper_test.cpp** | `test/ui/base/color_helper_test.cpp` | 颜色辅助工具 |
| **geometry_helper_test.cpp** | `test/ui/base/geometry_helper_test.cpp` | 圆角矩形路径生成、矩形裁剪 |

#### UI Components 测试

| 测试文件 | 路径 | 覆盖内容 |
|---------|------|---------|
| **state_machine_test.cpp** | `test/ui/components/state_machine_test.cpp` | Material 行为层：状态转换 (8)、组合状态 (2)、透明度计算 (6)、状态优先级 (2) |
| **ripple_helper_test.cpp** | `test/ui/components/ripple_helper_test.cpp` | 涟漪创建和生命周期、多涟漪支持、淡入动画 |
| **elevation_controller_test.cpp** | `test/ui/components/elevation_controller_test.cpp` | 阴影渲染、高度级别验证 (0-5)、暗主题色调覆盖 |
| **focus_ring_test.cpp** | `test/ui/components/focus_ring_test.cpp` | 焦点环渲染、Material 规范合规 (3dp 宽度, 3dp 内边距) |
| **painter_layer_test.cpp** | `test/ui/components/painter_layer_test.cpp` | 颜色覆盖层、Alpha 混合验证 |

#### UI Core 测试

| 测试文件 | 路径 | 覆盖内容 |
|---------|------|---------|
| **token_test.cpp** | `test/ui/core/token_test.cpp` | Token 系统验证、颜色方案 Token、排版 Token |

#### System 测试

| 测试文件 | 路径 | 覆盖内容 |
|---------|------|---------|
| **test_memory_info_query.cpp** | `test/system/test_memory_info_query.cpp` | 内存信息查询、跨平台内存 API 验证 |
| **test_cpu_info_query.cpp** | `test/system/test_cpu_info_query.cpp` | CPU 信息查询、处理器计数检测 |

---

## 四、待完成模块

### 4.1 Widget 测试 (P0 - Critical)

**状态**: 完全未实现 (0%)

**关键缺口**: 19个P0/P1控件全部无单元测试

**P0 核心控件** (7个):
- `button_test.cpp` - Button 控件测试
- `textfield_test.cpp` - TextField 控件测试
- `textarea_test.cpp` - TextArea 控件测试
- `checkbox_test.cpp` - CheckBox 控件测试
- `radiobutton_test.cpp` - RadioButton 控件测试
- `label_test.cpp` - Label 控件测试
- `groupbox_test.cpp` - GroupBox 控件测试

**P1 常用控件** (12个):
- `slider_test.cpp` - Slider 控件测试
- `progressbar_test.cpp` - ProgressBar 控件测试
- `switch_test.cpp` - Switch 控件测试
- `combobox_test.cpp` - ComboBox 控件测试
- `spinbox_test.cpp` - SpinBox 控件测试
- `doublespinbox_test.cpp` - DoubleSpinBox 控件测试
- `listview_test.cpp` - ListView 控件测试
- `treeview_test.cpp` - TreeView 控件测试
- `tableview_test.cpp` - TableView 控件测试
- `tabview_test.cpp` - TabView 控件测试
- `scrollview_test.cpp` - ScrollView 控件测试
- `separator_test.cpp` - Separator 控件测试

**目标路径**: `test/ui/widgets/`

### 4.2 Animation 测试 (P1)

**状态**: 未实现

必需的测试文件：
- `timing_animation_test.cpp` - 时间动画测试
- `spring_animation_test.cpp` - 弹簧动画测试
- `animation_group_test.cpp` - 动画组测试
- `animation_factory_test.cpp` - 动画工厂测试

**目标路径**: `test/ui/animation/`

### 4.3 Theme 测试 (P1)

**状态**: 部分 (token_test.cpp 已存在)

缺失的测试：
- `theme_manager_test.cpp` - 主题管理器测试
- `color_scheme_test.cpp` - 颜色方案测试
- `motion_spec_test.cpp` - 动画规格测试

**目标路径**: `test/ui/theme/`

### 4.4 集成/UI自动化/性能测试 (P2)

**状态**: 未实现

**集成测试**：
- `base_integration_test.cpp` - Base 库集成测试
- `sdk_integration_test.cpp` - SDK 集成测试
- `shell_integration_test.cpp` - Shell 集成测试

**UI 自动化测试**：
- 鼠标事件模拟
- 键盘事件模拟
- 触摸事件模拟
- 截图对比测试

**性能基准测试**：
- `widget_benchmark.cpp` - 控件渲染性能
- `animation_benchmark.cpp` - 动画帧率测试
- `memory_benchmark.cpp` - 内存使用分析

---

## 五、测试金字塔

```
                    /\
                   /  \
                  / E2E\         (端到端测试 - 少量)
                 /------\
                / 集成  \        (集成测试 - 适量)
               /--------\
              /  UI 自动化 \     (UI 自动化 - 中等)
             /------------\
            /   单元测试    \    (单元测试 - 大量，已完成 ~60%)
           /________________\
```

---

## 六、运行测试

### 构建测试

```bash
# 从项目根目录
cmake -B out/build_test -DBUILD_TESTING=ON
cmake --build out/build_test
```

### 运行所有测试

```bash
./out/build_test/bin/cf_desktop_tests
```

### 运行特定测试

```bash
# 运行特定测试套件
./out/build_test/bin/cf_desktop_tests --gtest_filter=MathHelperTest.*

# 运行特定测试用例
./out/build_test/bin/cf_desktop_tests --gtest_filter=MathHelperTest.Lerp*
```

### 使用脚本运行

**Windows**:
```powershell
.\scripts\run_all_tests.ps1
```

**Linux/macOS**:
```bash
./scripts/run_all_tests.sh
```

---

## 七、下一步行动建议

1. **[P0] 添加 Widget 测试** - 为所有 P0 Material 控件创建单元测试
2. **[P1] 添加 Animation 测试** - 测试时间/弹簧动画和动画组
3. **[P1] 完善 Theme 测试** - 补充主题管理器和颜色方案测试
4. **[P2] 创建 Mock 框架** - 用于硬件抽象层测试
5. **[P2] 添加 UI 自动化** - 鼠标/键盘/触摸模拟测试
6. **[P2] 性能基准测试** - 建立基线性能指标
7. **[P2] CI 集成** - 将自动化测试添加到 CI/CD 流程

---

## 八、相关文档

---

*文档版本: v1.0*
*生成时间: 2026-03-11*
