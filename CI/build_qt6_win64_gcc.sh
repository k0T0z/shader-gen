#!/bin/bash

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

# Add link-type flag
if [ "$link_type" = "Static" ]; then
    configureArgs+=("-static")
else
    configureArgs+=("-shared")
fi

# CMake options
configureArgs+=("--")
configureArgs+=("-G" "Ninja")
configureArgs+=("-D" "CMAKE_C_COMPILER=gcc" "-D" "CMAKE_CXX_COMPILER=g++") # We need to use MinGW GCC
configureArgs+=("-D" "QT_BUILD_EXAMPLES_BY_DEFAULT=OFF")
configureArgs+=("-D" "QT_BUILD_TESTS_BY_DEFAULT=OFF")
configureArgs+=("-D" "QT_BUILD_TOOLS_BY_DEFAULT=ON")

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
