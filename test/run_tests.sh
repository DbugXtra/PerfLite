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

# Run tests using CTest so we execute the configured test targets
if [ $? -eq 0 ]; then
    echo "Running tests (fast label by default)..."
    # Run only fast/deterministic unit tests by default (CI-friendly)
    ctest -L fast --output-on-failure || {
        echo "Fast tests failed; running all tests for details..."
        ctest --output-on-failure
    }
else
    echo "Build failed!"
    exit 1
fi
