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

# Shared helpers for the PowerShell-based CI dependency build scripts (MSVC).
# Dot-source this file from a build script:
#   . "$PSScriptRoot\common.ps1"

# Return the version/tag without a leading 'v'.
function Get-CiVersionWithoutV {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Version
    )
    return $Version.Replace("v", "")
}

# Map a link type to the BUILD_SHARED_LIBS value (ON/OFF).
function Get-CiSharedLibs {
    param(
        [Parameter(Mandatory = $true)]
        [string]$LinkType
    )
    switch ($LinkType) {
        "Dynamic" { return "ON" }
        "Static"  { return "OFF" }
        default   { throw "Invalid link_type '$LinkType'. Expected 'Dynamic' or 'Static'." }
    }
}

# Download $Url into $OutFile, labelling progress with $Label.
function Save-CiSource {
    param(
        [Parameter(Mandatory = $true)][string]$Url,
        [Parameter(Mandatory = $true)][string]$OutFile,
        [Parameter(Mandatory = $true)][string]$Label
    )
    Write-Host "Downloading $Label source from $Url..."
    Invoke-WebRequest -Uri $Url -OutFile $OutFile -Verbose
}

# Extract $Archive into the workspace, labelling progress with $Label.
# Handles both .zip (Expand-Archive) and tarballs (tar).
function Expand-CiSource {
    param(
        [Parameter(Mandatory = $true)][string]$Archive,
        [Parameter(Mandatory = $true)][string]$Label
    )
    Write-Host "Extracting $Label source..."
    if ($Archive -like "*.zip") {
        Expand-Archive -Path $Archive -DestinationPath $env:GITHUB_WORKSPACE
    } else {
        tar -xzf $Archive -C $env:GITHUB_WORKSPACE
    }
}

# Enter the extracted source directory and create/switch to its build directory.
function Enter-CiBuildDir {
    param(
        [Parameter(Mandatory = $true)][string]$SrcDir
    )
    Set-Location $SrcDir
    if (-Not (Test-Path "build")) {
        New-Item -ItemType Directory -Path build | Out-Null
    }
    Write-Host "Current location: $(Get-Location)"
    Set-Location build
}

# Return to the workspace root.
function Exit-CiBuildDir {
    Set-Location $env:GITHUB_WORKSPACE
}

# Build the current CMake project.
function Build-CiProject {
    param(
        [Parameter(Mandatory = $true)][string]$BuildType
    )
    cmake --build . --config $BuildType
}

# Install the current CMake project (parallel).
function Install-CiProject {
    param(
        [Parameter(Mandatory = $true)][string]$BuildType
    )
    cmake --install . --parallel $env:NUMBER_OF_PROCESSORS --config $BuildType
}
