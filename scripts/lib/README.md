# CFDesktop Scripts Library

CFDesktop 脚本公共库，提供可复用的 Bash 和 PowerShell 函数模块。

## 目录结构

```
scripts/
├── lib/
│   ├── bash/                    # Bash 库模块
│   │   ├── lib_common.sh        # 日志、颜色、通用工具
│   │   ├── lib_paths.sh         # 路径解析
│   │   ├── lib_config.sh        # INI 配置解析
│   │   ├── lib_args.sh          # 参数解析辅助
│   │   ├── lib_build.sh         # 构建相关工具
│   │   └── lib_git.sh           # Git 工具
│   │
│   └── powershell/              # PowerShell 库模块
│       ├── LibCommon.ps1        # 日志、通用工具
│       ├── LibPaths.ps1         # 路径解析
│       ├── LibConfig.ps1        # INI 配置解析
│       ├── LibArgs.ps1          # 参数解析辅助
│       ├── LibBuild.ps1         # 构建相关工具
│       └── LibGit.ps1           # Git 工具
│
test/scripts/lib/
├── bash/                        # Bash 单元测试
│   ├── test_lib_common.sh
│   ├── test_lib_config.sh
│   └── test_lib_git.sh
│
└── powershell/                  # PowerShell 单元测试
    ├── LibCommon.Tests.ps1
    └── LibConfig.Tests.ps1
```

## Bash 库使用

### 加载库

```bash
#!/bin/bash

# 获取库目录
SCRIPT_LIB="$(cd "$(dirname "${BASH_SOURCE[0]}")/../lib/bash" && pwd)"

# 加载需要的模块
source "$SCRIPT_LIB/lib_common.sh"
source "$SCRIPT_LIB/lib_paths.sh"
source "$SCRIPT_LIB/lib_config.sh"
```

### lib_common.sh - 日志和通用工具

```bash
# 日志函数
log_info "This is an info message"
log_success "Operation completed successfully"
log_warn "This is a warning"
log_error "An error occurred"

# 分隔线
log_separator
log_separator "-" 60

# 进度显示
log_progress 5 10 "Processing files"
```

### lib_paths.sh - 路径解析

```bash
# 获取路径
script_dir=$(get_script_dir)
project_root=$(get_project_root)
scripts_dir=$(get_scripts_dir)
lib_dir=$(get_lib_dir)

# 环境变量（source 时自动设置）
echo $SCRIPT_DIR
echo $PROJECT_ROOT
echo $SCRIPTS_DIR
echo $LIB_DIR

# 路径工具
ensure_dir "/path/to/directory"
if path_exists "/some/path"; then
    echo "Path exists"
fi
```

### lib_config.sh - INI 配置解析

```bash
# 解析 INI 文件
eval "$(get_ini_config /path/to/config.ini)"
echo "$config_cmake_generator"
echo "$config_paths_build_dir"

# 获取单个值
value=$(get_ini_value /path/to/config.ini "cmake" "generator")

# 检查值是否存在
if has_ini_value /path/to/config.ini "cmake" "generator"; then
    echo "Generator is defined"
fi

# 获取默认配置文件
config_file=$(get_default_config_file "develop")
```

### lib_args.sh - 参数解析

```bash
# 解析配置模式
if is_valid_config_mode "$1"; then
    CONFIG=$(parse_config_mode "$1")
fi

# 显示帮助
show_standard_usage "$0"
show_detailed_usage "$0" "Build the project"

# 错误处理
show_unknown_arg_error "$1"
show_missing_value_error "--config"
```

### lib_build.sh - 构建工具

```bash
# 清理构建目录
clean_build_dir "$BUILD_DIR"

# 运行 CMake 配置
run_cmake_configure "Ninja" "Release" "$SOURCE_DIR" "$BUILD_DIR"

# 运行 CMake 构建
run_cmake_build "$BUILD_DIR" "--all" $(get_parallel_job_count)

# 构建计时
build_timer_start
# ... 执行构建 ...
build_timer_end
```

### lib_git.sh - Git 工具

```bash
# 版本号解析
major=$(get_major_version "1.2.3")   # 输出: 1
minor=$(get_minor_version "1.2.3")   # 输出: 2
patch=$(get_patch_version "1.2.3")   # 输出: 3

# 比较版本
compare_versions "1.2.3" "1.2.4"     # 返回: 2 (第一个小于第二个)

# Git 版本标签
local_version=$(get_local_version)
remote_version=$(get_remote_version)

# 验证级别
verify_level=$(determine_verify_level "$local_version" "$remote_version")
description=$(get_verify_level_description "$verify_level")

# Git 状态
if has_uncommitted_changes; then
    log_warn "There are uncommitted changes"
fi

branch=$(get_current_branch)
if is_on_main_branch; then
    log_info "Currently on main branch"
fi
```

## PowerShell 库使用

### 加载库

```powershell
# 获取库目录
$LibDir = Join-Path (Split-Path -Parent $MyInvocation.MyCommand.Path) "..\lib\powershell"

# 加载需要的模块
. "$LibDir\LibCommon.ps1"
. "$LibDir\LibPaths.ps1"
. "$LibDir\LibConfig.ps1"
```

### LibCommon.ps1 - 日志和通用工具

```powershell
# 日志函数
Write-LogInfo "This is an info message"
Write-LogSuccess "Operation completed successfully"
Write-LogWarning "This is a warning"
Write-LogError "An error occurred"

# 分隔线
Write-LogSeparator
Write-LogSeparator -Char "-" -Width 60

# 进度显示
Write-LogProgress -Current 5 -Total 10 -Message "Processing files"
```

### LibPaths.ps1 - 路径解析

```powershell
# 获取路径
$scriptDir = Get-ScriptDir
$projectRoot = Get-ProjectRoot
$scriptsDir = Get-ScriptsDir
$libDir = Get-LibDir

# 路径工具
if (Test-PathExistence "C:\Build") { ... }
New-Directory "C:\Build\Output"

# 解析相对路径
$absolutePath = ConvertTo-AbsolutePath "..\Build" $scriptDir
```

### LibConfig.ps1 - INI 配置解析

```powershell
# 解析 INI 文件
$config = Get-IniConfig -FilePath "config.ini"
$generator = $config["cmake"]["generator"]

# 获取单个值
$value = Get-IniValue -FilePath "config.ini" -Section "cmake" -Key "generator"

# 检查值是否存在
if (Test-IniValue -FilePath "config.ini" -Section "cmake" -Key "generator") {
    Write-Host "Generator is defined"
}

# 获取默认配置文件
$configFile = Get-DefaultConfigFile -Mode "develop"
```

### LibBuild.ps1 - 构建工具

```powershell
# 清理构建目录
Clean-BuildDir "C:\Build\Output"

# 运行 CMake 配置
Invoke-CMakeConfigure -Generator "Ninja" -BuildType "Release" -SourceDir "." -BuildDir "build"

# 运行 CMake 构建
Invoke-CMakeBuild -BuildDir "build" -Parallel (Get-ParallelJobCount)

# 构建计时
Start-BuildTimer
# ... 执行构建 ...
Stop-BuildTimer
```

### LibGit.ps1 - Git 工具

```powershell
# 版本号解析
$major = Get-MajorVersion -Version "1.2.3"   # 输出: 1
$minor = Get-MinorVersion -Version "1.2.3"   # 输出: 2
$patch = Get-PatchVersion -Version "1.2.3"   # 输出: 3

# 比较版本
$result = Compare-Versions -Version1 "1.2.3" -Version2 "1.2.4"
# 返回: -1 (第一个小于第二个)

# Git 版本标签
$localVersion = Get-LocalVersion
$remoteVersion = Get-RemoteVersion

# 验证级别
$verifyLevel = Determine-VerifyLevel -LocalVersion $localVersion -RemoteVersion $remoteVersion
$description = Get-VerifyLevelDescription -Level $verifyLevel

# Git 状态
if (Test-UncommittedChanges) {
    Write-LogWarning "There are uncommitted changes"
}

$branch = Get-CurrentBranch
if (Test-MainBranch) {
    Write-LogInfo "Currently on main branch"
}
```

## 单元测试

### Bash 测试（使用 bash-unit）

```bash
# 安装 bash-unit
git clone https://github.com/bash-unit/bash_unit.git

# 运行测试
bash-unit test/scripts/lib/bash/test_lib_common.sh
bash-unit test/scripts/lib/bash/test_lib_config.sh
bash-unit test/scripts/lib/bash/test_lib_git.sh
```

### PowerShell 测试（使用 Pester）

```powershell
# 安装 Pester
Install-Module -Name Pester -Force

# 运行测试
Invoke-Pester test/scripts/lib/powershell/LibCommon.Tests.ps1
Invoke-Pester test/scripts/lib/powershell/LibConfig.Tests.ps1
```

## 迁移现有脚本

### 迁移前示例

```bash
#!/bin/bash
set -e

# 日志函数（~27 行重复代码）
log() {
    local level="$2"
    local message="$1"
    local timestamp=$(date "+%Y-%m-%d %H:%M:%S")
    # ... 更多代码
}

# INI 解析函数（~40 行重复代码）
get_ini_config() {
    # ... 大量代码
}

# 路径解析
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
```

### 迁移后示例

```bash
#!/bin/bash
set -e

# 加载公共库（3 行代替 ~70 行重复代码）
SCRIPT_LIB="$(cd "$(dirname "${BASH_SOURCE[0]}")/../lib/bash" && pwd)"
source "$SCRIPT_LIB/lib_common.sh"
source "$SCRIPT_LIB/lib_paths.sh"
source "$SCRIPT_LIB/lib_config.sh"

# 直接使用库函数
log_info "Starting build"
eval "$(get_ini_config "$CONFIG_FILE")"
```

## 代码收益

| 指标 | 重构前 | 重构后 | 改进 |
|------|--------|--------|------|
| 单脚本平均行数 | ~180 行 | ~100 行 | -44% |
| 日志函数重复 | 25 次 | 1 次 | -96% |
| 配置解析重复 | 15 次 | 1 次 | -93% |
| 总重复代码 | ~2,400 行 | ~600 行 | -75% |
