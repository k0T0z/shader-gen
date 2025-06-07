# https://doc.qt.io/qt-6/windows-building.html

param(
    [Parameter(Mandatory = $true, Position = 0)]
    [ValidateSet("Debug", "Release", "MinSizeRel", "RelWithDebInfo")]
    [string]$build_type,

    [Parameter(Mandatory = $true, Position = 1)]
    [ValidateSet("Dynamic", "Static")]
    [string]$link_type,

    [Parameter(Mandatory = $true, Position = 2)]
    [string]$install_dir
)

$Qt6Version = "6.9.1"
$ErrorActionPreference = "Stop"

# derive major.minor from version
$parts      = $Qt6Version.Split('.')
$majorMinor = "$($parts[0]).$($parts[1])"

# URLs and paths
$qtSrcZip = "qt-everywhere-src-$Qt6Version.zip"
$qtUrl    = "https://download.qt.io/official_releases/qt/$majorMinor/$Qt6Version/single/$qtSrcZip"

# Download source
Write-Host "Downloading Qt source from $qtUrl..."
Invoke-WebRequest -Uri $qtUrl -OutFile $qtSrcZip -Verbose

# Extract
Write-Host "Extracting Qt source..."
Expand-Archive -Path $qtSrcZip -DestinationPath $env:GITHUB_WORKSPACE -Verbose

$qtSrcDir = Join-Path $env:GITHUB_WORKSPACE "qt-everywhere-src-$Qt6Version"

# Build Qt
Set-Location $qtSrcDir

# Create & switch to the build directory.
if (-Not (Test-Path "build")) {
    New-Item -ItemType Directory -Path build | Out-Null
}
Set-Location build

Write-Host "Configuring Qt with prefix: $install_dir"

# Ensure Python and CMake are available
python --version
cmake --version

# Base configure args
$configureArgs = @(
    "-prefix", $install_dir,
    "-D", "QT_BUILD_EXAMPLES_BY_DEFAULT=OFF",
    "-D", "QT_BUILD_TESTS_BY_DEFAULT=OFF",
    "-D", "QT_BUILD_TOOLS_BY_DEFAULT=ON"
)

# Add build-type flags
switch ($build_type) {
    "Debug" {
        $configureArgs += "-debug"
    }
    "Release" {
        $configureArgs += "-release"
    }
    "MinSizeRel" {
        $configureArgs += "-release"
        $configureArgs += "-optimize-size"
    }
    "RelWithDebInfo" {
        $configureArgs += "-release"
        $configureArgs += "-force-debug-info"
    }
}

# Add link-type flag
if ($link_type -eq "Static") {
    $configureArgs += "-static"
} else {
    $configureArgs += "-shared"
}

# Run configure
Write-Host "Running configure.bat with arguments:`n  $($configureArgs -join ' ')"
& "$qtSrcDir\configure.bat" @configureArgs

# Build & install with CMake
Write-Host "Starting build..."
cmake --build . --parallel

Write-Host "Installing to $install_dir..."
cmake --install .

# Return to the workspace root
Set-Location $env:GITHUB_WORKSPACE

Write-Host "Qt build+install complete."
