---
title: Windows 内存检测
description: 本章节详细描述 Windows 平台下的内存检测实现，包括物理内存总量、可用内存、虚拟内存以及进程级
---

# Windows 内存检测

本章节详细描述 Windows 平台下的内存检测实现，包括物理内存总量、可用内存、虚拟内存以及进程级内存使用情况的采集方式。实现基于 `GlobalMemoryStatusEx` 等 Windows API 接口。

---

*Last updated: 2026-03-20*
