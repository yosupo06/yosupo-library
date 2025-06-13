# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is Yosupo Library - a competitive programming library containing data structures, algorithms, and mathematical utilities. The project includes both the custom yosupo library (`src/yosupo/`) and AtCoder's AC Library (`ac-library/`).

## Build and Test Commands

### Running Unit Tests
```bash
# Run all unit tests
./test/run_unittest.sh

# Manual build and test (from project root)
mkdir -p .work/build
cmake -S test -B .work/build -DCMAKE_BUILD_TYPE=Debug
cmake --build .work/build
.work/build/unittest
```

### Running Online Judge Tests
```bash
# Run specific OJ test file
./test/run_oj.sh test/oj/filename.test.cpp

# Manual verification (from project root)
competitive-verifier oj-resolve --no-bundle --config .verify-helper/config.toml --include test/oj/filename.test.cpp > verify_files.json
competitive-verifier verify --verify-json verify_files.json
```

### Build Configuration
- C++20 standard required
- Compiler flags defined in `compile_flags.txt`
- Debug builds include sanitizers (undefined behavior and address sanitizer)
- Uses Google Test for unit tests and Google Benchmark for performance tests

## Code Architecture

### Library Structure
- **Main Library**: `src/yosupo/` - Custom implementations organized by category:
  - `container/` - Data structures (segment trees, splay trees, hash containers)
  - `geo/` - Computational geometry (points, lines, polygons, convex hull)
  - `math/` - Mathematical utilities (number theory, factorization, primitives)
  - Core modules: algebra, convolution, graph algorithms, modular arithmetic

- **AC Library**: `ac-library/atcoder/` - AtCoder's official competitive programming library
  - Includes standard CP data structures and algorithms
  - Header-only implementation with specific competitive programming optimizations

### Testing Structure
- **Unit Tests**: `test/unittest/` - Mirrors the `src/yosupo/` structure
- **OJ Tests**: `test/oj/` - Online judge verification tests using competitive-verifier
- **Benchmarks**: `test/benchmark/` - Performance testing using Google Benchmark

### Include Paths
Code should include headers as:
- `#include <yosupo/module.hpp>` for main library
- `#include <atcoder/module.hpp>` for AC Library
- Both `src/` and `ac-library/` are in the include path

### Compiler Configuration
Uses competitive programming conventions:
- `int` instead of `size_t` for convenience
- Aggressive optimization and warning flags
- C++20 features utilized throughout