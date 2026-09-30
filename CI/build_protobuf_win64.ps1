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
    [string]$install_dir
)

$PROTOBUF_LATEST_TAG = "v29.3"
$ErrorActionPreference = 'Stop'

# Determine the CMake flag for shared library builds based on link_type.
switch ($link_type) {
    "Dynamic" { $shared_libs = "ON" }
    "Static"  { $shared_libs = "OFF" }
    default   { throw "Invalid link_type '$link_type'. Expected 'Dynamic' or 'Static'." }
}

# Check if the install prefix directory exists; if not, create it.
if (-not (Test-Path -Path $install_dir -PathType Container)) {
    Write-Output "Install prefix directory '$install_dir' does not exist. Creating it..."
    New-Item -ItemType Directory -Path $install_dir -Force | Out-Null
}

Write-Output "Installing Protobuf '$PROTOBUF_LATEST_TAG' with build type '$build_type' and link type '$link_type' (shared libs: '$shared_libs')..."

# Clone the Protobuf repository.
git clone --depth 1 -b $PROTOBUF_LATEST_TAG https://github.com/protocolbuffers/protobuf.git
Set-Location protobuf

# Update submodules.
git submodule update --init --recursive

# Create and switch to the build directory.
New-Item -ItemType Directory -Path build | Set-Location

# Note that we don't need to set CMAKE_MSVC_RUNTIME_LIBRARY as Protobuf already does this for us.

# Base configure args
$configureArgs = @(
    "-G", "Visual Studio 17 2022", "-A", "x64",
    "-D", "protobuf_BUILD_TESTS=OFF",
    "-D", "protobuf_BUILD_CONFORMANCE=OFF",
    "-D", "protobuf_BUILD_EXAMPLES=OFF",
    "-D", "protobuf_ABSL_PROVIDER=module",
    "-D", "CMAKE_BUILD_TYPE='$build_type'",
    "-D", "CMAKE_CXX_STANDARD=17",
    "-D", "BUILD_SHARED_LIBS='$shared_libs'",
    "-D", "protobuf_BUILD_SHARED_LIBS='$shared_libs'",
    "-D", "CMAKE_INSTALL_PREFIX='$install_dir'",

    "-D", "ABSL_PROPAGATE_CXX_STD=ON",
    "-D", "BUILD_TESTING=OFF",
    "-D", "ABSL_BUILD_TESTING=OFF",
    "-D", "ABSL_USE_GOOGLETEST_HEAD=OFF",
    "-D", "ABSL_ENABLE_INSTALL=ON",
    "-D", "ABSL_BUILD_MONOLITHIC_SHARED_LIBS='$shared_libs'",
    "-D", "CMAKE_MODULE_LINKER_FLAGS='-Wl,--no-undefined'"
)

cmake .. @configureArgs

# Build and install.
# DON'T use parallel builds with Protobuf on Windows as it breaks the CI.
cmake --build . --config $build_type
cmake --install . --parallel $env:NUMBER_OF_PROCESSORS --config $build_type

# Return to the original workspace (assumes GITHUB_WORKSPACE is set).
Set-Location $env:GITHUB_WORKSPACE
