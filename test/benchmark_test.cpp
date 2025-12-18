#include <gtest/gtest.h>
#include "../perf_lite.h"

// Test that benchmark can be created and run
TEST(BenchmarkTest, CanCreateAndRun) {
    PerfLite::Benchmark benchmark;
    auto result = benchmark.run([]() {
        volatile int x = 0;
        x += 1;
    });
    
    EXPECT_FALSE(result.durations.empty());
    EXPECT_GT(result.min_time, 0.0);
    EXPECT_GT(result.mean_time, 0.0);
    EXPECT_GE(result.stddev_time, 0.0);
}

// Test with non-void function
TEST(BenchmarkTest, NonVoidFunction) {
    PerfLite::Benchmark benchmark;
    auto result = benchmark.run([]() -> int {
        volatile int x = 0;
        return x + 1;
    });
    
    EXPECT_FALSE(result.durations.empty());
    EXPECT_GT(result.min_time, 0.0);
    EXPECT_GT(result.mean_time, 0.0);
}

// Test with custom configuration
TEST(BenchmarkTest, CustomConfiguration) {
    PerfLite::Benchmark benchmark;
    auto result = benchmark
        .warmup(5)
        .iterations(10)
        .target_duration(std::chrono::milliseconds(50))
        .unit(PerfLite::TimeUnit::Microseconds)
        .name("CustomBenchmark")
        .run([]() {
            volatile int x = 0;
            x += 1;
        });
    
    EXPECT_EQ(result.name, "CustomBenchmark");
    EXPECT_FALSE(result.durations.empty());
}

// Test that time unit conversion works correctly
TEST(BenchmarkTest, TimeUnitConversion) {
    PerfLite::Benchmark benchmark;
    auto result = benchmark
        .unit(PerfLite::TimeUnit::Milliseconds)
        .run([]() {
            volatile int x = 0;
            x += 1;
        });
    
    EXPECT_FALSE(result.durations.empty());
    EXPECT_GT(result.min_time, 0.0);
}

// Test with function that returns a value
TEST(BenchmarkTest, FunctionWithReturnValue) {
    PerfLite::Benchmark benchmark;
    auto result = benchmark.run([]() -> int {
        return 42;
    });
    
    EXPECT_FALSE(result.durations.empty());
    EXPECT_GT(result.min_time, 0.0);
}

// Test that benchmark handles zero iterations gracefully
TEST(BenchmarkTest, ZeroIterations) {
    PerfLite::Benchmark benchmark;
    // This should not crash, even though it's not a realistic use case
    auto result = benchmark
        .iterations(1)
        .run([]() {
            volatile int x = 0;
            x += 1;
        });
    
    EXPECT_FALSE(result.durations.empty());
}
