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

# Check for required arguments
if [ $# -ne 4 ]; then
    echo "Usage: $0 <build_type> <link_type> <install_dir> <version>"
    echo "  build_type: Debug | Release | MinSizeRel | RelWithDebInfo"
    echo "  link_type: Dynamic | Static"
    echo "  install_dir: Path to installation prefix"
    echo "  version: Qt version"
    exit 1
fi

build_type=$1
link_type=$2
install_dir=$3
Qt6Version=$4
majorMinor=$(echo "$Qt6Version" | cut -d. -f1-2)

ci_validate_build_type "$build_type"
ci_validate_link_type "$link_type"
ci_ensure_dir "$install_dir"

# URLs and paths
qtSrcCompressed="qt-everywhere-src-$Qt6Version.tar.xz"
qtUrl="https://download.qt.io/official_releases/qt/$majorMinor/$Qt6Version/single/$qtSrcCompressed"

ci_download "$qtUrl" "Qt"
ci_extract "$qtSrcCompressed" "Qt"

echo "Configuring Qt with prefix: $install_dir"

# Verify required tools
python --version || ci_die "Python not found"
cmake --version || ci_die "CMake not found"
gcc --version || ci_die "MINGW gcc not found"

# Configure arguments
configureArgs=(
    "-prefix" "$install_dir"
    "-submodules" "qtbase"
    "-skip" "qtdeclarative,qtmultimedia"
)

# Add build-type flags
case "$build_type" in
    Debug)
        configureArgs+=("-debug")
        ;;
    Release)
        configureArgs+=("-release")
        ;;
    MinSizeRel)
        configureArgs+=("-release" "-optimize-size")
        ;;
    RelWithDebInfo)
        configureArgs+=("-release" "-force-debug-info")
        ;;
esac

case "$link_type" in
    Static)
        configureArgs+=("-static")
        ;;
    Dynamic)
        configureArgs+=("-shared")
        ;;
esac

# CMake options
configureArgs+=("--")
configureArgs+=("-G" "Ninja")
configureArgs+=("-D" "CMAKE_C_COMPILER=gcc" "-D" "CMAKE_CXX_COMPILER=g++") # We need to use MinGW GCC
configureArgs+=("-D" "QT_BUILD_EXAMPLES_BY_DEFAULT=OFF")
configureArgs+=("-D" "QT_BUILD_TESTS_BY_DEFAULT=OFF")
configureArgs+=("-D" "QT_BUILD_TOOLS_BY_DEFAULT=ON")

# Use the MSYS2/MinGW PostgreSQL installation.
# C:/Program Files/PostgreSQL/17/lib/libpq.a has link issues with Qt.
configureArgs+=("-D" "PostgreSQL_ROOT=/mingw64")

# Build Qt
ci_enter_build_dir "qt-everywhere-src-$Qt6Version"

ci_print_configure_args "${configureArgs[@]}"

../configure "${configureArgs[@]}" || ci_die "Configuration failed"

echo "Starting build..."
ci_cmake_build "$build_type" || ci_die "Build failed"

echo "Installing to $install_dir..."
ci_cmake_install "$build_type" || ci_die "Installation failed"

ci_back_to_workspace
echo "Qt build+install complete."
