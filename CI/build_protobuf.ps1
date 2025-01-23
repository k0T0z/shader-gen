param(
    [string]$absl_prefix,
    [string]$protobuf_prefix
)

# Fetch latest ABSL tag
$ABSL_LATEST_TAG = git ls-remote --tags https://github.com/abseil/abseil-cpp.git |
    ForEach-Object { if ($_ -match 'refs/tags/([^{}]+)') { $matches[1] } } |
    Sort-Object { [version]($_.Replace('v','').Split('-')[0]) } |
    Select-Object -Last 1

# Fetch latest Protobuf tag
$PROTOBUF_LATEST_TAG = git ls-remote --tags https://github.com/protocolbuffers/protobuf.git |
    ForEach-Object { if ($_ -match 'refs/tags/([^{}]+)') { $matches[1] } } |
    Sort-Object { [version]($_.Replace('v','').Split('-')[0]) } |
    Select-Object -Last 1

$ErrorActionPreference = 'Stop'

# Validate parameters
if (-not $absl_prefix) { throw "Abseil install prefix is required" }
if (-not $protobuf_prefix) { throw "Protobuf install prefix is required" }

# Build and install Abseil
Write-Output "Installing Abseil $ABSL_LATEST_TAG..."
git clone --depth 1 -b $ABSL_LATEST_TAG https://github.com/abseil/abseil-cpp.git
cd abseil-cpp

mkdir build
cd build

cmake .. -G "Visual Studio 17 2022" `
         -DABSL_PROPAGATE_CXX_STD=ON `
         -DBUILD_TESTING=OFF `
         -DABSL_BUILD_TESTING=OFF `
         -DABSL_USE_GOOGLETEST_HEAD=OFF `
         -DCMAKE_BUILD_TYPE=Release `
         -DCMAKE_CXX_STANDARD=17 `
         -DABSL_ENABLE_INSTALL=ON `
         -DBUILD_SHARED_LIBS=ON `
         -DABSL_BUILD_MONOLITHIC_SHARED_LIBS=ON `
         -DCMAKE_INSTALL_PREFIX="$absl_prefix"

cmake --build . --config Release --parallel
cmake --install . --config Release

cd $env:GITHUB_WORKSPACE

# Build and install Protobuf
Write-Output "Installing Protobuf $PROTOBUF_LATEST_TAG..."
git clone --depth 1 -b $PROTOBUF_LATEST_TAG https://github.com/protocolbuffers/protobuf.git
cd protobuf

git submodule update --init --recursive

mkdir build
cd build

cmake .. -G "Visual Studio 17 2022" `
         -Dprotobuf_BUILD_TESTS=OFF `
         -Dprotobuf_ABSL_PROVIDER=package `
         -DCMAKE_BUILD_TYPE=Release `
         -DCMAKE_CXX_STANDARD=17 `
         -Dprotobuf_BUILD_SHARED_LIBS=ON `
         -DCMAKE_PREFIX_PATH="$absl_prefix" `
         -DCMAKE_INSTALL_PREFIX="$protobuf_prefix"

cmake --build . --config Release --parallel
cmake --install . --config Release

cd $env:GITHUB_WORKSPACE
