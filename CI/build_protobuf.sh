#!/bin/bash
set -e

# Fetch the latest tags
ABSL_LATEST_TAG=$(git ls-remote --tags https://github.com/abseil/abseil-cpp.git | tail -n1 | awk '{print $2}' | awk -F/ '{print $3}' | cut -d^ -f1)
PROTOBUF_LATEST_TAG=$(git ls-remote --tags https://github.com/protocolbuffers/protobuf.git | tail -n1 | awk '{print $2}' | awk -F/ '{print $3}' | cut -d^ -f1)

# Build and install Abseil
echo "Installing Abseil $ABSL_LATEST_TAG..."
git clone --depth 1 -b "$ABSL_LATEST_TAG" h
cd abseil-cpp

mkdir build && cd build

cmake ..  -G "Unix Makefiles" \
         -DABSL_PROPAGATE_CXX_STD=ON \
         -DBUILD_TESTING=OFF \
         -DABSL_BUILD_TESTING=OFF \
         -DABSL_USE_GOOGLETEST_HEAD=OFF \
         -DCMAKE_BUILD_TYPE=Release \
         -DABSL_ENABLE_INSTALL=ON \
         -DBUILD_SHARED_LIBS=ON \
         -DABSL_BUILD_MONOLITHIC_SHARED_LIBS=ON \
         -DCMAKE_MODULE_LINKER_FLAGS="-Wl,--no-undefined"

make -j$(nproc)

sudo make install

cd "$GITHUB_WORKSPACE"

# Build and install Protobuf
echo "Installing Protobuf $PROTOBUF_LATEST_TAG..."
git clone --depth 1 -b "$PROTOBUF_LATEST_TAG" https://github.com/protocolbuffers/protobuf.git
cd protobuf

git submodule update --init --recursive

mkdir build && cd build

cmake .. -G "Unix Makefiles" \
         -Dprotobuf_BUILD_TESTS=OFF \
         -Dprotobuf_ABSL_PROVIDER=package \
         -DCMAKE_BUILD_TYPE=Release \
         -DCMAKE_CXX_STANDARD=17 \
         -Dprotobuf_BUILD_SHARED_LIBS=ON

make -j$(nproc)

sudo make install

cd "$GITHUB_WORKSPACE"
