# 01. 前置要求

本文档详细介绍 CFDesktop 开发环境的所有前置要求，包括硬件要求、操作系统支持和必需软件的安装指南。

---

## 目录

- [硬件要求](#硬件要求)
- [操作系统支持](#操作系统支持)
- [必需软件安装](#必需软件安装)
  - [Docker Desktop](#docker-desktop)
  - [Git](#git)
  - [VSCode](#vscode)
  - [Qt6](#qt6)
- [可选软件](#可选软件)
- [验证安装](#验证安装)

---

## 硬件要求

### 最低配置

| 组件 | 要求 | 说明 |
|:---|:---|:---|
| **CPU** | 4 核心 | 支持 x86_64 或 ARM64 架构 |
| **RAM** | 8GB | 较大型项目可能需要更多内存 |
| **硬盘** | 20GB 可用空间 | 包含 Qt6、源码和构建产物 |

### 推荐配置

| 组件 | 要求 | 说明 |
|:---|:---|:---|
| **CPU** | 8 核心以上 | 更快的编译速度 |
| **RAM** | 16GB 或更多 | 同时运行 IDE、Docker 等工具 |
| **硬盘** | 50GB+ SSD | 更快的 I/O 性能 |

### 开发场景资源估算

| 开发场景 | CPU | RAM | 硬盘 |
|:---|:---:|:---:|:---:|
| **轻量开发** (仅修改代码) | 2 核心 | 4GB | 10GB |
| **常规开发** (本地编译) | 4 核心 | 8GB | 20GB |
| **完整开发** (Docker 多架构) | 8 核心以上 | 16GB+ | 50GB+ |

---

## 操作系统支持

### Windows

| 版本 | 支持状态 | 工具链 | 备注 |
|:---|:---:|:---|:---|
| **Windows 10** | 支持 | MinGW, LLVM | 需 64 位版本 |
| **Windows 11** | 支持 | MinGW, LLVM | 完全支持 |

### Linux

| 发行版 | 支持状态 | 工具链 | 备注 |
|:---|:---:|:---|:---|
| **Ubuntu 22.04 LTS** | 完全支持 | GCC, Clang | 主力开发平台 |
| **Ubuntu 24.04 LTS** | 完全支持 | GCC, Clang | 推荐 |
| **Debian 12+** | 支持 | GCC, Clang | 需手动安装部分依赖 |
| **其他发行版** | 部分支持 | GCC, Clang | 需手动适配 |

### 其他平台

| 平台 | 支持状态 | 备注 |
|:---|:---:|:---|
| **macOS** | 社区支持 | 需自行适配构建脚本 |
| **WSL** | 实验性 | 可能遇到 GUI 相关问题 |

---

## 必需软件安装

### Docker Desktop

Docker 用于多架构构建验证，是 CI/CD 流程的重要组成部分。

#### Windows 安装

1. **下载安装包**

   访问 Docker 官方下载页面：
   ```
   https://www.docker.com/products/docker-desktop/
   ```

   或直接下载 Windows 版本：
   - [Docker Desktop for Windows](https://desktop.docker.com/win/main/amd64/Docker%20Desktop%20Installer.exe)

2. **运行安装程序**

   双击下载的 `.exe` 文件，按照安装向导完成安装。

3. **启动 Docker Desktop**

   安装完成后，从开始菜单启动 Docker Desktop。首次启动可能需要重启计算机。

4. **验证安装**

   ```powershell
   docker --version
   docker compose version
   ```

#### Linux 安装

在 Ubuntu 22.04+ 上安装 Docker CE 和 Docker Compose：

```bash
# 1. 更新软件包列表
sudo apt update

# 2. 安装必要的依赖
sudo apt install -y ca-certificates curl gnupg lsb-release

# 3. 添加 Docker 官方 GPG 密钥
sudo mkdir -p /etc/apt/keyrings
curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo gpg --dearmor -o /etc/apt/keyrings/docker.gpg

# 4. 添加 Docker 仓库
echo \
  "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.gpg] https://download.docker.com/linux/ubuntu \
  $(lsb_release -cs) stable" | sudo tee /etc/apt/sources.list.d/docker.list > /dev/null

# 5. 安装 Docker CE
sudo apt update
sudo apt install -y docker-ce docker-ce-cli containerd.io docker-buildx-plugin docker-compose-plugin

# 6. 将当前用户添加到 docker 组（避免使用 sudo）
sudo usermod -aG docker $USER

# 7. 重新登录以使组权限生效

# 8. 验证安装
docker --version
docker compose version
```

#### 配置建议

**Windows 配置**:
- 在 Docker Desktop 设置中，分配至少 4GB 内存给 Docker
- 启用 WSL 2 集成以获得更好性能

**Linux 配置**:
- 配置 Docker 日志大小限制以避免磁盘占满
- 考虑配置镜像加速器（中国大陆用户）

---

### Git

Git 是版本控制工具，用于获取和管理 CFDesktop 源代码。

#### Windows 安装

1. **下载安装包**

   访问 Git 官方网站：
   ```
   https://git-scm.com/download/win
   ```

2. **运行安装程序**

   双击下载的安装程序，建议配置：
   - 默认编辑器：选择您喜欢的编辑器（如 VSCode）
   - PATH 环境：选择 "Git from the command line and also from 3rd-party software"
   - 行尾转换：默认即可

3. **验证安装**

   ```powershell
   git --version
   ```

#### Linux 安装

```bash
# Ubuntu/Debian
sudo apt update
sudo apt install -y git

# 验证安装
git --version
```

#### 配置建议

安装完成后，建议配置您的 Git 信息：

```bash
git config --global user.name "Your Name"
git config --global user.email "your.email@example.com"

# Windows 下配置行尾处理
git config --global core.autocrlf true

# Linux/Mac 下配置行尾处理
git config --global core.autocrlf input
```

---

### VSCode

Visual Studio Code 是推荐的代码编辑器，提供丰富的 C++ 开发插件支持。

#### Windows 安装

1. **下载安装包**

   访问 VSCode 官方网站：
   ```
   https://code.visualstudio.com/download
   ```

   下载 `VSCodeUserSetup-x64.exe`（或 ARM64 版本）

2. **运行安装程序**

   双击安装程序，建议勾选：
   - 通过 Code 打开添加到资源管理器上下文菜单
   - 将 Code 注册为受支持的文件类型的编辑器
   - 添加到 PATH（可从命令行访问）

3. **安装推荐插件**

   启动 VSCode 后，安装以下插件：
   - **C/C++** (ms-vscode.cpptools)
   - **C/C++ Extension Pack** (ms-vscode.cpptools-extension-pack)
   - **Clangd** (llvm-vs-code-extensions.vscode-clangd)
   - **CMake Tools** (ms-vscode.cmake-tools)
   - **Chinese (Simplified)** (MS-CEINTL.vscode-language-pack-zh-hans) - 可选

#### Linux 安装

```bash
# Ubuntu/Debian - 下载 .deb 包
wget https://go.microsoft.com/fwlink/?LinkID=760868
sudo dpkg -i <downloaded-file>.deb
sudo apt install -f  # 修复依赖

# 或使用 snap
sudo snap install --classic code

# 验证安装
code --version
```

#### 配置建议

CFDesktop 项目会自动生成 `.vscode` 配置文件，包括：
- `launch.json` - 调试配置
- `tasks.json` - 构建任务
- `.clangd` - Clangd 语言服务器配置

确保安装了 Clangd 插件以获得最佳开发体验。

---

### Qt6

Qt6 是 CFDesktop 的核心 UI 框架，需要安装 6.8.3 或更高版本。

#### 版本要求

| 组件 | 最低版本 | 推荐版本 |
|:---|:---:|:---:|
| **Qt Base** | 6.8.3 | 6.8.3 |
| **Qt Modules** | 6.8.3 | 6.8.3 |

必需的 Qt 模块：
- `Qt6Core`
- `Qt6Gui`
- `Qt6Widgets`

#### 安装方式

Qt6 有两种安装方式：

1. **官方在线安装器** (适合初学者)
2. **aqtinstall** (适合开发者和 CI)

##### 方式一：官方在线安装器

**Windows 安装**:

1. 下载 Qt 在线安装器：
   ```
   https://www.qt.io/download-qt-installer
   ```

2. 运行安装器并注册 Qt 账号（免费）

3. 在安装器中选择：
   - **Qt 6.8.3** (或最新版本)
   - **MinGW** 或 **LLVM** 编译器
   - **Qt Creator** (可选)

4. 安装目录建议：
   - Windows: `C:\Qt\6.8.3\`
   - Linux: `~/Qt/6.8.3/`

##### 方式二：aqtinstall (推荐)

aqtinstall 是一个命令行 Qt 安装工具，更适合开发环境。

**安装 aqtinstall**:

```bash
# 使用 pip 安装
pip install aqtinstall

# 或使用 pipx（推荐）
pipx install aqtinstall
```

**Windows 安装 Qt6**:

```powershell
# 安装 Qt 6.8.3 (MinGW 编译器)
aqt install-qt windows desktop 6.8.3 mingw-win64-everywhere -O C:\Qt

# 或安装 LLVM 编译器版本
aqt install-qt windows desktop 6.8.3 llvm-mingw-win64-everywhere -O C:\Qt
```

**Linux 安装 Qt6**:

```bash
# 安装 Qt 6.8.3 (GCC 编译器)
aqt install-qt linux desktop 6.8.3 gcc_64 -O ~/Qt

# 安装到系统目录（需要 sudo）
sudo aqt install-qt linux desktop 6.8.3 gcc_64 -O /opt/Qt
```

**ARM64 架构安装**:

```bash
# Linux ARM64
aqt install-qt linux desktop 6.8.3 linux_gcc_64 -O ~/Qt
```

#### 验证安装

```bash
# 检查 qmake 版本
# Windows
C:\Qt\6.8.3\mingw_64\bin\qmake.exe --version

# Linux
~/Qt/6.8.3/gcc_64/bin/qmake --version
```

#### 环境变量配置

**Windows**:

在系统环境变量中添加：

```
QTDIR=C:\Qt\6.8.3\mingw_64
QTDIR_BIN=C:\Qt\6.8.3\mingw_64\bin
CMAKE_PREFIX_PATH=C:\Qt\6.8.3\mingw_64
```

并将 `%QTDIR_BIN%` 添加到 PATH。

**Linux**:

在 `~/.bashrc` 或 `~/.zshrc` 中添加：

```bash
export QTDIR=~/Qt/6.8.3/gcc_64
export QTDIR_BIN=$QTDIR/bin
export PATH=$QTDIR_BIN:$PATH
export CMAKE_PREFIX_PATH=$QTDIR
```

然后执行 `source ~/.bashrc` 使配置生效。

---

## 可选软件

### CMake

CMake 用于构建项目，Docker 镜像中已包含。如需本地安装：

**Windows**:
```powershell
winget install Kitware.CMake
```

**Linux**:
```bash
sudo apt install -y cmake
```

### ccache

ccache 可以加速重复编译：

**Windows**:
```powershell
winget install ccache
```

**Linux**:
```bash
sudo apt install -y ccache
```

配置 ccache（可选）：

```bash
# 配置缓存目录
ccache -M 10G  # 设置缓存大小为 10GB
```

### Ninja

Ninja 是一个更快的构建工具：

**Windows**:
```powershell
winget install ninja
```

**Linux**:
```bash
sudo apt install -y ninja-build
```

---

## 验证安装

在继续下一步之前，请验证所有必需软件已正确安装。

### Windows 验证脚本

在 PowerShell 中运行：

```powershell
# 创建验证脚本
@'
Write-Host "=== CFDesktop 开发环境验证 ===" -ForegroundColor Cyan
Write-Host ""

# Git
Write-Host "检查 Git..." -NoNewline
if (Get-Command git -ErrorAction SilentlyContinue) {
    Write-Host " OK ($(git --version))" -ForegroundColor Green
} else {
    Write-Host " 未安装" -ForegroundColor Red
}

# Docker
Write-Host "检查 Docker..." -NoNewline
if (Get-Command docker -ErrorAction SilentlyContinue) {
    Write-Host " OK ($(docker --version))" -ForegroundColor Green
} else {
    Write-Host " 未安装" -ForegroundColor Red
}

# VSCode
Write-Host "检查 VSCode..." -NoNewline
if (Get-Command code -ErrorAction SilentlyContinue) {
    Write-Host " OK" -ForegroundColor Green
} else {
    Write-Host " 未安装" -ForegroundColor Yellow
}

# CMake
Write-Host "检查 CMake..." -NoNewline
if (Get-Command cmake -ErrorAction SilentlyContinue) {
    Write-Host " OK ($(cmake --version | Select-Object -First 1))" -ForegroundColor Green
} else {
    Write-Host " 未安装 (Docker 镜像中包含)" -ForegroundColor Yellow
}

# Python
Write-Host "检查 Python..." -NoNewline
if (Get-Command python -ErrorAction SilentlyContinue) {
    Write-Host " OK ($(python --version))" -ForegroundColor Green
} else {
    Write-Host " 未安装" -ForegroundColor Yellow
}

# Qt6 (需要手动检查)
Write-Host ""
Write-Host "请手动验证 Qt6 安装:" -ForegroundColor Yellow
Write-Host "  运行: `<Qt安装目录>\bin\qmake.exe --version`"
Write-Host "  应显示: Qt version 6.8.3"
Write-Host ""
'@ | Out-File -FilePath verify_env.ps1 -Encoding UTF8

# 运行验证
.\verify_env.ps1
```

### Linux 验证脚本

```bash
#!/bin/bash
echo "=== CFDesktop 开发环境验证 ==="
echo ""

# Git
echo -n "检查 Git... "
if command -v git &> /dev/null; then
    echo -e "\033[32mOK ($(git --version))\033[0m"
else
    echo -e "\033[31m未安装\033[0m"
fi

# Docker
echo -n "检查 Docker... "
if command -v docker &> /dev/null; then
    echo -e "\033[32mOK ($(docker --version))\033[0m"
else
    echo -e "\033[31m未安装\033[0m"
fi

# VSCode
echo -n "检查 VSCode... "
if command -v code &> /dev/null; then
    echo -e "\033[32mOK\033[0m"
else
    echo -e "\033[33m未安装\033[0m"
fi

# CMake
echo -n "检查 CMake... "
if command -v cmake &> /dev/null; then
    echo -e "\033[32mOK ($(cmake --version | head -n1))\033[0m"
else
    echo -e "\033[33m未安装 (Docker 镜像中包含)\033[0m"
fi

# Python
echo -n "检查 Python... "
if command -v python3 &> /dev/null; then
    echo -e "\033[32mOK ($(python3 --version))\033[0m"
else
    echo -e "\033[33m未安装\033[0m"
fi

# Qt6
echo ""
echo -e "\033[33m请手动验证 Qt6 安装:\033[0m"
echo "  运行: \`<Qt安装目录>/bin/qmake --version\`"
echo "  应显示: Qt version 6.8.3"
echo ""
```

---

## 下一步

完成前置要求安装和验证后，请继续阅读：

- **[02. 快速开始](02_quick_start.md)** - 快速上手开发
- **[03. 构建系统](03_build_system.md)** - 了解 CMake 构建系统和编译命令

---

## 常见问题

### Q1: 为什么 Docker 安装后无法启动？

**A (Windows)**: 确保启用了虚拟化和 Hyper-V/WSL 2 功能。

```powershell
# 检查虚拟化状态
systeminfo | find "Virtualization"
```

**A (Linux)**: 确保 Docker 服务已启动。

```bash
sudo systemctl start docker
sudo systemctl enable docker
```

### Q2: aqtinstall 下载速度太慢怎么办？

**A**: 可以配置国内镜像源：

```bash
# 使用清华大学镜像
aqt install-qt windows desktop 6.8.3 mingw_64 -O C:\Qt -b https://mirrors.tuna.tsinghua.edu.cn/qt
```

### Q3: Windows 下安装 Qt 时提示缺少运行库？

**A**: 安装 Microsoft Visual C++ Redistributable：

```
https://aka.ms/vs/17/release/vc_redist.x64.exe
```

### Q4: 可以使用系统包管理器安装 Qt 吗？

**A (Linux)**: 可以，但版本可能不是最新的。

```bash
# Ubuntu/Debian
sudo apt install qt6-base-dev qt6-tools-dev
```

建议使用 aqtinstall 安装特定版本的 Qt6。

---

## 附录

### 下载链接汇总

| 软件 | Windows | Linux |
|:---|:---|:---|
| **Docker Desktop** | [下载](https://www.docker.com/products/docker-desktop/) | [手动安装](https://docs.docker.com/engine/install/) |
| **Git** | [下载](https://git-scm.com/download/win) | `apt install git` |
| **VSCode** | [下载](https://code.visualstudio.com/download) | `snap install code` |
| **Qt 在线安装器** | [下载](https://www.qt.io/download-qt-installer) | [下载](https://www.qt.io/download-qt-installer) |
| **aqtinstall** | `pip install aqtinstall` | `pip install aqtinstall` |
| **CMake** | [下载](https://cmake.org/download/) | `apt install cmake` |

### 参考文档

- [Qt6 官方文档](https://doc.qt.io/qt-6/)
- [aqtinstall 文档](https://aqtinstall.readthedocs.io/)
- [Docker 官方文档](https://docs.docker.com/)
- [CMake 官方文档](https://cmake.org/documentation/)

---

<div align="center">

  [&larr; 返回](./) | [快速开始 &rarr;](02_quick_start.md)

  **版本**: 0.13.1 | **最后更新**: 2026-03-30

</div>
