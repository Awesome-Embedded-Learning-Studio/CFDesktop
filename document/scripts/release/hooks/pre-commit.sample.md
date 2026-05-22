---
title: "pre-commit.sample"
description: "文档编写日期: 2026-03-20，运行 installhooks.sh/installhooks"
---

# pre-commit.sample

> 文档编写日期: 2026-03-20

## 使用办法 (Usage)

### 安装方法
运行 install_hooks.sh/install_hooks.ps1 自动安装。

### 绕过方法
```bash
git commit --no-verify -m "message"
```text

## Scripts详解

### 钩子用途
在每次commit前自动检查代码质量。

### 检查内容
1. **C++格式化** (clang-format可用时)
   - 自动格式化暂存的C/C++文件
   - 重新添加到暂存区

2. **Doxygen注释** (Python可用时)
   - 检查头文件Doxygen注释
   - 失败时生成FAILED_DOXYGEN.md

### 检查的文件扩展名
- .cpp, .cc, .cxx, .h, .hpp, .hxx

### 退出码
- 0: 所有检查通过
- 1: 有检查失败
