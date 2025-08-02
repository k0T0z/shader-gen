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
$qtSrcCompressed = "qt-everywhere-src-$Qt6Version.tar.xz"
$qtUrl    = "https://download.qt.io/official_releases/qt/$majorMinor/$Qt6Version/single/$qtSrcCompressed"

# Download source
Write-Host "Downloading Qt source from $qtUrl..."
Invoke-WebRequest -Uri $qtUrl -OutFile $qtSrcCompressed -Verbose

# Extract
Write-Host "Extracting Qt source..."
tar -xf $qtSrcCompressed

$qtSrcDir = "qt-everywhere-src-$Qt6Version"

Write-Host "Configuring Qt with prefix: $install_dir"

# Ensure Python and CMake are available
python --version
cmake --version

# Base configure args
$configureArgs = @(
    "-prefix", $install_dir,
    "-submodules", "qtbase"
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
    default {
        throw "Invalid build type: $build_type"
    }
}

# Add link-type flag
if ($link_type -eq "Static") {
    $configureArgs += "-static"
} else {
    $configureArgs += "-shared"
}

# The “--” tells Qt’s configure.bat “what comes next goes to CMake”
$configureArgs += "--"

$configureArgs += @(
  "-D", "CMAKE_C_COMPILER=cl",
  "-D", "CMAKE_CXX_COMPILER=cl"
)

# https://doc.qt.io/qt-6/configure-options.html#cmake-generators.
$configureArgs += "-G"
$configureArgs += "Ninja" # The official supported generator for Qt6 on Windows.

$configureArgs += "-D"
$configureArgs += "QT_BUILD_EXAMPLES_BY_DEFAULT=OFF"

$configureArgs += "-D"
$configureArgs += "QT_BUILD_TESTS_BY_DEFAULT=OFF"

$configureArgs += "-D"
$configureArgs += "QT_BUILD_TOOLS_BY_DEFAULT=ON"

# Build Qt
Set-Location $qtSrcDir

# Create & switch to the build directory.
if (-Not (Test-Path "build")) {
    New-Item -ItemType Directory -Path build | Out-Null
}

Write-Host "Current location: $(Get-Location)"
Set-Location build

# https://github.com/actions/runner-images/blob/main/images/windows/Windows2022-Readme.md#visual-studio-enterprise-2022.
Write-Host "Setting up Visual Studio environment..."
$VSEdition = "Enterprise"
& "C:\Program Files\Microsoft Visual Studio\2022\$VSEdition\VC\Auxiliary\Build\vcvarsall.bat" amd64

# Run configure
Write-Host "Running configure.bat with arguments:`n  $($configureArgs -join ' ')"
& "$qtSrcDir\configure.bat" @configureArgs

# Build & install with CMake
Write-Host "Starting build..."
cmake --build . --parallel

Write-Host "Installing to $install_dir..."
cmake --install . --parallel

# Return to the workspace root
Set-Location $env:GITHUB_WORKSPACE

Write-Host "Qt build+install complete."
