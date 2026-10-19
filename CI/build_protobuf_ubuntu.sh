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

# Source the shared CI helpers.
. "$(dirname "$0")/common.sh"

# Validate input arguments: build_type, link_type, and version
if [ $# -ne 3 ]; then
    echo "Usage: $0 <build_type> <link_type> <version>"
    echo "Valid build types: Debug, Release, MinSizeRel, RelWithDebInfo"
    echo "Valid link types: Dynamic, Static"
    echo "version: Protobuf version/tag"
    exit 1
fi

build_type=$1
link_type=$2
version=$3

ci_validate_build_type "$build_type"
ci_validate_link_type "$link_type"
shared_libs="$(ci_shared_libs "$link_type")"

PROTOBUF_LATEST_TAG="$version"
PROTOBUF_LATEST_TAG_WITHOUT_V="${PROTOBUF_LATEST_TAG#v}"

# URLs and paths
pbSrcCompressed="protobuf-$PROTOBUF_LATEST_TAG_WITHOUT_V.tar.gz"
pbUrl="https://github.com/protocolbuffers/protobuf/releases/download/$PROTOBUF_LATEST_TAG/$pbSrcCompressed"

ci_download "$pbUrl" "Protobuf"
ci_extract "$pbSrcCompressed" "Protobuf"

# Build and install Protobuf
echo "Installing Protobuf $PROTOBUF_LATEST_TAG with build type '$build_type' and link type '$link_type'..."
ci_enter_build_dir "protobuf-$PROTOBUF_LATEST_TAG_WITHOUT_V"

# Configure arguments
configureArgs=(
    "-G" "Unix Makefiles"
    "-D" "protobuf_LOCAL_DEPENDENCIES_ONLY=ON"
    "-D" "CMAKE_BUILD_TYPE='$build_type'"
    "-D" "CMAKE_CXX_STANDARD=17"
    "-D" "BUILD_SHARED_LIBS='$shared_libs'"
    "-D" "protobuf_BUILD_SHARED_LIBS='$shared_libs'"
)

ci_print_configure_args "${configureArgs[@]}"

ci_cmake_configure "${configureArgs[@]}"

ci_make_build_install

ci_back_to_workspace
