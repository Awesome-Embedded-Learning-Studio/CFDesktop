# CI 工具链设置指南

## 目标

创建专门面向 CI 构建的精简工具链文件，利用现有的 `check_toolchain.cmake` 机制，实现 Docker 容器内的原生编译。

## 设计原则

1. **CI 专用**: 工具链文件专门为 CI 环境设计，不适用于本地开发
2. **精简高效**: 仅使用 GCC，去掉 Clang 支持以节约构建时间
3. **多架构兼容**: 采用分离的架构特定工具链（AMD64 和 ARM64）
4. **零修改**: 利用现有机制，不需要修改任何脚本

## 为什么不需要修改脚本？

现有的 `check_toolchain.cmake` 已经能够：

1. 解析 `platform/toolchain` 格式
2. 自动查找对应的工具链文件
3. 自动设置 `CMAKE_TOOLCHAIN_FILE`

只需要在正确位置创建工具链文件即可。

## 目录结构

```
cmake/
└── cmake_toolchain/
    ├── windows/                        # 已存在
    │   ├── llvm-toolchain.cmake
    │   └── gcc-toolchain.cmake
    └── linux/                          # 扩展
        ├── ci-x86_64-toolchain.cmake   # AMD64 CI 工具链
        └── ci-aarch64-toolchain.cmake  # ARM64 CI 工具链
```

## 工具链文件说明

### ci-x86_64-toolchain.cmake

**文件路径**: `cmake/cmake_toolchain/linux/ci-x86_64-toolchain.cmake`

用于 AMD64/x86_64 架构的 CI 构建，Qt 安装路径为 `/opt/Qt/6.8.1/gcc_64`。

**使用方式**:
```bash
cmake -DUSE_TOOLCHAIN=linux/ci-x86_64 -S . -B build
```

### ci-aarch64-toolchain.cmake

**文件路径**: `cmake/cmake_toolchain/linux/ci-aarch64-toolchain.cmake`

用于 ARM64/aarch64 架构的 CI 构建，Qt 安装路径为 `/opt/Qt/6.8.1/gcc_arm64`。

**使用方式**:
```bash
cmake -DUSE_TOOLCHAIN=linux/ci-aarch64 -S . -B build
```

## 工具链文件内容

两个工具链文件的结构相同，主要区别在于 Qt 路径：

```cmake
# =============================================================================
# CI Build Toolchain Configuration
# =============================================================================
# 专用工具链文件，用于 Docker 容器内的 CI 构建

set(CMAKE_SYSTEM_NAME Linux)

# -----------------------------------------------------------------------------
# Qt6 搜索路径（容器环境）
# Docker 容器通过 aqtinstall 安装 Qt 到 /opt/Qt/6.8.1/
# -----------------------------------------------------------------------------

# AMD64: /opt/Qt/6.8.1/gcc_64
# ARM64: /opt/Qt/6.8.1/gcc_arm64
set(QT6_BASE_DIR "/opt/Qt/6.8.1/<arch>")
set(QT6_CMAKE_DIR "${QT6_BASE_DIR}/lib/cmake/Qt6")

# 允许环境变量覆盖（用于调试）
if(DEFINED ENV{Qt6_DIR})
    set(QT6_CMAKE_DIR "$ENV{Qt6_DIR}")
elseif(DEFINED ENV{QT6_DIR})
    set(QT6_BASE_DIR "$ENV{QT6_DIR}")
    set(QT6_CMAKE_DIR "${QT6_BASE_DIR}/lib/cmake/Qt6")
endif()

# 设置 Qt6_DIR 和 CMAKE_PREFIX_PATH
set(Qt6_DIR "${QT6_CMAKE_DIR}" CACHE PATH "Qt6 installation directory")
set(CMAKE_PREFIX_PATH "${QT6_BASE_DIR}")

# -----------------------------------------------------------------------------
# 编译器配置
# 使用系统默认的 GCC，CMake 会自动找到 gcc/g++
# -----------------------------------------------------------------------------

# 启用 ccache 加速构建（容器内已安装）
find_program(CCACHE_PROGRAM ccache CACHE)
if(CCACHE_PROGRAM)
    set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}" CACHE FILEPATH "C compiler launcher")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}" CACHE FILEPATH "CXX compiler launcher")
endif()
```

## ccache 支持

ccache 是一个编译缓存工具，可以显著加速重复构建：

- 首次编译: 正常速度
- 后续编译: 10-100 倍加速

在容器环境中，ccache 配置建议：

```bash
# 在 Dockerfile 中配置
ccache --max-size=5G --set-config=compiler_check=%compiler%2S -s
```

## 多架构支持

采用分离的架构特定工具链方案：

```
┌─────────────────────────────────────────────────────────┐
│                    Docker 容器                          │
├──────────────────────────┬──────────────────────────────┤
│     AMD64 容器           │      ARM64 容器              │
│                          │                              │
│  工具链: linux/ci-x86_64 │  工具链: linux/ci-aarch64   │
│  Qt路径: gcc_64          │  Qt路径: gcc_arm64           │
│  编译器: x86_64-gcc      │  编译器: aarch64-gcc         │
└──────────────────────────┴──────────────────────────────┘
```

CI 构建脚本会自动检测容器架构并选择对应的工具链。

## 验证方法

### 本地验证（如果有 Linux 环境）

```bash
# 1. 配置项目 (AMD64)
cmake -DUSE_TOOLCHAIN=linux/ci-x86_64 -S . -B out/build_ci

# 2. 构建项目
cmake --build out/build_ci --parallel $(nproc)

# 3. 验证产物架构
file out/build_ci/bin/*

# AMD64 预期输出:
# ELF 64-bit LSB executable, x86-64, ...
```

### Docker 容器验证

使用 `docker_start.sh` 脚本：

```bash
# 测试 AMD64
bash scripts/build_helpers/docker_start.sh --arch amd64 --mode build

# 测试 ARM64
bash scripts/build_helpers/docker_start.sh --arch arm64 --mode build
```

## 预期结果

| 检查项 | 预期结果 |
|--------|----------|
| CMake 能解析 `linux/ci-x86_64` | ✓ 显示工具链文件路径 |
| CMake 能解析 `linux/ci-aarch64` | ✓ 显示工具链文件路径 |
| 找到 Qt6 | ✓ CMAKE_PREFIX_PATH 正确 |
| ccache 启用 | ✓ 编译器 launcher 设置为 ccache |
| AMD64 构建 | ✓ 生成 x86-64 可执行文件 |
| ARM64 构建 | ✓ 生成 aarch64 可执行文件 |

## 常见问题

### Q: CMake 找不到工具链文件

**原因**: 文件名或路径不正确

**解决方案**:
```bash
# 确认文件存在
ls cmake/cmake_toolchain/linux/ci-x86_64-toolchain.cmake
ls cmake/cmake_toolchain/linux/ci-aarch64-toolchain.cmake

# 确认使用正确的简写格式
cmake -DUSE_TOOLCHAIN=linux/ci-x86_64 -S . -B build
cmake -DUSE_TOOLCHAIN=linux/ci-aarch64 -S . -B build
```

### Q: Qt6 not found

**原因**: Qt6 路径不正确或 Qt6 未安装

**解决方案**:
```bash
# 检查 Qt6 安装位置
find /opt/Qt -name "Qt6Config.cmake" 2>/dev/null

# 在容器内安装 Qt6（使用 aqtinstall）
python3 -m aqt install-qt --outputdir /opt/Qt 6.8.1 linux desktop gcc_64
```

### Q: ccache 未启用

**原因**: 容器内未安装 ccache

**解决方案**:
```bash
# 在 Dockerfile 中添加
RUN apt-get install -y ccache
```

## 与现有系统集成

### 工具链选择流程

```
INI 配置: toolchain=linux/ci-x86_64 (或 ci-aarch64)
    ↓
构建脚本: ci_build_entry.sh 自动检测架构
    ↓
CMake 参数: -DUSE_TOOLCHAIN=linux/ci-<arch>
    ↓
check_toolchain.cmake 解析
    ↓
查找对应的工具链文件
    ↓
设置: CMAKE_TOOLCHAIN_FILE
```

### 配置文件示例

```ini
# scripts/build_helpers/build_ci_config.ini (AMD64)
[cmake]
generator=Ninja
toolchain=linux/ci-x86_64
build_type=Release
```

```ini
# scripts/build_helpers/build_ci_aarch64_config.ini (ARM64)
[cmake]
generator=Ninja
toolchain=linux/ci-aarch64
build_type=Release
```

## 后续步骤

Phase 1 完成后，继续实施：

- [Phase 2: Docker 构建环境](docker-environment.md)
- [Phase 3: CI 构建入口](ci-build-entry.md)

---

*文档版本: v2.0 | 最后更新: 2026-03-07*
