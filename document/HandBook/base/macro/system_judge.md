---
title: "systemjudge - 系统判断宏"
description: 定义了平台和架构检测的底层宏，是整个项目跨平台支持的基础。这个文件的设计原则很简单——只负责检测，不
---

# system_judge - 系统判断宏

`system_judge.h` 定义了平台和架构检测的底层宏，是整个项目跨平台支持的基础。这个文件的设计原则很简单——只负责检测，不做任何决策，把判断结果以宏的形式暴露给上层使用。

## 操作系统检测

我们支持三个主流桌面操作系统，检测基于编译器预定义宏：

```cpp
// Windows 平台检测（包括 32 位和 64 位）
#if defined(_WIN32) || defined(_WIN64)
#    define CFDESKTOP_OS_WINDOWS
#endif

// Linux 平台检测
#if defined(__linux__)
#    define CFDESKTOP_OS_LINUX
#endif
```text

Windows 的检测用了两个宏是因为 `_WIN32` 在 64 位 Windows 上也会被定义，而 `_WIN64` 只在 64 位上定义。用 `||` 连接可以覆盖所有情况。Linux 的 `__linux__` 则是所有类 Linux 系统通用的宏，如果你需要区分发行版，得在运行时检查 `/etc/os-release`。

## 架构检测

CPU 架构检测主要服务于性能优化和 SIMD 指令使用：

```cpp
// x86-64 (AMD64) 检测
#if defined(__x86_64__) || defined(_M_X64) || defined(__amd64__)
#    define CFDESKTOP_ARCH_X86_64
#endif

// ARM64 检测
#if defined(__aarch64__) || defined(_M_ARM64)
#    define CFDESKTOP_ARCH_ARM64
#endif

// ARM32 检测
#if defined(__arm__) || defined(_M_ARM)
#    define CFDESKTOP_ARCH_ARM32
#endif
```text

这里的 `__x86_64__` 和 `__aarch64__` 是 GCC/Clang 的约定，`_M_X64`、`_M_ARM64`、`_M_ARM` 是 MSVC 的约定。用多个宏检查是因为不同编译器的命名习惯不一样。

## 宏定义规则

每个宏定义都遵循相同的模式——检测到条件后定义一个不带值的标准宏：

```cpp
#if defined(<compiler_macro>)
#    define CFDESKTOP_<CATEGORY>_<NAME>
#endif
```text

不带值的设计是有意为之的。我们只需要知道"是不是这个平台"，不需要传递额外信息。如果需要细分版本（比如 Windows 10 vs Windows 11），应该在运行时检测，而不是用编译时宏。

## 使用示例

```cpp
#include "base/macro/system_judge.h"

// 根据平台选择不同的实现
#if defined(CFDESKTOP_OS_WINDOWS)
    // Windows 实现
#elif defined(CFDESKTOP_OS_LINUX)
    // Linux 实现
#else
    #error "Unsupported platform"
#endif

// 根据架构选择优化路径
#if defined(CFDESKTOP_ARCH_X86_64)
    // 使用 AVX2 指令集
#elif defined(CFDESKTOP_ARCH_ARM64)
    // 使用 NEON 指令集
#endif
```bash

⚠️ 记得在 `#else` 或 `#elif` 分支加上 `#error`，避免在不支持的平台上静默编译通过，结果运行时出错。

## 编译器兼容性

这些宏在主流编译器上都能工作：

| 编译器 | 平台宏支持 | 架构宏支持 |
|--------|-----------|-----------|
| MSVC   | 完全支持  | 完全支持  |
| GCC    | 完全支持  | 完全支持  |
| Clang  | 完全支持  | 完全支持  |
| MinGW  | 完全支持  | 完全支持  |

如果你用了其他编译器（比如 Intel ICC），可能需要额外添加检测逻辑。

## 扩展新平台

添加新平台支持时，需要在两个地方修改代码：

```cpp
// 1. 在 system_judge.h 添加检测
#if defined(__NEW_OS__)
#    define CFDESKTOP_OS_NEW_OS
#endif

// 2. 在对应实现文件添加平台特定代码
#if defined(CFDESKTOP_OS_NEW_OS)
    // 新平台实现
#endif
```text

记得同步更新文档和测试，确保 CI 环境能在新平台上正常运行。

## 常见问题

为什么不用 C++20 的 `#ifdef`？因为 `std::is_constant_evaluated()` 只能判断是否在常量求值上下文，不能区分平台。

为什么不用 CMake 的 `CMAKE_SYSTEM_NAME`？因为那是构建系统的信息，不能直接在代码里用。我们用宏是为了在预处理阶段就知道平台，实现条件编译。

## 相关文档

- [macros - 宏定义系统](../macros.md)
- [基础工具类概述](../overview.md)
