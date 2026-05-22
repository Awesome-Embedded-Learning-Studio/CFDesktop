---
title: 文档脚本
description: CFDesktop 文档系统已从 MkDocs 迁移到 VitePress。
---

# 文档脚本

CFDesktop 文档系统已从 MkDocs 迁移到 VitePress。

## 当前入口

```bash
pnpm install
pnpm dev
pnpm build
pnpm preview
```bash

## 配置文件

| 文件 | 说明 |
|------|------|
| `package.json` | pnpm 脚本和 VitePress 依赖 |
| `pnpm-lock.yaml` | 文档站依赖锁文件 |
| `project.config.ts` | 项目文档配置，参考 imx-forge 的组织方式 |
| `site/.vitepress/config.mts` | VitePress 主配置 |
| `.github/workflows/deploy.yml` | GitHub Pages 发布 |

## API 文档

Doxygen 相关配置暂时保留，但自动生成的 `document/api/**` 不进入主站导航。后续二期再决定是否以 Doxygen HTML 或修复后的 Markdown 形式发布 API 参考。
