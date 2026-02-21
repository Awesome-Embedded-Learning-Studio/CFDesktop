# This script configures and builds the project (FAST version - no cleaning)
# It calls the configure script first, then builds
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
Write-Log "Starting Windows FAST Build Process" "INFO"
Write-Log "========================================" "INFO"

# Get the script directory and project root
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectRoot = Split-Path -Parent (Split-Path -Parent $ScriptDir)

Write-Log "Project root: $ProjectRoot" "INFO"
Set-Location $ProjectRoot

# Step 1: Call the configure script
Write-Log "========================================" "INFO"
Write-Log "Step 1: Configuring with CMake" "INFO"
Write-Log "========================================" "INFO"

$ConfigureScript = Join-Path $ScriptDir "windows_configure.ps1"
Write-Log "Executing: $ConfigureScript -Config develop" "INFO"

try {
    & $ConfigureScript -Config "develop"
    if ($LASTEXITCODE -ne 0) {
        Write-Log "Configure script failed with exit code: $LASTEXITCODE" "ERROR"
        exit $LASTEXITCODE
    }
}
catch {
    Write-Log "Error during configuration: $_" "ERROR"
    exit 1
}

# Step 2: Load config for build
$ConfigFile = Join-Path $ScriptDir "build_develop_config.ini"
$Config = Get-IniConfig -FilePath $ConfigFile
$BuildDir = $Config["paths"]["build_dir"]
$Jobs = if ($Config["options"] -and $Config["options"]["jobs"]) { $Config["options"]["jobs"] } else { "" }

# Step 3: Build with CMake
Write-Log "========================================" "INFO"
Write-Log "Step 2: Building project" "INFO"

$buildArgs = @("--build", $BuildDir)
if ($Jobs) {
    $buildArgs += "--parallel", $Jobs
    Write-Log "Command: cmake --build $BuildDir --parallel $Jobs" "INFO"
}
else {
    Write-Log "Command: cmake --build $BuildDir" "INFO"
}
Write-Log "========================================" "INFO"

try {
    & cmake @buildArgs
    if ($LASTEXITCODE -eq 0) {
        Write-Log "========================================" "INFO"
        Write-Log "Build completed successfully!" "SUCCESS"
        Write-Log "========================================" "INFO"
    }
    else {
        Write-Log "Build failed with exit code: $LASTEXITCODE" "ERROR"
        exit $LASTEXITCODE
    }
}
catch {
    Write-Log "Error during build: $_" "ERROR"
    exit 1
}
