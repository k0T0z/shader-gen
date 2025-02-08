#!/bin/bash
set -e

# Check for the install prefix argument
if [ $# -ne 1 ]; then
    echo "Usage: $0 INSTALL_PREFIX"
    exit 1
fi

INSTALL_PREFIX=$1

# Fetch the latest tags
PROTOBUF_LATEST_TAG=$(git ls-remote --tags https://github.com/protocolbuffers/protobuf.git | tail -n1 | awk '{print $2}' | awk -F/ '{print $3}' | cut -d^ -f1)

# Build and install Protobuf
echo "Installing Protobuf $PROTOBUF_LATEST_TAG..."
git clone --depth 1 -b "$PROTOBUF_LATEST_TAG" https://github.com/protocolbuffers/protobuf.git
cd protobuf

git submodule update --init --recursive

mkdir build && cd build

cmake .. -G "Unix Makefiles" \
         -DCMAKE_MAKE_PROGRAM=mingw32-make \
                                        \
         -Dprotobuf_BUILD_TESTS=OFF \
         -Dprotobuf_BUILD_CONFORMANCE=OFF \
         -Dprotobuf_BUILD_EXAMPLES=OFF \
         -Dprotobuf_ABSL_PROVIDER=module \
         -DCMAKE_BUILD_TYPE=Release \
         -DCMAKE_CXX_STANDARD=17 \
         -Dprotobuf_BUILD_SHARED_LIBS=OFF \
         -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX" \
                                         \
         -DABSL_PROPAGATE_CXX_STD=ON \
         -DBUILD_TESTING=OFF \
         -DABSL_BUILD_TESTING=OFF \
         -DABSL_USE_GOOGLETEST_HEAD=OFF \
         -DABSL_ENABLE_INSTALL=ON \
         -DBUILD_SHARED_LIBS=OFF \
         -DABSL_BUILD_MONOLITHIC_SHARED_LIBS=OFF \
         -DCMAKE_MODULE_LINKER_FLAGS="-Wl,--no-undefined"

cmake --build . --config Release --parallel
cmake --install . --config Release

cd "$GITHUB_WORKSPACE"
