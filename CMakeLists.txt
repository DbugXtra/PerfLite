cmake_minimum_required(VERSION 3.10)
project(PerfLiteExample)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optionally add the example executable if a top-level main.cpp exists.
if (EXISTS "${CMAKE_SOURCE_DIR}/main.cpp")
    add_executable(perf_lite_example main.cpp)

    # Enable compiler warnings for the example target
    target_compile_options(perf_lite_example PRIVATE 
        $<$<CONFIG:Debug>:-Wall -Wextra -g>
        $<$<CONFIG:Release>:-O2 -DNDEBUG>
    )

    # Link pthread library (required for std::chrono on some systems)
    target_link_libraries(perf_lite_example pthread)
else()
    message(STATUS "No top-level main.cpp found; skipping perf_lite_example target.")
endif()

# Add tests
option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
    # Include the test directory
    add_subdirectory(test)
endif()
