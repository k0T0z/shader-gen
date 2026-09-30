#################################################################################
#                                                                               #
#  Copyright (C) 2026 Seif Kandil (k0T0z)                                       #
#                                                                               #
#  This file is a part of the ENIGMA Development Environment.                   #
#                                                                               #
#                                                                               #
#  ENIGMA is free software: you can redistribute it and/or modify it under the  #
#  terms of the GNU General Public License as published by the Free Software    #
#  Foundation, version 3 of the license or any later version.                   #
#                                                                               #
#  This application and its source code is distributed AS-IS, WITHOUT ANY       #
#  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS    #
#  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more        #
#  details.                                                                     #
#                                                                               #
#  You should have recieved a copy of the GNU General Public License along      #
#  with this code. If not, see <http://www.gnu.org/licenses/>                   #
#                                                                               #
#  ENIGMA is an environment designed to create games and other programs with a  #
#  high-level, fully compilable language. Developers of ENIGMA or anything      #
#  associated with ENIGMA are in no way responsible for its users or            #
#  applications created by its users, or damages caused by the environment      #
#  or programs made in the environment.                                         #
#                                                                               #
#################################################################################

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
echo "Installing Protobuf $PROTOBUF_LATEST_TAG with build type '$build_type' and link type '$link_type'..."
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
