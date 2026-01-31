#include <gtest/gtest.h>
#include "../perf_lite.h"

using namespace PerfLite;

// Tests for the to_unit helper ensuring precise conversions
TEST(UnitTests, ToUnitConversions) {
    using namespace std::chrono;
    duration<double, std::nano> d_ns(1500.0); // 1500 ns

    EXPECT_NEAR(to_unit(d_ns, TimeUnit::Nanoseconds), 1500.0, 1e-9);
    EXPECT_NEAR(to_unit(d_ns, TimeUnit::Microseconds), 1.5, 1e-12);
    EXPECT_NEAR(to_unit(d_ns, TimeUnit::Milliseconds), 0.0015, 1e-15);
    EXPECT_NEAR(to_unit(d_ns, TimeUnit::Seconds), 1.5e-6, 1e-18);
}

// Tests BenchmarkResult::calculate_statistics using deterministic durations
TEST(UnitTests, CalculateStatisticsDeterministic) {
    BenchmarkResult r(TimeUnit::Microseconds);
    r.name = "deterministic";
    // 1000ns, 2000ns, 3000ns
    r.durations.push_back(std::chrono::duration<double, std::nano>(1000.0));
    r.durations.push_back(std::chrono::duration<double, std::nano>(2000.0));
    r.durations.push_back(std::chrono::duration<double, std::nano>(3000.0));

    r.calculate_statistics();

    // Mean should be 2000 ns -> 2.0 us
    EXPECT_NEAR(r.mean_time, 2.0, 1e-12);
    // Min should be 1000 ns -> 1.0 us
    EXPECT_NEAR(r.min_time, 1.0, 1e-12);
    // Stddev (sample) should be 1000 ns -> 1.0 us
    EXPECT_NEAR(r.stddev_time, 1.0, 1e-12);
    // Ops/sec = 1e9 / 2000 ns = 500000
    EXPECT_NEAR(r.ops_per_sec, 500000.0, 1e-6);
}
