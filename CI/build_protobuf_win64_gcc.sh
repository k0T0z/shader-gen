#!/bin/bash
set -e

# Check for the required arguments: BUILD_TYPE, LINK_TYPE, and INSTALL_PREFIX
if [ "$#" -ne 3 ]; then
    echo "Usage: $0 <BUILD_TYPE> <LINK_TYPE> <INSTALL_PREFIX>"
    echo "Valid BUILD_TYPE values: Debug, Release, MinSizeRel, RelWithDebInfo"
    echo "Valid LINK_TYPE values: Dynamic, Static"
    exit 1
fi

BUILD_TYPE=$1
LINK_TYPE=$2
INSTALL_PREFIX=$3

# Validate BUILD_TYPE
if [ "$BUILD_TYPE" != "Debug" ] && [ "$BUILD_TYPE" != "Release" ] && [ "$BUILD_TYPE" != "MinSizeRel" ] && [ "$BUILD_TYPE" != "RelWithDebInfo" ]; then
    echo "Error: Invalid BUILD_TYPE '$BUILD_TYPE'. Valid options: Debug, Release, MinSizeRel, RelWithDebInfo."
    exit 1
fi

# Validate LINK_TYPE
if [ "$LINK_TYPE" != "Dynamic" ] && [ "$LINK_TYPE" != "Static" ]; then
    echo "Error: Invalid LINK_TYPE '$LINK_TYPE'. Valid options: Dynamic, Static."
    exit 1
fi

# Set shared libraries flag based on LINK_TYPE
if [ "$LINK_TYPE" = "Dynamic" ]; then
    SHARED_LIBS="ON"
else
    SHARED_LIBS="OFF"
fi

# Check if the install prefix directory exists; if not, create it
if [ ! -d "$INSTALL_PREFIX" ]; then
    echo "Install prefix directory '$INSTALL_PREFIX' does not exist. Creating it..."
    mkdir -p "$INSTALL_PREFIX"
fi

# Fetch the latest tag (hard-coded)
PROTOBUF_LATEST_TAG=v29.3

# Build and install Protobuf
echo "Installing Protobuf $PROTOBUF_LATEST_TAG with build type '$BUILD_TYPE' and link type '$LINK_TYPE' (shared libs: $SHARED_LIBS)..."
git clone --depth 1 -b "$PROTOBUF_LATEST_TAG" https://github.com/protocolbuffers/protobuf.git
cd protobuf

git submodule update --init --recursive

mkdir build && cd build

cmake .. -G "Unix Makefiles" \
         -DCMAKE_MAKE_PROGRAM=mingw32-make \
         -Dprotobuf_BUILD_TESTS=OFF \
         -Dprotobuf_BUILD_CONFORMANCE=OFF \
         -Dprotobuf_BUILD_EXAMPLES=OFF \
         -Dprotobuf_ABSL_PROVIDER=module \
         -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
         -DCMAKE_CXX_STANDARD=17 \
         -Dprotobuf_BUILD_SHARED_LIBS="$SHARED_LIBS" \
         -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX" \
                                                  \
         -DABSL_PROPAGATE_CXX_STD=ON \
         -DBUILD_TESTING=OFF \
         -DABSL_BUILD_TESTING=OFF \
         -DABSL_USE_GOOGLETEST_HEAD=OFF \
         -DABSL_ENABLE_INSTALL=ON \
         -DBUILD_SHARED_LIBS="$SHARED_LIBS" \
         -DABSL_BUILD_MONOLITHIC_SHARED_LIBS="$SHARED_LIBS" \
         -DCMAKE_MODULE_LINKER_FLAGS="-Wl,--no-undefined"

cmake --build . --config "$BUILD_TYPE"
cmake --install . --config "$BUILD_TYPE"

cd "$GITHUB_WORKSPACE"
