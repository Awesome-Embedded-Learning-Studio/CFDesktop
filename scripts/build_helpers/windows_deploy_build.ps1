# This script cleans the build directory then calls the fast version to build
$ErrorActionPreference = "Stop"

# Function to read INI configuration file
function Get-IniConfig {
    param(
        [string]$FilePath
    )

    $config = @{}
    $currentSection = ""

    if (Test-Path $FilePath) {
        Get-Content $FilePath | ForEach-Object {
            $line = $_.Trim()

            # Skip empty lines and comments
            if ([string]::IsNullOrEmpty($line) -or $line.StartsWith("#") -or $line.StartsWith(";")) {
                return
            }

            # Section header
            if ($line -match '^\[([^\]]+)\]$') {
                $currentSection = $matches[1]
                if (-not $config.ContainsKey($currentSection)) {
                    $config[$currentSection] = @{}
                }
                return
            }

            # Key=value pair
            if ($line -match '^([^=]+)=(.*)$') {
                $key = $matches[1].Trim()
                $value = $matches[2].Trim()

                if ($currentSection -and $config.ContainsKey($currentSection)) {
                    $config[$currentSection][$key] = $value
                }
            }
        }
    }
    else {
        throw "Configuration file not found: $FilePath"
    }

    return $config
}

# Log function
function Write-Log {
    param(
        [string]$Message,
        [string]$Level = "INFO"
    )
    $timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
    $color = switch ($Level) {
        "INFO" { "Cyan" }
        "SUCCESS" { "Green" }
        "WARNING" { "Yellow" }
        "ERROR" { "Red" }
        default { "White" }
    }
    Write-Host "[$timestamp] [$Level] $Message" -ForegroundColor $color
}

Write-Log "========================================" "INFO"
Write-Log "Starting Windows Build Process (Full Clean + Build)" "INFO"
Write-Log "========================================" "INFO"

# Get the script directory and project root
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectRoot = Split-Path -Parent (Split-Path -Parent $ScriptDir)

Write-Log "Project root: $ProjectRoot" "INFO"
Set-Location $ProjectRoot

# Load configuration from INI file
$ConfigFile = Join-Path $ScriptDir "build_deploy_config.ini"
Write-Log "Loading configuration from: $ConfigFile" "INFO"

try {
    $Config = Get-IniConfig -FilePath $ConfigFile
    Write-Log "Configuration loaded successfully!" "SUCCESS"
}
catch {
    Write-Log "Failed to load configuration: $_" "ERROR"
    exit 1
}

# Extract configuration values
$SourceDir = $Config["paths"]["source"]
$BuildDir = $Config["paths"]["build_dir"]

# Resolve source directory
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

Write-Log "Build directory: $BuildDir" "INFO"

# Step 1: Clean build directory
Write-Log "========================================" "INFO"
Write-Log "Step 1: Cleaning build directory" "INFO"
Write-Log "========================================" "INFO"

$FullBuildPath = Join-Path $ProjectRoot $BuildDir

if (Test-Path $FullBuildPath) {
    Write-Log "Removing existing build directory: $FullBuildPath" "INFO"
    try {
        Remove-Item -Path $FullBuildPath -Recurse -Force
        Write-Log "Build directory cleaned successfully!" "SUCCESS"
    }
    catch {
        Write-Log "Failed to clean build directory: $_" "ERROR"
        exit 1
    }
}
else {
    Write-Log "Build directory does not exist, skipping clean step" "INFO"
}

# Step 2: Call the fast build script
Write-Log "========================================" "INFO"
Write-Log "Step 2: Calling fast build script" "INFO"
Write-Log "========================================" "INFO"

$FastBuildScript = Join-Path $ScriptDir "windows_fast_deploy_build.ps1"
Write-Log "Executing: $FastBuildScript" "INFO"

try {
    & $FastBuildScript
    if ($LASTEXITCODE -eq 0) {
        Write-Log "========================================" "INFO"
        Write-Log "Build process completed successfully!" "SUCCESS"
        Write-Log "========================================" "INFO"
    }
    else {
        Write-Log "Fast build script failed with exit code: $LASTEXITCODE" "ERROR"
        exit $LASTEXITCODE
    }
}
catch {
    Write-Log "Error during fast build execution: $_" "ERROR"
    exit 1
}

# Step 3: Run tests
Write-Log "========================================" "INFO"
Write-Log "Step 3: Running tests" "INFO"
Write-Log "========================================" "INFO"

# Get build directory from config
$BuildDir = Join-Path $ProjectRoot $BuildDir

Write-Log "Test directory: $BuildDir" "INFO"
Write-Log "Command: ctest --test-dir $BuildDir --output-on-failure" "INFO"

try {
    & ctest --test-dir $BuildDir --output-on-failure
    if ($LASTEXITCODE -eq 0) {
        Write-Log "========================================" "INFO"
        Write-Log "All tests passed successfully!" "SUCCESS"
        Write-Log "========================================" "INFO"
    }
    else {
        Write-Log "========================================" "INFO"
        Write-Log "Some tests failed with exit code: $LASTEXITCODE" "WARNING"
        Write-Log "========================================" "INFO"
        # Don't exit on test failure, just warn
    }
}
catch {
    Write-Log "Error during test execution: $_" "WARNING"
}
