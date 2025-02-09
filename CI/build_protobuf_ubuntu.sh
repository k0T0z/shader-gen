#!/bin/bash
set -e

# Validate input arguments: build_type and link_type must be provided.
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <build_type> <link_type>"
    echo "Valid build types: Debug, Release, MinSizeRel, RelWithDebInfo"
    echo "Valid link types: Dynamic, Static"
    exit 1
fi

BUILD_TYPE="$1"
LINK_TYPE="$2"

# Validate build type
if [[ "$BUILD_TYPE" != "Debug" && "$BUILD_TYPE" != "Release" && "$BUILD_TYPE" != "MinSizeRel" && "$BUILD_TYPE" != "RelWithDebInfo" ]]; then
    echo "Error: Invalid build type '$BUILD_TYPE'. Valid options are: Debug, Release, MinSizeRel, RelWithDebInfo."
    exit 1
fi

# Validate link type
if [[ "$LINK_TYPE" != "Dynamic" && "$LINK_TYPE" != "Static" ]]; then
    echo "Error: Invalid link type '$LINK_TYPE'. Valid options are: Dynamic, Static."
    exit 1
fi

# Set shared libraries flag based on link_type
if [ "$LINK_TYPE" == "Dynamic" ]; then
    SHARED_LIBS="ON"
else
    SHARED_LIBS="OFF"
fi

# Hard-coded tags for demonstration purposes
ABSL_LATEST_TAG=20250127.0  # Abseil tag
PROTOBUF_LATEST_TAG=v29.3   # Protobuf tag

# Build and install Abseil
echo "Installing Abseil $ABSL_LATEST_TAG with build type '$BUILD_TYPE' and link type '$LINK_TYPE'..."
git clone --depth 1 -b "$ABSL_LATEST_TAG" https://github.com/abseil/abseil-cpp.git
cd abseil-cpp

mkdir build && cd build

cmake .. -G "Unix Makefiles" \
         -DABSL_PROPAGATE_CXX_STD=ON \
         -DBUILD_TESTING=OFF \
         -DABSL_BUILD_TESTING=OFF \
         -DABSL_USE_GOOGLETEST_HEAD=OFF \
         -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
         -DABSL_ENABLE_INSTALL=ON \
         -DBUILD_SHARED_LIBS="$SHARED_LIBS" \
         -DABSL_BUILD_MONOLITHIC_SHARED_LIBS="$SHARED_LIBS" \
         -DCMAKE_MODULE_LINKER_FLAGS="-Wl,--no-undefined"

make -j$(nproc)

sudo make install

# Return to the workspace directory (assuming GITHUB_WORKSPACE is set)
cd "${GITHUB_WORKSPACE:-..}"

# Build and install Protobuf
echo "Installing Protobuf $PROTOBUF_LATEST_TAG with build type '$BUILD_TYPE' and link type '$LINK_TYPE'..."
git clone --depth 1 -b "$PROTOBUF_LATEST_TAG" https://github.com/protocolbuffers/protobuf.git
cd protobuf

git submodule update --init --recursive

mkdir build && cd build

cmake .. -G "Unix Makefiles" \
         -Dprotobuf_BUILD_TESTS=OFF \
         -Dprotobuf_BUILD_CONFORMANCE=OFF \
         -Dprotobuf_BUILD_EXAMPLES=OFF \
         -Dprotobuf_ABSL_PROVIDER=package \
         -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
         -DCMAKE_CXX_STANDARD=17 \
         -Dprotobuf_BUILD_SHARED_LIBS="$SHARED_LIBS"

make -j$(nproc) 

sudo make install

# Return to the workspace directory
cd "${GITHUB_WORKSPACE:-..}"
