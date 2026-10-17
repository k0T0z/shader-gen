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

$ErrorActionPreference = 'Stop'

# Set up the MSVC 2022 x64 build environment.
. "$PSScriptRoot\setup_msvc_win64.ps1"

$PROTOBUF_LATEST_TAG = $version
$PROTOBUF_LATEST_TAG_WITHOUT_V = $PROTOBUF_LATEST_TAG.Replace("v", "")

# URLs and paths
$pbSrcCompressed = "protobuf-$PROTOBUF_LATEST_TAG_WITHOUT_V.zip"
$pbUrl    = "https://github.com/protocolbuffers/protobuf/releases/download/$PROTOBUF_LATEST_TAG/$pbSrcCompressed"

# Download source
Write-Host "Downloading Protobuf source from $pbUrl..."
Invoke-WebRequest -Uri $pbUrl -OutFile $pbSrcCompressed -Verbose

# Extract
Write-Host "Extracting Protobuf source..."
Expand-Archive -Path $pbSrcCompressed -DestinationPath $env:GITHUB_WORKSPACE

$pbSrcDir = Join-Path $env:GITHUB_WORKSPACE "protobuf-$PROTOBUF_LATEST_TAG_WITHOUT_V"

Write-Host "Configuring Protobuf with prefix: $install_dir"

# Determine the CMake flag for shared library builds based on link_type.
switch ($link_type) {
    "Dynamic" { $shared_libs = "ON" $static_libs = "OFF" }
    "Static"  { $shared_libs = "OFF" $static_libs = "ON" }
    default   { throw "Invalid link_type '$link_type'. Expected 'Dynamic' or 'Static'." }
}

# Base configure args
$configureArgs = @(
    "-G", "Visual Studio 17 2022", "-A", "x64",
    "-D", "protobuf_FORCE_FETCH_DEPENDENCIES=ON",
    "-D", "CMAKE_BUILD_TYPE='$build_type'",
    "-D", "CMAKE_CXX_STANDARD=17",
    "-D", "BUILD_SHARED_LIBS='$shared_libs'",
    "-D", "protobuf_BUILD_SHARED_LIBS='$shared_libs'",
    "-D", "CMAKE_INSTALL_PREFIX='$install_dir'"

    # ABSL Options https://github.com/abseil/abseil-cpp/blob/20250512.1/CMakeLists.txt
    "-D", "ABSL_PROPAGATE_CXX_STD=ON",
    "-D", "ABSL_MSVC_STATIC_RUNTIME='$static_libs'",
    "-D", "ABSL_BUILD_TESTING=OFF",
    "-D", "ABSL_BUILD_TEST_HELPERS=OFF",
    "-D", "ABSL_BUILD_MONOLITHIC_SHARED_LIBS='$shared_libs'"
)

# Build Protobuf
Set-Location $pbSrcDir

# Create & switch to the build directory.
if (-Not (Test-Path "build")) {
    New-Item -ItemType Directory -Path build | Out-Null
}

Write-Host "Current location: $(Get-Location)"
Set-Location build

cmake .. @configureArgs

# Build and install.
# DON'T use parallel builds with Protobuf on Windows as it breaks the CI.
cmake --build . --config $build_type
cmake --install . --parallel $env:NUMBER_OF_PROCESSORS --config $build_type

# Return to the original workspace (assumes GITHUB_WORKSPACE is set).
Set-Location $env:GITHUB_WORKSPACE
