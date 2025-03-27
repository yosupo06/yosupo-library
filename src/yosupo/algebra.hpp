#pragma once

#include <algorithm>
#include <concepts>
#include <functional>
#include <limits>

namespace yosupo {

template <class T>
concept monoid = requires(T t, typename T::S s) {
    requires std::same_as<decltype(t.e), typename T::S>;
    { t.op(s, s) } -> std::same_as<typename T::S>;
};

template <class _S, class OP> struct Monoid {
    using S = _S;
    S e;
    OP op;
    explicit Monoid(S _e, OP _op = OP()) : e(_e), op(_op) {}
};

template <monoid Monoid> struct ReversibleMonoid {
    struct S {
        typename Monoid::S val;
        typename Monoid::S rev;
        S(typename Monoid::S v) : val(v), rev(v) {}
        S(typename Monoid::S _val, typename Monoid::S _rev)
            : val(_val), rev(_rev) {}
    };
    explicit ReversibleMonoid(const Monoid& _monoid)
        : monoid(_monoid), e(monoid.e) {}
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
concept acted_monoid = requires(T t, typename T::S s, typename T::F f) {
    requires monoid<decltype(T::monoid)>;
    requires monoid<decltype(T::act)>;
    requires std::same_as<typename T::S, typename decltype(T::monoid)::S>;
    requires std::same_as<typename T::F, typename decltype(T::act)::S>;
    { t.mapping(f, s) } -> std::same_as<typename T::S>;
};
template <monoid Monoid, monoid Act, class Mapping> struct ActedMonoid {
    using S = Monoid::S;
    using F = Act::S;

    explicit ActedMonoid(const Monoid& _monoid = Monoid(),
                         const Act& _act = Act(),
                         const Mapping& _mapping = Mapping())
        : monoid(_monoid), act(_act), mapping(_mapping) {}

    Monoid monoid;
    Act act;
    Mapping mapping;
};

template <class T>
concept static_top_tree_dp = requires(T t,
                                      typename T::Path path,
                                      typename T::Point point,
                                      typename T::Vertex v) {
    requires monoid<decltype(T::path)>;
    requires monoid<decltype(T::point)>;
    requires std::same_as<typename T::Path, typename decltype(T::path)::S>;
    requires std::same_as<typename T::Point, typename decltype(T::point)::S>;
    { t.add_vertex(point, v) } -> std::same_as<typename T::Path>;
    { t.add_edge(path) } -> std::same_as<typename T::Point>;
};

}  // namespace yosupo
