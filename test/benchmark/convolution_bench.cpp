#include <vector>

#include "benchmark/benchmark.h"
#include "yosupo/convolution.hpp"
#include "yosupo/modint.hpp"

using mint = yosupo::ModInt998244353;

void CONV_same_length(benchmark::State& state) {
    std::vector<mint> a(state.range(0)), b(state.range(0));
    for (int i = 0; i < state.range(0); i++) {
        a[i] = i + 1234;
        b[i] = i + 5678;
    }
    for (auto _ : state) {
        benchmark::DoNotOptimize(convolution(a, b));
    }
}
BENCHMARK(CONV_same_length)->RangeMultiplier(2)->Range(1, 1 << 20);
BENCHMARK(CONV_same_length)->DenseRange(1, 100, 1);

BENCHMARK_MAIN();
