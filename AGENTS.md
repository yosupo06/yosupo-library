# agents.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is Yosupo Library - a competitive programming library containing data structures, algorithms, and mathematical utilities. The project includes both the custom yosupo library (`src/yosupo/`) and AtCoder's AC Library (`ac-library/`).

## Documentation Management

- Library documentation is published on **GitHub Pages**, using the existing `competitive-verifier` → Jekyll → Pages pipeline in `.github/workflows/verify.yml`. Header pages and index entries are generated without requiring a hand-written Markdown file.
- **Keep API specifications in the corresponding `.hpp` file as the single source of truth.** Write concise Japanese Doxygen-style comments beside public classes and functions (`@brief`, `@param`, `@tparam`, `@return`, `@pre`, `@note`, and `@par 計算量`). Use `src/yosupo/networksimplex.hpp` as an example.
- Cover what problem is solved, arguments and return values, preconditions, mutation/reuse restrictions, complexity, and relevant edge cases such as parallel edges and self-loops. Numeric limits must account for intermediate arithmetic; distinguish conservative sufficient bounds from exact limits. Put short usage examples in source comments or point to existing tests.
- **Do not add library API references or usage examples to `README.md`.** Keep the README focused on repository setup and development commands. Do not create parallel per-header `.md` API references or require manual synchronization between comments and Markdown. Add separate explanatory documents only when requested, without duplicating the API contract.
- Keep a suitable class/file-level `@brief` before method comments: the current `parse-doxygen` action uses the **first** `@title` or `@brief` in a file as its page title.
- **Function comments currently appear in the generated page's source-code section, not as separately formatted API explanations.** The current `parse-doxygen` action extracts only the title. API-body generation is tracked in [issue #195](https://github.com/yosupo06/yosupo-library/issues/195); do not duplicate the specifications in Markdown to work around this limitation.
- Do not edit or commit generated `_jekyll/` or `_site/` output.
- Check documentation with the existing generator, using resolved verification metadata and verification results:

  ```sh
  competitive-verifier docs --verify-json verify_files.json --destination .competitive-verifier/_jekyll merged-result.json
  ```

  It discovers inputs via `git ls-files`; use an index that reflects added/deleted files (a temporary index is fine). Confirm that the header page, title, source comments, and index entry are generated. Compile/run changed examples where appropriate. A local generation check is not a deployment; publication happens through the existing workflow on `main`.

## Build and Test Commands

### Verification Policy

- **Use verification-helper only for Library Checker problems** (`https://judge.yosupo.jp/problem/...`), including OJ verification run through `competitive-verifier`. Do not add problems from other judges or custom/local tests using verification-helper directives.
- Keep unit tests, randomized tests, regression tests, and checks without a corresponding Library Checker problem in Google Test under `test/unittest/`. Do not use verification-helper `UNITTEST` directives or custom verification commands for these tests.
- The existing `competitive-verifier` documentation generation pipeline remains in use as described above.

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
