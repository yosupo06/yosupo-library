"""Generate an isolated profiling copy; never instrument the library in place.

python3 experiments/sushi_rebuild/profile.py --output-dir /tmp/sushi-profile
g++ -std=c++20 -O3 -DNDEBUG -I/tmp/sushi-profile/include \
    /tmp/sushi-profile/benchmark.cpp -o /tmp/sushi-profile/benchmark

Whole-epoch phases use timers once per epoch. Query-level nested timers sample
one query in 64. Nested categories are inclusive and must not be summed.
"""

import argparse
import json
from pathlib import Path
import subprocess


def measure(binary_dir, output, repeats):
    """Run serially; compare all answers/final-array checksums across variants."""
    checksums = {}
    with output.open("w") as stream:
        for n, q in [(200000, 200000), (400000, 25000)]:
            for mode in ["random", "increasing", "full"]:
                for repeat in range(repeats):
                    variants = ["base", "profile", "popcnt"]
                    if repeat % 2:
                        variants.reverse()
                    for variant in variants:
                        name = "benchmark" if variant == "profile" else "benchmark_" + variant
                        result = subprocess.run(
                            [str(binary_dir / name), str(n), str(q), mode, "0"],
                            check=True, text=True, capture_output=True,
                        )
                        row = result.stdout.strip().split(",")
                        key = (n, q, mode)
                        checksum = int(row[8])
                        if key in checksums and checksums[key] != checksum:
                            raise RuntimeError(f"Checksum mismatch: {key}, {variant}")
                        checksums[key] = checksum
                        record = dict(n=n, q=q, mode=mode, repeat=repeat,
                                      variant=variant, batch=0, wall_s=float(row[5]),
                                      cpu_s=float(row[6]), rss_kib=int(row[7]),
                                      checksum=checksum)
                        if variant == "profile":
                            record["timers"] = json.loads(result.stderr)
                        stream.write(json.dumps(record) + "\n")
                        stream.flush()
                        print(n, q, mode, repeat, variant, row[6], flush=True)


SUPPORT = r'''#pragma once
#include <array>
#include <chrono>
#include <cstdint>
#include <iostream>

namespace sushi_profile {
enum Category { Build, Compress, WM, QueryLoop, Materialize, QuerySample,
                Range, Update, Boundary, Count };
inline std::array<std::uint64_t, Count> nanos{}, calls{};
inline std::uint64_t query_count = 0;
inline bool active = false;
inline std::uint64_t now() {
    return std::uint64_t(std::chrono::steady_clock::now().time_since_epoch().count());
}
struct Scope {
    Category category;
    bool enabled;
    std::uint64_t start;
    explicit Scope(Category c, bool on = true)
        : category(c), enabled(on), start(on ? now() : 0) {}
    ~Scope() {
        if (enabled) { nanos[category] += now() - start; ++calls[category]; }
    }
};
struct QueryScope {
    Scope timer;
    QueryScope() : timer(QuerySample, (query_count++ & 63) == 0) {
        active = timer.enabled;
    }
    ~QueryScope() { active = false; }
};
inline void report() {
    static_assert(std::chrono::steady_clock::period::num == 1 &&
                  std::chrono::steady_clock::period::den == 1000000000);
    const char* names[] = {"build", "compression", "wm_build", "query_loop",
        "materialize", "query_sample", "range", "update", "boundary"};
    std::cerr << "{\"query_count\":" << query_count;
    for (int i = 0; i < Count; ++i)
        std::cerr << ",\"" << names[i] << "_ns\":" << nanos[i]
                  << ",\"" << names[i] << "_calls\":" << calls[i];
    std::cerr << "}\n";
}
}
'''


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--output-dir", type=Path, required=True)
    parser.add_argument("--measure", type=Path,
                        help="Measure existing binaries and save JSONL instead of generating sources")
    parser.add_argument("--repeats", type=int, default=3)
    args = parser.parse_args()
    if args.measure:
        if args.repeats < 1:
            parser.error("--repeats must be positive")
        measure(args.output_dir.resolve(), args.measure, args.repeats)
        return
    repo = Path(__file__).resolve().parents[2]
    header = (repo / "src/yosupo/container/sushi.hpp").read_text()

    def replace(old, new):
        nonlocal header
        if header.count(old) != 1:
            raise RuntimeError(f"Expected exactly one instrumentation site: {old}")
        header = header.replace(old, new)

    replace("#pragma once", '#pragma once\n#include "profile_support.hpp"')
    replace("SushiWavelet(std::vector<int> a, int height) {", """SushiWavelet(std::vector<int> a, int height) {
        sushi_profile::Scope timer(sushi_profile::WM);""")
    replace("int count = int(a.size() + qs.size());", """sushi_profile::Scope timer(sushi_profile::Compress);
        int count = int(a.size() + qs.size());""")
    replace("int maximum_l(int v, int mid, int end) const {", """int maximum_l(int v, int mid, int end) const {
        sushi_profile::Scope timer(sushi_profile::Boundary, sushi_profile::active);""")
    replace("void edit_history(int key, int p, int delta) {", """void edit_history(int key, int p, int delta) {
        sushi_profile::Scope timer(sushi_profile::Update, sushi_profile::active);""")
    replace("int range_minimum(int l, int r) const {", """int range_minimum(int l, int r) const {
        sushi_profile::Scope timer(sushi_profile::Range, sushi_profile::active);""")
    replace("T query(int index, const SushiQuery<T>& q) {", """T query(int index, const SushiQuery<T>& q) {
        sushi_profile::QueryScope timer;""")
    replace("void materialize(std::vector<T>& a) const {", """void materialize(std::vector<T>& a) const {
        sushi_profile::Scope timer(sushi_profile::Materialize);""")
    replace("internal::SushiEpoch<T> epoch(a, batch);", """auto build_start = sushi_profile::now();
            internal::SushiEpoch<T> epoch(a, batch);
            sushi_profile::nanos[sushi_profile::Build] += sushi_profile::now() - build_start;
            ++sushi_profile::calls[sushi_profile::Build];
            { sushi_profile::Scope query_timer(sushi_profile::QueryLoop);""")
    replace("epoch.materialize(a);", "}\n            epoch.materialize(a);")

    output = args.output_dir
    include = output / "include"
    (include / "yosupo/container").mkdir(parents=True, exist_ok=True)
    (include / "yosupo/container/sushi.hpp").write_text(header)
    (include / "profile_support.hpp").write_text(SUPPORT)
    benchmark = (repo / "experiments/sushi_rebuild/benchmark.cpp").read_text()
    marker = "    rusage usage{};"
    if benchmark.count(marker) != 1:
        raise RuntimeError("Could not find benchmark reporting site")
    benchmark = benchmark.replace(marker, "    sushi_profile::report();\n" + marker)
    (output / "benchmark.cpp").write_text(benchmark)


if __name__ == "__main__":
    main()
