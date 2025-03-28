#include <cstddef>
#include <utility>

#include "yosupo/algebra.hpp"
#include "yosupo/container/splaytree.hpp"

namespace yosupo {

namespace internal {

template <monoid M> struct IndexedMonoid {
    IndexedMonoid(const M& _m) : e({-1, _m.e}), m(_m) {}

    using S = std::pair<int, typename M::S>;

    S e;
    S op(S a, S b) {
        return {std::max(a.first, b.first), m.op(a.second, b.second)};
    }

  private:
    M m;
};

template <acted_monoid M> struct IndexedActedMonoid {
    IndexedActedMonoid(const M& _m) : monoid(_m.monoid), act(_m.act), m(_m) {}

    IndexedMonoid<decltype(M::monoid)> monoid;
    using S = decltype(monoid)::S;

    decltype(M::act) act;
    using F = typename M::F;

    S mapping(F f, S s) { return {s.first, m.mapping(f, s.second)}; }

  private:
    M m;
};

}  // namespace internal

template <acted_monoid M> struct DynamicSegtree {
    DynamicSegtree(const M& _m) : manager(internal::IndexedActedMonoid(_m)) {}

    struct Tree {
        int size;
        typename SplayTree<internal::IndexedActedMonoid<M>>::Tree tr;
    };

    Tree empty(int size) { return {size, manager.make_empty()}; }

    void set(Tree& tr, int k, typename M::S s) {
        int i = manager.max_right(tr.tr, [&](auto x) { return x.first < k; });
        if (i < int(manager.size(tr.tr)) && manager.get(tr.tr, i).first == k) {
            manager.set(tr.tr, i, {k, s});
        } else {
            manager.insert(tr.tr, i, {k, s});
        }
    }

    typename M::S prod(Tree& tr, int l, int r) {
        int li = manager.max_right(tr.tr, [&](auto x) { return x.first < l; });
        int ri = manager.max_right(tr.tr, [&](auto x) { return x.first < r; });
        return manager.prod(tr.tr, li, ri).second;
    }

    typename M::S all_prod(Tree& tr) { return manager.all_prod(tr.tr).second; }

    void apply(Tree& tr, int l, int r, typename M::F f) {
        int li = manager.max_right(tr.tr, [&](auto x) { return x.first < l; });
        int ri = manager.max_right(tr.tr, [&](auto x) { return x.first < r; });
        manager.apply(tr.tr, li, ri, f);
    }

    void all_apply(Tree& tr, typename M::F f) { manager.all_apply(tr.tr, f); }

  private:
    SplayTree<internal::IndexedActedMonoid<M>> manager;
};

}  // namespace yosupo
