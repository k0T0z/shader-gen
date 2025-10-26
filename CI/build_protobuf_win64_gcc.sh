#!/bin/bash
set -e

# Check for the required arguments: BUILD_TYPE, LINK_TYPE, and INSTALL_PREFIX
if [ "$#" -ne 3 ]; then
    echo "Usage: $0 <BUILD_TYPE> <LINK_TYPE> <INSTALL_PREFIX>"
    echo "Valid BUILD_TYPE values: Debug, Release, MinSizeRel, RelWithDebInfo"
    echo "Valid LINK_TYPE values: Dynamic, Static"
    exit 1
fi

build_type=$1
link_type=$2
install_dir=$3

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

# Check if the install prefix directory exists; if not, create it
if [ ! -d "$install_dir" ]; then
    echo "Install prefix directory '$install_dir' does not exist. Creating it..."
    mkdir -p "$install_dir"
fi

PROTOBUF_LATEST_TAG=v29.3

# Build and install Protobuf
echo "Installing Protobuf $PROTOBUF_LATEST_TAG with build type '$build_type' and link type '$link_type' (shared libs: $shared_libs)..."
git clone --depth 1 -b "$PROTOBUF_LATEST_TAG" https://github.com/protocolbuffers/protobuf.git
cd protobuf

git submodule update --init --recursive

mkdir build && cd build

# Configure arguments
configureArgs=(
    "-G" "Unix Makefiles"
    "-D" "CMAKE_MAKE_PROGRAM=mingw32-make"
    "-D" "protobuf_BUILD_TESTS=OFF"
    "-D" "protobuf_BUILD_CONFORMANCE=OFF"
    "-D" "protobuf_BUILD_EXAMPLES=OFF"
    "-D" "protobuf_ABSL_PROVIDER=module"
    "-D" "CMAKE_BUILD_TYPE='$build_type'"
    "-D" "CMAKE_CXX_STANDARD=17"
    "-D" "BUILD_SHARED_LIBS='$shared_libs'"
    "-D" "protobuf_BUILD_SHARED_LIBS='$shared_libs'"
    "-D" "CMAKE_INSTALL_PREFIX='$install_dir'"

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

cmake --build . --config "$build_type"
cmake --install . --parallel $(nproc) --config "$build_type"

cd "$GITHUB_WORKSPACE"
