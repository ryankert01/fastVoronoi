#!/bin/bash

# Build script for fastVoronoi

set -e

echo "Building fastVoronoi..."

# Clean previous builds
echo "Cleaning previous builds..."
rm -rf build/ dist/ *.egg-info/

# Create build directory
mkdir -p build
cd build

# Run CMake
echo "Running CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
echo "Building C++ extension..."
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 2)

cd ..

# Install Python package
echo "Installing Python package..."
pip install -e .

echo "Build complete!"
echo "Run 'pytest tests/' to run tests"
