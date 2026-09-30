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
    [string]$llvm_prefix
)

$LLVM_VERSION = "llvmorg-20.1.0"
$ErrorActionPreference = 'Stop'

# Determine the LLVM link type parameters
switch ($link_type) {
    "Dynamic" { $shared_libs = "ON" }
    "Static"  { $shared_libs = "OFF" }
    default   { throw "Invalid link_type '$link_type'. Expected 'Dynamic' or 'Static'." }
}

# Create install directory if needed
if (-not (Test-Path -Path $llvm_prefix -PathType Container)) {
    Write-Output "Install prefix directory '$llvm_prefix' does not exist. Creating it..."
    New-Item -ItemType Directory -Path $llvm_prefix -Force | Out-Null
}

Write-Output "Installing LLVM '$LLVM_VERSION' with build type '$build_type' and link type '$link_type'..."

# Clone LLVM monorepo
git clone --depth 1 -b $LLVM_VERSION https://github.com/llvm/llvm-project.git
Set-Location llvm-project

# Create build directory
New-Item -ItemType Directory -Path build | Set-Location

# Here we need to set LLVM_USE_CRT_DEBUG depending on the build type
# See https://mesa-docs.readthedocs.io/en/latest/llvmpipe.html#llvmpipe


# Configure LLVM with CMake
cmake ../llvm -G "Visual Studio 17 2022" -A x64 `
              -DBUILD_SHARED_LIBS="$shared_libs" `
              -DLLVM_ENABLE_BACKTRACES=OFF `
              -DLLVM_ENABLE_UNWIND_TABLES=OFF `
              -DLLVM_ENABLE_CRASH_OVERRIDES=OFF `
              -DLLVM_ENABLE_LIBXML2=OFF `
              -DLLVM_ENABLE_LIBEDIT=OFF `
              -DLLVM_ENABLE_LIBPFM=OFF `
              -DLLVM_ENABLE_ZLIB=OFF `
              -DLLVM_ENABLE_Z3_SOLVER=OFF `
              -DLLVM_ENABLE_PEDANTIC=OFF `
              -DLLVM_ENABLE_WERROR=OFF `
              -DLLVM_ENABLE_ASSERTIONS=OFF `
              -DLLVM_INCLUDE_UTILS=OFF `
              -DLLVM_BUILD_UTILS=OFF `
              -DLLVM_BUILD_EXAMPLES=OFF `
              -DLLVM_INCLUDE_EXAMPLES=OFF `
              -DLLVM_BUILD_TESTS=OFF `
              -DLLVM_INCLUDE_TESTS=OFF `
              -DLLVM_BUILD_BENCHMARKS=OFF `
              -DLLVM_INCLUDE_BENCHMARKS=OFF `
              -DLLVM_BUILD_DOCS=OFF `
              -DLLVM_INCLUDE_DOCS=OFF `
              -DLLVM_ENABLE_BINDINGS=OFF `
              -DLLVM_BUILD_LLVM_C_DYLIB=OFF `
              -DLLVM_OPTIMIZED_TABLEGEN=ON `
              -DLLVM_ENABLE_PLUGINS=OFF `
              -DLLVM_ENABLE_IDE=OFF `
              -DCMAKE_INSTALL_PREFIX="$llvm_prefix" `
              -DCMAKE_BUILD_TYPE="$build_type"

# Set the CRT flags based on the build type and link type
if ($build_type -eq "Debug" -and $link_type -eq "Static") {
    cmake ../llvm -DLLVM_USE_CRT_DEBUG=MTd `
                  -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDebug
} elseif ($build_type -eq "Debug" -and $link_type -eq "Dynamic") {
    cmake ../llvm -DLLVM_USE_CRT_DEBUG=MDd `
                  -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDebugDLL
} elseif ($build_type -eq "Release" -and $link_type -eq "Static") {
    cmake ../llvm -DLLVM_USE_CRT_RELEASE=MT `
                  -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded
} elseif ($build_type -eq "Release" -and $link_type -eq "Dynamic") {
    cmake ../llvm -DLLVM_USE_CRT_RELEASE=MD `
                  -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDLL
}

# Build and install
cmake --build . --config $build_type --target install

# Return to original workspace
Set-Location $env:GITHUB_WORKSPACE
