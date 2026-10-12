#include <sys/resource.h>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <vector>

#include "yosupo/container/sushi.hpp"

// Usage: benchmark N Q mode batch_size [wm|naive]
// mode: random, increasing, full, noop; batch_size=0 uses the library default.
// Input generation is outside the timer; peak RSS includes input and output.
int main(int argc, char** argv) {
    if (argc < 5) return 2;
    int n = std::atoi(argv[1]), q = std::atoi(argv[2]),
        batch = std::atoi(argv[4]);
    if (n < 1 || q < 0 || batch < 0) return 2;
    std::string mode = argv[3], method = argc > 5 ? argv[5] : "wm";
    if (mode != "random" && mode != "increasing" && mode != "full" &&
        mode != "noop")
        return 2;
    if (method != "wm" && method != "naive") return 2;
    std::mt19937_64 rng(12026);
    std::vector<int> a(n);
    std::iota(a.begin(), a.end(), 0);
    std::shuffle(a.begin(), a.end(), rng);
    if (mode == "random")
        for (int& x : a) x = int(rng() % unsigned(n + q));
    if (mode == "full") std::iota(a.rbegin(), a.rend(), 0);
    std::vector<yosupo::SushiQuery<int>> queries;
    queries.reserve(q);
    for (int j = 0; j < q; ++j) {
        int l = int(rng() % unsigned(n)), r = int(rng() % unsigned(n));
        if (l > r) std::swap(l, r);
        ++r;
        int x = mode == "random" ? int(rng() % unsigned(n + q)) : n + j;
        if (mode == "full") {
            l = 0;
            r = n;
        }
        if (mode == "noop") x = -1;
        queries.push_back({l, r, x});
    }
    auto start = std::chrono::steady_clock::now();
    auto cpu_start = std::clock();
    std::uint64_t checksum = 0;
    auto mix = [&](int x) {
        checksum = checksum * 1000000007 + std::uint64_t(x);
    };
    if (method == "wm") {
        yosupo::SushiBatch<int> sushi(std::move(a));
        auto answers = sushi.apply(queries, batch);
        for (int x : answers) mix(x);
        for (int x : sushi.values()) mix(x);
    } else {
        for (auto [l, r, x] : queries) {
            for (int i = l; i < r; ++i)
                if (a[i] < x) std::swap(a[i], x);
            mix(x);
        }
        for (int x : a) mix(x);
    }
    double cpu = double(std::clock() - cpu_start) / CLOCKS_PER_SEC;
    double wall =
        std::chrono::duration<double>(std::chrono::steady_clock::now() - start)
            .count();
    rusage usage{};
    getrusage(RUSAGE_SELF, &usage);
    std::cout << n << ',' << q << ',' << mode << ',' << batch << ',' << method
              << ',' << wall << ',' << cpu << ',' << usage.ru_maxrss << ','
              << checksum << '\n';
}
