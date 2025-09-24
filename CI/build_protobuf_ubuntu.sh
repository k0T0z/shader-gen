#!/bin/bash
set -e

# Validate input arguments: build_type and link_type must be provided.
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <build_type> <link_type>"
    echo "Valid build types: Debug, Release, MinSizeRel, RelWithDebInfo"
    echo "Valid link types: Dynamic, Static"
    exit 1
fi

build_type=$1
link_type=$2

# Validate build_type
case "$build_type" in
    Debug|Release|MinSizeRel|RelWithDebInfo) ;;
    *) 
        echo "Invalid build type: $build_type"
        exit 1
        ;;
esac

# Validate link_type
case "$link_type" in
    Dynamic|Static) ;;
    *)
        echo "Invalid link type: $link_type"
        exit 1
        ;;
esac

case "$link_type" in
    Static)
        shared_libs="OFF"
        ;;
    Dynamic)
        shared_libs="ON"
        ;;
esac

PROTOBUF_LATEST_TAG=v29.3

# Build and install Protobuf
echo "Installing Protobuf $PROTOBUF_LATEST_TAG with build type '$BUILD_TYPE' and link type '$LINK_TYPE'..."
git clone --depth 1 -b "$PROTOBUF_LATEST_TAG" https://github.com/protocolbuffers/protobuf.git
cd protobuf

git submodule update --init --recursive

mkdir build && cd build

# Configure arguments
configureArgs=(
    "-G" "Unix Makefiles"
    "-D" "protobuf_BUILD_TESTS=OFF"
    "-D" "protobuf_BUILD_CONFORMANCE=OFF"
    "-D" "protobuf_BUILD_EXAMPLES=OFF"
    "-D" "protobuf_ABSL_PROVIDER=module"
    "-D" "CMAKE_BUILD_TYPE='$build_type'"
    "-D" "CMAKE_CXX_STANDARD=17"
    "-D" "BUILD_SHARED_LIBS='$shared_libs'"
    "-D" "protobuf_BUILD_SHARED_LIBS='$shared_libs'"

    "-D" "ABSL_PROPAGATE_CXX_STD=ON"
    "-D" "BUILD_TESTING=OFF"
    "-D" "ABSL_BUILD_TESTING=OFF"
    "-D" "ABSL_USE_GOOGLETEST_HEAD=OFF"
    "-D" "ABSL_ENABLE_INSTALL=ON"
    "-D" "ABSL_BUILD_MONOLITHIC_SHARED_LIBS='$shared_libs'"
    "-D" "CMAKE_MODULE_LINKER_FLAGS='-Wl,--no-undefined'"
)

echo "Running configure with arguments:"
printf '  %s\n' "${configureArgs[@]}"

cmake .. "${configureArgs[@]}" || { echo "Configuration failed"; exit 1; }

make -j$(nproc)

sudo make install

# Return to the workspace directory
cd "${GITHUB_WORKSPACE:-..}"
