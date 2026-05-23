---
title: CFDesktop 前期阶段设计文档总览
description: 本目录包含 CFDesktop 项目前期阶段的详细设计文档，覆盖 Phase 0 至 Phase 8 的完整规划。
---

# CFDesktop 前期阶段设计文档总览

本目录包含 CFDesktop 项目前期阶段的详细设计文档，覆盖 Phase 0 至 Phase 8 的完整规划。内容包括系统整体架构设计、多显示后端方案、硬件探测层设计、基础库架构以及输入抽象层设计等核心模块的技术方案。

## 文档列表

| 文档 | 阶段 | 内容概要 |
|------|------|----------|
| [system_architecture_overview.md](system_architecture_overview.md) | 总览 | 系统架构总览 — 模块层次、核心接口、平台抽象层、显示后端、UI 分层、初始化流程 |
| [multi_display_backend_architecture.md](multi_display_backend_architecture.md) | 架构 | 多显示后端架构 — 运行时后端选择、组件复用矩阵、各后端实现指导 |
| [00_phase0_project_skeleton.md](00_phase0_project_skeleton.md) | Phase 0 | 工程骨架搭建 - CMake 构建系统、目录结构、CI/CD 配置 |
| [01_phase1_hardware_probe.md](01_phase1_hardware_probe.md) | Phase 1 | 硬件探针与能力分级 - CPU/GPU/内存检测、HWTier 档位判定 |
| [02_phase2_base_library.md](02_phase2_base_library.md) | Phase 2 | Base 库核心 - 主题引擎、动画管理、DPI 适配、配置中心、日志系统 |
| [03_phase3_input_layer.md](03_phase3_input_layer.md) | Phase 3 | 输入抽象层 - 触摸/按键/旋钮处理、手势识别、焦点导航 |
| [04_phase6_simulator.md](04_phase6_simulator.md) | Phase 6 | 多平台模拟器 - PC 端模拟器、设备外壳、触摸可视化、参数注入 |
| [05_phase8_testing.md](05_phase8_testing.md) | Phase 8 | 测试体系 - 单元测试、集成测试、UI 测试、CI/CD |

---

## 按模块查找

| 模块 | 文档 |
|------|------|
| 系统架构 | [系统架构总览](system_architecture_overview.md) |
| 多显示后端 | [多显示后端架构](multi_display_backend_architecture.md) |
| 目录结构 | [Phase 0](00_phase0_project_skeleton.md) |
| 构建系统 | [Phase 0](00_phase0_project_skeleton.md) |
| 硬件检测 | [Phase 1](01_phase1_hardware_probe.md) |
| 主题引擎 | [Phase 2](02_phase2_base_library.md) |
| 动画管理 | [Phase 2](02_phase2_base_library.md) |
| DPI 适配 | [Phase 2](02_phase2_base_library.md) |
| 配置系统 | [Phase 2](02_phase2_base_library.md) |
| 日志系统 | [Phase 2](02_phase2_base_library.md) |
| 输入处理 | [Phase 3](03_phase3_input_layer.md) |
| 模拟器 | [Phase 6](04_phase6_simulator.md) |
| 测试 | [Phase 8](05_phase8_testing.md) |

---

## 阅读顺序

1. 先阅读系统架构总览了解整体设计
2. 从 Phase 0 开始了解工程结构
3. 根据开发任务阅读对应 Phase 文档

---

*Last updated: 2026-03-20*
