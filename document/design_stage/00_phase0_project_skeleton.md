---
title: "Phase 0: 工程骨架搭建详细设计文档"
description: "项目骨架：三层目录结构、CMake 构建系统、CI/CD 流水线的设计意图"
---

# Phase 0: 工程骨架搭建 -- 设计意图

## 为什么选择这种方案

CFDesktop 需要同时支持 x86_64 开发机和 ARM 嵌入式板卡（IMX6ULL/RK3568/RK3588），因此构建系统必须在第一天就具备交叉编译能力。我们选择 CMake 作为构建基础，搭配 Ninja 后端和 CMakePresets.json，让开发者通过一条 preset 命令即可切换目标平台，避免手动维护多套 Makefile。

目录结构采用三层单向依赖（`desktop/ -> ui/ -> base/ -> Qt/OS API`），这是整个项目最重要的架构约束。每一层只允许依赖下层，禁止反向引用。这种严格分层确保了 `base/` 可以独立测试和发布，`ui/` 可以在不同桌面环境中复用，而 `desktop/` 作为最终集成层可以自由组合下层能力。

CI/CD 采用 GitHub Actions 矩阵构建，在 Linux x64、ARMv7、ARM64 三个目标上并行编译，并集成 clang-tidy 静态分析和 clang-format 格式检查。所有代码质量门禁通过 Git pre-commit hook 在本地前置拦截，减少 CI 反馈周期。

## 关键决策

| 决策 | 理由 | 被否决的替代方案 |
|------|------|------------------|
| CMake + CMakePresets.json | 跨平台成熟、交叉编译原生支持、IDE 集成广泛 | Meson（生态不够成熟）、QMake（Qt6 已弃用） |
| 三层单向依赖 (base/ui/desktop) | 强制解耦、每层可独立测试、支持嵌入式裁剪 | 扁平结构（耦合严重）、四层以上（过度工程化） |
| 共享库输出 (DLL/SO) | 支持运行时替换模块、减少嵌入式 Flash 占用 | 静态库全链接（编译慢、无法按需裁剪） |
| GitHub Actions 矩阵构建 | 免费额度充足、ARM 交叉编译容器支持好 | Jenkins（维护成本高）、GitLab CI（需自建实例） |
| Git pre-commit hook | 质量门禁前置到本地，减少 CI 失败率 | 仅依赖 CI 检查（反馈周期长） |

## 当前状态

已实现 -- 三层目录、CMake 构建体系、GitHub Actions CI、代码格式化配置均已完成。详见 `document/design_stage/status/`。
