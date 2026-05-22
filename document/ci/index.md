---
title: CI/CD
description: CFDesktop 当前使用 GitHub Actions 分层验证。
---

# CI/CD

CFDesktop 当前使用 GitHub Actions 分层验证。

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
