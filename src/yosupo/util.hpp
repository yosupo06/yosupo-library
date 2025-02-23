#pragma once

#include <ranges>
#include <vector>

namespace yosupo {

template <std::ranges::input_range R>
std::vector<std::ranges::range_value_t<R>> to_vec(R&& r) {
    return std::vector(std::begin(r), std::end(r));
}

}  // namespace yosupo
