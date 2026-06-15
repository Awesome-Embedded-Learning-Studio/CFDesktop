# =============================================================================
# Git Hooks 安装脚本 (Windows PowerShell)
# =============================================================================
#
# 用法: .\scripts\release\hooks\install_hooks.ps1
#
# 推荐：直接 cmake configure 即自动设置 core.hooksPath，无需本脚本。
# 本脚本为 fallback（复制钩子到 .git/hooks/），仅用于不跑 cmake 的场景。
# 此脚本将钩子安装到 .git/hooks/ 目录
# =============================================================================

$ErrorActionPreference = "Stop"

# 导入路径模块
# $PSScriptRoot = scripts/release/hooks, 需要向上 3 级到项目根目录
$ProjectRootForLib = Split-Path -Parent (Split-Path -Parent (Split-Path -Parent $PSScriptRoot))
$LibDir = Join-Path $ProjectRootForLib "scripts\lib\powershell"
Import-Module (Join-Path $LibDir "LibPaths.psm1") -Force

# Set caller's PSScriptRoot for module functions to access
$global:CallerPSScriptRoot = $PSScriptRoot
$global:CallerMyInvocationPath = $MyInvocation.MyCommand.Path

# =============================================================================
# 颜色定义
# =============================================================================
$Green = "`e[0;32m"
$Blue = "`e[0;34m"
$Yellow = "`e[0;33m"
$Red = "`e[0;31m"
$Reset = "`e[0m"

# =============================================================================
# 路径设置
# =============================================================================
$ScriptDir = Get-ScriptDir
# $PSScriptRoot = scripts/release/hooks, 需要向上 3 级到项目根目录
$ProjectRoot = Split-Path -Parent (Split-Path -Parent (Split-Path -Parent $ScriptDir))
$HooksSourceDir = $ScriptDir
$HooksTargetDir = "$ProjectRoot\.git\hooks"

# =============================================================================
# 日志函数
# =============================================================================
function Log-Info {
    param([string]$Message)
    Write-Host "${Blue}>>>${Reset} $Message"
}

function Log-Success {
    param([string]$Message)
    Write-Host "${Green}✓${Reset} $Message"
}

function Log-Warning {
    param([string]$Message)
    Write-Host "${Yellow}⚠${Reset} $Message"
}

function Log-Error {
    param([string]$Message)
    Write-Host "${Red}✗${Reset} $Message"
}

# =============================================================================
# 检查是否在 Git 仓库中
# =============================================================================
if (-not (Test-Path "$ProjectRoot\.git")) {
    Log-Error "未找到 .git 目录"
    Write-Host ""
    Write-Host "请确认你在 Git 仓库中运行此脚本"
    exit 1
}

# =============================================================================
# 打印标题
# =============================================================================
Write-Host ""
Log-Info "========================================"
Log-Info "Git Hooks 安装 (Windows)"
Log-Info "========================================"
Write-Host ""

# =============================================================================
# 显示安装信息
# =============================================================================
Write-Host "项目根目录: $ProjectRoot"
Write-Host "钩子源目录: $HooksSourceDir"
Write-Host "钩子目标目录: $HooksTargetDir"
Write-Host ""

# =============================================================================
# 确认安装
# =============================================================================
$confirmation = Read-Host "是否继续安装? [Y/n]"
if ($confirmation -ne "" -and $confirmation -ne "Y" -and $confirmation -ne "y") {
    Log-Warning "安装已取消"
    exit 0
}

# =============================================================================
# 备份现有钩子
# =============================================================================
function Backup-ExistingHook {
    param([string]$HookName)

    $targetHook = "$HooksTargetDir\$HookName"

    if (Test-Path $targetHook) {
        # 检查是否是我们之前安装的钩子
        $content = Get-Content $targetHook -Raw -ErrorAction SilentlyContinue
        if ($content -match "CFDesktop Git Hooks") {
            # 是我们安装的，直接覆盖，不备份
            return
        }

        # 是用户自定义的钩子，需要备份
        $backupFile = "$targetHook.backup.$(Get-Date -Format 'yyyyMMddHHmmss')"
        Log-Warning "发现现有钩子: $HookName"
        Log-Warning "备份到: $backupFile"
        Copy-Item $targetHook $backupFile
    }
}

# =============================================================================
# 安装单个钩子
# =============================================================================
function Install-Hook {
    param(
        [string]$SampleName,
        [string]$HookName
    )

    $sourceHook = "$HooksSourceDir\$SampleName"
    $targetHook = "$HooksTargetDir\$HookName"

    # 检查源文件是否存在
    if (-not (Test-Path $sourceHook)) {
        Log-Warning "$SampleName 不存在，跳过"
        return
    }

    # 备份现有钩子
    Backup-ExistingHook $HookName

    # 复制钩子（Git 会自动处理执行权限）
    Copy-Item $sourceHook $targetHook -Force

    Log-Success "$HookName 已安装"
}

# =============================================================================
# 安装所有钩子
# =============================================================================
Write-Host ""
Log-Info "安装 Git Hooks..."
Write-Host ""

# 安装 pre-commit
Install-Hook "pre-commit" "pre-commit"

# 安装 pre-push
Install-Hook "pre-push" "pre-push"

# =============================================================================
# 完成
# =============================================================================
Write-Host ""
Log-Success "========================================"
Log-Success "Git Hooks 安装完成！"
Log-Success "========================================"
Write-Host ""
Write-Host "已安装的钩子:"
Write-Host "  - pre-commit:  代码格式检查（所有分支）"
Write-Host "  - pre-push:    Docker 构建验证（仅 main 和 release 分支）"
Write-Host ""
Write-Host "验证安装:"
Write-Host "  dir .git\hooks\pre-commit.*"
Write-Host ""
Write-Host "卸载方法:"
Write-Host "  Remove-Item .git\hooks\pre-commit, .git\hooks\pre-push"
Write-Host ""
Write-Host "更多信息请参考: scripts/release/hooks/README.md"
Write-Host ""
