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

# Check for the required arguments: BUILD_TYPE, LINK_TYPE, INSTALL_PREFIX, and optional VERSION
if [ $# -ne 4 ]; then
    echo "Usage: $0 <BUILD_TYPE> <LINK_TYPE> <INSTALL_PREFIX> <VERSION>"
    echo "Valid BUILD_TYPE values: Debug, Release, MinSizeRel, RelWithDebInfo"
    echo "Valid LINK_TYPE values: Dynamic, Static"
    echo "VERSION: Protobuf version/tag"
    exit 1
fi

build_type=$1
link_type=$2
install_dir=$3
version=$4

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

PROTOBUF_LATEST_TAG="$version"
PROTOBUF_LATEST_TAG_WITHOUT_V="${PROTOBUF_LATEST_TAG#v}"

# URLs and paths
pbSrcCompressed="protobuf-$PROTOBUF_LATEST_TAG_WITHOUT_V.tar.gz"
pbUrl="https://github.com/protocolbuffers/protobuf/releases/download/$PROTOBUF_LATEST_TAG/$pbSrcCompressed"

# Download source
echo "Downloading Protobuf source from $pbUrl..."
curl -L -O "$pbUrl" || { echo "Download failed"; exit 1; }

# Extract
echo "Extracting Protobuf source..."
tar -xf "$pbSrcCompressed"

pbSrcDir="protobuf-$PROTOBUF_LATEST_TAG_WITHOUT_V"

# Build and install Protobuf
echo "Installing Protobuf $PROTOBUF_LATEST_TAG with build type '$build_type' and link type '$link_type' (shared libs: $shared_libs)..."
cd "$pbSrcDir"

# Create & switch to the build directory.
mkdir -p build
echo "Current location: $(pwd)"
cd build

# Configure arguments
configureArgs=(
    "-G" "Unix Makefiles"
    "-D" "CMAKE_MAKE_PROGRAM=mingw32-make"
    "-D" "protobuf_FORCE_FETCH_DEPENDENCIES=ON"
    "-D" "CMAKE_BUILD_TYPE='$build_type'"
    "-D" "CMAKE_CXX_STANDARD=17"
    "-D" "BUILD_SHARED_LIBS='$shared_libs'"
    "-D" "protobuf_BUILD_SHARED_LIBS='$shared_libs'"
    "-D" "CMAKE_INSTALL_PREFIX='$install_dir'"
)

echo "Running configure with arguments:"
printf '  %s\n' "${configureArgs[@]}"

cmake .. "${configureArgs[@]}" || { echo "Configuration failed"; exit 1; }

cmake --build . --config "$build_type"
cmake --install . --parallel $(nproc) --config "$build_type"

cd "$GITHUB_WORKSPACE"
