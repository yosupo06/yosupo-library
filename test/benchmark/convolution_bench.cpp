#include <vector>

#include "benchmark/benchmark.h"
#include "yosupo/convolution.hpp"
#include "yosupo/modint.hpp"

using mint = yosupo::ModInt998244353;

static void GenerateArgs(benchmark::internal::Benchmark* b) {
    for (int i = 0; i <= 10; ++i) {
        for (int j = 0; j <= 10; ++j) {
            b->Args({1LL << i, 1LL << j});
        }
    }
}

static void BM_ConvolutionNaive(benchmark::State& state) {
    long long n = state.range(0);
    long long m = state.range(1);
    std::vector<mint> a(n), b(m);
    for (long long i = 0; i < n; ++i) a[i] = mint(i + 1234);
    for (long long i = 0; i < m; ++i) b[i] = mint(i + 5678);

    for (auto _ : state) {
        benchmark::DoNotOptimize(yosupo::convolution_naive(a, b));
    }
}
BENCHMARK(BM_ConvolutionNaive)->Apply(GenerateArgs);

static void BM_ConvolutionFFT(benchmark::State& state) {
    long long n = state.range(0);
    long long m = state.range(1);
    std::vector<mint> a(n), b(m);
    for (long long i = 0; i < n; ++i) a[i] = mint(i + 1234);
    for (long long i = 0; i < m; ++i) b[i] = mint(i + 5678);

    for (auto _ : state) {
        benchmark::DoNotOptimize(yosupo::convolution_fft(a, b));
    }
}
BENCHMARK(BM_ConvolutionFFT)->Apply(GenerateArgs);

BENCHMARK_MAIN();
