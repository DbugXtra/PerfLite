[![Run Tests](https://github.com/DbugXtra/PerfLite/actions/workflows/run_tests.yml/badge.svg?branch=main)](https://github.com/DbugXtra/PerfLite/actions/workflows/run_tests.yml)
<p align="left">
  <a href="https://donate.stripe.com/5kQ4gAa0b4pP7YQaW35EY00">
    <img src="./images/stripe-donate-qrcode.png" 
         alt="Scan to buy us a coffee with Stripe" 
         width="150" 
         height="150" 
         style="border: 2px solid #6772E5; border-radius: 5px;">
  </a>
  <br>
  <medium>Click or Scan to Buy us a Coffee</medium>
</p>

# PerfLite: A Minimalist C++ Micro-Benchmarking Library

PerfLite is a single-header C++ library designed for quick, lightweight, and customizable micro-benchmarking. It provides accurate measurement of function execution times, automatically adjusts iteration counts for stable results, and calculates essential statistical measures including mean, standard deviation, and throughput.

---

## 🚀 Features

* **Single-Header:** Easy to integrate—just include `perf_lite.h`.
* **Time Unit Flexibility:** Output results in nanoseconds (`ns`), microseconds (`µs`), milliseconds (`ms`), or seconds (`s`).
* **Statistical Output:** Calculates **Min Time**, **Mean Time**, **Standard Deviation (StdDev)**, and **Operations Per Second (Ops/sec)**.
* **Dynamic Iteration Adjustment:** Automatically increases the number of iterations to meet a target duration (defaulting to 100ms) for more stable, reliable measurements.
* **Optimization Barriers:** Includes a portable `DoNotOptimize` helper to prevent the compiler from eliminating benchmarked code.
* **Warmup Phase:** Executes a configurable number of warmup iterations to stabilize CPU caches and branch predictors.

---

## 🛠️ Usage and Setup

### 1. Requirements

* C++17 or later (required for `std::chrono`, `std::is_invocable_v`, etc.)
* A compatible C++ compiler (GCC, Clang, MSVC, etc.)

### 2. Integration

Since `PerfLite` is a header-only library, simply copy the `perf_lite.h` file into your project and include it in your source files:

```cpp
#include "perf_lite.h"

// Your main code...
````

### 3\. Basic Example

Benchmarking a simple function call:

```cpp
#include <iostream>
#include "perf_lite.h"

// Function to benchmark
void heavy_computation(int N) {
    long sum = 0;
    for (int i = 0; i < N; ++i) {
        sum += i * i;
    }
    // Use DoNotOptimize to prevent the compiler from optimizing out the calculation
    PerfLite::DoNotOptimize(sum); 
}

int main() {
    // 1. Run with default settings (100ms target duration, ns output)
    PerfLite::BenchmarkResult result = PerfLite::benchmark(heavy_computation, 1000);
    result.print();

    // 2. Custom configuration
    PerfLite::Benchmark()
        .name("Array Sort")
        .unit(PerfLite::TimeUnit::Microseconds) // Output in µs
        .warmup(50)                              // 50 warmup runs
        .target_duration(std::chrono::milliseconds(500)) // Run for 500ms
        .run([]() {
            std::vector<int> data(1000, 0);
            std::sort(data.begin(), data.end());
        })
        .print();

    return 0;
}
```

-----

## ⚙️ Configuration Methods

The `PerfLite::Benchmark` class uses a fluent interface for configuration:

| Method | Description | Default |
| :--- | :--- | :--- |
| `.name(std::string n)` | Sets the name for the benchmark run. | `"Benchmark"` |
| `.unit(TimeUnit unit)` | Sets the time unit for the printed output. | `Nanoseconds` |
| `.warmup(size_t count)` | Sets the number of **unmeasured** warmup iterations. | `10` |
| `.iterations(size_t count)` | Sets the initial/minimum number of measured iterations. | `1000` |
| `.target_duration(std::chrono::milliseconds duration)` | Sets the total time the benchmark will attempt to run for. | `100ms` |
| `.run(Func&& func)` | Executes the benchmark. | N/A |

-----

## 📊 Understanding the Results

A robust performance measurement relies on more than just the average time.

| Field | Description | Importance |
| :--- | :--- | :--- |
| **Min Time** | The **fastest** recorded single execution time. | Represents the best-case, highly optimized performance, often achieved when the CPU cache is "hot" and the OS is not interfering. |
| **Mean Time** | The arithmetic **average** of all recorded execution times. | The most common measure of typical performance. |
| **StdDev** | **Standard Deviation** (Sample). | Measures the **spread** or **volatility** of the samples. A **low** StdDev indicates stable, reliable measurements. A **high** StdDev suggests the results are polluted by system interference (OS jitter, context switching). |
| **Ops/sec** | Operations Per Second. | The **throughput** of the function, calculated as $1 / \text{Mean Time (in seconds)}$. |

**Key Takeaway:** For a benchmark to be trustworthy, the **Mean Time** and **Min Time** should be close, and the **StdDev** should be small relative to the Mean.

-----

## ⚠️ Important Considerations for Micro-benchmarking

Due to the sensitive nature of micro-benchmarking, results can be heavily influenced by external factors. To get the most accurate, stable data:

1.  **Isolation:** Try to run benchmarks on a dedicated core or system with minimal background processes.
2.  **Stability:** Lock your CPU frequency (disable Turbo Boost) if possible to prevent dynamic frequency scaling from skewing results.
3.  **Prevent Optimization:** Always use `PerfLite::DoNotOptimize` on the return value or any side effects of the code you are measuring to ensure the compiler doesn't eliminate the code entirely.

<!-- end list -->

-----

## 🧪 Build & Tests

This project uses CMake. The test suite integrates GoogleTest. If GoogleTest is not installed on your system, CMake will automatically fetch and build it using FetchContent.

Recommended local workflow:

```bash
# create a build directory and configure
mkdir -p build && cd build
cmake ..

# build everything
make -j

# Run only fast, deterministic unit tests (recommended for CI)
ctest -L fast --output-on-failure

# Run all tests (includes benchmark-style tests)
ctest --output-on-failure

# Or run the test executables directly
./build/test/perf_lite_unit_tests
./build/test/perf_lite_benchmarks
```

Notes:
- The test suite is split into two executables: `perf_lite_unit_tests` (fast, deterministic unit tests) and `perf_lite_benchmarks` (benchmark/integration-style tests that exercise timing behavior).
- Use `ctest -L fast` to execute only the quick unit tests in CI to avoid flaky timing-sensitive runs.
- If you prefer a system-installed GoogleTest, set `CMAKE_PREFIX_PATH` or install `googletest` via your package manager; CMake will prefer an existing package over downloading.

## 📝 Recent meaningful changes (user-facing)

- `to_unit()` now returns double-precision converted values and preserves fractional precision (previous implementation used integer casts which truncated sub-unit values).
- Benchmark durations are recorded as `std::chrono::duration<double, std::nano>` to preserve fractional-nanosecond precision.
- Fixed local variable shadowing in `Benchmark::run` (renamed temporary timing/result variables for clarity and safety).
- Tests: Added deterministic unit tests for `to_unit` and `BenchmarkResult::calculate_statistics`, plus edge-case tests (empty lambda, death tests for zero warmup/iterations, exception propagation, and time-unit ordering).
- CMake: test `CMakeLists.txt` now fetches GoogleTest automatically if it's not available on the system, making it easier to run tests without installing gtest manually.
