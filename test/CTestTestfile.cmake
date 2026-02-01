# CMake generated Testfile for 
# Source directory: /code/test
# Build directory: /code/test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(perf_lite_unit_tests "/code/test/perf_lite_unit_tests")
set_tests_properties(perf_lite_unit_tests PROPERTIES  LABELS "fast" _BACKTRACE_TRIPLES "/code/test/CMakeLists.txt;31;add_test;/code/test/CMakeLists.txt;0;")
add_test(perf_lite_benchmarks "/code/test/perf_lite_benchmarks")
set_tests_properties(perf_lite_benchmarks PROPERTIES  LABELS "benchmark" _BACKTRACE_TRIPLES "/code/test/CMakeLists.txt;43;add_test;/code/test/CMakeLists.txt;0;")
subdirs("../_deps/googletest-build")
