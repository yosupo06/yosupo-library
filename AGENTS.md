# agents.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is Yosupo Library - a competitive programming library containing data structures, algorithms, and mathematical utilities. The project includes both the custom yosupo library (`src/yosupo/`) and AtCoder's AC Library (`ac-library/`).

## Documentation Management

- Library documentation is published on **GitHub Pages**, using the existing `competitive-verifier` → Jekyll → Pages pipeline in `.github/workflows/verify.yml`.
- **Do not add library API references or usage examples to `README.md`.** Keep the README focused on repository setup and development commands. Put library documentation next to the corresponding header, not in a new standalone documentation tree or in the vendored `ac-library/` directory.
- For `src/yosupo/foo.hpp`, create or edit `src/yosupo/foo.md` with front matter like:

  ```yaml
  ---
  title: Foo
  documentation_of: ./foo.hpp
  ---
  ```

  This attaches the Markdown body to the generated header page and its index entry. Use `src/yosupo/networksimplex.md` as an example. Do not edit or commit generated `_jekyll/` or `_site/` output.
- Write user-facing library documentation in Japanese, with an AC Library-style structure: what problem is solved, API signatures and semantics, constraints, return values, complexity, and a working example. State relevant edge cases (such as parallel edges and self-loops), mutation/reuse restrictions, and numeric limits that account for intermediate arithmetic. Distinguish conservative sufficient bounds from exact limits; keep long derivations and explanations in Markdown.
- Add concise Doxygen-style comments beside public classes and functions for editor/source readers (`@brief`, `@param`, `@tparam`, `@return`, `@pre`, `@note`, and `@par 計算量`). Keep a suitable class/file-level `@brief` before method comments: the current `parse-doxygen` action uses the **first** `@title` or `@brief` in a file as its page title.
- **Function comments are not currently rendered as API explanations in the page body.** The current `parse-doxygen` action extracts only the title. Until [issue #195](https://github.com/yosupo06/yosupo-library/issues/195) adds API-body generation, retain complete public documentation in the adjacent Markdown and update its contract together with the source comments. Do not move API explanations solely into comments and assume they will appear on Pages.
- Use Jekyll-aware links to other generated pages, e.g. `{% link test/oj/min_cost_b_flow.test.cpp.md %}`, or explicit GitHub links for source files. A repository-relative `.hpp` link does not necessarily resolve to the generated page. Use explicit anchors for in-page links when needed.
- Check documentation with the existing generator, using resolved verification metadata and verification results:

  ```sh
  competitive-verifier docs --verify-json verify_files.json --destination .competitive-verifier/_jekyll merged-result.json
  ```

  It discovers inputs via `git ls-files`; ensure new Markdown files are included in the index used for validation (a temporary index is fine). Confirm that the text is attached to the intended header page and that the index and example links exist. Compile/run changed examples where appropriate. A local generation check is not a deployment; publication happens through the existing workflow on `main`.

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
