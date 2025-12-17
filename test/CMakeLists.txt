cmake_minimum_required(VERSION 3.10)
project(PerfLiteTests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Google Test
find_package(GTest REQUIRED)

# Create test executable
add_executable(perf_lite_tests 
    benchmark_test.cpp
    main.cpp
)

# Link with Google Test and PerfLite library
target_link_libraries(perf_lite_tests 
    GTest::gtest 
    GTest::gtest_main
    pthread
)

# Enable testing
enable_testing()
add_test(NAME perf_lite_tests COMMAND perf_lite_tests)
