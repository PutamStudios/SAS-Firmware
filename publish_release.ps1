<#
.SYNOPSIS
    SAS-Firmware Automated PlatformIO Compiler and GitHub Release Publisher
.DESCRIPTION
    This script compiles the SAS-Firmware locally using PlatformIO,
    commits & pushes your latest source code changes to GitHub,
    and automatically creates a new GitHub Release with the compiled .bin file attached!
.EXAMPLE
    # Compile and publish a release with automatic date-tag:
    .\publish_release.ps1

    # Compile and publish with a specific version tag:
    .\publish_release.ps1 -VersionTag "v1.2.0" -ReleaseNotes "Added new RF protocols and cleaned icons"
#>

param (
    [string]$VersionTag = "",
    [string]$ReleaseNotes = "Automated release of SAS-Firmware (Predatory OS) compiled binary."
)

Clear-Host
Write-Host "==============================================" -ForegroundColor Yellow
Write-Host "     SAS-FIRMWARE GH RELEASE PUBLISHER        " -ForegroundColor Cyan
Write-Host "==============================================" -ForegroundColor Yellow

# 1. Verify GitHub CLI auth status
Write-Host "`n[+] Checking GitHub CLI authentication..." -ForegroundColor Cyan
$authCheck = gh auth status 2>&1
if ($authCheck -like "*Logged in to*") {
    Write-Host "[SUCCESS] GitHub CLI is authenticated!" -ForegroundColor Green
} else {
    Write-Host "[WARNING] GitHub CLI is not logged in. Attempting login prompt..." -ForegroundColor Yellow
    gh auth login
    # Re-check
    $authCheck = gh auth status 2>&1
    if ($authCheck -notlike "*Logged in to*") {
        Write-Host "[ERROR] GitHub CLI authentication failed. Please login with 'gh auth login' first." -ForegroundColor Red
        exit
    }
}

# 2. Compile the firmware locally
Write-Host "`n[+] Compiling firmware with PlatformIO..." -ForegroundColor Cyan
$env:PYTHONIOENCODING="utf-8"
C:\Users\roblo\.platformio\penv\Scripts\pio.exe run -e m5stack-cplus1_1

# Check if compile succeeded
$binPath = "e:\SAS-Firmware\.pio\build\m5stack-cplus1_1\firmware.bin"
if (Test-Path $binPath) {
    Write-Host "`n[SUCCESS] Firmware compiled successfully!" -ForegroundColor Green
} else {
    Write-Host "`n[ERROR] Compilation failed. Cannot publish release." -ForegroundColor Red
    exit
}

# 3. Determine Version Tag
if ([string]::IsNullOrEmpty($VersionTag)) {
    # Generate tag based on current date/time to ensure uniqueness (e.g. v2026.05.17-1945)
    $VersionTag = "v" + (Get-Date -Format "yyyy.MM.dd-HHmm")
}

Write-Host "`n[*] Release Tag set to: $VersionTag" -ForegroundColor White

# 4. Commit and Push source code changes
Write-Host "`n[+] Pushing latest source code changes to GitHub..." -ForegroundColor Cyan
git add .
git commit -m "Source code backup for release $VersionTag"
git push origin main --force

# 5. Create GitHub Release and Upload Binary
Write-Host "`n[+] Creating GitHub Release '$VersionTag' and uploading binary..." -ForegroundColor Cyan
# Rename binary to professional name for the release asset
$finalBinPath = "e:\SAS-Firmware\SAS-m5stack-cplus1_1.bin"
Copy-Item $binPath -Destination $finalBinPath -Force

gh release create $VersionTag $finalBinPath --title "SAS-Firmware $VersionTag" --notes $ReleaseNotes

if ($LASTEXITCODE -eq 0) {
    Write-Host "`n==============================================" -ForegroundColor Green
    Write-Host "   RELEASE PUBLISHED SUCCESSFULLY ON GITHUB!  " -ForegroundColor Green
    Write-Host "   Tag: $VersionTag" -ForegroundColor White
    Write-Host "   URL: https://github.com/PutamStudios/SAS-Firmware/releases/tag/$VersionTag" -ForegroundColor Cyan
    Write-Host "==============================================" -ForegroundColor Green
} else {
    Write-Host "`n[ERROR] Failed to create GitHub release." -ForegroundColor Red
}
