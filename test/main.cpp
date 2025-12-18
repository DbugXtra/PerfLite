#include <gtest/gtest.h>
#include "../perf_lite.h"

void simple_test_function() {
    volatile int x = 0;
    x += 1; // Simple operation to benchmark
}

int non_void_function() {
    volatile int x = 0;
    return x + 1;
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    
    // Test void function
    auto result1 = PerfLite::benchmark(simple_test_function);
    assert(!result1.durations.empty() && result1.mean_time > 0.0 && "simple_test_function benchmark failed");
    std::cout << "simple_test_function benchmark test passed" << std::endl;
    result1.print();

    // Test non-void function
    auto result2 = PerfLite::benchmark(non_void_function);
    assert(!result2.durations.empty() && result2.mean_time > 0.0 && "non_void_function benchmark failed");
    std::cout << "non_void_function benchmark test passed" << std::endl;
    result2.print();

    return RUN_ALL_TESTS();
}
