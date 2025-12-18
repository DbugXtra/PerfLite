#!/bin/bash

# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake .. -DBUILD_TESTS=ON

# Clean previous builds
make clean

# Build the project
make

# Run tests
if [ $? -eq 0 ]; then
    echo "Running tests..."
    ./perf_lite_tests
else
    echo "Build failed!"
    exit 1
fi
