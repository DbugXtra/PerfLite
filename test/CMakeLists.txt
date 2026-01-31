cmake_minimum_required(VERSION 3.10)
project(PerfLiteTests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Try to find an installed GoogleTest; if not found, fetch it at configure time.
find_package(GTest QUIET)
if (NOT GTest_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/heads/main.zip
    )
    # Prevent overriding the parent project's compiler/linker settings on Windows
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

# Create test executable
### Unit tests (fast, deterministic)
add_executable(perf_lite_unit_tests
    unit_tests.cpp
)
target_link_libraries(perf_lite_unit_tests
    GTest::gtest_main
    pthread
)

enable_testing()
add_test(NAME perf_lite_unit_tests COMMAND perf_lite_unit_tests)
set_tests_properties(perf_lite_unit_tests PROPERTIES LABELS "fast")

### Benchmark tests (may be slower/flaky) - separate target
add_executable(perf_lite_benchmarks
    benchmark_test.cpp
    main.cpp
)
target_link_libraries(perf_lite_benchmarks
    GTest::gtest
    pthread
)
add_test(NAME perf_lite_benchmarks COMMAND perf_lite_benchmarks)
set_tests_properties(perf_lite_benchmarks PROPERTIES LABELS "benchmark")
