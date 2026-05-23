---
title: Logger 单实例链接架构
description: 通过 INTERFACE 头文件库和 --whole-archive 保证 cflogger 单例在 DLL 边界内唯一
---

# Logger 单实例链接架构 -- 设计意图

## 为什么选择这种方案

CFDesktop 采用"多静态库合并为单一共享库"架构：所有静态库通过 `--whole-archive` 打包进 `CFDesktop_shared.dll`，EXE 只链接这个 DLL。`cflogger` 使用单例模式，如果同一份 `.cpp` 被编译进多个模块，会产生多个独立实例，导致日志输出丢失和全局状态不一致。

核心问题在于：DLL 内部的模块需要调用日志 API（如 `CFLOG_INFO()`），但不应各自链接 `cflogger` 静态库实体。解决方案是引入 `cflogger_headers` 这个 CMake `INTERFACE` 库——它没有编译产物，仅传递 include 目录和编译定义。DLL 内部模块链接 `cflogger_headers` 只拿头文件，唯一实际链接 `cflogger` 静态库的地方是 `CFDesktop_shared`（通过 `--whole-archive`）。这保证了 `Logger` 实现只编译一次，单例在整个进程中唯一。

`--whole-archive` 在此架构中不可省略：它强制链接器保留 `cflogger` 的所有符号（包括未被直接引用的全局对象构造函数），否则链接器会丢弃"未使用"的 Logger 实现。`CFLOG_STATIC_BUILD` 宏作为补充手段，消除 DLL 内部 `.obj` 文件通过 `__declspec(dllimport)` 引用本地符号时产生的 LNK4217 警告。

## 关键决策

| 决策 | 理由 | 被否决的替代方案 |
|------|------|------------------|
| `cflogger_headers` INTERFACE 库隔离头文件与链接实体 | `.cpp` 只编译一次，`.h` 可以到处传，零运行时开销 | OBJECT library：需要所有使用方从同一 target 获取 `.o`，管理复杂度高 |
| `--whole-archive` 打包所有静态库进单一 DLL | 保证单例唯一且所有符号可被 EXE 通过 DLL 导入表访问 | 去掉 DLL 全静态链接：改动量大，不符合项目共享库架构 |
| `CFLOG_STATIC_BUILD` PRIVATE 编译定义 | DLL 内模块用本地引用，EXE 用 `dllimport`，各取所需 | PUBLIC 编译定义：会传播给 EXE 导致链接失败 |
| `CFLOG_API` 标记公开类，内部类不标记 | `Logger`/全局日志函数通过 DLL 导出，`FileSink`/`ConsoleSink` 等内部类仅供 `CFDesktopMain` 直接从静态库解析 | 所有类都标记导出：暴露实现细节，增加 ABI 维护负担 |
| `CFDesktopMain` PRIVATE 链接 `cflogger`（非 headers） | 直接使用 `FileSink`/`ConsoleSink` 等未导出的内部类，必须从静态库解析符号 | 改用 headers：链接时 undefined symbol |

## 当前状态

已实现并验证。`cflogger_headers` 定义于 `desktop/base/logger/CMakeLists.txt`，所有 DLL 内部模块已迁移至链接 headers。该模式可作为事件系统、配置系统等其他核心单例模块的参考模板。
