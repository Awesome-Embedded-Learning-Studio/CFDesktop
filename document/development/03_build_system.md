# Build System Documentation

> Comprehensive guide to the CFDesktop CMake build system

## Table of Contents

- [Overview](#overview)
- [CMake Architecture](#cmake-architecture)
- [Project Modules](#project-modules)
- [Build Types](#build-types)
- [Toolchain Configuration](#toolchain-configuration)
- [Build Scripts](#build-scripts)
- [Output Directories](#output-directories)
- [Common Build Options](#common-build-options)
- [Advanced Usage](#advanced-usage)

---

## Overview

CFDesktop uses a modular CMake-based build system designed for cross-platform development and embedded deployment. The build system supports:

- **Multiple platforms**: Windows (MinGW/LLVM), Linux (GCC/Clang)
- **Multiple architectures**: x86_64, ARM64, ARMhf
- **Build types**: Debug, Release, RelWithDebInfo
- **Containerized builds**: Docker with multi-architecture support

### Build System Diagram

```
CFDesktop Build System
├── Configuration Files (.ini)
│   ├── build_develop_config.ini    (Debug builds)
│   ├── build_deploy_config.ini     (Release builds)
│   └── build_ci_config.ini         (CI builds)
│
├── Build Scripts
│   ├── windows_*.ps1               (PowerShell scripts)
│   ├── linux_*.sh                  (Bash scripts)
│   └── docker_start.sh             (Docker wrapper)
│
├── CMake Modules
│   ├── check_toolchain.cmake       (Toolchain selection)
│   ├── OutputDirectoryConfig.cmake (Output directory management)
│   └── generate_develop_helpers.cmake (IDE config generation)
│
└── Toolchains
    ├── windows/llvm-toolchain.cmake
    ├── windows/gcc-toolchain.cmake
    ├── linux/ci-x86_64-toolchain.cmake
    └── linux/ci-aarch64-toolchain.cmake
```

---

## CMake Architecture

### Root CMakeLists.txt

The root `CMakeLists.txt` is the entry point for the build system:

```cmake
cmake_minimum_required(VERSION 3.16)
project(CFDesktop VERSION 0.9.0 LANGUAGES CXX)

# Toolchain configuration (supports shorthand: -DUSE_TOOLCHAIN=windows/llvm)
include(cmake/check_toolchain.cmake)

# Build type validation
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo")

# Compiler flags per build type
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG" CACHE STRING "" FORCE)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# Qt6 dependency
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# Subdirectories
add_subdirectory(base)
add_subdirectory(ui)
add_subdirectory(example)
add_subdirectory(test)
```

### CMake Module Structure

```
cmake/
├── build_log_helper.cmake         # Logging utilities
├── check_toolchain.cmake          # Toolchain selection
├── custom_target_helper.cmake     # Custom target helpers
├── OutputDirectoryConfig.cmake    # Output directory management
├── ExampleLauncher.cmake          # Windows launcher generation
├── QtDeployUtils.cmake            # Qt deployment utilities
└── generate_develop_helpers.cmake # IDE configuration generation
```

---

## Project Modules

### Module Overview

| Module | Description | Output | Dependencies |
|:-------|:------------|:-------|:-------------|
| **base/** | Base utilities and platform abstractions | `cfbase.dll` / `libcfbase.so` | Qt6::Core |
| **ui/** | UI framework and components | `cfui.dll` / `libcfui.so` | base, Qt6::Core, Qt6::Gui |
| **example/** | Example programs | `examples/{category}/` | base, ui |
| **test/** | Unit tests | `test/` | base, ui, GoogleTest |

### Base Module

The base module provides fundamental utilities:

```
base/
├── include/                    # Header-only utilities
│   ├── cfbase/
│   │   ├── expected.hpp       # std::expected-like type
│   │   ├── scope_guard.hpp    # RAII resource management
│   │   └── weak_ptr.hpp       # Weak pointer utilities
│   └── CFDesktop/
│       └── Base/
│           └── system/
│               ├── cpu/        # CPU detection
│               └── memory/     # Memory detection
└── system/
    ├── cpu/CMakeLists.txt      # CPU module
    └── memory/CMakeLists.txt   # Memory module
```

**Unified Base Library:**
All base components are linked into a single shared library (`cfbase.dll` on Windows, `libcfbase.so` on Linux).

```cmake
# base/CMakeLists.txt
add_library(cfbase SHARED)
target_sources(cfbase PRIVATE
    $<TARGET_OBJECTS:cfbase_cpu>
    $<TARGET_OBJECTS:cfbase_memory>
)
target_link_libraries(cfbase PUBLIC Qt6::Core)
```

### UI Module

The UI module provides the Material Design framework:

```
ui/
├── base/                       # Math utilities
│   ├── math_helper.hpp
│   ├── color_helper.hpp
│   ├── geometry_helper.hpp
│   └── easing.hpp
├── core/                       # Theme engine
│   ├── theme/
│   ├── color_scheme/
│   ├── motion_spec/
│   └── typography/
├── components/                 # Animation system
│   ├── animation/
│   ├── timing_animation/
│   └── spring_animation/
└── widget/                     # Widget adapters
    └── material/
        ├── button/
        ├── label/
        ├── textfield/
        └── ...
```

**Unified UI Library:**
All UI components are linked into a single shared library (`cfui.dll` on Windows, `libcfui.so` on Linux).

```cmake
# ui/CMakeLists.txt
add_library(cfui SHARED)
target_link_libraries(cfui PUBLIC
    cf_ui_core
    cf_ui_base
    cf_ui_components
    cf_ui_widget
    CFDesktop::base
    Qt6::Core
    Qt6::Gui
)
```

---

## Build Types

### Available Build Types

| Build Type | Optimization | Debug Info | Use Case |
|:-----------|:------------|:----------:|:---------|
| **Debug** | `-O0` | Full (`-g`) | Development and debugging |
| **Release** | `-O3` | None | Production deployment |
| **RelWithDebInfo** | `-O2` | Full (`-g`) | Profiling and testing |

### Build Type Selection

Build types are configured via `.ini` files:

```ini
[cmake]
build_type=Debug    # or Release, RelWithDebInfo
```

**Configuration Files:**

| File | Build Type | Use Case |
|:-----|:-----------|:---------|
| `build_develop_config.ini` | Debug | Daily development |
| `build_deploy_config.ini` | Release | Production builds |
| `build_ci_config.ini` | Release | CI/CD pipeline |

---

## Toolchain Configuration

### Toolchain Shorthand

CFDesktop supports a shorthand notation for toolchain selection:

```bash
cmake -DUSE_TOOLCHAIN=windows/llvm -S . -B build
cmake -DUSE_TOOLCHAIN=windows/gcc -S . -B build
cmake -DUSE_TOOLCHAIN=linux/ci-x86_64 -S . -B build
```

### Available Toolchains

| Platform | Toolchain | Shorthand | Compiler |
|:---------|:----------|:----------|:---------|
| **Windows** | LLVM-MinGW | `windows/llvm` | clang/LLVM |
| **Windows** | MinGW-GCC | `windows/gcc` | gcc/MinGW |
| **Linux** | CI x86_64 | `linux/ci-x86_64` | gcc (Docker) |
| **Linux** | CI ARM64 | `linux/ci-aarch64` | aarch64 gcc (Docker) |

### Toolchain File Structure

Toolchain files are located in `cmake/cmake_toolchain/{platform}/`:

```
cmake/cmake_toolchain/
├── windows/
│   ├── llvm-toolchain.cmake
│   └── gcc-toolchain.cmake
└── linux/
    ├── ci-x86_64-toolchain.cmake
    └── ci-aarch64-toolchain.cmake
```

### Windows LLVM-MinGW Toolchain

```cmake
# cmake/cmake_toolchain/windows/llvm-toolchain.cmake
set(CMAKE_PREFIX_PATH "D:/QT/Qt6.6.0/6.8.3/llvm-mingw_64")
set(CMAKE_C_COMPILER "D:/QT/Qt6.6.0/Tools/llvm-mingw1706_64/bin/gcc.exe")
set(CMAKE_CXX_COMPILER "D:/QT/Qt6.6.0/Tools/llvm-mingw1706_64/bin/g++.exe")
```

### Linux CI Toolchain

```cmake
# cmake/cmake_toolchain/linux/ci-x86_64-toolchain.cmake
set(CMAKE_SYSTEM_NAME Linux)
set(QT6_BASE_DIR "/opt/Qt/6.8.1/gcc_64")
set(Qt6_DIR "${QT6_BASE_DIR}/lib/cmake/Qt6")
set(CMAKE_PREFIX_PATH "${QT6_BASE_DIR}")
```

---

## Build Scripts

### Script Overview

Build scripts are organized by platform and purpose:

```
scripts/build_helpers/
├── windows_*.ps1              # Windows PowerShell scripts
│   ├── windows_configure.ps1
│   ├── windows_fast_develop_build.ps1
│   ├── windows_develop_build.ps1
│   └── windows_run_tests.ps1
├── linux_*.sh                 # Linux Bash scripts
│   ├── linux_configure.sh
│   ├── linux_fast_develop_build.sh
│   ├── linux_develop_build.sh
│   └── linux_run_tests.sh
└── docker_start.sh            # Docker wrapper script
```

### Windows Build Scripts

#### Configure Script

```powershell
# Configure only (no build)
.\scripts\build_helpers\windows_configure.ps1 [-Config <develop|deploy>]
```

**What it does:**
1. Loads configuration from `.ini` file
2. Validates build type
3. Runs CMake configuration
4. Generates build files

#### Fast Build Script

```powershell
# Fast incremental build
.\scripts\build_helpers\windows_fast_develop_build.ps1
```

**What it does:**
1. Calls configure script
2. Builds with CMake (no clean)
3. Uses parallel jobs

#### Full Build Script

```powershell
# Full clean build
.\scripts\build_helpers\windows_develop_build.ps1
```

**What it does:**
1. Cleans build directory
2. Calls fast build script
3. Runs tests

### Linux Build Scripts

#### Configure Script

```bash
# Configure only
bash scripts/build_helpers/linux_configure.sh [develop|deploy|ci] [-c <config_file>]
```

#### Fast Build Script

```bash
# Fast incremental build
bash scripts/build_helpers/linux_fast_develop_build.sh [develop|deploy|ci]
```

#### Full Build Script

```bash
# Full clean build
bash scripts/build_helpers/linux_develop_build.sh [develop|deploy|ci]
```

### Docker Build Script

```bash
# Interactive shell
bash scripts/build_helpers/docker_start.sh

# Fast build
bash scripts/build_helpers/docker_start.sh --fast-build --build-project-fast

# Full build
bash scripts/build_helpers/docker_start.sh --build-project

# Run tests
bash scripts/build_helpers/docker_start.sh --run-project-test

# CI verification
bash scripts/build_helpers/docker_start.sh --verify

# ARM64 build
bash scripts/build_helpers/docker_start.sh --arch arm64 --verify
```

**Docker Options:**

| Option | Description |
|:-------|:------------|
| `--arch amd64|arm64` | Target architecture |
| `--fast-build` | Reuse existing image |
| `--verify` | Run CI verification |
| `--build-project` | Full clean build |
| `--build-project-fast` | Fast incremental build |
| `--run-project-test` | Run tests |
| `--stay-on-error` | Keep container on error |
| `--no-log` | Disable file logging |
| `--no-deps` | Skip dependency installation |

---

## Output Directories

### Output Directory Structure

```
out/build_{config}/
├── bin/                        # Executables and shared libraries
│   ├── cfbase.dll              # Base library (Windows)
│   ├── cfui.dll                # UI library (Windows)
│   └── ...
├── lib/                        # Static libraries
│   ├── libcfbase.a
│   └── ...
├── examples/                   # Example programs
│   ├── base/                   # Base examples
│   │   ├── cpu_info
│   │   └── memory_info
│   ├── ui/                     # UI widget examples
│   │   ├── button
│   │   ├── label
│   │   └── ...
│   └── gui/                    # GUI examples
│       ├── material_gallery
│       └── theme
├── plugins/                    # Qt plugins
├── resources/                  # Resource files
├── runtimes/                   # Qt runtime DLLs (Windows)
└── test/                       # Test executables
    ├── base_test
    ├── ui_test
    └── ...
```

### Output Directory Configuration

Output directories are configured in `cmake/OutputDirectoryConfig.cmake`:

```cmake
# Global output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# Example-specific outputs
function(cf_set_example_output_dir TARGET_NAME CATEGORY)
    set_target_properties(${TARGET_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/examples/${CATEGORY}"
    )
endfunction()
```

---

## Common Build Options

### Configuration File Options

Configuration files use INI format:

```ini
[cmake]
# CMake generator
generator=MinGW Makefiles        # or "Unix Makefiles", "Ninja"

# Toolchain selection
toolchain=windows/llvm          # or windows/gcc, linux/ci-x86_64

# Build type
build_type=Debug                # or Release, RelWithDebInfo

[paths]
# Source directory (relative to project root)
source=.

# Build output directory (relative to project root)
build_dir=out/build_develop

[options]
# Parallel jobs for compilation
jobs=16
```

### CMake Options

| Option | Description | Default |
|:-------|:------------|:--------|
| `CMAKE_BUILD_TYPE` | Build type (Debug/Release/RelWithDebInfo) | Required |
| `CMAKE_PREFIX_PATH` | Qt installation path | From toolchain |
| `USE_TOOLCHAIN` | Toolchain shorthand | Required |
| `CMAKE_EXPORT_COMPILE_COMMANDS` | Generate compile_commands.json | ON |
| `BUILD_TESTING` | Build tests | ON |

---

## Advanced Usage

### Custom Toolchain Configuration

To use a custom toolchain:

1. Create a toolchain file in `cmake/cmake_toolchain/{platform}/`
2. Use the shorthand notation:

```bash
cmake -DUSE_TOOLCHAIN=windows/mytoolchain -S . -B build
```

Or use the full path:

```bash
cmake -DCMAKE_TOOLCHAIN_FILE=/path/to/toolchain.cmake -S . -B build
```

### Incremental Builds

For faster development, use incremental builds:

**Linux:**
```bash
bash scripts/build_helpers/linux_fast_develop_build.sh
```

**Windows:**
```powershell
.\scripts\build_helpers\windows_fast_develop_build.ps1
```

### Parallel Builds

Control the number of parallel jobs via the configuration file:

```ini
[options]
jobs=8                          # Use 8 parallel jobs
```

Or via CMake:

```bash
cmake --build build --parallel 8
```

### Building Specific Targets

To build specific targets:

```bash
cmake --build build --target cfbase
cmake --build build --target cfui
cmake --build build --target material_gallery
```

### Clean Builds

For a completely clean build:

**Linux:**
```bash
bash scripts/build_helpers/linux_develop_build.sh
```

**Windows:**
```powershell
.\scripts\build_helpers\windows_develop_build.ps1
```

Or manually:

```bash
rm -rf out/build_develop
cmake -DUSE_TOOLCHAIN=windows/llvm -DCMAKE_BUILD_TYPE=Debug -S . -B out/build_develop
cmake --build out/build_develop
```

---

## Cross-Platform Build Matrix

### Supported Platforms

| Platform | Architecture | Toolchain | Status |
|:---------|:------------:|:----------|:------:|
| Windows 10+ | x86_64 | LLVM-MinGW | Supported |
| Windows 10+ | x86_64 | MinGW-GCC | Supported |
| Linux | x86_64 | GCC | Supported |
| Linux | x86_64 | Clang | Supported |
| Linux (Docker) | x86_64 | GCC | Supported |
| Linux (Docker) | ARM64 | aarch64 gcc | Supported |

### Build Command Reference

| Platform | Command |
|:---------|:---------|
| **Windows (LLVM)** | `.\scripts\build_helpers\windows_fast_develop_build.ps1` |
| **Windows (GCC)** | Edit `build_develop_config.ini`: `toolchain=windows/gcc` |
| **Linux (Native)** | `bash scripts/build_helpers/linux_fast_develop_build.sh` |
| **Linux (Docker)** | `bash scripts/build_helpers/docker_start.sh --build-project-fast` |
| **ARM64 (Docker)** | `bash scripts/build_helpers/docker_start.sh --arch arm64 --build-project-fast` |

---

## IDE Integration

### VSCode Configuration

The build system automatically generates VSCode configuration files:

- `.vscode/launch.json` - Debug configurations
- `.clangd` - Clangd language server configuration
- `compile_commands.json` - Compilation database

These are generated by `cmake/generate_develop_helpers.cmake` during CMake configuration.

### QtCreator

QtCreator can open the project directly:

1. File -> Open File or Project
2. Select `CMakeLists.txt`
3. Configure the build directory
4. Choose the toolchain
5. Click "Run CMake"

---

## Troubleshooting

### Common Issues

| Issue | Solution |
|:------|:----------|
| CMake not found | Install CMake 3.16+ or add to PATH |
| Qt not found | Set `Qt6_DIR` or use correct toolchain |
| Compiler not found | Install compiler or use Docker build |
| Permission denied | Use `sudo` on Linux or run as admin on Windows |
| Out of memory during build | Reduce parallel jobs in `.ini` file |

### Debug Mode Builds

For debugging, ensure you're using the Debug build type:

```ini
[cmake]
build_type=Debug
```

This will:
- Disable optimizations (`-O0`)
- Include full debug symbols (`-g`)
- Enable assertions

### Release Builds

For production:

```ini
[cmake]
build_type=Release
```

This will:
- Enable maximum optimizations (`-O3`)
- Disable debug info
- Define `NDEBUG`

---

## Related Documentation

- [Quick Start Guide](02_quick_start.md) - Get started in 30 minutes
- [Project Skeleton Design](../design_stage/00_phase0_project_skeleton.md) - Detailed project architecture
- [Base Library Design](../design_stage/02_phase2_base_library.md) - Base module documentation
- [UI Framework Design](../todo/base/99_ui_material_framework.md) - UI module documentation

---

**Last Updated**: 2026-03-07
