#################################################################################
#                                                                               #
#  Copyright (C) 2026 Seif Kandil (k0T0z)                                       #
#                                                                               #
#  This file is a part of the ENIGMA Development Environment.                   #
#                                                                               #
#                                                                               #
#  ENIGMA is free software: you can redistribute it and/or modify it under the  #
#  terms of the GNU General Public License as published by the Free Software    #
#  Foundation, version 3 of the license or any later version.                   #
#                                                                               #
#  This application and its source code is distributed AS-IS, WITHOUT ANY       #
#  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS    #
#  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more        #
#  details.                                                                     #
#                                                                               #
#  You should have recieved a copy of the GNU General Public License along      #
#  with this code. If not, see <http://www.gnu.org/licenses/>                   #
#                                                                               #
#  ENIGMA is an environment designed to create games and other programs with a  #
#  high-level, fully compilable language. Developers of ENIGMA or anything      #
#  associated with ENIGMA are in no way responsible for its users or            #
#  applications created by its users, or damages caused by the environment      #
#  or programs made in the environment.                                         #
#                                                                               #
#################################################################################

# https://doc.qt.io/qt-6/windows-building.html

param(
    [Parameter(Mandatory = $true, Position = 0)]
    [ValidateSet("Debug", "Release", "MinSizeRel", "RelWithDebInfo")]
    [string]$build_type,

    [Parameter(Mandatory = $true, Position = 1)]
    [ValidateSet("Dynamic", "Static")]
    [string]$link_type,

    [Parameter(Mandatory = $true, Position = 2)]
    [string]$install_dir,

    [Parameter(Mandatory = $true, Position = 3)]
    [string]$version
)

$ErrorActionPreference = "Stop"

# Shared CI helpers.
. "$PSScriptRoot\common.ps1"

$Qt6Version = $version

# derive major.minor from version
$parts      = $Qt6Version.Split('.')
$majorMinor = "$($parts[0]).$($parts[1])"

# URLs and paths
$qtSrcCompressed = "qt-everywhere-src-$Qt6Version.zip"
$qtUrl    = "https://download.qt.io/official_releases/qt/$majorMinor/$Qt6Version/single/$qtSrcCompressed"

Save-CiSource -Url $qtUrl -OutFile $qtSrcCompressed -Label "Qt"
Expand-CiSource -Archive $qtSrcCompressed -Label "Qt"

$qtSrcDir = Join-Path $env:GITHUB_WORKSPACE "qt-everywhere-src-$Qt6Version"

Write-Host "Configuring Qt with prefix: $install_dir"

# Ensure Python and CMake are available
python --version
cmake --version

# Set up the MSVC 2022 x64 build environment.
. "$PSScriptRoot\setup_msvc_win64.ps1"

# Base configure args
$configureArgs = @(
    "-prefix", $install_dir,
    "-submodules", "qtbase",
    "-skip", "qtdeclarative,qtmultimedia"
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
Enter-CiBuildDir -SrcDir $qtSrcDir

# Run configure
Write-Host "Running configure.bat with arguments:`n  $($configureArgs -join ' ')"
& "$qtSrcDir\configure.bat" @configureArgs

# Build & install with CMake
Write-Host "Starting build..."
Build-CiProject -BuildType $build_type

Write-Host "Installing to $install_dir..."
Install-CiProject -BuildType $build_type

Exit-CiBuildDir

Write-Host "Qt build+install complete."
