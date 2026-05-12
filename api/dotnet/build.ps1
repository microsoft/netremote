<#
.SYNOPSIS
    Build script for NetRemote NuGet package (Windows)

.DESCRIPTION
    This script builds and packs the Microsoft.Net.Remote.Client NuGet package

.PARAMETER Configuration
    Build configuration (Debug or Release). Default: Release

.PARAMETER Version
    Package version (e.g., 1.0.0). If not specified, determined from git tag or defaults to 0.5.2

.PARAMETER OutputDir
    Output directory for the NuGet package

.PARAMETER SkipRestore
    Skip package restore

.PARAMETER SkipBuild
    Skip build (pack only)

.PARAMETER Clean
    Clean build artifacts before building

.EXAMPLE
    .\build.ps1 -Configuration Release -Version 1.0.0

.EXAMPLE
    .\build.ps1 -Clean
#>

param(
    [ValidateSet("Debug", "Release")]
    [string]$Configuration = "Release",

    [string]$Version = "",

    [string]$OutputDir = "",

    [switch]$SkipRestore,

    [switch]$SkipBuild,

    [switch]$Clean
)

$ErrorActionPreference = "Stop"

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectRoot = (Get-Item "$ScriptDir\..\..").FullName

if ([string]::IsNullOrEmpty($OutputDir)) {
    $OutputDir = Join-Path $ProjectRoot "out\nuget"
}

# Check for dotnet CLI
$dotnetCmd = Get-Command dotnet -ErrorAction SilentlyContinue
if (-not $dotnetCmd) {
    Write-Error "dotnet CLI not found. Please install .NET SDK from https://dotnet.microsoft.com/download"
    exit 1
}

Write-Host "=== NetRemote NuGet Package Build ===" -ForegroundColor Cyan
Write-Host "Configuration: $Configuration"
Write-Host "Output: $OutputDir"

# Determine version if not specified
if ([string]::IsNullOrEmpty($Version)) {
    # Try to get version from git tag
    $gitCmd = Get-Command git -ErrorAction SilentlyContinue
    if ($gitCmd -and (Test-Path (Join-Path $ProjectRoot ".git"))) {
        $latestTag = git -C $ProjectRoot tag -l --sort=-v:refname | Select-Object -First 1
        if ($latestTag -match '^v(\d+)\.(\d+)\.(\d+)') {
            $Version = "$($Matches[1]).$($Matches[2]).$($Matches[3])"
        }
    }

    # Fallback to default version
    if ([string]::IsNullOrEmpty($Version)) {
        $Version = "0.5.2"
    }
}

Write-Host "Version: $Version"

Push-Location $ScriptDir

try {
    # Clean if requested
    if ($Clean) {
        Write-Host ""
        Write-Host "Cleaning build artifacts..." -ForegroundColor Yellow
        dotnet clean NetRemoteClient.csproj -c $Configuration 2>$null
        if (Test-Path "bin") { Remove-Item -Recurse -Force "bin" }
        if (Test-Path "obj") { Remove-Item -Recurse -Force "obj" }
    }

    # Restore packages
    if (-not $SkipRestore) {
        Write-Host ""
        Write-Host "Restoring packages..." -ForegroundColor Yellow
        dotnet restore NetRemoteClient.csproj
        if ($LASTEXITCODE -ne 0) { throw "Restore failed" }
    }

    # Build
    if (-not $SkipBuild) {
        Write-Host ""
        Write-Host "Building..." -ForegroundColor Yellow
        dotnet build NetRemoteClient.csproj -c $Configuration --no-restore
        if ($LASTEXITCODE -ne 0) { throw "Build failed" }
    }

    # Create output directory
    if (-not (Test-Path $OutputDir)) {
        New-Item -ItemType Directory -Path $OutputDir -Force | Out-Null
    }

    # Pack
    Write-Host ""
    Write-Host "Creating NuGet package..." -ForegroundColor Yellow
    dotnet pack NetRemoteClient.csproj `
        -c $Configuration `
        -o $OutputDir `
        -p:Version=$Version `
        --no-build

    if ($LASTEXITCODE -ne 0) { throw "Pack failed" }

    Write-Host ""
    Write-Host "=== Build Complete ===" -ForegroundColor Green
    Write-Host "NuGet package created at: $OutputDir"

    Get-ChildItem -Path $OutputDir -Filter "*.nupkg" | ForEach-Object {
        Write-Host "  - $($_.Name)" -ForegroundColor Cyan
    }
}
finally {
    Pop-Location
}
