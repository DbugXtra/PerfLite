#include <gtest/gtest.h>
#include "../perf_lite.h"
#include <stdexcept>

// Test that benchmark can be created and run
TEST(BenchmarkTest, CanCreateAndRun) {
    PerfLite::Benchmark benchmark;
    auto result = benchmark.run([]() {
        volatile int x = 0;
        x += 1;
    });
    
    EXPECT_FALSE(result.durations.empty());
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
    EXPECT_GT(result.mean_time, 0.0);
}

// Test with function that returns a value
TEST(BenchmarkTest, FunctionWithReturnValue) {
    PerfLite::Benchmark benchmark;
    auto result = benchmark.run([]() -> int {
        return 42;
    });
    
    EXPECT_FALSE(result.durations.empty());
    EXPECT_GT(result.mean_time, 0.0);
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

// Edge case: empty lambda should run and record durations
TEST(BenchmarkTest, EmptyLambda) {
    PerfLite::Benchmark benchmark;
    auto result = benchmark.run([]() {});
    EXPECT_FALSE(result.durations.empty());
    EXPECT_GT(result.mean_time, 0.0);
}

// Edge case: setting zero iterations or zero warmup should trigger the assert
// Assertions terminate the process; use death tests to ensure they fire.
TEST(BenchmarkDeathTest, ZeroIterationsDeath) {
#if GTEST_HAS_DEATH_TEST
    EXPECT_DEATH({ PerfLite::Benchmark().iterations(0); }, ".*");
#else
    GTEST_SKIP() << "Death tests not supported on this platform";
#endif
}

TEST(BenchmarkDeathTest, ZeroWarmupDeath) {
#if GTEST_HAS_DEATH_TEST
    EXPECT_DEATH({ PerfLite::Benchmark().warmup(0); }, ".*");
#else
    GTEST_SKIP() << "Death tests not supported on this platform";
#endif
}

// Exception handling: benchmark should propagate exceptions from the function
TEST(BenchmarkTest, ExceptionPropagation) {
    PerfLite::Benchmark benchmark;
    EXPECT_THROW(
        benchmark.run([]() { throw std::runtime_error("boom"); }),
        std::runtime_error
    );
}

// Verify time unit ordering: numeric mean_time should decrease as unit magnitude increases
TEST(BenchmarkTest, TimeUnitOrdering) {
    auto make_mean = [](PerfLite::TimeUnit unit) {
        PerfLite::Benchmark b;
        auto r = b.unit(unit).iterations(10).run([](){ volatile int x = 0; x++; });
        return r.mean_time;
    };

    double mean_ns = make_mean(PerfLite::TimeUnit::Nanoseconds);
    double mean_us = make_mean(PerfLite::TimeUnit::Microseconds);
    double mean_ms = make_mean(PerfLite::TimeUnit::Milliseconds);
    double mean_s  = make_mean(PerfLite::TimeUnit::Seconds);

    // Numeric ordering: ns > us > ms > s
    EXPECT_GT(mean_ns, 0.0);
    EXPECT_GT(mean_us, 0.0);
    EXPECT_GT(mean_ms, 0.0);
    EXPECT_GT(mean_s, 0.0);
    EXPECT_GT(mean_ns, mean_us);
    EXPECT_GT(mean_us, mean_ms);
    EXPECT_GT(mean_ms, mean_s);
}
