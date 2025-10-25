#!/bin/bash

# Build script for perf_lite example

# Compile main.cpp with perf_lite.h
g++ -std=c++17 -O2 -pthread -o perf_lite_example main.cpp

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Build successful! Run with: ./perf_lite_example"
else
    echo "Build failed!"
    exit 1
fi
