#pragma once

#include <algorithm>
#include <concepts>
#include <functional>
#include <limits>
#include <utility>

namespace yosupo {

template <class T>
concept monoid = requires(T t, typename T::S s) {
    requires std::same_as<decltype(t.e), typename T::S>;
    { t.op(s, s) } -> std::same_as<typename T::S>;
};

template <class _S, class OP> struct Monoid {
    using S = _S;
    OP op;
    S e;
    explicit Monoid(S _e, OP _op = OP()) : op(_op), e(_e) {}
};

template <monoid Monoid> struct ReversibleMonoid {
    struct S {
        typename Monoid::S val;
        typename Monoid::S rev;
    };
    explicit ReversibleMonoid(const Monoid& _monoid)
        : monoid(_monoid), e({monoid.e, monoid.e}) {}
    S op(S a, S b) {
        return {monoid.op(a.val, b.val), monoid.op(b.rev, a.rev)};
    }

  private:
    Monoid monoid;

  public:
    S e;
};

template <class S> using Sum = Monoid<S, std::plus<S>>;
template <class S> using Prod = Monoid<S, std::multiplies<S>>;

template <class _S> struct Max {
    using S = _S;
    Max(S _e = std::numeric_limits<S>::min()) : e(_e) {}
    S op(S a, S b) { return std::max(a, b); }
    S e;
};
template <class _S> struct Min {
    using S = _S;
    Min(S _e = std::numeric_limits<S>::max()) : e(_e) {}
    S op(S a, S b) { return std::min(a, b); }
    S e;
};

template <class T>
concept static_top_tree_dp = requires(T t) {
    requires monoid<decltype(T::path)>;
    requires monoid<decltype(T::point)>;
    {
        t.add_vertex(std::declval<typename decltype(T::point)::S>(),
                     std::declval<int>())
    } -> std::same_as<typename decltype(T::path)::S>;
    {
        t.add_edge(std::declval<typename decltype(T::path)::S>())
    } -> std::same_as<typename decltype(T::point)::S>;
};

}  // namespace yosupo
