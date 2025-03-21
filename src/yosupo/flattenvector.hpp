#pragma once

#include <ranges>
#include <utility>
#include <vector>

namespace yosupo {

template <class T> struct FlattenVector {
    std::vector<T> v;
    std::vector<int> start;
    FlattenVector(int n, const std::vector<std::pair<int, T>>& _v)
        : start(n + 1) {
        for (const auto& x : _v) {
            start[x.first + 1]++;
        }
        for (int i = 1; i <= n; i++) {
            start[i] += start[i - 1];
        }
        v = std::vector<T>(start[n]);

        auto pos = start;
        for (const auto& x : _v) {
            v[pos[x.first]] = x.second;
            pos[x.first]++;
        }
    }

    auto at(int i) {
        return v | std::ranges::views::take(start[i + 1]) |
               std::ranges::views::drop(start[i]);
    }
    auto at(int i) const {
        return v | std::ranges::views::take(start[i + 1]) |
               std::ranges::views::drop(start[i]);
    }

    template <class Pred> int erase_if(Pred pred) {
        int n = int(start.size()) - 1;

        int removed = 0;
        int r = start[0];
        for (int i = 0; i < n; i++) {
            int l = r;
            r = start[i + 1];
            start[i + 1] = start[i];
            for (int j = l; j < r; j++) {
                std::pair<int, T> p = {i, v[j]};
                if (pred(p)) {
                    removed++;
                } else {
                    v[start[i + 1]] = v[j];
                    start[i + 1]++;
                }
            }
        }
        v.resize(start[n]);
        return removed;
    }
};

}  // namespace yosupo
