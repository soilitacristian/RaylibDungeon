#!/usr/bin/env bash

# stops the script if any command fails
set -e

BUILD_DIR="build"

# -d : does this file exist and is it a directory
# ! : negation, so we check if the directory doesn't exist
if [ ! -d "$BUILD_DIR" ]; then
    echo "No build directory found, configuring..."
    cmake -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Debug
fi

cmake --build "$BUILD_DIR" --parallel

echo "Launching..."
"$BUILD_DIR/main"
