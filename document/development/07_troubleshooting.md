# 常见问题排查

本文档提供 CFDesktop 项目开发和构建过程中常见问题的解决方案。

## 目录

- [Docker 相关问题](#docker-相关问题)
- [构建问题](#构建问题)
- [Git Hooks 问题](#git-hooks-问题)
- [性能优化](#性能优化)
- [获取更多帮助](#获取更多帮助)

## Docker 相关问题

### Docker 未启动

**症状**:

```
error: Docker daemon is not running!
```

**解决方案**:

```bash
# Linux
sudo systemctl start docker
sudo systemctl status docker

# macOS/Windows
# 启动 Docker Desktop 应用程序
```

**验证 Docker 运行状态**:

```bash
docker info
docker run --rm hello-world
```

---

### Docker 镜像构建失败

**症状**: 构建镜像时出现错误

**常见原因和解决方案**:

#### 1. 网络问题

```bash
# 使用国内镜像加速
# 编辑 /etc/docker/daemon.json (Linux) 或 Docker Desktop 设置
{
  "registry-mirrors": [
    "https://docker.mirrors.ustc.edu.cn",
    "https://hub-mirror.c.163.com"
  ]
}

# 重启 Docker
sudo systemctl restart docker
```

#### 2. 基础镜像拉取失败

```bash
# 手动拉取基础镜像
docker pull ubuntu:24.04

# 或使用镜像加速
docker pull registry.cn-hangzhou.aliyuncs.com/library/ubuntu:24.04
```

#### 3. 磁盘空间不足

```bash
# 清理未使用的镜像和容器
docker system prune -a

# 查看 Docker 占用空间
docker system df
```

#### 4. 构建缓存问题

```bash
# 使用 --no-cache 重新构建
docker build --no-cache -f scripts/docker/Dockerfile.build -t cfdesktop-build .
```

---

### 容器网络问题

**症状**: 容器内无法访问外部网络

**解决方案**:

```bash
# 检查 Docker 网络
docker network ls
docker network inspect bridge

# 重启 Docker 网络服务
sudo systemctl restart docker

# 使用 host 网络模式（Linux）
docker run --rm --network host -v $(pwd):/project cfdesktop-build
```

---

### ARM64 QEMU 仿真慢

**症状**: ARM64 构建非常缓慢（30+ 分钟）

**原因**: x86_64 主机使用 QEMU 仿真 ARM64

**解决方案**:

1. **使用快速构建模式**:

```bash
# 缓存镜像，避免重复构建
bash scripts/build_helpers/docker_start.sh --fast-build --verify
```

2. **跳过 ARM64 验证**（如果不是必需）:

```bash
# 修改 pre-push hook 或使用 --no-verify
git push --no-verify
```

3. **在原生 ARM64 环境构建**:

   - 使用 ARM64 服务器/实例
   - 使用 Apple Silicon Mac

---

### Windows 路径挂载问题

**症状**: 容器内找不到挂载的目录

**解决方案**:

```bash
# 确保路径使用正确格式
# Docker 需要 POSIX 路径

# Git Bash 自动转换
bash scripts/build_helpers/docker_start.sh

# PowerShell 需要手动转换
$env:MSYS_NO_PATHCONV=1
docker run --rm -v "D:/ProjectHome/CFDesktop:/project" cfdesktop-build
```

---

### 容器内权限问题

**症状**: 容器内无法写入文件

**解决方案**:

```bash
# Linux: 使用当前用户运行
docker run --rm -u $(id -u):$(id -g) -v $(pwd):/project cfdesktop-build

# 或修改文件权限
sudo chown -R $USER:$USER out/
```

## 构建问题

### 编译错误

**症状**: C++ 编译时出现错误

**常见类型和解决方案**:

#### 1. 头文件未找到

```
fatal error: some_header.h: No such file or directory
```

**解决方案**:

```bash
# 检查 CMake_INCLUDE_PATH
# 检查依赖是否正确安装

# Docker 内重新安装依赖
docker run --rm -v $(pwd):/project cfdesktop-build bash -c "
  apt-get update && \
  bash /project/scripts/dependency/install_build_dependencies.sh
"
```

#### 2. Qt 版本不匹配

```
error: 'Qt6::xxx' not found
```

**解决方案**:

```bash
# 检查 Qt 版本
qmake --version
qmlplugindump --version

# CMakeLists.txt 中指定正确版本
find_package(Qt6 6.8.1 REQUIRED COMPONENTS Core Gui Widgets)
```

#### 3. C++ 标准问题

```
error: 'xxx' is not a member of 'std'
```

**解决方案**:

```cmake
# CMakeLists.txt
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
```

---

### 链接错误

**症状**: 编译成功但链接失败

**常见错误和解决方案**:

#### 1. 未定义引用

```
undefined reference to 'xxx'
```

**解决方案**:

```cmake
# 检查链接库是否正确添加
target_link_libraries(your_target
    PRIVATE
        Qt6::Core
        Qt6::Widgets
        # 添加缺失的库
)
```

#### 2. 库文件未找到

```
cannot find -lxxx
```

**解决方案**:

```bash
# 检查库文件路径
ldconfig -p | grep xxx

# 添加库路径
link_directories(/path/to/lib)
```

---

### CMake 配置错误

**症状**: CMake 配置阶段失败

**常见问题**:

#### 1. 构建类型未指定

```
CMAKE_BUILD_TYPE is not set
```

**解决方案**:

```bash
# 使用正确的配置脚本
bash scripts/build_helpers/linux_develop_build.sh -c build_develop_config.ini
```

#### 2. 工具链问题

```
CMAKE_CXX_COMPILER not set
```

**解决方案**:

```bash
# 指定工具链
cmake -DCMAKE_TOOLCHAIN_FILE=/path/to/toolchain.cmake -S . -B out/build

# 或使用预设工具链
cmake -DUSE_TOOLCHAIN=linux/llvm -S . -B out/build
```

---

### Qt 相关问题

#### Qt 未找到

```
Could not find Qt6
```

**解决方案**:

```bash
# 设置 Qt 路径
export Qt6_DIR=/path/to/Qt/6.8.1/gcc_64/lib/cmake/Qt6
cmake -DQt6_DIR=/path/to/Qt/6.8.1/gcc_64/lib/cmake/Qt6 -S . -B out/build
```

#### Qt 模块缺失

```
Qt6 component 'Widgets' not found
```

**解决方案**:

```bash
# Docker 内重新安装 Qt
docker run --rm -v $(pwd):/project cfdesktop-build bash -c "
  python3 -m aqt install-qt --outputdir /opt linux desktop 6.8.1 gcc_64 -m qtbase qttools
"
```

---

### 测试失败

**症状**: 构建成功但测试失败

**调试步骤**:

```bash
# 详细测试输出
bash scripts/build_helpers/linux_run_tests.sh ci -c build_ci_config.ini --verbose

# 运行特定测试
ctest -R test_name --verbose

# 输出到文件
ctest --output-on-failure > test_results.txt
```

## Git Hooks 问题

### Pre-Commit 失败

**症状**: 提交时被 Hook 阻止

#### 1. 空白字符检查失败

```
error: 空白字符检查失败
```

**解决方案**:

```bash
# 自动修复
bash scripts/develop/remove_trailing_space.sh --staged

# 或强制提交（不推荐）
git commit --no-verify -m "message"
```

#### 2. clang-format 失败

```
error: clang-format failed
```

**解决方案**:

```bash
# 安装 clang-format
sudo apt install clang-format

# 或跳过格式化（删除 Hook 中的格式化部分）
```

---

### Pre-Push 验证失败

**症状**: 推送时被 Hook 阻止

#### 1. 版本号检查失败

```
error: 版本号未变更，推送被阻止
```

**解决方案**:

```bash
# 更新版本号
vim CMakeLists.txt  # 修改 VERSION x.y.z

# 提交版本变更
git add CMakeLists.txt
git commit -m "chore: bump version to x.y.z"

# 重新推送
git push
```

#### 2. Docker 构建失败

```
error: Docker 构建验证失败
```

**解决方案**:

```bash
# 本地手动验证
bash scripts/build_helpers/docker_start.sh --verify

# 查看详细日志
# 日志保存在 scripts/docker/logger/ci_build_*.log

# 或强制推送（不推荐）
git push --no-verify
```

---

### Hook 未执行

**症状**: Hook 脚本没有运行

**解决方案**:

```bash
# 检查 Hook 是否安装
ls -la .git/hooks/pre-commit .git/hooks/pre-push

# 重新安装
bash scripts/release/hooks/install_hooks.sh

# 检查执行权限
chmod +x .git/hooks/pre-commit .git/hooks/pre-push
```

---

### Git Bash 路径问题

**症状**: Windows Git Bash 中 Hook 路径错误

**解决方案**:

```bash
# 使用 MSYS_NO_PATHCONV=1
export MSYS_NO_PATHCONV=1

# 或修改 Hook 脚本，使用项目根目录的绝对路径
```

## 性能优化

### 使用快速构建缓存

```bash
# 复用现有 Docker 镜像
bash scripts/build_helpers/docker_start.sh --fast-build

# 结合验证使用
bash scripts/build_helpers/docker_start.sh --fast-build --verify
```

---

### 配置 ccache

```bash
# 在 Docker 容器内配置 ccache
docker run --rm -v $(pwd):/project -v ~/.ccache:/root/.ccache cfdesktop-build

# 或使用环境变量
export CCACHE_DIR=~/.ccache
export CCACHE_MAXSIZE=10G
```

---

### 并行编译调整

```bash
# 限制并行任务数（减少内存压力）
cmake -S . -B out/build -DCMAKE_BUILD_PARALLEL_LEVEL=4

# 或使用 make
make -j4
```

---

### 本地构建（跳过 Docker）

对于频繁的本地开发，可以使用宿主机环境：

```bash
# 设置 Qt 路径
export Qt6_DIR=/path/to/Qt/6.8.1/gcc_64/lib/cmake/Qt6

# 配置和构建
cmake -S . -B out/build -DCMAKE_BUILD_TYPE=Debug
cmake --build out/build --parallel
```

---

### 增量构建

```bash
# 使用快速构建脚本（不清理）
bash scripts/build_helpers/linux_fast_develop_build.sh -c build_develop_config.ini

# 只构建变更的目标
cmake --build out/build --target <specific_target>
```

## 获取更多帮助

### 查看日志

```bash
# Docker 构建日志
ls scripts/docker/logger/ci_build_*.log

# CMake 配置日志
cat out/build/CMakeFiles/CMakeOutput.log
cat out/build/CMakeFiles/CMakeError.log
```

---

### 调试模式

```bash
# Docker 交互式调试
bash scripts/build_helpers/docker_start.sh
# 在容器内手动运行命令

# CMake 详细输出
cmake -S . -B out/build --trace-expand

# Make 详细输出
make VERBOSE=1
```

---

### 项目资源

- **GitHub Issues**: [提交问题](https://github.com/your-org/CFDesktop/issues)
- **文档首页**: [文档索引](../index.md)
- **开发指南**: [环境准备](./01_prerequisites.md)
- **构建系统**: [构建系统详解](./03_build_system.md)

---

### 联系方式

- **项目维护者**: 通过 GitHub Issues 联系
- **讨论区**: GitHub Discussions

---

### 报告问题模板

提交问题时，请包含以下信息：

```markdown
## 环境信息
- 操作系统: [Ubuntu 24.04 / Windows 11 / macOS 14]
- Docker 版本: [Docker version 24.0.7]
- Qt 版本: [6.8.1]
- CMake 版本: [3.28.0]

## 问题描述
[简要描述问题]

## 复现步骤
1. [步骤 1]
2. [步骤 2]

## 错误信息
```
[粘贴错误输出]
```

## 已尝试的解决方案
[列出已尝试的方法]
```
