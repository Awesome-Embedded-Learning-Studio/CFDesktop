# =============================================================================
# CFDesktop Scripts Library - Build Module (PowerShell)
# =============================================================================
#
# 提供构建相关的辅助函数
#
# 用法:
#   . "$PSScriptRoot\LibCommon.ps1"
#   . "$PSScriptRoot\LibBuild.ps1"
#   Clean-BuildDir $BUILD_DIR
#
# 注意: 此模块依赖 LibCommon.ps1
#
# =============================================================================

function Clean-BuildDir {
    <#
    .SYNOPSIS
        清理构建目录（带安全检查）

    .PARAMETER BuildPath
        构建目录路径

    .EXAMPLE
        Clean-BuildDir "C:\Build\Output"
    #>
    param(
        [Parameter(Mandatory=$true)]
        [string]$BuildPath
    )

    # 安全检查：路径不能为空
    if ([string]::IsNullOrWhiteSpace($BuildPath)) {
        Write-LogError "ERROR: Build path is empty! Refusing to clean to prevent accidental deletion."
        return $false
    }

    # 安全检查：路径不能是根目录
    $resolvedPath = Resolve-Path -LiteralPath $BuildPath -ErrorAction SilentlyContinue
    if ($resolvedPath) {
        $normalizedPath = $resolvedPath.Path
    } else {
        $normalizedPath = $BuildPath
    }
    if ($normalizedPath -eq "" -or $normalizedPath -eq "/" -or $normalizedPath -eq "\") {
        Write-LogError "ERROR: Refusing to clean root directory!"
        return $false
    }

    # 安全检查：路径不能是用户主目录
    $homePath = $env:USERPROFILE
    if ($normalizedPath -eq $homePath -or $normalizedPath -eq "$homePath\") {
        Write-LogError "ERROR: Refusing to clean home directory!"
        return $false
    }

    # 安全检查：路径不能是项目根目录（如果已设置）
    if ($global:ProjectRoot -and ($normalizedPath -eq $global:ProjectRoot -or $normalizedPath -eq "$global:ProjectRoot\")) {
        Write-LogError "ERROR: Refusing to clean project root directory!"
        return $false
    }

    # 安全检查：路径名称必须匹配预期模式
    $dirName = Split-Path -Leaf $BuildPath
    $validPatterns = @("build", "out", "cmake-build", "build-")
    $patternMatches = $false
    foreach ($pattern in $validPatterns) {
        if ($dirName -like "$pattern*") {
            $patternMatches = $true
            break
        }
    }

    if (-not $patternMatches) {
        Write-LogWarning "WARNING: Build directory name '$dirName' does not match expected pattern. Aborting for safety."
        Write-LogWarning "Expected patterns: build, out, cmake-build, build-*"
        return $false
    }

    if (Test-Path $BuildPath) {
        Write-LogInfo "Removing build directory: $BuildPath"
        try {
            Remove-Item -Path $BuildPath -Recurse -Force
            Write-LogSuccess "Build directory cleaned successfully"
            return $true
        }
        catch {
            Write-LogError "Failed to clean build directory: $_"
            return $false
        }
    }
    else {
        Write-LogInfo "Build directory does not exist, skipping clean"
        return $true
    }
}

function Ensure-BuildDir {
    <#
    .SYNOPSIS
        确保构建目录存在

    .PARAMETER BuildPath
        构建目录路径

    .EXAMPLE
        Ensure-BuildDir "C:\Build\Output"
    #>
    param(
        [Parameter(Mandatory=$true)]
        [string]$BuildPath
    )

    if (!(Test-Path -Path $BuildPath -PathType Container)) {
        Write-LogInfo "Creating build directory: $BuildPath"
        New-Item -ItemType Directory -Path $BuildPath -Force | Out-Null
    }
}

function Invoke-CMakeConfigure {
    <#
    .SYNOPSIS
        运行 CMake 配置

    .PARAMETER Generator
        生成器名称 (如 "Ninja", "Visual Studio 17 2022")

    .PARAMETER BuildType
        构建类型 (Debug, Release, RelWithDebInfo)

    .PARAMETER SourceDir
        源代码目录

    .PARAMETER BuildDir
        构建目录

    .PARAMETER ExtraArgs
        额外的 CMake 参数（可选）

    .EXAMPLE
        Invoke-CMakeConfigure -Generator "Ninja" -BuildType "Release" -SourceDir "." -BuildDir "build"
    #>
    param(
        [Parameter(Mandatory=$true)]
        [string]$Generator,
        [Parameter(Mandatory=$true)]
        [ValidateSet("Debug", "Release", "RelWithDebInfo")]
        [string]$BuildType,
        [Parameter(Mandatory=$true)]
        [string]$SourceDir,
        [Parameter(Mandatory=$true)]
        [string]$BuildDir,
        [string[]]$ExtraArgs = @()
    )

    Write-LogInfo "Configuring with CMake..."
    Write-LogInfo "  Generator: $Generator"
    Write-LogInfo "  Build Type: $BuildType"
    Write-LogInfo "  Source: $SourceDir"
    Write-LogInfo "  Build: $BuildDir"

    $cmakeArgs = @(
        "-G", $Generator,
        "-DCMAKE_BUILD_TYPE=$BuildType",
        "-S", $SourceDir,
        "-B", $BuildDir
    ) + $ExtraArgs

    try {
        & cmake @cmakeArgs
        if ($LASTEXITCODE -eq 0) {
            Write-LogSuccess "CMake configuration completed successfully"
            return $true
        }
        else {
            Write-LogError "CMake configuration failed with exit code: $LASTEXITCODE"
            return $false
        }
    }
    catch {
        Write-LogError "Error during CMake configuration: $_"
        return $false
    }
}

function Invoke-CMakeBuild {
    <#
    .SYNOPSIS
        运行 CMake 构建

    .PARAMETER BuildDir
        构建目录

    .PARAMETER Target
        目标名称（可选，默认构建所有）

    .PARAMETER Parallel
        并行作业数（可选，默认自动）

    .EXAMPLE
        Invoke-CMakeBuild -BuildDir "build"
    #>
    param(
        [Parameter(Mandatory=$true)]
        [string]$BuildDir,
        [string]$Target = "--all",
        [int]$Parallel = 0
    )

    Write-LogInfo "Building with CMake..."
    Write-LogInfo "  Build directory: $BuildDir"

    $buildArgs = @("--build", $BuildDir, "--target", $Target)

    if ($Parallel -gt 0) {
        Write-LogInfo "  Parallel jobs: $Parallel"
        $buildArgs += "--parallel", $Parallel
    }

    try {
        & cmake @buildArgs
        if ($LASTEXITCODE -eq 0) {
            Write-LogSuccess "Build completed successfully"
            return $true
        }
        else {
            Write-LogError "Build failed with exit code: $LASTEXITCODE"
            return $false
        }
    }
    catch {
        Write-LogError "Error during build: $_"
        return $false
    }
}

function Test-CmakeCache {
    <#
    .SYNOPSIS
        检查 CMake 缓存是否存在

    .PARAMETER BuildDir
        构建目录

    .OUTPUTS
        Boolean: True 表示缓存存在

    .EXAMPLE
        if (Test-CmakeCache "build") { ... }
    #>
    param(
        [Parameter(Mandatory=$true)]
        [string]$BuildDir
    )

    Test-Path (Join-Path $BuildDir "CMakeCache.txt")
}

function Get-CmakeCacheVar {
    <#
    .SYNOPSIS
        获取 CMake 缓存中的变量值

    .PARAMETER BuildDir
        构建目录

    .PARAMETER VarName
        变量名

    .OUTPUTS
        变量值，如果未找到则返回空字符串

    .EXAMPLE
        $generator = Get-CmakeCacheVar -BuildDir "build" -VarName "CMAKE_GENERATOR"
    #>
    param(
        [Parameter(Mandatory=$true)]
        [string]$BuildDir,
        [Parameter(Mandatory=$true)]
        [string]$VarName
    )

    $cacheFile = Join-Path $BuildDir "CMakeCache.txt"

    if (Test-Path $cacheFile) {
        $line = Select-String -Path $cacheFile -Pattern "^${VarName}:" | Select-Object -First 1
        if ($line) {
            $parts = $line.Line -split '=', 2
            if ($parts.Length -eq 2) {
                return $parts[1]
            }
        }
    }

    return ""
}

function Get-ParallelJobCount {
    <#
    .SYNOPSIS
        获取推荐的并行作业数

    .OUTPUTS
        CPU 核心数

    .EXAMPLE
        $jobs = Get-ParallelJobCount
    #>
    return [System.Environment]::ProcessorCount
}

# 构建计时相关变量和函数
$script:BuildStartTime = $null

function Start-BuildTimer {
    <#
    .SYNOPSIS
        开始构建计时

    .EXAMPLE
        Start-BuildTimer
    #>
    $script:BuildStartTime = Get-Date
}

function Stop-BuildTimer {
    <#
    .SYNOPSIS
        结束构建计时并显示耗时

    .EXAMPLE
        Stop-BuildTimer
    #>
    if ($null -ne $script:BuildStartTime) {
        $endTime = Get-Date
        $duration = $endTime - $script:BuildStartTime
        $minutes = [int]$duration.TotalMinutes
        $seconds = $duration.Seconds
        Write-LogInfo "Build time: ${minutes}m ${seconds}s"
        $script:BuildStartTime = $null
    }
}

# 导出函数
Export-ModuleMember -Function `
    Clean-BuildDir, `
    Ensure-BuildDir, `
    Invoke-CMakeConfigure, `
    Invoke-CMakeBuild, `
    Test-CmakeCache, `
    Get-CmakeCacheVar, `
    Get-ParallelJobCount, `
    Start-BuildTimer, `
    Stop-BuildTimer
