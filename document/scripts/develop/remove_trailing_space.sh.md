---
title: removetrailingspace.sh
description: "文档编写日期: 2026-03-20，删除项目中所有文本文件的行尾空格和制表符，保持代码库整洁。"
---

# remove_trailing_space.sh

> 文档编写日期: 2026-03-20

## 使用办法 (Usage)

### 基本语法
```bash
./scripts/develop/remove_trailing_space.sh [OPTIONS]
```bash

### 参数说明
| 参数 | 说明 |
|------|------|
| -n, --dry-run | 显示将要更改的内容，不修改文件 |
| -c, --check | 检查模式，发现空格返回错误码1 |
| -s, --staged | 仅处理已暂存的文件 |
| -S, --staged-check | 检查已暂存文件（用于pre-commit钩子） |
| -h, --help | 显示帮助信息 |

### 使用示例
```bash
# 删除所有文件的行尾空格
./scripts/develop/remove_trailing_space.sh

# 仅处理暂存文件
./scripts/develop/remove_trailing_space.sh --staged

# 预览哪些文件有空格
./scripts/develop/remove_trailing_space.sh --dry-run

# 检查模式（CI/CD场景）
./scripts/develop/remove_trailing_space.sh --check
```bash

## Scripts详解 (Detailed Explanation)

### 脚本用途
删除项目中所有文本文件的行尾空格和制表符，保持代码库整洁。

### 工作模式

| 模式 | 说明 |
|------|------|
| 正常模式 | 处理git追踪的所有文件，自动删除行尾空格 |
| Staged模式 | 仅处理git已暂存的文件（使用 `git diff --cached`） |
| Dry Run | 预览模式，不修改文件，仅显示有问题文件的行号 |
| Check | 检查模式，有问题返回错误码1（用于CI/CD） |
| StagedCheck | 检查暂存文件但不修改，专为pre-commit钩子设计 |

### Git集成
- 使用 `git ls-files` 获取追踪文件（尊重.gitignore）
- 使用 `git diff --cached --name-only --diff-filter=ACM` 获取暂存文件
- 过滤器 `ACM` 仅包含 Added, Copied, Modified 的文件
- 适合用作pre-commit钩子

### 处理细节
- 删除行尾的空格和制表符（TAB）
- 使用 `grep -q "[ \t]$"` 检测行尾空白
- 使用 `sed -i "s/[ \t]*$//"` 就地修改文件
- 每个文件最多显示5行有问题内容（dry-run/check模式）

### 输出格式
```yaml
=== Remove Trailing Whitespace ===
Project: /path/to/project
Mode: Staged files only

src/main.cpp:
  12:int main()   {
  45:    return 0;

=== Summary ===
Processed: 150 files
Fixed: 2 files
```bash

### 退出码
| 退出码 | 说明 |
|--------|------|
| 0 | 成功，无行尾空白或已修复 |
| 1 | 检查模式下发现行尾空白 |

### 依赖项
- git (必需)
- bash 4.0+
- sed, grep (标准Unix工具)

### 注意事项
- 自动跳过二进制文件（无法被grep读取）
- 彩色输出需要终端支持
- 使用 `set -eo pipefail` 确保错误传播
