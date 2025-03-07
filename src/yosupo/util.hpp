#pragma once

#include <algorithm>
#include <ranges>
#include <vector>

namespace yosupo {

template <std::ranges::input_range R>
std::vector<std::ranges::range_value_t<R>> to_vec(R&& r) {
    return std::vector(std::begin(r), std::end(r));
}

template <class T, class Comp = std::equal_to<>>
void dedup(std::vector<T>& v, Comp comp = Comp{}) {
    auto it = std::ranges::unique(v, comp);
    v.erase(it.begin(), it.end());
}

}  // namespace yosupo
