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

# Shared helpers for the bash-based CI dependency build scripts (MSYS2 / Ubuntu).
# Source this file from a build script:
#   . "$(dirname "$0")/common.sh"

# Print an error to stderr and abort.
ci_die() {
    echo "$1" >&2
    exit 1
}

# Validate a CMake build type.
ci_validate_build_type() {
    case "$1" in
        Debug|Release|MinSizeRel|RelWithDebInfo) ;;
        *) ci_die "Invalid build type: $1" ;;
    esac
}

# Validate a link type.
ci_validate_link_type() {
    case "$1" in
        Dynamic|Static) ;;
        *) ci_die "Invalid link type: $1" ;;
    esac
}

# Map a link type to the BUILD_SHARED_LIBS value (ON/OFF).
ci_shared_libs() {
    case "$1" in
        Static)  echo "OFF" ;;
        Dynamic) echo "ON" ;;
    esac
}

# Create the install prefix directory when it does not exist.
ci_ensure_dir() {
    if [ ! -d "$1" ]; then
        echo "Install prefix directory '$1' does not exist. Creating it..."
        mkdir -p "$1"
    fi
}

# Download $1 (URL) into the current directory, labelling progress with $2.
ci_download() {
    local url="$1" label="$2"
    echo "Downloading $label source from $url..."
    curl -L -O "$url" || ci_die "Download failed"
}

# Extract the archive $1 into the current directory, labelling progress with $2.
ci_extract() {
    local archive="$1" label="$2"
    echo "Extracting $label source..."
    tar -xf "$archive"
}

# Enter the extracted source directory $1 and create/switch to its build directory.
ci_enter_build_dir() {
    local src_dir="$1"
    cd "$src_dir" || ci_die "Source directory '$src_dir' not found"
    mkdir -p build
    echo "Current location: $(pwd)"
    cd build || ci_die "Failed to enter build directory"
}

# Echo the configure arguments.
ci_print_configure_args() {
    echo "Running configure with arguments:"
    printf '  %s\n' "$@"
}

# Configure with CMake from the build directory (aborts on failure).
ci_cmake_configure() {
    cmake .. "$@" || ci_die "Configuration failed"
}

# Build with CMake.
ci_cmake_build() {
    cmake --build . --config "$1"
}

# Install with CMake (parallel).
ci_cmake_install() {
    cmake --install . --parallel "$(nproc)" --config "$1"
}

# Build and install with Make (system-wide, Ubuntu).
ci_make_build_install() {
    make -j"$(nproc)"
    sudo make install
}

# Return to the GitHub workspace root.
ci_back_to_workspace() {
    cd "${GITHUB_WORKSPACE:-..}" || ci_die "Failed to return to workspace"
}
