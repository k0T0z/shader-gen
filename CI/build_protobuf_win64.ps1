param(
    [Parameter(Mandatory = $true, Position = 0)]
    [ValidateSet("Debug", "Release", "MinSizeRel", "RelWithDebInfo")]
    [string]$build_type,

    [Parameter(Mandatory = $true, Position = 1)]
    [ValidateSet("Dynamic", "Static")]
    [string]$link_type,

    [Parameter(Mandatory = $true, Position = 2)]
    [string]$protobuf_prefix
)

$PROTOBUF_LATEST_TAG = "v29.3"
$ErrorActionPreference = 'Stop'

# Determine the CMake flag for shared library builds based on link_type.
switch ($link_type) {
    "Dynamic" { $shared_libs = "ON" }
    "Static"  { $shared_libs = "OFF" }
    default   { throw "Invalid link_type '$link_type'. Expected 'Dynamic' or 'Static'." }
}

Write-Output "Installing Protobuf '$PROTOBUF_LATEST_TAG' with build type '$build_type' and link type '$link_type' (shared libs: '$shared_libs')..."

# Clone the Protobuf repository.
git clone --depth 1 -b $PROTOBUF_LATEST_TAG https://github.com/protocolbuffers/protobuf.git
Set-Location protobuf

# Update submodules.
git submodule update --init --recursive

# Create and switch to the build directory.
New-Item -ItemType Directory -Path build | Set-Location

# Configure the build with CMake using the provided build type and shared libraries setting.
cmake .. -G "Visual Studio 17 2022" -A x64 `
         -Dprotobuf_BUILD_TESTS=OFF `
         -Dprotobuf_BUILD_CONFORMANCE=OFF `
         -Dprotobuf_BUILD_EXAMPLES=OFF `
         -Dprotobuf_ABSL_PROVIDER=module `
         -DCMAKE_BUILD_TYPE="$build_type" `
         -DCMAKE_CXX_STANDARD=17 `
         -Dprotobuf_BUILD_SHARED_LIBS="$shared_libs" `
         -DCMAKE_INSTALL_PREFIX="$protobuf_prefix" `
                                                   `
         -DABSL_PROPAGATE_CXX_STD=ON `
         -DBUILD_TESTING=OFF `
         -DABSL_BUILD_TESTING=OFF `
         -DABSL_USE_GOOGLETEST_HEAD=OFF `
         -DABSL_ENABLE_INSTALL=ON `
         -DBUILD_SHARED_LIBS="$shared_libs" `
         -DABSL_BUILD_MONOLITHIC_SHARED_LIBS="$shared_libs" `
         -DCMAKE_MODULE_LINKER_FLAGS='-Wl,--no-undefined'

# Build and install.
# DON'T use parallel builds with Protobuf on Windows as it breaks the CI.
cmake --build . --config $build_type
cmake --install . --config $build_type

# Return to the original workspace (assumes GITHUB_WORKSPACE is set).
Set-Location $env:GITHUB_WORKSPACE
