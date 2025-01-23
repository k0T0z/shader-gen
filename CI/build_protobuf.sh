#!/bin/bash
set -e

# Fetch the latest Protobuf tag
PROTOBUF_LATEST_TAG=$(git ls-remote --tags https://github.com/protocolbuffers/protobuf.git | tail -n1 | awk '{print $2}' | awk -F/ '{print $3}' | cut -d^ -f1)

# Build and install Protobuf
echo "Installing Protobuf $PROTOBUF_LATEST_TAG..."
git clone --depth 1 -b "$PROTOBUF_LATEST_TAG" https://github.com/protocolbuffers/protobuf.git
cd protobuf

git submodule update --init --recursive

mkdir build && cd build

cmake .. -G "Unix Makefiles" \
         -Dprotobuf_BUILD_TESTS=OFF \
         -Dprotobuf_ABSL_PROVIDER=module \
         -DCMAKE_BUILD_TYPE=Release \
         -DCMAKE_CXX_STANDARD=17 \
         -Dprotobuf_BUILD_SHARED_LIBS=ON

cmake --build . --config Release --parallel
cmake --install . --config Release

cd "$GITHUB_WORKSPACE"
