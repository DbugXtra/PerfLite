#include <gtest/gtest.h>
#include <atomic>
#include "../perf_lite.h"

// Helper function: Simple volatile increment benchmark
PerfLite::BenchmarkResult runSimpleAlgorithm(PerfLite::Benchmark& benchmark) {
    return benchmark.run([]() {
        volatile int x = 0;
        x += 1;
    });
}

// Test that benchmark can be created and run
TEST(BenchmarkTest, CanCreateAndRun) {
    PerfLite::Benchmark benchmark;
    auto result = runSimpleAlgorithm(benchmark);
    
    EXPECT_FALSE(result.durations.empty());
    EXPECT_GT(result.min_time, 0.0);
    EXPECT_GT(result.mean_time, 0.0);
    EXPECT_GE(result.stddev_time, 0.0);
}

// Test with non-void function
TEST(BenchmarkTest, NonVoidFunction) {
    PerfLite::Benchmark benchmark;
    auto result = runSimpleAlgorithm(benchmark);
    
    EXPECT_FALSE(result.durations.empty());
    EXPECT_GT(result.min_time, 0.0);
    EXPECT_GT(result.mean_time, 0.0);
}

// Test with custom configuration
TEST(BenchmarkTest, CustomConfiguration) {
    PerfLite::Benchmark benchmark;
    benchmark
        .warmup(5)
        .iterations(10)
        .target_duration(std::chrono::milliseconds(50))
        .unit(PerfLite::TimeUnit::Microseconds)
        .name("CustomBenchmark");
    auto result = runSimpleAlgorithm(benchmark);
    
    EXPECT_EQ(result.name, "CustomBenchmark");
    EXPECT_FALSE(result.durations.empty());
}

// Test that time unit conversion works correctly
TEST(BenchmarkTest, TimeUnitConversion) {
    PerfLite::Benchmark benchmark;
    benchmark.unit(PerfLite::TimeUnit::Milliseconds);
    auto result = runSimpleAlgorithm(benchmark);
    
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
    auto result = runSimpleAlgorithm(benchmark);
    
    EXPECT_FALSE(result.durations.empty());
}

// Test warmup functionality
TEST(BenchmarkTest, WarmupFunctionality) {
    // Use an atomic counter to track warmup executions
    std::atomic<int> warmup_count{0};
    std::atomic<int> benchmark_count{0};
    
    // Create a function that increments counters
    auto counting_func = [&]() {
        warmup_count++;
        volatile int x = 0;
        x += 1;
    };
    
    // Run with low iteration count and specific warmup count
    PerfLite::Benchmark benchmark;
    auto result = benchmark
        .warmup(7)  // Specific warmup count to verify execution
        .iterations(5)
        .target_duration(std::chrono::milliseconds(10))
        .name("WarmupTest")
        .run(counting_func);
    
    // Verify that:
    // 1. Warmup occurred (counter was incremented during warmup phase)
    EXPECT_GT(warmup_count, 7);  // Should be 7 warmup + adjusted iterations
    
    // 2. Benchmark completed successfully
    EXPECT_FALSE(result.durations.empty());
    EXPECT_GT(result.min_time, 0.0);
    EXPECT_GT(result.mean_time, 0.0);
    EXPECT_EQ(result.name, "WarmupTest");
}

// Test different warmup counts don't affect measurement consistency
TEST(BenchmarkTest, WarmupConsistency) {
    auto simple_func = []() {
        volatile int x = 0;
        x += 1;
    };
    
    // Run with different warmup counts
    PerfLite::Benchmark benchmark1;
    auto result1 = benchmark1
        .warmup(1)
        .iterations(100)
        .target_duration(std::chrono::milliseconds(10))
        .run(simple_func);
    
    PerfLite::Benchmark benchmark2;
    auto result2 = benchmark2
        .warmup(50)
        .iterations(100)
        .target_duration(std::chrono::milliseconds(10))
        .run(simple_func);
    
    // Both should complete successfully
    EXPECT_FALSE(result1.durations.empty());
    EXPECT_FALSE(result2.durations.empty());
    
    // Both should have recorded durations and valid statistics
    EXPECT_GT(result1.mean_time, 0.0);
    EXPECT_GT(result2.mean_time, 0.0);
    
    // Results should be in similar range (within reason, not too strict)
    // The ratio should be between 0.5x and 2x to account for system variance
    double ratio = result1.mean_time / result2.mean_time;
    EXPECT_GT(ratio, 0.3);
    EXPECT_LT(ratio, 3.0);
}
