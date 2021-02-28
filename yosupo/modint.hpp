#pragma once

#include "atcoder/modint.hpp"

namespace atcoder {

template <int MOD>
std::ostream& operator<<(std::ostream& os, const static_modint<MOD>& x) {
    return os << x.val();
}

}
