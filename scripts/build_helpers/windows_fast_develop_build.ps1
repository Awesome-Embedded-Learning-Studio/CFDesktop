# This script configures and builds the project (FAST version - no cleaning)
# It calls the configure script first, then builds

# 导入库模块
$LibDir = Join-Path (Split-Path -Parent $PSScriptRoot) "lib\powershell"
Import-Module (Join-Path $LibDir "LibCommon.psm1") -Force
Import-Module (Join-Path $LibDir "LibConfig.psm1") -Force
Import-Module (Join-Path $LibDir "LibPaths.psm1") -Force
Import-Module (Join-Path $LibDir "LibBuild.psm1") -Force

$ErrorActionPreference = "Stop"

# Set caller's PSScriptRoot for module functions to access
$global:CallerPSScriptRoot = $PSScriptRoot
$global:CallerMyInvocationPath = $MyInvocation.MyCommand.Path

Write-LogSeparator
Write-LogInfo "Starting Windows FAST Build Process"
Write-LogSeparator

# Get the script directory and project root using library functions
$ScriptDir = Get-ScriptDir
$ProjectRoot = Get-ProjectRoot

Write-LogInfo "Project root: $ProjectRoot"
Set-Location $ProjectRoot

# Start build timer
Start-BuildTimer

# Step 1: Call the configure script
Write-LogSeparator
Write-LogInfo "Step 1: Configuring with CMake"
Write-LogSeparator

$ConfigureScript = Join-Path $ScriptDir "windows_configure.ps1"
Write-LogInfo "Executing: $ConfigureScript -Config develop"

try {
    & $ConfigureScript -Config "develop"
    if ($LASTEXITCODE -ne 0) {
        Write-LogError "Configure script failed with exit code: $LASTEXITCODE"
        Stop-BuildTimer
        exit $LASTEXITCODE
    }
}
catch {
    Write-LogError "Error during configuration: $_"
    Stop-BuildTimer
    exit 1
}

# Step 2: Load config for build
$ConfigFile = Join-Path $ScriptDir "build_develop_config.ini"

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

$Config = Get-IniConfig -FilePath $ConfigFile
$BuildDir = $Config["paths"]["build_dir"]

# Safety check: BUILD_DIR must not be empty
if ([string]::IsNullOrWhiteSpace($BuildDir)) {
    Write-LogError "Configuration error: build_dir is not set in config file"
    Write-LogError "Please check the [paths] section in: $ConfigFile"
    exit 1
}

$Jobs = if ($Config["options"] -and $Config["options"]["jobs"]) { $Config["options"]["jobs"] } else { "" }

# Step 3: Build with CMake
Write-LogSeparator
Write-LogInfo "Step 2: Building project"

$buildArgs = @("--build", $BuildDir)
if ($Jobs) {
    $buildArgs += "--parallel", $Jobs
    Write-LogInfo "Command: cmake --build $BuildDir --parallel $Jobs"
}
else {
    Write-LogInfo "Command: cmake --build $BuildDir"
}
Write-LogSeparator

try {
    & cmake @buildArgs
    if ($LASTEXITCODE -eq 0) {
        Write-LogSeparator
        Write-LogSuccess "Build completed successfully!"
        Write-LogSeparator
    }
    else {
        Write-LogError "Build failed with exit code: $LASTEXITCODE"
        Stop-BuildTimer
        exit $LASTEXITCODE
    }
}
catch {
    Write-LogError "Error during build: $_"
    Stop-BuildTimer
    exit 1
}

# Stop build timer
Stop-BuildTimer
