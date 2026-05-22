---
title: "macros - 宏定义系统"
description: 是 CFDesktop 项目的宏定义入口文件，本身不直接定义宏，而是引入  来完成平台检测。这个设计
---

# macros - 宏定义系统

`macros.h` 是 CFDesktop 项目的宏定义入口文件，本身不直接定义宏，而是引入 `system_judge.h` 来完成平台检测。这个设计把平台相关的宏定义集中管理，避免在代码里到处散落 `#ifdef` 检查。

## 平台检测

项目目前支持 Windows 和 Linux 两个桌面平台，通过 `system_judge.h` 自动检测：

```cpp
#include "base/macros.h"

// 编译时会自动定义以下宏之一
#ifdef CFDESKTOP_OS_WINDOWS
    // Windows 特定代码
#elif defined(CFDESKTOP_OS_LINUX)
    // Linux 特定代码
#endif
```text

检测逻辑基于编译器预定义宏。Windows 平台检查 `_WIN32` 或 `_WIN64`，Linux 平台检查 `__linux__`。这些是主流编译器（MSVC、GCC、Clang）都遵守的约定。

## 架构检测

除了操作系统，我们还关心 CPU 架构，特别是在做性能优化或 SIMD 操作时：

```cpp
#ifdef CFDESKTOP_ARCH_X86_64
    // x86-64 (AMD64) 特定代码
#elif defined(CFDESKTOP_ARCH_ARM64)
    // ARM64 特定代码
#elif defined(CFDESKTOP_ARCH_ARM32)
    // ARM32 特定代码
#endif
```text

x86-64 检查 `__x86_64__`、`_M_X64` 或 `__amd64__`，ARM64 检查 `__aarch64__` 或 `_M_ARM64`，ARM32 检查 `__arm__` 或 `_M_ARM`。这覆盖了我们项目目标平台的所有主流架构。

## 使用场景

平台检测宏最常见的用途是条件编译和平台特定代码隔离：

```cpp
// 场景一：条件编译
std::string get_config_path() {
#ifdef CFDESKTOP_OS_WINDOWS
    return getenv("APPDATA") + std::string("/myapp/config");
#elif defined(CFDESKTOP_OS_LINUX)
    return getenv("HOME") + std::string("/.config/myapp");
#endif
}

// 场景二：平台特定头文件
#include "base/macros.h"

#ifdef CFDESKTOP_OS_WINDOWS
    #include <windows.h>
#elif defined(CFDESKTOP_OS_LINUX)
    #include <unistd.h>
#endif

// 场景三：不同平台的实现细节
void set_thread_priority(int priority) {
#ifdef CFDESKTOP_OS_WINDOWS
    SetThreadPriority(GetCurrentThread(), priority);
#elif defined(CFDESKTOP_OS_LINUX)
    // Linux 实现省略...
#endif
}
```text

## 命名约定

所有自定义宏都带 `CFDESKTOP_` 前缀，避免和第三方库或系统宏冲突。如果你需要添加新的平台检测宏，记得遵守这个约定。

## 注意事项

使用宏检测有几个需要注意的地方。第一，这些宏是编译时确定的，不能在运行时切换。如果你需要同一个二进制在多个平台运行，那得用动态加载或抽象工厂模式。

第二，不要过度使用平台宏。大部分跨平台差异应该通过封装类或函数来处理，而不是到处写 `#ifdef`。我们这套基础工具库的目的就是帮你隔离这些差异，让上层业务代码不需要关心平台细节。

第三，宏定义在预处理阶段生效，IDE 的跳转和重构工具通常不会跟踪它们。如果你在重构时修改了宏名字，记得全局搜索替换，不要只信任 IDE 的引用查找。

## 扩展指南

如果需要支持新的平台或架构，在 `system_judge.h` 中添加相应的检测逻辑：

```cpp
// 添加新平台的示例
#if defined(__NEW_OS__)
#    define CFDESKTOP_OS_NEW_OS
#endif
```text

然后记得在对应的地方添加平台特定实现，并写测试确保在新平台上能正确编译和运行。

## 相关文档

- [system_judge - 系统判断宏](./macro/system_judge.md)
- [基础工具类概述](./overview.md)
