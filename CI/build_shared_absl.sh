#!/bin/bash

# Exit with a non-zero value if any command fails.
set -e

# Check if a tag was provided
if [ -z "$1" ]; then
  echo "No tag provided. Usage: $0 <tag>"
  exit 1
fi

# Define the repository URL
REPO_URL="https://github.com/abseil/abseil-cpp.git"
LATEST_TAG=$1

# Clone Abseil
git clone --depth 1 -b $LATEST_TAG $REPO_URL

cd abseil-cpp

# Create build directory
mkdir build && cd build

# Configure and build
cmake .. -DABSL_PROPAGATE_CXX_STD=ON \
         -DBUILD_TESTING=OFF \
         -DABSL_BUILD_TESTING=OFF \
         -DABSL_USE_GOOGLETEST_HEAD=OFF \
         -DCMAKE_BUILD_TYPE=Release \
         -DCMAKE_CXX_STANDARD=17 \
         -DABSL_ENABLE_INSTALL=ON \
         -DBUILD_SHARED_LIBS=ON \
         -DABSL_BUILD_MONOLITHIC_SHARED_LIBS=ON \
         -DCMAKE_MODULE_LINKER_FLAGS="-Wl,--no-undefined" # -DCMAKE_INSTALL_PREFIX=

make -j$(nproc)

# Install
sudo make install

# Go back to the original directory
cd $GITHUB_WORKSPACE
