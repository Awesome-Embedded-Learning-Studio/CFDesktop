# /cross-platform — 跨平台兼容性检查

检查代码的跨平台兼容性，确保正确的平台抽象和隔离。

## 触发方式

- `/cross-platform <模块路径>` — 检查跨平台兼容性
- Review 流程内部调用

## 支持平台

| 平台 | API | 编译器 |
|------|-----|--------|
| Windows 10/11 | Win32, DWM | MSVC, MinGW |
| Linux/WSL | POSIX, X11, Wayland | GCC, Clang |
| Embedded ARM | EGLFS, LinuxFB (规划中) | GCC交叉编译 |

## 检查流程

### Step 1: 条件编译检查

#### 合法的平台宏 (优先使用)
- `Q_OS_WIN` — Windows 平台 (Qt 宏，**首选**)
- `Q_OS_LINUX` — Linux 平台 (Qt 宏，**首选**)
- `Q_OS_MAC` — macOS 平台 (Qt 宏)
- `_WIN32` / `_MSC_VER` — Windows 特定低层代码（可接受）
- `WIN32` — CMake 级别（可接受）

#### 禁止的宏
- `__linux__` — 应使用 `Q_OS_LINUX`
- `__APPLE__` — 应使用 `Q_OS_MAC`
- `#ifdef _WIN64` — 应使用 `Q_OS_WIN`

#### 路径处理
- 硬编码路径分隔符 (`"\"` 或 `"/"`) — 应使用 `QDir`/`QFileInfo`
- 硬编码行尾 — 应使用 `QFile` 自动处理
- 硬编码临时目录 — 应使用 `QStandardPaths`

### Step 2: 平台抽象目录结构检查

验证平台特定代码的隔离模式：

**正确模式** (base/system/)：
```
base/system/cpu/
├── cfcpu.h              ← 公共头文件（平台无关接口）
├── cfcpu.cpp            ← 公共实现（调用 host.h）
└── private/
    ├── cpu_host.h       ← 平台选择器
    ├── linux_impl/      ← Linux 实现
    │   └── cpu_linux.cpp
    └── win_impl/        ← Windows 实现
        └── cpu_win.cpp
```

**正确模式** (desktop/ui/platform/)：
```
desktop/ui/platform/
├── interface.h          ← 平台无关接口
├── windows/             ← Windows 实现
└── linux_wsl/           ← Linux/WSL 实现
```

### Step 3: 共享层纯度检查

以下位置**禁止**包含平台特定代码：
- `ui/core/` — 主题引擎
- `ui/widget/` — MD3 控件
- `ui/components/` — 动画框架
- `base/include/base/` — 工具头文件

平台特定代码**必须**隔离在：
- `base/system/*/private/`
- `desktop/ui/platform/windows/`
- `desktop/ui/platform/linux_wsl/`

### Step 4: DLL 导出宏检查

验证正确的导出宏使用：
- base: `CF_BASE_EXPORT`
- ui: `CF_UI_EXPORT`
- desktop: `CF_DESKTOP_EXPORT`

每个导出宏应处理 Windows `__declspec` 和 Linux `__attribute__((visibility))`。

### Step 5: 输出报告 (中文)

```markdown
# 跨平台兼容性报告: <scope>

## 条件编译检查
### 使用的平台宏
| 文件 | 行号 | 宏 | 合规 |
|------|------|-----|------|

### 不符合规范的宏
(需修改的项，含建议替换)

## 平台抽象层检查
### 正确隔离的代码
- ...

### 泄漏到共享层的平台代码
- ...

## 路径处理检查
- 合规项: ...
- 违规项: ...

## DLL 导出检查
- 合规项: ...
- 缺失导出宏: ...

## 修复建议
(按优先级排列的具体修改方案)
```
