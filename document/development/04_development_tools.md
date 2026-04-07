# 开发工具配置

本文档介绍 CFDesktop 项目推荐的开发工具配置，包括 VSCode 设置、clangd 配置、代码格式化和调试配置。

## 目录

- [VSCode 配置](#vscode-配置)
- [clangd 配置](#clangd-配置)
- [推荐 VSCode 扩展](#推荐-vscode-扩展)
- [代码格式化](#代码格式化)
- [调试配置](#调试配置)

---

## VSCode 配置

### .vscode/settings.json 说明

项目根目录的 `.vscode/settings.json` 包含以下关键配置：

```json
{
    "clangd.path": "D:/QT/Qt6.6.0/Tools/llvm-mingw1706_64/bin/clangd.exe",
    "clangd.arguments": [
        "--compile-commands-dir=D:/ProjectHome/CFDesktop/out/build_develop",
        "--query-driver=D:/QT/Qt6.6.0/Tools/*/bin/g++.exe",
        "--background-index",
        "--clang-tidy",
        "--header-insertion=iwyu",
        "--log=error",
        "--header-insertion-decorators",
        "--ranking-model=decision_forest"
    ]
}
```

**参数说明：**

| 参数 | 说明 |
|------|------|
| `clangd.path` | clangd 可执行文件路径（使用 Qt 自带的 LLVM） |
| `--compile-commands-dir` | compile_commands.json 所在目录（构建输出目录） |
| `--query-driver` | 允许 clangd 索引的编译器路径 |
| `--background-index` | 启用后台索引，加快全局搜索速度 |
| `--clang-tidy` | 启用静态代码分析 |
| `--header-insertion=iwyu` | 智能头文件插入（IWYU：Include What You Use） |
| `--ranking-model` | 代码补全排序算法 |

### 生成 compile_commands.json

`compile_commands.json` 由 CMake 自动生成，位于构建输出目录：

```bash
# Windows (Develop 配置)
cmake -B out/build_develop -DCMAKE_BUILD_TYPE=Develop
# 生成 out/build_develop/compile_commands.json
```

---

## clangd 配置

### 代码补全

clangd 提供以下代码补全功能：

- **函数/类名补全**：输入函数名前几个字符自动补全
- **参数提示**：调用函数时显示参数列表
- **成员访问**：使用 `.` 或 `->` 时自动显示成员列表
- **头文件补全**：`#include` 时自动搜索可用头文件

### 代码导航

| 快捷键 | 功能 |
|--------|------|
| `F12` | 跳转到定义 |
| `Shift+F12` | 查找所有引用 |
| `Ctrl+T` | 符号搜索 |
| `Alt+←/→` | 前进/后退 |

### 常见问题

**Q: clangd 无法索引 Qt 头文件？**

A: 检查 `--query-driver` 参数是否包含 Qt 的编译器路径，或检查 compile_commands.json 中的编译器路径。

**Q: 代码补全很慢？**

A: 确保 `--background-index` 已启用，首次索引可能需要几分钟。

---

## 推荐 VSCode 扩展

项目 `.vscode/extensions.json` 定义了推荐的扩展列表：

| 扩展 ID | 名称 | 用途 |
|---------|------|------|
| `llvm-vs-code-extensions.vscode-clangd` | clangd | C++ 代码补全、导航、诊断 |
| `ms-vscode.cmake-tools` | CMake Tools | CMake 项目管理、构建、调试 |
| `twxs.cmake` | CMake Syntax | CMake 语法高亮 |
| `qt-labs.qt-all` | Qt ALL | Qt 资源文件预览、.ui 编辑 |

### 安装推荐扩展

1. 打开 VSCode
2. 按 `Ctrl+Shift+X` 打开扩展面板
3. 搜索 "Recommended"
4. 点击 "Workspace Recommended" 安装所有推荐扩展

或通过命令行安装：

```bash
code --install-extension llvm-vs-code-extensions.vscode-clangd
code --install-extension ms-vscode.cmake-tools
code --install-extension twxs.cmake
code --install-extension qt-labs.qt-all
```

---

## 代码格式化

### .clang-format 配置

项目使用 LLVM 风格的代码格式化规则，配置文件为 `.clang-format`：

```yaml
# 基础风格: LLVM
BasedOnStyle: LLVM

# 缩进设置
IndentWidth: 4
UseTab: Never
ColumnLimit: 100

# 大括号风格: 附加式 { 在语句末尾
BreakBeforeBraces: Attach

# 指针/引用星号位置: 左侧 (int* a)
PointerAlignment: Left
DerivePointerAlignment: false

# 其他设置
Language: Cpp
Standard: c++17
SortIncludes: true
```

### 格式化规则摘要

| 规则 | 设置 |
|------|------|
| 基础风格 | LLVM |
| 缩进 | 4 空格 |
| 列宽 | 100 字符 |
| 大括号 | 附加式 (`if (x) {` |
| 指针星号 | 左侧 (`int* a`) |
| C++ 标准 | C++17 |
| 头文件排序 | 启用 |

### 手动格式化命令

**格式化整个文件：**

```bash
# 格式化单个文件
clang-format -i path/to/file.cpp

# 格式化整个项目
find . -name "*.cpp" -o -name "*.h" | xargs clang-format -i
```

**VSCode 快捷键：**

- `Shift+Alt+F`：格式化当前文件
- 右键 -> "Format Document"：格式化当前文件

### 预提交检查

在提交前检查代码格式：

```bash
# 检查格式是否正确（不修改文件）
clang-format --dry-run --Werror path/to/file.cpp
```

---

## 调试配置

### GDB 配置

项目使用 GDB 作为调试器，`.vscode/launch.json` 配置示例：

```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug Target",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/out/build_develop/bin/textarea_example.exe",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "miDebuggerPath": "D:/QT/Qt6.6.0/Tools/mingw1310_64/bin/gdb.exe",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ]
        }
    ]
}
```

### 断点设置

| 操作 | 快捷键 |
|------|--------|
| 设置/取消断点 | `F9` |
| 条件断点 | 右键断点 -> "Edit Breakpoint" -> 输入条件 |
| 日志断点 | 右键断点 -> "Add Logpoint" -> 输入日志消息 |

### 变量查看

| 面板 | 功能 |
|------|------|
| VARIABLES | 查看当前作用域的变量值 |
| WATCH | 添加表达式监视 |
| CALL STACK | 查看调用栈 |

### 调试快捷键

| 快捷键 | 功能 |
|--------|------|
| `F5` | 开始调试 |
| `Shift+F5` | 停止调试 |
| `F10` | 单步跳过 |
| `F11` | 单步进入 |
| `Shift+F11` | 单步跳出 |

### Linux 调试

在 Docker 容器中调试：

```bash
# 进入容器
bash scripts/build_helpers/docker_start.sh

# 构建调试版本
cmake -B out/build_develop -DCMAKE_BUILD_TYPE=Debug
cmake --build out/build_develop

# 使用 gdb 调试
gdb out/build_develop/bin/your_app
```

---

## 相关文档

- [快速开始](./02_quick_start.md)
- [构建系统](./03_build_system.md)
- [Docker 构建](./05_docker_build.md)
- [项目索引](../index.md)
