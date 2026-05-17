<#
.SYNOPSIS
    SAS-Firmware Automated Git Committer and Pusher
.DESCRIPTION
    This script automates adding, committing, and pushing your SAS-Firmware changes to GitHub.
    You can run it immediately, or schedule it to run at a specific date/time.
.EXAMPLE
    # Run immediately:
    .\schedule_git.ps1 -CommitMessage "Added custom sensor code"

    # Schedule to run at a specific date/time (e.g. 11:30 PM):
    .\schedule_git.ps1 -CommitMessage "Nightly build push" -ScheduleTime "23:30"
#>

param (
    [string]$CommitMessage = "Automated SAS-Firmware update",
    [string]$ScheduleTime = "" # Optional format: "HH:mm" (e.g., "22:00" or "02:30")
)

Clear-Host
Write-Host "==============================================" -ForegroundColor Yellow
Write-Host "       SAS-FIRMWARE AUTO-GIT SCHEDULER        " -ForegroundColor Cyan
Write-Host "==============================================" -ForegroundColor Yellow

# Function to run the Git commands
function Invoke-GitPush {
    Write-Host "`n[+] Staging all changes..." -ForegroundColor Cyan
    git add .
    
    Write-Host "[+] Committing changes with message: '$CommitMessage'..." -ForegroundColor Cyan
    git commit -m $CommitMessage
    
    Write-Host "[+] Pushing to GitHub (main)..." -ForegroundColor Cyan
    git push origin main
    
    Write-Host "`n[SUCCESS] Everything is pushed successfully to GitHub!" -ForegroundColor Green
}

# If no schedule time is provided, run immediately
if ([string]::IsNullOrEmpty($ScheduleTime)) {
    Write-Host "[!] No schedule time specified. Running immediately..." -ForegroundColor Yellow
    Invoke-GitPush
    exit
}

# Parse and wait for schedule time
try {
    $targetTime = [DateTime]::ParseExact($ScheduleTime, "HH:mm", $null)
    $currentTime = Get-Date

    # If the target time has already passed today, set it for tomorrow
    if ($targetTime -lt $currentTime) {
        $targetTime = $targetTime.AddDays(1)
    }

    $timeToWait = $targetTime - $currentTime
    Write-Host "[*] Current Time: $($currentTime.ToString('HH:mm:ss'))" -ForegroundColor White
    Write-Host "[*] Scheduled Push Time: $($targetTime.ToString('yyyy-MM-dd HH:mm:ss'))" -ForegroundColor Green
    Write-Host "[*] Waiting for $($timeToWait.Hours)h $($timeToWait.Minutes)m $($timeToWait.Seconds)s..." -ForegroundColor Yellow
    
    # Start wait loop
    $secondsToWait = [int]$timeToWait.TotalSeconds
    for ($i = 0; $i -lt $secondsToWait; $i++) {
        if ([Console]::KeyAvailable) {
            $key = [Console]::ReadKey($true)
            Write-Host "`n[!] Aborted by user." -ForegroundColor Red
            exit
        }
        
        $remaining = $secondsToWait - $i
        $hours = [Math]::Floor($remaining / 3600)
        $minutes = [Math]::Floor(($remaining % 3600) / 60)
        $seconds = $remaining % 60
        
        Write-Progress -Activity "Waiting for Scheduled Push" -Status ("Time remaining: {0:D2}:{1:D2}:{2:D2}" -f $hours, $minutes, $seconds) -PercentComplete (($i / $secondsToWait) * 100)
        Start-Sleep -Seconds 1
    }
    
    # Trigger push
    Invoke-GitPush
}
catch {
    Write-Host "[ERROR] Invalid time format. Please use 'HH:mm' (e.g. '23:30')" -ForegroundColor Red
}
