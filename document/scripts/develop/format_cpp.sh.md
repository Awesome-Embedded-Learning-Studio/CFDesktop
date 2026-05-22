---
title: formatcpp.sh
description: "文档编写日期: 2026-03-20，使用  自动格式化项目中的 C++ 源代码文件，确保代码风格一"
---

# format_cpp.sh

> 文档编写日期: 2026-03-20

## 使用办法 (Usage)

### 基本语法

```bash
./scripts/develop/format_cpp.sh [OPTIONS]
```bash

### 参数说明

| 参数 | 说明 |
|------|------|
| `-n`, `--dry-run` | 显示将要更改的内容，不修改文件 |
| `-c`, `--check` | 检查文件格式，需要格式化的文件返回错误码1 |
| `-h`, `--help` | 显示帮助信息 |

### 使用示例

```bash
# 格式化所有C++文件
./scripts/develop/format_cpp.sh

# 检查哪些文件需要格式化
./scripts/develop/format_cpp.sh --check

# 预览格式化效果
./scripts/develop/format_cpp.sh --dry-run
```bash

## Scripts详解 (Detailed Explanation)

### 脚本用途

使用 `clang-format` 自动格式化项目中的 C++ 源代码文件，确保代码风格一致。

### 支持的文件类型

| 扩展名 | 说明 |
|--------|------|
| `.cpp` | C++ 源文件 |
| `.cc` | C++ 源文件 |
| `.cxx` | C++ 源文件 |
| `.h` | 头文件 |
| `.hpp` | C++ 头文件 |

### 排除目录

- `third_party/` - 第三方依赖
- `build/` - 构建输出目录
- `out/` - 输出目录
- `.git/` - Git 版本控制目录

### 依赖项

- **clang-format** (必需) - C++ 代码格式化工具

### 安装 clang-format

```bash
# Ubuntu/Debian
sudo apt install clang-format

# Fedora/RHEL
sudo dnf install clang-format

# Arch Linux
sudo pacman -S clang

# macOS
brew install clang-format
```bash

### 工作模式

| 模式 | 说明 |
|------|------|
| **默认模式** | 直接格式化文件，就地修改 |
| **Dry Run** | 显示将要格式化的文件列表，不修改任何文件 |
| **Check** | 检查模式，需要格式化的文件会返回错误码1，适用于 CI/CD |

### 输出日志

脚本使用彩色输出来区分不同类型的消息：

- **绿色** - 成功消息
- **黄色** - 警告消息
- **红色** - 错误消息
- **青色** - 标题信息
- **灰色** - 一般信息

### 配置文件

使用项目根目录的 `.clang-format` 配置文件。

### 返回码

| 返回码 | 说明 |
|--------|------|
| 0 | 所有文件格式正确或格式化成功 |
| 1 | clang-format 未安装或 Check 模式下发现需要格式化的文件 |

### 注意事项

1. 确保已安装 `clang-format` 并在 PATH 中
2. 脚本会自动定位项目根目录
3. 使用 `--check` 参数可用于 CI/CD 流水线中验证代码格式
