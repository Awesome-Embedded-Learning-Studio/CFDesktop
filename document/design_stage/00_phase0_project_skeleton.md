# Phase 0: 工程骨架搭建详细设计文档

## 文档信息
| 项目 | 内容 |
|------|------|
| 文档版本 | v1.0 |
| 创建日期 | 2026-02-20 |
| 阶段代号 | Phase 0 - 工程骨架 |
| 预计周期 | 1~2 周 |

---

## 一、阶段目标

### 1.1 核心目标
建立项目的工程化基础，确保后续所有开发工作在稳固的架构上进行。

### 1.2 具体交付物
- [ ] 完整的 CMake 构建体系
- [ ] 三层模块目录结构（base/sdk/shell）
- [ ] 跨平台 CI/CD 流水线
- [ ] 开发环境配置指南
- [ ] 代码规范与格式化配置

---

## 三、CMake 构建系统设计

### 3.1 主 CMakeLists.txt 结构

```cmake
cmake_minimum_required(VERSION 3.24)
project(CFDesktop
    VERSION 0.1.0
    DESCRIPTION "CFDesktop - Qt-based Embedded Desktop System"
    LANGUAGES CXX
)

# ==================== 基础配置 ====================
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 导出编译命令（用于 IDE 支持）
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ==================== 依赖项 ====================
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Multimedia
    Network
)

# ==================== 子目录 ====================
add_subdirectory(cmake)
add_subdirectory(src/base)
add_subdirectory(src/sdk)
add_subdirectory(src/shell)
add_subdirectory(src/simulator)
add_subdirectory(tests)

# ==================== 打包配置 ====================
include(CPack)
```

### 3.2 Base 库 CMakeLists.txt

```cmake
project(CFDesktopBase)

# ==================== 源文件 ====================
set(BASE_PUBLIC_HEADERS
    include/CFDesktop/Base/HardwareProbe/HardwareProbe>
    include/CFDesktop/Base/HardwareProbe/HWTier>
    include/CFDesktop/Base/ThemeEngine/ThemeEngine>
    include/CFDesktop/Base/AnimationManager/AnimationManager>
    include/CFDesktop/Base/DPIManager/DPIManager>
    include/CFDesktop/Base/ConfigStore/ConfigStore>
    include/CFDesktop/Base/Logger/Logger>
)

set(BASE_SOURCES
    src/hardware/HardwareProbe.cpp
    src/theme/ThemeEngine.cpp
    src/animation/AnimationManager.cpp
    src/dpi/DPIManager.cpp
    src/config/ConfigStore.cpp
    src/logging/Logger.cpp
)

# ==================== 库目标 ====================
add_library(CFDesktopBase STATIC
    ${BASE_PUBLIC_HEADERS}
    ${BASE_SOURCES}
)

# ==================== 编译选项 ====================
target_compile_features(CFDesktopBase PUBLIC cxx_std_23)
target_include_directories(CFDesktopBase PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(CFDesktopBase PUBLIC
    Qt6::Core
    Qt6::Gui
)

# ==================== 平台特定配置 ====================
if(UNIX AND NOT APPLE)
    target_link_libraries(CFDesktopBase PUBLIC pthread dl)
endif()

# ==================== 安装规则 ====================
install(TARGETS CFFesktopBase
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(FILES ${BASE_PUBLIC_HEADERS}
    DESTINATION include/CFDesktop/Base
)
```

### 3.3 SDK 库 CMakeLists.txt

```cmake
project(CFDesktopSDK)

# 依赖 Base 库
target_link_libraries(CFDesktopSDK PUBLIC CFDesktopBase)

# 导出 CMake 配置
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    CFDesktopSDKConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    cmake/CFDesktopSDKConfig.cmake
    cmake/CFDesktopSDKConfigVersion.cmake
    DESTINATION lib/cmake/CFDesktopSDK
)
```

### 3.4 Shell 主程序 CMakeLists.txt

```cmake
project(CFDesktopShell)

add_executable(cfdesktop-shell
    src/main.cpp
    # ... 其他源文件
)

target_link_libraries(cfdesktop-shell PRIVATE
    CFDesktopSDK
    Qt6::Widgets
    Qt6::Multimedia
)

install(TARGETS cfdesktop-shell
    RUNTIME DESTINATION bin
)
```

### 3.5 交叉编译工具链配置

#### 3.5.1 ARMv7 (IMX6ULL) 工具链

```cmake
# cmake/toolchains/arm-linux-gnueabihf.cmake

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)

set(CMAKE_FIND_ROOT_PATH /opt/arm-sfm-toolchain)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(Qt6_DIR /opt/qt6-arm/lib/cmake/Qt6)
```

#### 3.5.2 ARM64 (RK3568/RK3588) 工具链

```cmake
# cmake/toolchains/aarch64-linux-gnu.cmake

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)

set(CMAKE_FIND_ROOT_PATH /opt/arm-hf-toolchain)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(Qt6_DIR /opt/qt6-arm64/lib/cmake/Qt6)
```

### 3.6 CMakePresets.json

```json
{
    "version": 3,
    "cmakeMinimumRequired": {
        "major": 3,
        "minor": 24,
        "patch": 0
    },
    "configurePresets": [
        {
            "name": "linux-default",
            "displayName": "Linux x64",
            "generator": "Ninja",
            "binaryDir": "${sourceDir}/build/linux-default",
            "cacheVariables": {
                "CMAKE_BUILD_TYPE": "Debug",
                "BUILD_TESTING": "ON"
            }
        },
        {
            "name": "linux-arm-sf",
            "displayName": "ARMv7 Software Rendering",
            "generator": "Ninja",
            "binaryDir": "${sourceDir}/build/linux-arm-sf",
            "toolchainFile": "${sourceDir}/cmake/toolchains/arm-linux-gnueabihf.cmake",
            "cacheVariables": {
                "CMAKE_BUILD_TYPE": "Release",
                "QT_PLATFORM": "linuxfb"
            }
        },
        {
            "name": "linux-arm-hf",
            "displayName": "ARM64 Hardware Accelerated",
            "generator": "Ninja",
            "binaryDir": "${sourceDir}/build/linux-arm-hf",
            "toolchainFile": "${sourceDir}/cmake/toolchains/aarch64-linux-gnu.cmake",
            "cacheVariables": {
                "CMAKE_BUILD_TYPE": "Release",
                "QT_PLATFORM": "eglfs"
            }
        }
    ],
    "buildPresets": [
        {
            "name": "linux-default-debug",
            "configurePreset": "linux-default",
            "configuration": "Debug"
        }
    ]
}
```

---

## 四、CI/CD 流水线设计

### 4.1 主构建流程 (.github/workflows/build.yml)

```yaml
name: Build Matrix

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main ]

jobs:
  build-linux-x64:
    runs-on: ubuntu-latest
    container: ghcr.io/cfdesktop/builder-linux-x64:latest

    steps:
      - uses: actions/checkout@v3

      - name: Configure CMake
        run: |
          cmake -B build -DCMAKE_BUILD_TYPE=Release \
            -DBUILD_TESTING=ON

      - name: Build
        run: cmake --build build --parallel

      - name: Run Tests
        run: ctest --test-dir build --output-on-failure

      - name: Upload Artifacts
        uses: actions/upload-artifact@v3
        with:
          name: linux-x64-build
          path: build/src/

  build-arm-sf:
    runs-on: ubuntu-latest
    container: ghcr.io/cfdesktop/builder-arm-sf:latest

    steps:
      - uses: actions/checkout@v3

      - name: Cross-Compile for ARMv7
        run: |
          cmake -B build \
            -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/arm-linux-gnueabihf.cmake \
            -DCMAKE_BUILD_TYPE=Release

      - name: Build
        run: cmake --build build --parallel

      - name: Package
        run: cpack --config build/CPackConfig.cmake

      - name: Upload Artifacts
        uses: actions/upload-artifact@v3
        with:
          name: arm-sf-build
          path: build/*.tar.gz

  code-quality:
    runs-on: ubuntu-latest

    steps:
      - uses: actions/checkout@v3

      - name: Run clang-tidy
        run: |
          cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
          run-clang-tidy -p build src/

      - name: Format Check
        run: |
          ./tools/format.sh --check
```

### 4.2 部署流程 (.github/workflows/deploy.yml)

```yaml
name: Deploy to Device

on:
  workflow_dispatch:
    inputs:
      target:
        description: 'Target device'
        required: true
        type: choice
        options:
          - imx6ull-dev
          - rk3568-dev
          - rk3588-dev

jobs:
  deploy:
    runs-on: ubuntu-latest
    steps:
      - name: Download Build Artifacts
        uses: actions/download-artifact@v3

      - name: Deploy via SSH
        uses: appleboy/ssh-action@v0.1.5
        with:
          host: ${{ secrets.DEVICE_HOST }}
          username: ${{ secrets.DEVICE_USER }}
          key: ${{ secrets.DEVICE_SSH_KEY }}
          script: |
            systemctl stop cfdesktop
            tar -xzf cfdesktop.tar.gz -C /opt/
            systemctl start cfdesktop
```

---

## 五、开发环境配置

### 5.1 VSCode 配置 (.vscode/settings.json)

```json
{
    "cmake.configureArgs": [
        "-DBUILD_TESTING=ON"
    ],
    "C_Cpp.default.configurationProvider": "ms-vscode.cmake-tools",
    "files.associations": {
        "*.cmake": "cmake",
        "CMakeLists.txt": "cmake"
    },
    "editor.formatOnSave": true,
    "editor.formatOnType": true,
    "C_Cpp.clang_format_style": "file",
    "clang-format.style.location": ".clang-format"
}
```

### 5.2 推荐扩展

- `ms-vscode.cmake-tools`
- `twxs.cmake`
- `xaver.clang-format`
- "xaver.clang-tidy"
- `ms-vscode.cpptools`

### 5.3 代码格式化配置 (.clang-format)

```yaml
---
BasedOnStyle: Google
Language: Cpp
Standard: c++23

IndentWidth: 4
TabWidth: 4
UseTab: Never
ColumnLimit: 100

PointerAlignment: Left
ReferenceAlignment: Left

SortIncludes: true
IncludeBlocks: Regroup
IncludeCategories:
  - Regex:           '^<Qt'
    Priority:        1
  - Regex:           '^<CFDesktop'
    Priority:        2
  - Regex:           '^<.*'
    Priority:        3
  - Regex:           '^".*"'
    Priority:        4
```

---

## 六、详细任务清单

### 6.1 Week 1 任务

#### Day 1-2: 目录结构创建
- [ ] 创建完整目录树
- [ ] 编写主 CMakeLists.txt
- [ ] 配置 CMakePresets.json
- [ ] 创建各子模块的 CMakeLists.txt 框架

#### Day 3-4: 交叉编译配置
- [ ] 编写 ARMv7 工具链文件
- [ ] 编写 ARM64 工具链文件
- [ ] 配置 Qt6 路径查找逻辑
- [ ] 测试交叉编译（可用 Docker 验证）

#### Day 5: 代码规范配置
- [ ] 配置 .clang-format
- [ ] 配置 .clang-tidy
- [ ] 编写代码格式化脚本
- [ ] 配置 Git pre-commit hook

### 6.2 Week 2 任务

#### Day 1-3: CI/CD 搭建
- [ ] 创建 Docker 构建镜像
- [ ] 编写 GitHub Actions 工作流
- [ ] 配置 artifact 上传
- [ ] 测试完整构建流程

#### Day 4-5: 开发工具完善
- [ ] 创建 VSCode 工作区配置
- [ ] 编写开发环境设置文档
- [ ] 创建应用模板框架
- [ ] 编写 Hello World 测试程序

---

## 七、验收标准

### 7.1 构建系统
- [ ] `cmake --preset linux-default && cmake --build build --preset linux-default` 成功
- [ ] `cmake --preset linux-arm-sf && cmake --build build --preset linux-arm-sf` 产出 ARM 二进制
- [ ] 所有目标编译无警告

### 7.2 CI/CD
- [ ] Push 代码自动触发构建
- [ ] 构建失败发送通知
- [ ] 测试用例自动运行
- [ ] Artifact 正确上传

### 7.3 开发环境
- [ ] VSCode 能够正确索引所有符号
- [ ] 代码格式化一键执行
- [ ] 远程调试配置可用
- [ ] 新团队成员能在 30 分钟内完成环境搭建

---

## 八、依赖软件版本清单

| 软件 | 版本要求 | 说明 |
|------|----------|------|
| CMake | >= 3.24 | 支持 C++23 和 CMakePresets |
| GCC | >= 12.0 | C++23 特性支持 |
| Qt6 | >= 6.5 | 核心依赖 |
| Ninja | >= 1.10 | 构建后端 |
| Docker | >= 24.0 | CI 容器化 |
| clang-format | >= 16.0 | 代码格式化 |
| clang-tidy | >= 16.0 | 静态分析 |

---

## 九、风险与缓解措施

| 风险 | 影响 | 缓解措施 |
|------|------|----------|
| Qt6 交叉编译配置复杂 | 延迟 2-3 天 | 使用 Docker 预配置环境 |
| CI 资源不足 | 构建时间长 | 优化依赖缓存，使用矩阵策略 |
| 工具链版本兼容性 | 编译失败 | 固定 Docker 镜像版本 |

---

## 十、下一步行动

完成 Phase 0 后，立即进入 **Phase 1: 硬件探针与能力分级**。
