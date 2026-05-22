# 开发指南

本节面向重新启动开发时的日常入口。

## 推荐阅读顺序

1. [当前项目状态](../status/current.md)
2. [前置环境](01_prerequisites.md)
3. [快速开始](02_quick_start.md)
4. [构建系统](03_build_system.md)
5. [开发工具](04_development_tools.md)
6. [Git Hooks](06_git_hooks.md)
7. [AI 辅助开发指南](ai-assistant-guide.md)

## 常用命令

```bash
# Linux 快速构建
bash scripts/build_helpers/linux_fast_develop_build.sh

# Linux 完整构建 + 测试
bash scripts/build_helpers/linux_develop_build.sh

# 仅运行已有 CTest
QT_QPA_PLATFORM=offscreen ctest --test-dir out/build_develop/test --output-on-failure

# 文档开发
pnpm install
pnpm dev
pnpm build
```
