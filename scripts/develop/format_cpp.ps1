#!/usr/bin/env pwsh
<#
.SYNOPSIS
    Format all C++ files in the project using clang-format

.DESCRIPTION
    This script formats all C++ source files (.cpp, .h, .hpp, .cc, .cxx)
    in the project using clang-format, excluding third_party and build dirs.

.PARAMETER DryRun
    Show what would be changed without modifying files

.PARAMETER Check
    Return exit code 1 if any files need formatting

.EXAMPLE
    .\scripts\develop\format_cpp.ps1
    Format all C++ files

.EXAMPLE
    .\scripts\develop\format_cpp.ps1 -DryRun
    Show what would be formatted

.EXAMPLE
    .\scripts\develop\format_cpp.ps1 -Check
    Check if files need formatting (for CI)
#>

param(
    [switch]$DryRun,
    [switch]$Check
)

# Colors for output
function Write-ColorOutput {
    param(
        [string]$Message,
        [string]$Color = "White"
    )
    Write-Host $Message -ForegroundColor $Color
}

# Check if clang-format exists
function Test-ClangFormat {
    $clangFormat = Get-Command "clang-format" -ErrorAction SilentlyContinue
    if ($null -eq $clangFormat) {
        Write-ColorOutput "ERROR: clang-format not found!" "Red"
        Write-ColorOutput "Please install clang-format:" "Yellow"
        Write-ColorOutput "  - Windows: LLVM installer or apt install clang-format" "Gray"
        Write-ColorOutput "  - Or add to PATH" "Gray"
        return $false
    }

    $version = & clang-format --version
    Write-ColorOutput "Found: $version" "Green"
    return $true
}

# Get all C++ files (excluding third_party and build directories)
function Get-CppFiles {
    $rootPath = $PSScriptRoot -replace "\\scripts\\develop$", ""
    if ($rootPath -eq $PSScriptRoot) {
        $rootPath = (Get-Location).Path
    }

    $excludePatterns = @(
        "*third_party*",
        "*build*",
        "*out*",
        "*.git*"
    )

    $files = Get-ChildItem -Path $rootPath -Recurse -Include @("*.cpp", "*.h", "*.hpp", "*.cc", "*.cxx") |
        Where-Object {
            $file = $_
            $shouldExclude = $false
            foreach ($pattern in $excludePatterns) {
                if ($file.FullName -like "*$pattern*") {
                    $shouldExclude = $true
                    break
                }
            }
            -not $shouldExclude
        }

    return $files
}

# Main
$ErrorActionPreference = "Stop"

Write-ColorOutput "`n=== C++ Code Formatter ===" "Cyan"
Write-ColorOutput "Project: CFDesktop" "Gray"

if (-not (Test-ClangFormat)) {
    exit 1
}

$files = Get-CppFiles
$fileCount = $files.Count

if ($fileCount -eq 0) {
    Write-ColorOutput "No C++ files found!" "Yellow"
    exit 0
}

Write-ColorOutput "`nFound $fileCount C++ files to process`n" "Gray"

$processed = 0
$changed = 0

foreach ($file in $files) {
    $processed++
    $relativePath = $file.FullName.Substring((Get-Location).Path.Length + 1)

    if ($DryRun) {
        # Check if file would be changed
        $original = Get-Content $file.FullName -Raw
        $formatted = & clang-format $file.FullName

        if ($original -ne $formatted) {
            Write-ColorOutput "[$processed/$fileCount] Would reformat: $relativePath" "Yellow"
            $changed++
        } else {
            Write-Host "[$processed/$fileCount] OK: $relativePath"
        }
    }
    elseif ($Check) {
        # Check mode: return error if files need formatting
        $original = Get-Content $file.FullName -Raw
        $formatted = & clang-format $file.FullName

        if ($original -ne $formatted) {
            Write-ColorOutput "[$processed/$fileCount] Needs formatting: $relativePath" "Red"
            $changed++
        } else {
            Write-Host "[$processed/$fileCount] OK: $relativePath"
        }
    }
    else {
        # Format in-place
        Write-Host "[$processed/$fileCount] Formatting: $relativePath"
        & clang-format -i $file.FullName
        $changed++
    }
}

Write-ColorOutput "`n=== Summary ===" "Cyan"
Write-ColorOutput "Processed: $processed / $fileCount files" "Gray"

if ($DryRun) {
    Write-ColorOutput "Would reformat: $changed files" "Yellow"
}
elseif ($Check) {
    if ($changed -gt 0) {
        Write-ColorOutput "Files needing formatting: $changed" "Red"
        Write-ColorOutput "Run without -Check to format" "Yellow"
        exit 1
    } else {
        Write-ColorOutput "All files formatted correctly!" "Green"
    }
}
else {
    Write-ColorOutput "Formatted: $changed files" "Green"
}

exit 0
