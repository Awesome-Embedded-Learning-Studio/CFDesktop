# This script ONLY configures the project using CMake
# It does NOT build the project
# Usage: .\windows_configure.ps1 [-Config <develop|deploy>]
param(
    [Parameter(Position=0)]
    [ValidateSet("develop", "deploy")]
    [string]$Config = "develop"
)

# 导入库模块
$LibDir = Join-Path (Split-Path -Parent $PSScriptRoot) "lib\powershell"
Import-Module (Join-Path $LibDir "LibCommon.psm1") -Force
Import-Module (Join-Path $LibDir "LibConfig.psm1") -Force
Import-Module (Join-Path $LibDir "LibPaths.psm1") -Force

$ErrorActionPreference = "Stop"

Write-LogSeparator
Write-LogInfo "Starting Windows CMake Configuration"
Write-LogInfo "Configuration: $Config"
Write-LogSeparator

# Set caller's PSScriptRoot for module functions to access
$global:CallerPSScriptRoot = $PSScriptRoot
$global:CallerMyInvocationPath = $MyInvocation.MyCommand.Path

# Get the script directory and project root using library functions
$ScriptDir = Get-ScriptDir
$ProjectRoot = Get-ProjectRoot

Write-LogInfo "Project root: $ProjectRoot"
Write-LogInfo "Changing to project directory"

Set-Location $ProjectRoot

# Determine which config file to use
if ($Config -eq "deploy") {
    $ConfigFile = Join-Path $ScriptDir "build_deploy_config.ini"
}
else {
    $ConfigFile = Join-Path $ScriptDir "build_develop_config.ini"
}

Write-LogInfo "Loading configuration from: $ConfigFile"

# Safety check: config file must exist
if (!(Test-Path $ConfigFile)) {
    Write-LogError "Configuration file not found: $ConfigFile"
    Write-LogError "Please create the configuration file from the .template file"
    $templateFile = "$ConfigFile.template"
    if (Test-Path $templateFile) {
        Write-LogError "Example: Copy-Item '$templateFile' '$ConfigFile'"
    }
    exit 1
}

try {
    $ConfigData = Get-IniConfig -FilePath $ConfigFile
    Write-LogSuccess "Configuration loaded successfully!"
}
catch {
    Write-LogError "Failed to load configuration: $_"
    exit 1
}

# Extract configuration values
$Generator = $ConfigData["cmake"]["generator"]
$Toolchain = $ConfigData["cmake"]["toolchain"]
$BuildType = $ConfigData["cmake"]["build_type"]
if (-not $BuildType) {
    Write-LogError "ERROR: build_type not specified in config file"
    exit 1
}

# Validate BuildType value
$ValidBuildTypes = @("Debug", "Release", "RelWithDebInfo")
if ($BuildType -notin $ValidBuildTypes) {
    Write-LogError "ERROR: Invalid build_type '$BuildType'. Must be one of: $($ValidBuildTypes -join ', ')"
    exit 1
}

$SourceDir = $ConfigData["paths"]["source"]
$BuildDir = $ConfigData["paths"]["build_dir"]

# Safety check: BUILD_DIR must not be empty
if ([string]::IsNullOrWhiteSpace($BuildDir)) {
    Write-LogError "Configuration error: build_dir is not set in config file"
    Write-LogError "Please check the [paths] section in: $ConfigFile"
    exit 1
}

# Safety check: BUILD_DIR must not be project root
if ($BuildDir -eq "." -or $BuildDir -eq "/" -or $BuildDir -eq "\") {
    Write-LogError "Configuration error: build_dir cannot be project root or root directory"
    Write-LogError "Current value: $BuildDir"
    exit 1
}

# Resolve source directory: if relative, make it relative to project root
if ([System.IO.Path]::IsPathRooted($SourceDir)) {
    $ResolvedSourceDir = $SourceDir
}
else {
    $ResolvedSourceDir = Join-Path $ProjectRoot $SourceDir
    $ResolvedSourceDir = (Resolve-Path -Path $ResolvedSourceDir -ErrorAction SilentlyContinue).Path
    if (-not $ResolvedSourceDir) {
        $ResolvedSourceDir = Join-Path $ProjectRoot $SourceDir
    }
}

Write-LogInfo "Generator: $Generator"
Write-LogInfo "Toolchain: $Toolchain"
Write-LogInfo "Build Type: $BuildType"
Write-LogInfo "Source directory: $SourceDir (resolved: $ResolvedSourceDir)"
Write-LogInfo "Build directory: $BuildDir"

# Configure with CMake
Write-LogSeparator
Write-LogInfo "Configuring with CMake (NO BUILD)"
Write-LogInfo "Command: cmake -G $Generator -DUSE_TOOLCHAIN=$Toolchain -DCMAKE_BUILD_TYPE=$BuildType -S $ResolvedSourceDir -B $BuildDir"
Write-LogSeparator

$cmakeConfigArgs = @(
    "-G", $Generator,
    "-DUSE_TOOLCHAIN=$Toolchain",
    "-DCMAKE_BUILD_TYPE=$BuildType",
    "-S", $ResolvedSourceDir,
    "-B", $BuildDir
)

try {
    & cmake @cmakeConfigArgs
    if ($LASTEXITCODE -eq 0) {
        Write-LogSeparator
        Write-LogSuccess "CMake configuration completed successfully!"
        Write-LogInfo "To build the project, run: cmake --build $BuildDir"
        Write-LogSeparator
    }
    else {
        Write-LogError "CMake configuration failed with exit code: $LASTEXITCODE"
        exit $LASTEXITCODE
    }
}
catch {
    Write-LogError "Error during CMake configuration: $_"
    exit 1
}
