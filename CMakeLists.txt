cmake_minimum_required(VERSION 3.10)
project(PerfLiteExample)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the main executable
add_executable(perf_lite_example main.cpp)

# Enable compiler warnings
target_compile_options(perf_lite_example PRIVATE 
    $<$<CONFIG:Debug>:-Wall -Wextra -g>
    $<$<CONFIG:Release>:-O2 -DNDEBUG>
)

# Link pthread library (required for std::chrono on some systems)
target_link_libraries(perf_lite_example pthread)

# Add tests
option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
    # Find Google Test
    find_package(GTest REQUIRED)
    
    # Create test executable
    add_executable(perf_lite_tests test/benchmark_test.cpp)
    
    # Link with Google Test and PerfLite library
    target_link_libraries(perf_lite_tests 
        GTest::gtest 
        GTest::gtest_main
        perf_lite_example
    )
    
    # Enable testing
    enable_testing()
    add_test(NAME perf_lite_tests COMMAND perf_lite_tests)
endif()
