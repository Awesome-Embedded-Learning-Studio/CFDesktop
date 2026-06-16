# /architecture — 架构一致性守护

检查代码是否符合 CFDesktop 三层架构规则。

## 层级定义

### Layer 1: base/ (基础层)
- **路径**: `base/`
- **CMake targets**: `cfbase`, `cfbase_headers`, `cfbase_cpu`, `cfbase_memory`, `cfbase_gpu`, `cfbase_network`, `cfbase_console`
- **允许依赖**: Qt6::Core, OS APIs (POSIX, Win32)
- **职责**: 硬件探测、工具库、平台抽象
- **禁止**: 不可感知 UI 或桌面环境概念

### Layer 2: ui/ (UI 框架层)
- **路径**: `ui/`
- **CMake targets**: `cfui`, `cf_ui_base`, `cf_ui_core`, `cf_ui_widget_material`, `cf_ui_components_material`
- **允许依赖**: Qt6::Core, Qt6::Gui, CFDesktop::base
- **职责**: Material Design 3 组件、主题引擎、动画框架
- **禁止**: 不可感知桌面环境或窗口管理概念

### Layer 3: desktop/ (桌面环境层)
- **路径**: `desktop/`
- **CMake targets**: `CFDesktop_shared`, `CFDesktopMain`, `CFDesktopUi`, `cffilesystem`, `cfconfig`, `cflogger`
- **允许依赖**: Qt6::Core, Qt6::Gui, Qt6::Widgets, cfbase, cflogger, cfui
- **职责**: 桌面环境实现、窗口管理、显示后端、配置管理

## 依赖规则 (STRICT)

```
base/  →  禁止 include ui/ 或 desktop/ 的任何头文件
ui/    →  禁止 include desktop/ 的任何头文件
desktop/ → 可以 include ui/ 和 base/ 的头文件
```

## 检查流程

### Step 1: 确定审查范围

根据用户指定的模块/文件/目录，确定需要检查的文件列表。

### Step 2: 检查 #include 指令

对每个源文件和头文件：
1. 提取所有 `#include` 行
2. 将每个 include 映射到其所属层级 (base/ui/desktop/外部)
3. 标记任何向上依赖（低层级 include 高层级）

**检测模式**：
- base 层违规: `#include` 匹配 `ui/` 或 `desktop/` 或 `cfui` 或 `CFDesktop`
- ui 层违规: `#include` 匹配 `desktop/` 或 `CFDesktop_shared`

### Step 3: 检查 CMake 依赖

对每个 `CMakeLists.txt`：
1. 检查 `target_link_libraries` 只引用同层或更低层
2. base: 只允许 Qt 和系统库
3. ui: Qt + CFDesktop::base
4. desktop: Qt + cfbase + cfui + 自身静态库

### Step 4: 检查接口驱动设计

- 抽象接口应在 `components/` 或 `include/` 目录
- 具体实现在 `platform/` 或 `private/` 目录
- 运行时平台选择使用工厂模式
- 行为变化使用策略模式

### Step 5: 检查平台抽象隔离

平台特定代码必须隔离在 `private/<platform>_impl/` 目录中：
- `base/system/*/private/linux_impl/` — Linux 实现
- `base/system/*/private/win_impl/` — Windows 实现
- `desktop/ui/platform/windows/` — Windows 桌面后端
- `desktop/ui/platform/linux_wsl/` — Linux/WSL 后端

共享代码（ui/core, ui/widget, base/include）中不得出现平台特定代码。

## 输出格式 (中文)

```markdown
# 架构一致性报告: <scope>

## 依赖关系图
(ASCII 图示实际依赖关系)

## 违规项
| 文件 | 行号 | 违规类型 | 说明 |
|------|------|----------|------|

## 符合项
(确认观察到的正确模式)

## 建议修改
(针对每个违规的具体修复方案)
```

如果无违规：
> 架构一致性检查通过，未发现层级依赖违规。
