#ifndef PERF_LITE_H
#define PERF_LITE_H

#include <chrono>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <string>
#include <functional>
#include <cassert>
#include <cstdint>
#include <type_traits>
#include <atomic>

namespace PerfLite {

// Time unit enumeration for specifying output time units.
enum class TimeUnit {
    Nanoseconds,
    Microseconds,
    Milliseconds,
    Seconds
};

// Helper function to convert duration to specified time unit.
// Throws an assertion if an invalid unit is provided.
template<typename Duration>
double to_unit(const Duration& d, TimeUnit unit) {
    switch (unit) {
        case TimeUnit::Nanoseconds:
            return std::chrono::duration_cast<std::chrono::nanoseconds>(d).count();
        case TimeUnit::Microseconds:
            return std::chrono::duration_cast<std::chrono::microseconds>(d).count();
        case TimeUnit::Milliseconds:
            return std::chrono::duration_cast<std::chrono::milliseconds>(d).count();
        case TimeUnit::Seconds:
            return std::chrono::duration_cast<std::chrono::seconds>(d).count();
        default:
            assert(false && "Invalid TimeUnit specified");
            return 0; // Unreachable, satisfies compiler
    }
}

// Portable optimization barrier to prevent compiler from optimizing out values.
// Supports GCC, MSVC, and other compilers.
template<typename T>
inline void DoNotOptimize(T& value) {
#if defined(__GNUC__)
    asm volatile("" : : "r,m"(value) : "memory");
#elif defined(_MSC_VER)
    volatile T temp = value;
    (void)temp;
#else
    std::atomic_thread_fence(std::memory_order_seq_cst);
#endif
}

// Structure to hold benchmark results and compute statistics.
struct BenchmarkResult {
    std::string name;
    std::vector<std::chrono::nanoseconds> durations;
    double min_time;
    double mean_time;
    double stddev_time;
    double ops_per_sec;
    TimeUnit time_unit;

    // Constructor initializes all fields to safe defaults.
    explicit BenchmarkResult(TimeUnit unit = TimeUnit::Nanoseconds)
        : min_time(0.0), mean_time(0.0), stddev_time(0.0), ops_per_sec(0.0), time_unit(unit) {}

    // Calculates statistics from collected durations.
    // Converts results to the specified time unit.
    void calculate_statistics() {
        if (durations.empty()) {
            std::cerr << "Warning: No durations recorded for benchmark '" << name << "'\n";
            return;
        }

        // Calculate min
        min_time = to_unit(*std::min_element(durations.begin(), durations.end()), time_unit);

        // Calculate mean
        auto sum = std::accumulate(durations.begin(), durations.end(), std::chrono::nanoseconds(0));
        mean_time = to_unit(sum, time_unit) / durations.size();

        // Calculate standard deviation
        double variance = 0.0;
        for (const auto& d : durations) {
            double diff = to_unit(d, time_unit) - mean_time;
            variance += diff * diff;
        }
        if (durations.size() > 1) {
            variance /= (durations.size() - 1);
        } else {
            variance = 0.0;
        }
        stddev_time = std::sqrt(variance);

        // Calculate operations per second (avoid division by zero)
        double mean_time_ns = to_unit(sum, TimeUnit::Nanoseconds) / durations.size();
        if (mean_time_ns > 1e-9) {
            ops_per_sec = 1e9 / mean_time_ns; // 1e9 ns = 1 second
        } else {
            ops_per_sec = 0.0;
        }
    }

    // Prints results to the specified output stream with unit-appropriate precision.
    void print(std::ostream& os = std::cout) const {
        // Adjust precision based on time unit
        int precision = (time_unit == TimeUnit::Nanoseconds) ? 2 : 
                        (time_unit == TimeUnit::Microseconds) ? 3 :
                        (time_unit == TimeUnit::Milliseconds) ? 4 : 6;
        os << std::fixed << std::setprecision(precision);
        os << "Benchmark: " << name << "\n";
        os << "  Min:      " << min_time << " " << time_unit_to_string() << "\n";
        os << "  Mean:     " << mean_time << " " << time_unit_to_string() << "\n";
        os << "  StdDev:   " << stddev_time << " " << time_unit_to_string() << "\n";
        os << "  Ops/sec:  " << ops_per_sec << "\n\n";
    }

private:
    // Helper to convert TimeUnit to string for output
    std::string time_unit_to_string() const {
        switch (time_unit) {
            case TimeUnit::Nanoseconds: return "ns";
            case TimeUnit::Microseconds: return "µs";
            case TimeUnit::Milliseconds: return "ms";
            case TimeUnit::Seconds: return "s";
            default: return "unknown";
        }
    }
};

// Benchmark runner class for configuring and executing benchmarks.
class Benchmark {
private:
    size_t warmup_iterations_;
    size_t iterations_;
    std::chrono::milliseconds target_duration_;
    TimeUnit time_unit_;
    std::string name_;

public:
    // Constructor with default configuration.
    Benchmark()
        : warmup_iterations_(10),
          iterations_(1000),
          target_duration_(100),
          time_unit_(TimeUnit::Nanoseconds),
          name_("Benchmark") {}

    // Sets the number of warmup iterations (must be non-zero).
    Benchmark& warmup(size_t count) {
        assert(count > 0 && "Warmup iterations must be greater than zero");
        warmup_iterations_ = count;
        return *this;
    }

    // Sets the number of benchmark iterations (must be non-zero).
    Benchmark& iterations(size_t count) {
        assert(count > 0 && "Benchmark iterations must be greater than zero");
        iterations_ = count;
        return *this;
    }

    // Sets the target total duration for benchmarking.
    Benchmark& target_duration(std::chrono::milliseconds duration) {
        assert(duration.count() > 0 && "Target duration must be greater than zero");
        target_duration_ = duration;
        return *this;
    }

    // Sets the time unit for output.
    Benchmark& unit(TimeUnit unit) {
        time_unit_ = unit;
        return *this;
    }

    // Sets the benchmark name.
    Benchmark& name(const std::string& n) {
        name_ = n;
        return *this;
    }

    // Runs the benchmark with the specified function.
    // Adjusts iterations to meet the target duration (minimum 100ms by default).
    // Handles both void and non-void return types.
    template<typename Func, typename = std::enable_if_t<std::is_invocable_v<Func>>>
    BenchmarkResult run(Func&& func) const {
        BenchmarkResult result(time_unit_);
        result.name = name_;
        result.durations.reserve(iterations_);

        // Warmup phase to stabilize performance
        for (size_t i = 0; i < warmup_iterations_; ++i) {
            func();
        }

        // Measure execution time for 1000 iterations to estimate per-iteration time
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < 1000; ++i) {
            if constexpr (std::is_same_v<std::invoke_result_t<Func>, void>) {
                func();
                std::atomic_thread_fence(std::memory_order_seq_cst); // Memory barrier for void functions
            } else {
                auto result = func();
                DoNotOptimize(result);
            }
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto total_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

        uint64_t adjusted_iterations = iterations_;

        if (total_time.count() > 0) {
            // 1. Calculate Target Duration in nanoseconds (1 ms = 1,000,000 ns)
            const uint64_t target_duration_ns = target_duration_.count() * 1'000'000LL;

            // 2. Calculate Time Per Iteration in nanoseconds
            // total_time is the time for 1000 runs.
            const double time_per_iteration_ns = static_cast<double>(total_time.count()) / 1000.0;

            // 3. Calculate Adjusted Iterations: Target_ns / Time_Per_Iteration_ns
            if (time_per_iteration_ns > 0.0) {
                adjusted_iterations = static_cast<uint64_t>(
                    static_cast<double>(target_duration_ns) / time_per_iteration_ns
                );
            }
            
            // Ensure minimum 1000 iterations and cap at 1,000,000 to prevent overflow/excessive runtime
            adjusted_iterations = std::max(adjusted_iterations, static_cast<uint64_t>(1000));
            adjusted_iterations = std::min(adjusted_iterations, static_cast<uint64_t>(1'000'000));
        }

        // Run actual benchmark
        for (size_t i = 0; i < adjusted_iterations; ++i) {
            auto start = std::chrono::high_resolution_clock::now();
            if constexpr (std::is_same_v<std::invoke_result_t<Func>, void>) {
                func();
                std::atomic_thread_fence(std::memory_order_seq_cst); // Memory barrier for void functions
            } else {
                auto result = func();
                DoNotOptimize(result);
            }
            auto end = std::chrono::high_resolution_clock::now();
            result.durations.push_back(end - start);
        }

        result.calculate_statistics();
        return result;
    }

    // Runs the benchmark with a function and arguments.
    template<typename Func, typename... Args>
    BenchmarkResult run(Func&& func, Args&&... args) const {
        auto wrapper = [&func, args...]() {
            return func(args...);
        };
        return run(wrapper);
    }
};

// Convenience function for running a benchmark with default configuration.
template<typename Func>
BenchmarkResult benchmark(Func&& func) {
    return Benchmark().run(func);
}

template<typename Func, typename... Args>
BenchmarkResult benchmark(Func&& func, Args&&... args) {
    return Benchmark().run(func, args...);
}

} // namespace PerfLite

#endif // PERF_LITE_H