param(
    [Parameter(Mandatory = $true)]
    [string]$TargetDir
)

Write-Output "Target directory for extraction: $TargetDir"

# Ensure the target directory exists
if (-Not (Test-Path $TargetDir)) {
    Write-Output "Target directory does not exist. Creating it."
    New-Item -ItemType Directory -Force -Path $TargetDir | Out-Null
}

Write-Output "Downloading patched windeployqt64releaseonly.zip..."
$zipFile = "windeployqt64releaseonly.zip"
$retryCount = 3
$success = $false
for ($i = 1; $i -le $retryCount; $i++) {
    try {
        Invoke-WebRequest -Uri "http://tripleboot.org/Uploads/windeployqt64releaseonly.zip" -OutFile $zipFile -UseBasicParsing
        $success = $true
        break
    } catch {
        Write-Output "Download attempt $i of $retryCount failed: $_"
        Start-Sleep -Seconds 5
    }
}
if (-not $success) {
    Write-Error "Failed to download patched windeployqt64releaseonly.zip after $retryCount attempts."
    exit 1
}

Write-Output "Extracting patched executable into $TargetDir..."
Expand-Archive -Path $zipFile -DestinationPath $TargetDir -Force
