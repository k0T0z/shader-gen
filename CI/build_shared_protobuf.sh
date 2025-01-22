#!/bin/bash

# Exit with a non-zero value if any command fails.
set -e

# Check if a tag was provided
if [ -z "$1" ]; then
  echo "No tag provided. Usage: $0 <tag>"
  exit 1
fi

# Define the repository URL
REPO_URL="https://github.com/protocolbuffers/protobuf.git"
LATEST_TAG=$1

# Clone Abseil
git clone --depth 1 -b $LATEST_TAG $REPO_URL

cd protobuf

# Checkout the latest release
git submodule update --init --recursive

# Create build directory
mkdir build && cd build

# Configure and build
cmake .. -Dprotobuf_BUILD_TESTS=OFF \
         -Dprotobuf_ABSL_PROVIDER=package \
         -DCMAKE_BUILD_TYPE=Release \
         -DCMAKE_CXX_STANDARD=17 \
         -Dprotobuf_BUILD_SHARED_LIBS=ON # -DCMAKE_INSTALL_PREFIX=

make -j$(nproc)

# Install
sudo make install

# Go back to the original directory
cd $GITHUB_WORKSPACE
