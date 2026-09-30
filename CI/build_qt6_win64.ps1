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
$qtSrcCompressed = "qt-everywhere-src-$Qt6Version.zip"
$qtUrl    = "https://download.qt.io/official_releases/qt/$majorMinor/$Qt6Version/single/$qtSrcCompressed"

# Download source
Write-Host "Downloading Qt source from $qtUrl..."
Invoke-WebRequest -Uri $qtUrl -OutFile $qtSrcCompressed -Verbose

# Extract
Write-Host "Extracting Qt source..."
Expand-Archive -Path $qtSrcCompressed -DestinationPath $env:GITHUB_WORKSPACE

$qtSrcDir = Join-Path $env:GITHUB_WORKSPACE "qt-everywhere-src-$Qt6Version"

Write-Host "Configuring Qt with prefix: $install_dir"

# Ensure Python and CMake are available
python --version
cmake --version

Write-Host "Setting up Visual Studio environment..."
if (Get-Command cl.exe -ErrorAction SilentlyContinue) {
    Write-Host "MSVC compiler (cl.exe) is already available in PATH."
} else {
    $devShell = $null
    $vsWhere = Join-Path ${env:ProgramFiles(x86)} "Microsoft Visual Studio\Installer\vswhere.exe"
    if (Test-Path $vsWhere) {
        $vsPath = (& $vsWhere -products * -latest -property installationPath)
        if ($vsPath) {
            $candidate = Join-Path ($vsPath.Trim()) "Common7\Tools\Launch-VsDevShell.ps1"
            if (Test-Path $candidate) {
                $devShell = $candidate
            }
        }
    }

    if (-not $devShell) {
        $editions = @("Enterprise", "Professional", "Community")
        foreach ($year in @("2022", "18", "2019")) {
            foreach ($edition in $editions) {
                $candidate = "C:\Program Files\Microsoft Visual Studio\$year\$edition\Common7\Tools\Launch-VsDevShell.ps1"
                if (Test-Path $candidate) {
                    $devShell = $candidate
                    break
                }
            }
            if ($devShell) { break }
        }
    }

    if ($devShell -and (Test-Path $devShell)) {
        Write-Host "Using dev shell: $devShell"
        & $devShell -Arch amd64 -SkipAutomaticLocation
    } else {
        throw "Could not find Launch-VsDevShell.ps1. Please ensure Visual Studio is installed with C++ workload."
    }
}

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
switch ($link_type) {
    "Dynamic" {
        $configureArgs += "-shared"
    }
    "Static" {
        $configureArgs += "-static"
    }
    default {
        throw "Invalid link type: $link_type"
    }
}

# The “--” tells Qt’s configure.bat “what comes next goes to CMake”
$configureArgs += "--"

# https://doc.qt.io/qt-6/configure-options.html#cmake-generators.
$configureArgs += "-G"
$configureArgs += "Ninja" # The official supported generator for Qt6 on Windows.

# We need to use MSVC
$configureArgs += @(
  "-D", "CMAKE_C_COMPILER=cl",
  "-D", "CMAKE_CXX_COMPILER=cl"
)

switch ($link_type) {
    "Dynamic" {
        switch ($build_type) {
            "Debug" {
                $msvcRuntime = 'MultiThreadedDebugDLL'
            }
            default {
                $msvcRuntime = 'MultiThreadedDLL'
            }
        }

        $configureArgs += @(
            "-D", "CMAKE_MSVC_RUNTIME_LIBRARY=$msvcRuntime"
        )
    }
    "Static" {
        switch ($build_type) {
            "Debug" {
                $msvcRuntime = 'MultiThreadedDebug'
            }
            default {
                $msvcRuntime = 'MultiThreaded'
            }
        }

        $configureArgs += @(
            "-D", "CMAKE_MSVC_RUNTIME_LIBRARY=$msvcRuntime"
        )
    }
    default {
        throw "Invalid link type: $link_type"
    }
}

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

# Run configure
Write-Host "Running configure.bat with arguments:`n  $($configureArgs -join ' ')"
& "$qtSrcDir\configure.bat" @configureArgs

# Build & install with CMake
Write-Host "Starting build..."
cmake --build . --config $build_type

Write-Host "Installing to $install_dir..."
cmake --install . --parallel $env:NUMBER_OF_PROCESSORS --config $build_type

# Return to the workspace root
Set-Location $env:GITHUB_WORKSPACE

Write-Host "Qt build+install complete."
