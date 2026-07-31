#!/usr/bin/env bash
set -e

BUILD_DIR="build"

if [ ! -d "$BUILD_DIR" ]; then
    echo "No build directory found, configuring..."
    cmake -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Debug
fi

cmake --build "$BUILD_DIR" --parallel
