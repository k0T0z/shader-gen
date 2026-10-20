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
    [string]$version,

    [Parameter(Mandatory = $true, Position = 4)]
    [string]$absl_dir
)

$ErrorActionPreference = 'Stop'

# Shared CI helpers.
. "$PSScriptRoot\common.ps1"

# Set up the MSVC 2022 x64 build environment.
. "$PSScriptRoot\setup_msvc_win64.ps1"

$PROTOBUF_LATEST_TAG = $version
$PROTOBUF_LATEST_TAG_WITHOUT_V = Get-CiVersionWithoutV -Version $PROTOBUF_LATEST_TAG

# URLs and paths
$pbSrcCompressed = "protobuf-$PROTOBUF_LATEST_TAG_WITHOUT_V.zip"
$pbUrl    = "https://github.com/protocolbuffers/protobuf/releases/download/$PROTOBUF_LATEST_TAG/$pbSrcCompressed"

Save-CiSource -Url $pbUrl -OutFile $pbSrcCompressed -Label "Protobuf"
Expand-CiSource -Archive $pbSrcCompressed -Label "Protobuf"

$pbSrcDir = Join-Path $env:GITHUB_WORKSPACE "protobuf-$PROTOBUF_LATEST_TAG_WITHOUT_V"

Write-Host "Configuring Protobuf with prefix: $install_dir"

# Determine the CMake flag for shared library builds based on link_type.
$shared_libs = Get-CiSharedLibs -LinkType $link_type

# Base configure args
$configureArgs = @(
    "-G", "Visual Studio 17 2022", "-A", "x64",
    "-D", "protobuf_LOCAL_DEPENDENCIES_ONLY=ON",
    "-D", "CMAKE_PREFIX_PATH='$absl_dir'",
    "-D", "CMAKE_BUILD_TYPE='$build_type'",
    "-D", "CMAKE_CXX_STANDARD=17",
    "-D", "BUILD_SHARED_LIBS='$shared_libs'",
    "-D", "protobuf_BUILD_SHARED_LIBS='$shared_libs'",
    "-D", "CMAKE_INSTALL_PREFIX='$install_dir'"
)

# Build Protobuf
Enter-CiBuildDir -SrcDir $pbSrcDir

Write-Host "Running CMake with arguments: $configureArgs"

cmake .. @configureArgs

# Build and install.
# DON'T use parallel builds with Protobuf on Windows as it breaks the CI.
Build-CiProject -BuildType $build_type
Install-CiProject -BuildType $build_type

Exit-CiBuildDir
