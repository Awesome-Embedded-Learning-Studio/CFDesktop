# Quick Start Guide

> Get up and running with CFDesktop in 30 minutes

## Table of Contents

- [Prerequisites](#prerequisites)
- [5-Step Quick Start](#5-step-quick-start)
- [Windows Users](#windows-users)
- [Verification](#verification)
- [Next Steps](#next-steps)
- [Troubleshooting](#troubleshooting)

---

## Prerequisites

Before starting, ensure you have the following installed:

| Requirement | Minimum Version | Recommended | How to Check |
|:------------|----------------:|:------------|:-------------|
| **Git** | 2.30+ | Latest | `git --version` |
| **CMake** | 3.16 | 3.20+ | `cmake --version` |
| **Qt6** | 6.8.3 | 6.8+ | Check Qt Installation |
| **Compiler** | LLVM/Clang or GCC | Latest | `clang --version` or `gcc --version` |
| **Docker** (optional) | 20.10+ | Latest | `docker --version` |

### Platform-Specific Requirements

**Windows:**
- Git Bash or WSL2 for running build scripts
- Qt6 installed with MinGW or LLVM-MinGW toolchain
- PowerShell 5.1+ for Windows-specific scripts

**Linux:**
- GCC 12+ or Clang 16+
- Qt6 development packages
- Ninja build system (optional but recommended)

---

## 5-Step Quick Start

### Step 1: Clone the Repository

```bash
git clone https://github.com/Awesome-Embedded-Learning-Studio/CFDesktop.git
cd CFDesktop
```

### Step 2: Install VSCode Extensions

Install the following VSCode extensions for optimal development experience:

| Extension | Purpose | Required |
|:----------|:--------|:--------:|
| [Clangd](https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.vscode-clangd) | C++ language server | Yes |
| [CMake](https://marketplace.visualstudio.com/items?itemName=twxs.cmake) | CMake syntax highlighting | Recommended |
| [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) | CMake integration | Recommended |
| [Qt for Python](https://marketplace.visualstudio.com/items?itemName=seanwu.vscode-qt-for-python) | Qt support | Optional |

**Quick Install (Command Palette):**

1. Press `Ctrl+Shift+P` (Windows/Linux) or `Cmd+Shift+P` (macOS)
2. Type "Extensions: Install Extensions"
3. Search for each extension name above

### Step 3: Docker First Build (Recommended)

The fastest way to get started is using Docker, which provides a pre-configured build environment:

```bash
# Fast build (reuses existing image if available)
bash scripts/build_helpers/docker_start.sh --fast-build --build-project-fast
```

**What this does:**
1. Builds (or reuses) the Docker image with all dependencies
2. Configures the project with CMake
3. Builds all modules (base, ui, examples)
4. Places outputs in `out/build_develop/`

**Docker Options:**

| Option | Description |
|:-------|:------------|
| `--fast-build` | Skip image cleanup, reuse existing image |
| `--build-project` | Full clean build |
| `--build-project-fast` | Fast incremental build |
| `--run-project-test` | Build and run tests |
| `--arch arm64` | Build for ARM64 architecture |
| `--verify` | Run CI-style verification build |

### Step 4: Run Example Programs

After successful build, run the example programs:

**Windows:**
```powershell
# Material Design Gallery
.\out\build_develop\examples\gui\material_gallery.exe

# Button Widget Example
.\out\build_develop\examples\ui\button.exe

# CPU Information Demo
.\out\build_develop\examples\base\cpu_info.exe
```

**Linux:**
```bash
# Material Design Gallery
./out/build_develop/examples/gui/material_gallery

# Button Widget Example
./out/build_develop/examples/ui/button

# CPU Information Demo
./out/build_develop/examples/base/cpu_info
```

**Available Examples:**

| Category | Examples | Description |
|:---------|:---------|:------------|
| **base/** | cpu_info, memory_info | System information demos |
| **ui/** | button, label, textfield, checkbox, radiobutton, textarea, groupbox | Material Design widgets |
| **gui/** | material_gallery, theme | Complete UI demos |

### Step 5: Run Tests

Verify your build by running the test suite:

**Windows:**
```powershell
.\scripts\build_helpers\windows_run_tests.ps1
```

**Linux:**
```bash
bash scripts/build_helpers/linux_run_tests.sh
```

**Docker:**
```bash
bash scripts/build_helpers/docker_start.sh --run-project-test
```

---

## Windows Users

### Using Git Bash

CFDesktop build scripts are designed for Unix-style shells. On Windows, use **Git Bash**:

1. Install [Git for Windows](https://git-scm.com/download/win)
2. Open Git Bash from the Start Menu
3. Navigate to your project directory:
   ```bash
   cd /d/ProjectHome/CFDesktop
   ```

### Using WSL2 (Alternative)

For better performance, use Windows Subsystem for Linux:

1. Install WSL2:
   ```powershell
   wsl --install
   ```

2. Access your Windows files from WSL:
   ```bash
   cd /mnt/d/ProjectHome/CFDesktop
   ```

### Path Format Conversion

When running scripts on Windows, be aware of path formats:

| Type | Format | Example |
|:-----|:-------|:--------|
| **Windows** | `D:\Path\To\File` | `D:/ProjectHome/CFDesktop` |
| **Git Bash** | `/d/Path/To/File` | `/d/ProjectHome/CFDesktop` |
| **Docker Mount** | `/d/Path/To/File` | `/d/ProjectHome/CFDesktop` |
| **PowerShell** | `D:\Path\To\File` | `D:\ProjectHome\CFDesktop` |

The build scripts automatically handle path conversion for Docker mounts.

### Windows Build Scripts

For native Windows builds (without Docker), use PowerShell scripts:

```powershell
# Configure and build (fast)
.\scripts\build_helpers\windows_fast_develop_build.ps1

# Configure and build (full clean)
.\scripts\build_helpers\windows_develop_build.ps1

# Configure only
.\scripts\build_helpers\windows_configure.ps1

# Run tests
.\scripts\build_helpers\windows_run_tests.ps1
```

---

## Verification

### Check Docker Installation

```bash
# Verify Docker is installed
docker --version

# Verify Docker daemon is running
docker info
```

Expected output:
```
Docker version 20.10.x
...
Server Version: 20.10.x
```

### Check Build Success

A successful build will create the following directory structure:

```
out/build_develop/
├── bin/
│   ├── cfbase.dll          # Base library (Windows)
│   ├── cfui.dll            # UI library (Windows)
│   └── ...
├── lib/
│   ├── libcfbase.a         # Static libraries (Linux)
│   └── ...
├── examples/
│   ├── base/               # Base examples
│   ├── ui/                 # UI widget examples
│   └── gui/                # GUI examples
├── runtimes/               # Qt runtime DLLs (Windows)
└── test/                   # Test executables
```

### Check Build Logs

Build logs are saved to `scripts/docker/logger/` when using Docker:

```bash
# List recent build logs
ls -lt scripts/docker/logger/

# View the latest log
cat scripts/docker/logger/ci_build_*.log | tail -50
```

---

## Next Steps

After completing the quick start:

1. **Read the Build System Documentation**: [`03_build_system.md`](03_build_system.md)
2. **Explore the Examples**: Browse `example/` directory for sample code
3. **Review Project Structure**: Check the [Project Skeleton Design](../design_stage/00_phase0_project_skeleton.md)
4. **Set Up Your Development Environment**: See [VSCode Configuration](../design_stage/00_phase0_project_skeleton.md#五开发环境配置)

---

## Troubleshooting

### Docker Build Failures

| Issue | Solution |
|:------|:----------|
| Docker daemon not running | Start Docker Desktop |
| Permission denied | Use `sudo` on Linux or run terminal as Administrator on Windows |
| Port conflicts | Ensure no other containers are using required ports |
| Out of memory | Increase Docker memory limit in settings |

### Build Errors

| Issue | Solution |
|:------|----------|
| Qt not found | Set `Qt6_DIR` environment variable or use Docker |
| Compiler not found | Install LLVM/Clang or GCC, or use Docker build |
| CMake version too old | Upgrade CMake to 3.16+ |
| Missing dependencies | Use Docker build which includes all dependencies |

### Windows-Specific Issues

| Issue | Solution |
|:------|----------|
| Path too long | Enable long path support in Windows or move project closer to drive root |
| PowerShell execution policy | Run `Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser` |
| Git Bash path issues | Use `/d/` format for D: drive |

### Getting Help

If you encounter issues not covered here:

1. Check the [Design Documentation](../../design_stage/) for detailed information
2. Review the [Build System Documentation](03_build_system.md)
3. Open an issue on [GitHub](https://github.com/Awesome-Embedded-Learning-Studio/CFDesktop/issues)

---

## Quick Reference

### Essential Commands

```bash
# Clone and setup
git clone https://github.com/Awesome-Embedded-Learning-Studio/CFDesktop.git
cd CFDesktop

# Docker build (fastest)
bash scripts/build_helpers/docker_start.sh --fast-build --build-project-fast

# Run tests
bash scripts/build_helpers/docker_start.sh --run-project-test

# Interactive shell in Docker
bash scripts/build_helpers/docker_start.sh

# Build specific architecture
bash scripts/build_helpers/docker_start.sh --arch arm64 --verify
```

### Configuration Files

| File | Purpose |
|:-----|:---------|
| `build_develop_config.ini` | Development build configuration |
| `build_deploy_config.ini` | Deployment build configuration |
| `build_ci_config.ini` | CI build configuration |

### Output Locations

| Build Type | Output Directory |
|:-----------|:-----------------|
| Development | `out/build_develop/` |
| Deployment | `out/build_deploy/` |
| CI | `out/build_ci/` |

---

**Last Updated**: 2026-03-07
