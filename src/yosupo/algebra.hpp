#pragma once

#include <algorithm>
#include <concepts>
#include <limits>

namespace yosupo {

template <class T>
concept monoid = requires(T& x, typename T::S s) {
    { x.op(s, s) } -> std::same_as<typename T::S>;
    { x.e() } -> std::same_as<typename T::S>;
};

template <class _S> struct Max {
    using S = _S;
    Max(int e = std::numeric_limits<S>::min()) : _e(e) {}
    S op(S a, S b) { return std::max(a, b); }
    S e() { return _e; }

  private:
    int _e;
};

template <class _S> struct Min {
    using S = _S;
    Min(int e = std::numeric_limits<S>::max()) : _e(e) {}
    S op(S a, S b) { return std::min(a, b); }
    S e() { return _e; }

  private:
    int _e;
};

}  // namespace yosupo
