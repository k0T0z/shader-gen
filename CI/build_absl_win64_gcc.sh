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

# Check for the required arguments: BUILD_TYPE, LINK_TYPE, INSTALL_PREFIX, and VERSION
if [ $# -ne 4 ]; then
    echo "Usage: $0 <BUILD_TYPE> <LINK_TYPE> <INSTALL_PREFIX> <VERSION>"
    echo "Valid BUILD_TYPE values: Debug, Release, MinSizeRel, RelWithDebInfo"
    echo "Valid LINK_TYPE values: Dynamic, Static"
    echo "VERSION: Abseil version/tag"
    exit 1
fi

build_type=$1
link_type=$2
install_dir=$3
version=$4

ci_validate_build_type "$build_type"
ci_validate_link_type "$link_type"
shared_libs="$(ci_shared_libs "$link_type")"
ci_ensure_dir "$install_dir"

ABSL_LATEST_TAG="$version"
ABSL_LATEST_TAG_WITHOUT_V="${ABSL_LATEST_TAG#v}"

# URLs and paths
abslSrcCompressed="abseil-cpp-$ABSL_LATEST_TAG_WITHOUT_V.tar.gz"
abslUrl="https://github.com/abseil/abseil-cpp/releases/download/$ABSL_LATEST_TAG/$abslSrcCompressed"

ci_download "$abslUrl" "Abseil"
ci_extract "$abslSrcCompressed" "Abseil"

# Build and install Abseil
echo "Installing Abseil $ABSL_LATEST_TAG with build type '$build_type' and link type '$link_type' (shared libs: $shared_libs)..."
ci_enter_build_dir "abseil-cpp-$ABSL_LATEST_TAG_WITHOUT_V"

# Configure arguments
configureArgs=(
    "-G" "Unix Makefiles"
    "-D" "CMAKE_MAKE_PROGRAM=mingw32-make"
    "-D" "CMAKE_BUILD_TYPE='$build_type'"
    "-D" "CMAKE_CXX_STANDARD=17"
    "-D" "BUILD_SHARED_LIBS='$shared_libs'"
    "-D" "CMAKE_INSTALL_PREFIX='$install_dir'"

    "-D" "BUILD_TESTING=OFF"
    "-D" "ABSL_ENABLE_INSTALL=ON"
    "-D" "ABSL_PROPAGATE_CXX_STD=ON"
    "-D" "ABSL_BUILD_TESTING=OFF"
    "-D" "ABSL_BUILD_TEST_HELPERS=OFF"
    "-D" "ABSL_BUILD_MONOLITHIC_SHARED_LIBS='$shared_libs'"
)

ci_print_configure_args "${configureArgs[@]}"

ci_cmake_configure "${configureArgs[@]}"

ci_cmake_build "$build_type"
ci_cmake_install "$build_type"

ci_back_to_workspace
