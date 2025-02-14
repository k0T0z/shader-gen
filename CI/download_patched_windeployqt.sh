#!/bin/bash

# Ensure the target directory parameter is provided
if [ -z "$1" ]; then
    echo "Usage: $0 target_dir"
    exit 1
fi

TARGET_DIR="$1"
RETRY_COUNT=3
SLEEP_TIME=5
ZIP_FILE="windeployqt64releaseonly.zip"
DOWNLOAD_URL="http://tripleboot.org/Uploads/windeployqt64releaseonly.zip"

echo  "Target directory: $TARGET_DIR"

# Ensure the target directory exists
if  [ ! -d  "$TARGET_DIR"  ]; then
    echo  "Creating target directory: $TARGET_DIR"
    mkdir  -p  "$TARGET_DIR"
fi

download() {
    echo "Downloading patched ${ZIP_FILE}..."
    local attempt=1
    while [ $attempt -le $RETRY_COUNT ]; do
        curl -L -o "$ZIP_FILE" "$DOWNLOAD_URL"
        if [ $? -eq 0 ]; then
            echo "Download succeeded."
            return 0
        fi
        echo "Download failed (attempt $attempt/$RETRY_COUNT). Retrying in $SLEEP_TIME seconds..."
        sleep $SLEEP_TIME
        ((attempt++))
    done
    echo "Download failed after $RETRY_COUNT attempts. Exiting."
    return 1
}

# Attempt to download; exit if unsuccessful
download || exit 1

echo "Extracting patched executable into $TARGET_DIR..."
unzip -o "$ZIP_FILE" -d "$TARGET_DIR"
