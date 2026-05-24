---
title: "Phase 8: 测试体系详细设计文档"
description: "建立单元测试、集成测试、UI 自动化测试和跨平台 CI 的完整测试体系"
---

# Phase 8: 测试体系 -- 设计意图

## 为什么选择这种方案

CFDesktop 的测试策略遵循经典测试金字塔：70% 单元测试 + 20% 集成测试 + 10% UI 测试。选择 GoogleTest 作为主框架（项目已在 CLAUDE.md 中明确），配合 Qt6::Test 处理信号/控件相关的测试。这个组合既能测纯逻辑（CPU 检测、主题引擎、动画管理），也能测 Qt 信号槽和 Widget 行为。

测试目录按 `test/<module>/<component>/<component>_test.cpp` 组织，与源码结构镜像，便于定位。硬件相关测试通过 Mock 系统注入伪造的 /proc/cpuinfo、/proc/meminfo 等文件，避免测试依赖真实硬件。CI 使用 GitHub Actions，单元测试和集成测试在容器中运行，UI 测试通过 Xvfb 虚拟显示执行，性能测试单独 job 跑 Release 构建。

## 关键决策

| 决策 | 理由 | 被否决的替代方案 |
|------|------|------------------|
| GoogleTest + Qt6::Test 双框架 | GoogleTest 适合纯逻辑单元测试，Qt::Test 提供 QSignalSpy 等 Qt 专项支持 | 纯 GoogleTest（Qt 信号测试麻烦）或纯 QtTest（缺少参数化等高级特性） |
| Mock 文件注入（伪造 /proc、/sys） | 硬件探测逻辑依赖内核文件系统，Mock 让测试在任何机器上可复现 | 每次测试都要求特定硬件环境 |
| 测试金字塔 70/20/10 | 单元测试快速稳定覆盖大部分逻辑，集成/UI 测试聚焦关键路径 | 均衡分配或只做单元测试 |
| CI 分阶段 (unit -> integration -> UI -> performance) | 单元测试快速失败尽早反馈，UI/性能测试依赖前序通过再执行 | 所有测试一次性全跑 |

## 当前状态

部分实现（约 55%）。base 层核心模块已有单元测试覆盖，CI 已配置 GitHub Actions。参见 [status/current.md](../status/current.md) Phase 8 部分。
