# This script ONLY configures the project using CMake
# It does NOT build the project
# Usage: .\windows_configure.ps1 [-Config <develop|deploy>]
param(
    [Parameter(Position=0)]
    [ValidateSet("develop", "deploy")]
    [string]$Config = "develop"
)

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
Write-Log "Starting Windows CMake Configuration" "INFO"
Write-Log "Configuration: $Config" "INFO"
Write-Log "========================================" "INFO"

# Get the script directory and project root
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectRoot = Split-Path -Parent (Split-Path -Parent $ScriptDir)

Write-Log "Project root: $ProjectRoot" "INFO"
Write-Log "Changing to project directory" "INFO"

Set-Location $ProjectRoot

# Determine which config file to use
if ($Config -eq "deploy") {
    $ConfigFile = Join-Path $ScriptDir "build_deploy_config.ini"
}
else {
    $ConfigFile = Join-Path $ScriptDir "build_develop_config.ini"
}

Write-Log "Loading configuration from: $ConfigFile" "INFO"

try {
    $ConfigData = Get-IniConfig -FilePath $ConfigFile
    Write-Log "Configuration loaded successfully!" "SUCCESS"
}
catch {
    Write-Log "Failed to load configuration: $_" "ERROR"
    exit 1
}

# Extract configuration values
$Generator = $ConfigData["cmake"]["generator"]
$Toolchain = $ConfigData["cmake"]["toolchain"]
$SourceDir = $ConfigData["paths"]["source"]
$BuildDir = $ConfigData["paths"]["build_dir"]

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

Write-Log "Generator: $Generator" "INFO"
Write-Log "Toolchain: $Toolchain" "INFO"
Write-Log "Source directory: $SourceDir (resolved: $ResolvedSourceDir)" "INFO"
Write-Log "Build directory: $BuildDir" "INFO"

# Configure with CMake
Write-Log "========================================" "INFO"
Write-Log "Configuring with CMake (NO BUILD)" "INFO"
Write-Log "Command: cmake -G $Generator -DUSE_TOOLCHAIN=$Toolchain -S $ResolvedSourceDir -B $BuildDir" "INFO"
Write-Log "========================================" "INFO"

$cmakeConfigArgs = @(
    "-G", $Generator,
    "-DUSE_TOOLCHAIN=$Toolchain",
    "-S", $ResolvedSourceDir,
    "-B", $BuildDir
)

try {
    & cmake @cmakeConfigArgs
    if ($LASTEXITCODE -eq 0) {
        Write-Log "========================================" "INFO"
        Write-Log "CMake configuration completed successfully!" "SUCCESS"
        Write-Log "To build the project, run: cmake --build $BuildDir" "INFO"
        Write-Log "========================================" "INFO"
    }
    else {
        Write-Log "CMake configuration failed with exit code: $LASTEXITCODE" "ERROR"
        exit $LASTEXITCODE
    }
}
catch {
    Write-Log "Error during CMake configuration: $_" "ERROR"
    exit 1
}
