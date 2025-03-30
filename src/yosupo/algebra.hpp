#pragma once

#include <algorithm>
#include <concepts>
#include <functional>
#include <limits>

namespace yosupo {

template <class T>
concept monoid = requires(T t, const typename T::S& s) {
    requires std::same_as<decltype(t.e), typename T::S>;
    { t.op(s, s) } -> std::same_as<typename T::S>;
};

template <class _S, class OP>
    requires requires(OP op, const _S& a, const _S& b) {
        { op(a, b) } -> std::same_as<_S>;
    }
struct Monoid {
    using S = _S;
    S e;
    OP op;
    explicit Monoid(S _e, const OP& _op = OP()) : e(_e), op(_op) {}
};

struct NoOpMonoid {
    struct S {};
    S e;
    S op(const S&, const S&) { return S{}; }
};

template <monoid Monoid> struct ReversibleMonoid {
    struct S {
        typename Monoid::S val;
        typename Monoid::S rev;
        S(typename Monoid::S v) : val(v), rev(v) {}
        S(typename Monoid::S _val, typename Monoid::S _rev)
            : val(std::move(_val)), rev(std::move(_rev)) {}
    };
    explicit ReversibleMonoid(const Monoid& _monoid)
        : monoid(_monoid), e(monoid.e) {}
    S op(const S& a, const S& b) {
        return {monoid.op(a.val, b.val), monoid.op(b.rev, a.rev)};
    }

  private:
    Monoid monoid;

  public:
    S e;
};

template <class T>
concept acted_monoid =
    requires(T t, const typename T::S& s, const typename T::F& f) {
        requires monoid<decltype(T::monoid)>;
        requires monoid<decltype(T::act)>;
        requires std::same_as<typename T::S, typename decltype(T::monoid)::S>;
        requires std::same_as<typename T::F, typename decltype(T::act)::S>;
        { t.mapping(f, s) } -> std::same_as<typename T::S>;
    };

struct IdentityRight {
    template <class S, class T> T operator()(const S&, T a) {
        return std::move(a);
    }
};

template <monoid Monoid, monoid Act, class Mapping>
    requires requires(Mapping mapping,
                      const typename Act::S& a,
                      const typename Monoid::S& b) {
        { mapping(a, b) } -> std::same_as<typename Monoid::S>;
    }
struct ActedMonoid {
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
template <class Monoid>
ActedMonoid(const Monoid& _monoid)
    -> ActedMonoid<Monoid, NoOpMonoid, IdentityRight>;

template <class T>
concept static_top_tree_dp = requires(T t,
                                      const typename T::Path& path,
                                      const typename T::Point& point,
                                      const typename T::Vertex& v) {
    requires monoid<decltype(T::path)>;
    requires monoid<decltype(T::point)>;
    requires std::same_as<typename T::Path, typename decltype(T::path)::S>;
    requires std::same_as<typename T::Point, typename decltype(T::point)::S>;
    { t.add_vertex(point, v) } -> std::same_as<typename T::Path>;
    { t.add_edge(path) } -> std::same_as<typename T::Point>;
};

// usuful monoids
template <class S> using Sum = Monoid<S, std::plus<S>>;
template <class S> using Prod = Monoid<S, std::multiplies<S>>;
template <class _S> struct Max {
    using S = _S;
    Max(S _e = std::numeric_limits<S>::min()) : e(_e) {}
    S op(const S& a, const S& b) { return std::max(a, b); }
    S e;
};
template <class _S> struct Min {
    using S = _S;
    Min(S _e = std::numeric_limits<S>::max()) : e(_e) {}
    S op(const S& a, const S& b) { return std::min(a, b); }
    S e;
};

}  // namespace yosupo
