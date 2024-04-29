#pragma once

#include <vector>
#include <utility>
#include <ranges>

namespace yosupo {

template <class T> struct FlattenVector {
    std::vector<T> v;
    std::vector<int> start;
    FlattenVector(int n, const std::vector<std::pair<int, T>>& _v) : start(n + 1) {
        for (const auto& x : _v) {
            start[x.second + 1]++;
        }
        for (int i = 1; i <= n; i++) {
            start[i] += start[i - 1];
        }
        v = std::vector<T>(start[n]);

        auto pos = start;
        for (const auto& x : _v) {
            v[pos[x.second]] = x.first;
            pos[x.second]++;
        }
    }

    auto at(int i) {
        return v | std::ranges::views::take(start[i + 1]) | std::ranges::views::drop(start[i]);
    }
};

}
