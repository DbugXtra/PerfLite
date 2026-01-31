# Changelog

All notable changes to this project will be documented in this file.

## [Unreleased]

  - `perf_lite_unit_tests` (fast deterministic tests) — labeled `fast` for CI
  - `perf_lite_benchmarks` (benchmark-style timing tests) — labeled `benchmark`
## v0.1.1 - 2026-01-31

- to_unit() now returns double-precision converted values and preserves fractional precision (prevents integer truncation of sub-unit times).
- Durations are recorded as `std::chrono::duration<double, std::nano>` to preserve fractional-nanosecond precision.
- Fixed local variable shadowing in `Benchmark::run` by renaming temporary timing/result variables.
- Added deterministic unit tests for `to_unit` and `BenchmarkResult::calculate_statistics`.
- Added edge-case tests: empty lambda, death tests for zero warmup/iterations, exception propagation, and time-unit ordering.
- `test/CMakeLists.txt` now fetches GoogleTest automatically if not present; tests are split into two targets:
  - `perf_lite_unit_tests` (fast deterministic tests) — labeled `fast` for CI
  - `perf_lite_benchmarks` (benchmark-style timing tests) — labeled `benchmark`
- Added GitHub Actions workflow `.github/workflows/ci-fast-tests.yml` to run only the `fast` labeled tests in CI.
## How to create a release tag

When ready to tag a release locally and push it:

```bash
git add .
git commit -m "chore: prepare release with test and precision improvements"
git tag -a v0.1.0 -m "v0.1.0: precision and test improvements"
git push origin main --follow-tags
```
