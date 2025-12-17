#include "../perf_lite.h"

void simple_test_function() {
    volatile int x = 0;
    x += 1; // Simple operation to benchmark
}

int non_void_function() {
    volatile int x = 0;
    return x + 1;
}

int main() {
    // Test void function
    auto result1 = PerfLite::benchmark(simple_test_function);
    result1.print();

    // Test non-void function
    auto result2 = PerfLite::benchmark(non_void_function);
    result2.print();

    return 0;
}
