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

# Sets up the MSVC 2022 x64 build environment for the current process.
# Dot-source this script from other CI scripts so the environment is inherited:
#   . "$PSScriptRoot\setup_msvc_win64.ps1"

Write-Host "Setting up Visual Studio environment..."
$vsYears    = @("2022")
$vsEditions = @("Enterprise", "Professional", "Community")
$vsRequiredComponent = "Microsoft.VisualStudio.Component.VC.Tools.x86.x64"

$vsPath = $null
if (Get-Command cl.exe -ErrorAction SilentlyContinue) {
    if ($env:VSCMD_ARG_TGT_ARCH -eq "x64") {
        Write-Host "MSVC compiler (cl.exe) is already available with x64 target toolchain."
        $vsPath = $env:VSINSTALLDIR
    } else {
        $archStatus = if ([string]::IsNullOrEmpty($env:VSCMD_ARG_TGT_ARCH)) { 'VSCMD_ARG_TGT_ARCH not set' } else { "target=$env:VSCMD_ARG_TGT_ARCH" }
        Write-Host "cl.exe found in PATH but x64 toolchain not confirmed ($archStatus); re-initializing MSVC 2022 x64 environment."
    }
}

if (-not $vsPath) {
    $vsWhere = Join-Path ${env:ProgramFiles(x86)} "Microsoft Visual Studio\Installer\vswhere.exe"
    if (Test-Path $vsWhere) {
        $found = (& $vsWhere -products * -version "[17.0,18.0)" -requires $vsRequiredComponent -latest -property installationPath)
        if ($found -and (Test-Path $found.Trim())) {
            $vsPath = $found.Trim()
            Write-Host "vswhere found MSVC 2022 with x64 tools at: $vsPath"
        }
    }

    if (-not $vsPath) {
        foreach ($year in $vsYears) {
            foreach ($edition in $vsEditions) {
                $candidate = "C:\Program Files\Microsoft Visual Studio\$year\$edition"
                $vcMarker = Join-Path $candidate "VC\Auxiliary\Build\vcvarsall.bat"
                if (Test-Path $candidate -and (Test-Path $vcMarker)) {
                    $vsPath = $candidate
                    Write-Host "Fallback scan found MSVC 2022 at: $vsPath"
                    break
                }
            }
            if ($vsPath) { break }
        }
    }
}

if (-not $vsPath) {
    throw "Could not locate MSVC 2022 (v17.x) with the C++ x64 build tools. Please install the 'Desktop development with C++' workload for Visual Studio 2022."
}

$vcvarsall = $null
$candidate = Join-Path $vsPath "VC\Auxiliary\Build\vcvarsall.bat"
if (Test-Path $candidate) {
    $vcvarsall = $candidate
}

if ($vcvarsall) {
    Write-Host "Using vcvarsall: $vcvarsall amd64"
    $envLines = & cmd /c "`"$vcvarsall`" amd64 >nul && set"
    foreach ($line in $envLines) {
        if ($line -match '^([^=]+)=(.*)$') {
            [Environment]::SetEnvironmentVariable($Matches[1], $Matches[2], "Process")
        }
    }
} else {
    $devShell = $null
    $candidate = Join-Path $vsPath "Common7\Tools\Launch-VsDevShell.ps1"
    if (Test-Path $candidate) {
        $devShell = $candidate
    }

    if ($devShell -and (Test-Path $devShell)) {
        Write-Host "Falling back to dev shell: $devShell"
        & $devShell -Arch amd64 -SkipAutomaticLocation
    } else {
        throw "Could not find vcvarsall.bat or Launch-VsDevShell.ps1. Please ensure Visual Studio is installed with C++ workload."
    }
}
