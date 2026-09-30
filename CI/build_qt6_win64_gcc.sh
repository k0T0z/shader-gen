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

# Check for required arguments
if [ $# -ne 3 ]; then
    echo "Usage: $0 <build_type> <link_type> <install_dir>"
    echo "  build_type: Debug | Release | MinSizeRel | RelWithDebInfo"
    echo "  link_type: Dynamic | Static"
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

# Check if the install prefix directory exists; if not, create it
if [ ! -d "$install_dir" ]; then
    echo "Install prefix directory '$install_dir' does not exist. Creating it..."
    mkdir -p "$install_dir"
fi

Qt6Version="6.9.1"
majorMinor=$(echo "$Qt6Version" | cut -d. -f1-2)

# URLs and paths
qtSrcCompressed="qt-everywhere-src-$Qt6Version.tar.xz"
qtUrl="https://download.qt.io/official_releases/qt/$majorMinor/$Qt6Version/single/$qtSrcCompressed"

# Download source
echo "Downloading Qt source from $qtUrl..."
curl -L -O "$qtUrl" || { echo "Download failed"; exit 1; }

# Extract
echo "Extracting Qt source..."
tar -xf "$qtSrcCompressed"

qtSrcDir="qt-everywhere-src-$Qt6Version"

echo "Configuring Qt with prefix: $install_dir"

# Verify required tools
python --version || { echo "Python not found"; exit 1; }
cmake --version || { echo "CMake not found"; exit 1; }
gcc --version || { echo "MINGW gcc not found"; exit 1; }

# Configure arguments
configureArgs=(
    "-prefix" "$install_dir"
    "-submodules" "qtbase"
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

# Build configuration
cd "$qtSrcDir" || exit
mkdir -p build
cd build || exit

echo "Running configure with arguments:"
printf '  %s\n' "${configureArgs[@]}"

../configure "${configureArgs[@]}" || { echo "Configuration failed"; exit 1; }

# Build and install
echo "Starting build..."
cmake --build . --config "$build_type" || { echo "Build failed"; exit 1; }

echo "Installing to $install_dir..."
cmake --install . --parallel $(nproc) --config "$build_type" || { echo "Installation failed"; exit 1; }

cd ../..
echo "Qt build+install complete."
