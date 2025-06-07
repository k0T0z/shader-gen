param(
    [Parameter(Mandatory = $true, Position = 0)]
    [ValidateSet("Debug", "Release", "MinSizeRel", "RelWithDebInfo")]
    [string]$build_type,

    [Parameter(Mandatory = $true, Position = 1)]
    [ValidateSet("Dynamic", "Static")]
    [string]$link_type,

    [Parameter(Mandatory = $true, Position = 2)]
    [string]$install_dir,
)

$Qt6Version = "6.9.1"
$ErrorActionPreference = "Stop"

# derive major.minor from version
$parts       = $Qt6Version.Split('.')
$majorMinor  = "$($parts[0]).$($parts[1])"

# URLs and paths
$qtSrcZip = "qt-everywhere-src-$Qt6Version.zip"
$qtUrl = "https://download.qt.io/official_releases/qt/$majorMinor/$Qt6Version/single/$qtSrcZip"

# Download source
Write-Host "Downloading Qt source from $qtUrl..."
Invoke-WebRequest -Uri $qtUrl -OutFile $qtSrcZip

# Extract
Write-Host "Extracting Qt source..."
Expand-Archive -Path $qtSrcZip -DestinationPath $env:GITHUB_WORKSPACE

$qtSrcDir = "$env:GITHUB_WORKSPACE\qt-everywhere-src-$Qt6Version"

# Build Qt
Set-Location $qtSrcDir

# Create and switch to the build directory.
New-Item -ItemType Directory -Path build | Set-Location

Write-Host "Configuring Qt with prefix: $InstallDir"

# Ensure Python and CMake are available
python --version
cmake --version

# Build configuration
# https://doc.qt.io/qt-6/configure-options.html
$configureArgs = @("-prefix", "$install_dir")

switch ($build_type) {
    "Debug"          { $configureArgs += "-debug" }
    "Release"        { $configureArgs += "-release" }
    "MinSizeRel"     { $configureArgs += "-release" $configureArgs += "-optimize-size" }
    "RelWithDebInfo" { $configureArgs += "-release" $configureArgs += "-force-debug-info" }
}

if ($link_type -eq "Static") {
    $configureArgs += "-static"
} else {
    $configureArgs += "-shared"
}

# Run configure
Write-Host "Running configure..."
& .\${qtSrcDir}\configure.bat @configureArgs -D QT_BUILD_EXAMPLES_BY_DEFAULT=OFF -D QT_BUILD_TESTS_BY_DEFAULT=OFF -D QT_BUILD_TOOLS_BY_DEFAULT=ON

# Build with CMake
Write-Host "Starting build..."
cmake --build . --parallel
cmake --install .

# Return to the original workspace (assumes GITHUB_WORKSPACE is set).
Set-Location $env:GITHUB_WORKSPACE
