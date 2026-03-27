# This script runs CMake tests using CTest
# It reads the build directory from the specified config file

param(
    [Parameter(Mandatory = $false)]
    [string]$Config = "develop"
)

# 导入库模块
$LibDir = Join-Path (Split-Path -Parent $PSScriptRoot) "lib\powershell"
Import-Module (Join-Path $LibDir "LibCommon.psm1") -Force
Import-Module (Join-Path $LibDir "LibConfig.psm1") -Force
Import-Module (Join-Path $LibDir "LibPaths.psm1") -Force

$ErrorActionPreference = "Stop"

# Set caller's PSScriptRoot for module functions to access
$global:CallerPSScriptRoot = $PSScriptRoot
$global:CallerMyInvocationPath = $MyInvocation.MyCommand.Path

Write-LogSeparator
Write-LogInfo "Running Tests (Config: $Config)"
Write-LogSeparator

# Get the script directory and project root using library functions
$ScriptDir = Get-ScriptDir
$ProjectRoot = Get-ProjectRoot

Write-LogInfo "Project root: $ProjectRoot"
Set-Location $ProjectRoot

# Determine which config file to use
$ConfigFile = switch ($Config) {
    "develop" { "build_develop_config.ini" }
    "deploy" { "build_deploy_config.ini" }
    default {
        Write-LogError "Unknown config: $Config. Valid options are: develop, deploy"
        exit 1
    }
}

$ConfigFilePath = Join-Path $ScriptDir $ConfigFile
Write-LogInfo "Loading configuration from: $ConfigFilePath"

# Safety check: config file must exist
if (!(Test-Path $ConfigFilePath)) {
    Write-LogError "Configuration file not found: $ConfigFilePath"
    Write-LogError "Please create the configuration file from the .template file"
    $templateFile = "$ConfigFilePath.template"
    if (Test-Path $templateFile) {
        Write-LogError "Example: Copy-Item '$templateFile' '$ConfigFilePath'"
    }
    exit 1
}

try {
    $ConfigData = Get-IniConfig -FilePath $ConfigFilePath
    Write-LogSuccess "Configuration loaded successfully!"
}
catch {
    Write-LogError "Failed to load configuration: $_"
    exit 1
}

# Get build directory from config
$BuildDir = $ConfigData["paths"]["build_dir"]

# Safety check: BUILD_DIR must not be empty
if ([string]::IsNullOrWhiteSpace($BuildDir)) {
    Write-LogError "Configuration error: build_dir is not set in config file"
    Write-LogError "Please check the [paths] section in: $ConfigFilePath"
    exit 1
}

$BuildDir = Join-Path (Join-Path $ProjectRoot $BuildDir) "test"

Write-LogInfo "Test directory: $BuildDir"
Write-LogInfo "Command: ctest --test-dir $BuildDir --output-on-failure"

# Check if build directory exists
if (-not (Test-Path $BuildDir)) {
    Write-LogError "Build directory does not exist: $BuildDir"
    Write-LogError "Please run the build script first before running tests."
    exit 1
}

# Run tests
Write-LogSeparator
Write-LogInfo "Running tests..."
Write-LogSeparator

try {
    & ctest --test-dir $BuildDir --output-on-failure
    $exitCode = $LASTEXITCODE

    if ($exitCode -eq 0) {
        Write-LogSeparator
        Write-LogSuccess "All tests passed successfully!"
        Write-LogSeparator
        exit 0
    }
    else {
        Write-LogSeparator
        Write-LogWarning "Some tests failed with exit code: $exitCode"
        Write-LogSeparator
        exit $exitCode
    }
}
catch {
    Write-LogError "Error during test execution: $_"
    exit 1
}
