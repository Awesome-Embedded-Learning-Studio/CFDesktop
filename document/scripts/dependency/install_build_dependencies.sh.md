---
title: installbuilddependencies.sh
description: "文档编写日期: 2026-03-20，本脚本用于安装CFDesktop项目的构建依赖，主要包含："
---

# install_build_dependencies.sh

> 文档编写日期: 2026-03-20

## 使用办法 (Usage)

### 基本语法
```bash
./scripts/dependency/install_build_dependencies.sh
```bash

### 环境变量
| 环境变量 | 默认值 | 说明 |
|---------|--------|------|
| QT_VERSION | 6.8.1 | Qt版本 |
| QT_MIRROR | https://mirrors.aliyun.com/qt | Qt下载镜像 |
| QT_ARCH | 自动检测 | Qt架构 (linux_gcc_64/linux_gcc_arm64) |
| QT_MIRROR_APT | 自动选择 | APT镜像源 |
| QT6_DIR | /opt/Qt/$QT_VERSION/$ARCH_SUBDIR | Qt安装目录 |
| QT_BASE_DIR | /opt/Qt | Qt基础目录 |

### 使用示例
```bash
# 默认安装
./scripts/dependency/install_build_dependencies.sh

# 指定Qt版本
QT_VERSION=6.7.0 ./scripts/dependency/install_build_dependencies.sh

# 指定镜像
QT_MIRROR=https://mirrors.tuna.tsinghua.edu.cn/qt ./scripts/dependency/install_build_dependencies.sh

# 指定APT镜像
QT_MIRROR_APT=https://mirrors.ustc.edu.cn/ubuntu ./scripts/dependency/install_build_dependencies.sh
```bash

## Scripts详解 (Detailed Explanation)

### 脚本用途
本脚本用于安装CFDesktop项目的构建依赖，主要包含：
- Qt 6.8.1 (通过aqtinstall安装)
- 构建工具链 (GCC, CMake, Ninja, ccache)
- Qt运行时依赖库 (OpenGL, XCB, Wayland等)

专为Docker容器环境优化，支持自动架构检测和镜像源配置。

### 支持的架构
| 系统架构 | Qt架构标识 | aqtinstall host |
|----------|-----------|-----------------|
| x86_64/amd64 | linux_gcc_64 | linux |
| aarch64/arm64 | linux_gcc_arm64 | linux_arm64 |

### 依赖项
- **操作系统**: Ubuntu 24.04 或兼容系统
- **权限**: root 权限 (用于apt安装)
- **网络**: 互联网连接 (下载Qt和依赖包)
- **工具**: pip3 (用于安装aqtinstall)

### 安装步骤

#### Step 1: 配置APT镜像源
- 自动检测系统架构选择合适镜像
- 支持HTTP bootstrap (当ca-certificates未安装时)
- 镜像列表自动回退机制

#### Step 2: 安装系统依赖
包含以下包组：
- **编译工具链**: build-essential, cmake, ninja-build, ccache
- **Python支持**: python3, python3-pip
- **OpenGL/EGL**: libgl1-mesa-dev, libglu1-mesa-dev, libegl1-mesa-dev
- **XCB核心**: libxcb1-dev, libx11-dev 及相关扩展
- **字体渲染**: libfontconfig1-dev, libfreetype-dev, libharfbuzz-dev
- **系统库**: libglib2.0-dev, libdbus-1-dev, libssl-dev 等

#### Step 3: 安装aqtinstall
- 使用pip3安装aqtinstall (Python Qt安装工具)
- 配置aqtinstall日志输出到/dev/null

#### Step 4: 安装Qt 6.8.1
- 基础Qt模块
- 扩展模块: qt5compat, qtserialport, qtimageformats
- 支持镜像回退机制

#### Step 5: 配置ccache
- 最大缓存: 5GB
- 启用压缩 (级别1)
- 内容检查模式

### 关键函数

| 函数 | 说明 |
|------|------|
| _detect_qt_arch() | 自动检测系统架构，返回Qt架构标识 |
| _is_valid_qt_arch() | 验证Qt架构是否在支持列表中 |
| _write_http_mirror() | 写入HTTP镜像配置 (用于无CA证书环境) |
| _try_qt_install() | 尝试从指定镜像安装Qt，支持超时控制 |
| main() | 主函数，协调整个安装流程 |

### 环境变量输出
安装完成后，以下环境变量将设置并写入 `/root/.bashrc`:
```bash
export Qt6_DIR=/opt/Qt/6.8.1/<arch>/lib/cmake/Qt6
export PATH=$Qt6_DIR/bin:$PATH
export LD_LIBRARY_PATH=$Qt6_DIR/lib:$LD_LIBRARY_PATH
```text

### 安装目录结构
```text
/opt/Qt/
└── 6.8.1/
    ├── gcc_64/              # x86_64架构
    │   ├── bin/
    │   ├── lib/
    │   ├── lib/cmake/Qt6/
    │   └── ...
    └── linux_gcc_arm64/     # aarch64架构
        ├── bin/
        ├── lib/
        └── ...
```text

### 注意事项
- 需要 **root 权限** 执行
- 安装时间约 **5-15分钟** (取决于网络速度)
- 需要约 **5GB** 磁盘空间
- 脚本使用 `set -euo pipefail` 严格模式
- 自动处理架构不匹配问题
- Docker环境优化 (移除不必要的venv创建)
