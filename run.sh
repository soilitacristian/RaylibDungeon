#!/usr/bin/env bash
set -e

./build.sh

echo "Launching..."
./build/main
