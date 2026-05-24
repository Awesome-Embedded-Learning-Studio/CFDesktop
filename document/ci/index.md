---
title: CI/CD
description: CFDesktop 当前使用 GitHub Actions 分层验证，并支持多架构 Docker 容器构建。
---

# CI/CD

CFDesktop 采用多架构 Docker 容器构建方案，通过 `check_toolchain.cmake` 实现工具链选择，确保代码在 AMD64 / ARM64 / ARM32 平台上正确编译和运行。

## CI 架构

### 工具链与平台

项目为每种目标架构提供独立的工具链文件和构建配置：

| 架构 | 工具链文件 | Qt 路径 |
|------|-----------|---------|
| AMD64/x86_64 | `ci-x86_64-toolchain.cmake` | `/opt/Qt/6.8.1/gcc_64` |
| ARM64/aarch64 | `ci-aarch64-toolchain.cmake` | `/opt/Qt/6.8.1/gcc_arm64` |
| ARM32/armhf | `ci-armhf-toolchain.cmake` | `/opt/Qt/6.8.1/gcc_armhf` |

多架构容器方案使用 Docker `--platform` 参数实现原生编译，无需交叉编译工具链，可在对应架构容器中直接运行测试。

### Docker 快速验证

```bash
# AMD64 构建
bash scripts/build_helpers/docker_start.sh --verify

# ARM64 构建
bash scripts/build_helpers/docker_start.sh --arch arm64 --verify

# 直接运行 CI 构建
bash scripts/build_helpers/ci_build_entry.sh ci
```

## 实施阶段

| 阶段 | 名称 | 说明 | 状态 |
|------|------|------|------|
| Phase 1 | [CI 工具链设置](toolchain-setup.md) | 创建 CI 专用工具链文件 | ✅ 已完成 |
| Phase 2 | [Docker 构建环境](docker-environment.md) | 创建多架构 Dockerfile | ✅ 已完成 |
| Phase 3 | [CI 构建入口](ci-build-entry.md) | 创建统一构建脚本 | ✅ 已完成 |
| Phase 4 | GitHub Actions | 配置自动化工作流 | ⏭️ 跳过 |
| Phase 5 | 异步合并机制 | 实现 pre-push hook | ⏳ 待实施 |

## 工作流

| Workflow | 触发 | 作用 |
|----------|------|------|
| `docs-check.yml` | PR 到 `develop` 且带 `build-doc` 标签 | 运行 VitePress 文档构建 |
| `cpp-check.yml` | PR 到 `main` | 运行 Linux C++ 构建、CTest 和文档构建 |
| `deploy.yml` | push 到 `main` | 构建并发布 VitePress 到 GitHub Pages |

## 分支策略

- `feature/* -> develop`: 默认轻量，不强制全量构建。
- 文档相关 PR: 打 `build-doc` 标签触发文档构建。
- `develop -> main`: 必须通过 C++ build/test 和 docs build。
- `main`: 合入后发布文档站。

---

*最后更新: 2026-05-23*
