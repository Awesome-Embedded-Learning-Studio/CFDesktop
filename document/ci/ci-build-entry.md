# CI 构建入口设置指南

**状态**: ✅ 已完成
**完成日期**: 2026-03-07

## 概述

本部分创建了统一的 CI 构建入口脚本，作为 Docker 容器的构建入口点。

## 设计目标

1. **统一接口**: Docker 和本地测试使用相同的入口脚本
2. **独立配置**: CI 使用独立的配置文件，不干扰本地开发
3. **环境适配**: 自动检测架构并选择对应的工具链
4. **简洁高效**: 调用现有脚本，避免重复代码

## 文件结构

```
scripts/build_helpers/
├── ci_build_entry.sh              # CI 构建入口脚本
├── build_ci_config.ini            # AMD64 CI 配置
├── build_ci_aarch64_config.ini    # ARM64 CI 配置
└── build_ci_armhf_config.ini      # ARM32 CI 配置
```

## CI 构建配置

### build_ci_config.ini (AMD64)

**文件**: `scripts/build_helpers/build_ci_config.ini`

AMD64/x86_64 架构的 CI 构建配置：

```ini
[cmake]
generator=Unix Makefiles
toolchain=linux/ci-x86_64          # AMD64 工具链
build_type=Release

[paths]
source=.
build_dir=out/build_ci             # CI 专用构建目录

[options]
jobs=16                            # 并行编译任务数
```

### build_ci_aarch64_config.ini (ARM64)

**文件**: `scripts/build_helpers/build_ci_aarch64_config.ini`

ARM64/aarch64 架构的 CI 构建配置：

```ini
[cmake]
generator=Unix Makefiles
toolchain=linux/ci-aarch64         # ARM64 工具链
build_type=Release

[paths]
source=.
build_dir=out/build_ci_aarch64     # ARM64 专用构建目录

[options]
jobs=16
```

### build_ci_armhf_config.ini (ARM32)

**文件**: `scripts/build_helpers/build_ci_armhf_config.ini`

ARM32/ARMHF 架构的 CI 构建配置（用于 IMX6ULL）：

```ini
[cmake]
generator=Unix Makefiles
toolchain=linux/ci-armhf          # ARM32 工具链
build_type=Release

[paths]
source=.
build_dir=out/build_ci_armhf      # ARM32 专用构建目录

[options]
jobs=8
```

## CI 构建入口脚本

**文件**: `scripts/build_helpers/ci_build_entry.sh`

### 功能特性

- 自动检测容器架构（x86_64/aarch64/armhf）
- 根据架构选择对应的配置文件
- 支持两种模式：完整 CI 构建（ci）和仅测试（test）
- 彩色日志输出（INFO、SUCCESS、WARNING、ERROR）
- 完善的错误处理和退出码管理

### 使用方式

```bash
# 完整 CI 构建（配置 + 编译 + 测试）
bash scripts/build_helpers/ci_build_entry.sh ci

# 仅运行测试
bash scripts/build_helpers/ci_build_entry.sh test
```

### 架构检测流程

```
ci_build_entry.sh
    │
    ├── 检测架构 (uname -m)
    │     │
    │     ├── x86_64 / amd64
    │     │     └──> build_ci_config.ini (linux/ci-x86_64)
    │     │
    │     ├── aarch64
    │     │     └──> build_ci_aarch64_config.ini (linux/ci-aarch64)
    │     │
    │     └── armv7l / armhf
    │           └──> build_ci_armhf_config.ini (linux/ci-armhf)
    │
    └── 执行构建
          └──> linux_develop_build.sh ci -c <config>
```

## 日志系统

脚本提供彩色日志输出：

| 级别 | 颜色 | 说明 |
|------|------|------|
| INFO | 青色 | 一般信息 |
| SUCCESS | 绿色 | 成功消息 |
| WARNING | 黄色 | 警告消息 |
| ERROR | 红色 | 错误消息 |

**示例输出**:

```
[2026-03-07 10:30:00] [INFO] ========================================
[2026-03-07 10:30:00] [INFO] CI Build Entry Point
[2026-03-07 10:30:00] [INFO] Mode: ci
[2026-03-07 10:30:00] [INFO] Architecture: x86_64
[2026-03-07 10:30:00] [INFO] Config: build_ci_config.ini
[2026-03-07 10:30:00] [INFO] ========================================
[2026-03-07 10:30:01] [INFO] Starting CI build process...
...
[2026-03-07 10:35:00] [SUCCESS] CI build completed successfully!
[2026-03-07 10:35:00] [INFO] ========================================
```

## 集成方式

### Docker 集成

由 `docker_start.sh` 调用：

```bash
# AMD64
docker run --rm --platform linux/amd64 \
    -v $(pwd):/project \
    cfdesktop-build \
    bash scripts/build_helpers/ci_build_entry.sh ci

# ARM64
docker run --rm --platform linux/arm64 \
    -v $(pwd):/project \
    cfdesktop-build:arm64 \
    bash scripts/build_helpers/ci_build_entry.sh ci
```

### 直接运行

```bash
# 在 Linux 环境中直接运行
bash scripts/build_helpers/ci_build_entry.sh ci
```

## 错误处理

脚本使用 `set -e` 确保任何命令失败都会中断执行，并通过退出码正确传递错误状态：

```bash
if bash "$SCRIPT_DIR/linux_develop_build.sh" ci -c "$CONFIG_FILE"; then
    log "CI build completed successfully!" "SUCCESS"
    exit 0
else
    exit_code=$?
    log "CI build failed with exit code: $exit_code" "ERROR"
    exit $exit_code
fi
```

## 验证方法

### 1. Docker 验证

```bash
# AMD64
bash scripts/build_helpers/docker_start.sh --verify

# ARM64
bash scripts/build_helpers/docker_start.sh --arch arm64 --verify
```

### 2. 验证配置

```bash
# 检查配置文件
cat scripts/build_helpers/build_ci_config.ini

# 预期输出应包含：
# [cmake]
# generator=Unix Makefiles
# toolchain=linux/ci-x86_64
# build_type=Release
```

### 3. 验证构建目录

```bash
# CI 构建应输出到独立目录
ls -la out/build_ci/

# 与开发构建分离
ls -la out/build_develop/
```

## 技术细节

### 架构检测

```bash
ARCH=$(uname -m)
case "$ARCH" in
    x86_64|amd64)
        CONFIG_FILE="build_ci_config.ini"
        ;;
    aarch64)
        CONFIG_FILE="build_ci_aarch64_config.ini"
        ;;
    armv7l|armhf)
        CONFIG_FILE="build_ci_armhf_config.ini"
        ;;
    *)
        log "Unsupported architecture: $ARCH" "ERROR"
        exit 1
        ;;
esac
```

### 脚本位置计算

```bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
```

这确保脚本可以从任何位置正确调用，并找到相关文件。

## 与现有脚本的集成

CI 入口脚本调用现有的脚本：

| 现有脚本 | 用途 |
|----------|------|
| `linux_develop_build.sh` | CI 模式下的配置和构建 |
| `linux_run_tests.sh` | 测试执行 |

这种设计确保：
- **单一职责**: CI 入口只负责流程控制
- **代码复用**: 利用现有脚本
- **维护性**: 构建逻辑修改只需改一处

## 常见问题

### Q: 如何选择正确的配置文件？

**A**: 脚本会自动检测架构并选择对应的配置文件，无需手动指定。

### Q: 如何为不同架构构建？

**A**: 使用不同的 Docker 平台：
```bash
# AMD64
docker run --rm --platform linux/amd64 ...

# ARM64
docker run --rm --platform linux/arm64 ...
```

### Q: 构建产物在哪里？

**A**: 根据配置文件中的 `build_dir` 设置：
- AMD64: `out/build_ci/`
- ARM64: `out/build_ci_aarch64/`
- ARM32: `out/build_ci_armhf/`

## 后续步骤

Phase 3 已完成，相关 Phase：

- [Phase 4]: GitHub Actions 配置 - **跳过**（用户决定不在远端构建）
- [Phase 5]: 异步合并机制 - **待实施**

## 相关文件

| 文件 | 说明 |
|------|------|
| `ci_build_entry.sh` | CI 构建入口脚本 |
| `build_ci_config.ini` | AMD64 CI 配置 |
| `build_ci_aarch64_config.ini` | ARM64 CI 配置 |
| `build_ci_armhf_config.ini` | ARM32 CI 配置 |

---

*文档版本: v2.0 | 最后更新: 2026-03-07*
