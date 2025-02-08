param(
    [string]$protobuf_prefix
)

$PROTOBUF_LATEST_TAG = v29.3 # ($(git ls-remote --tags https://github.com/protocolbuffers/protobuf.git | Select-Object -Last 1) -split '\s+')[1].Split('/')[-1].Split('^')[0]

$ErrorActionPreference = 'Stop'

# Validate parameters
if (-not $protobuf_prefix) { throw "Protobuf install prefix is required" }

# Build and install Protobuf
Write-Output "Installing Protobuf $PROTOBUF_LATEST_TAG..."
git clone --depth 1 -b $PROTOBUF_LATEST_TAG https://github.com/protocolbuffers/protobuf.git
Set-Location protobuf

git submodule update --init --recursive

New-Item -ItemType Directory -Path build | Set-Location

cmake .. -G "Visual Studio 17 2022" -A x64 `
         -Dprotobuf_BUILD_TESTS=OFF `
         -Dprotobuf_BUILD_CONFORMANCE=OFF `
         -Dprotobuf_BUILD_EXAMPLES=OFF `
         -Dprotobuf_ABSL_PROVIDER=module `
         -DCMAKE_BUILD_TYPE=Release `
         -DCMAKE_CXX_STANDARD=17 `
         -Dprotobuf_BUILD_SHARED_LIBS=ON `
         -DCMAKE_INSTALL_PREFIX="$protobuf_prefix"

cmake --build . --config Release --parallel
cmake --install . --config Release

Set-Location $env:GITHUB_WORKSPACE
