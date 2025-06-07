param(
    [string]$Version = "6.9.1",
    [string]$InstallDir = "$env:GITHUB_WORKSPACE\Qt"
)

# stop on any error
$ErrorActionPreference = 'Stop'

# derive major.minor folder from version
$parts       = $Version.Split('.')
$majorMinor  = "$($parts[0]).$($parts[1])"

$installer   = "qt-opensource-windows-x64-$Version.exe"
$url         = "https://download.qt.io/official_releases/qt/$majorMinor/$Version/$installer"

Write-Host "Downloading Qt $Version from $url..."
Invoke-WebRequest -Uri $url -OutFile $installer

Write-Host "Running Qt installer in silent mode..."
# /VERYSILENT for Inno Setup; adjust if this installer uses a different switch
& .\${installer} `
    --platform minimal `
    --verbose `
    --silent `
    --output-install-dir "$InstallDir"

# set Qt6_DIR environment variable for GitHub Actions
$qtBin = Join-Path $InstallDir "Qt\$Version\msvc_64\bin"
if (-Not (Test-Path $qtBin)) {
    Write-Error "Cannot find Qt bin folder at $qtBin"
}

# emit GitHub Actions env commands
Write-Host "::add-path::$qtBin"
Write-Host "::set-env name=Qt6_DIR::$InstallDir\Qt\$Version\msvc_64"
