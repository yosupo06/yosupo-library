#include <cassert>
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
    DynamicSegtree(const M& _m)
        : m(internal::IndexedActedMonoid(_m)), manager(m) {}

    struct Tree {
        int len;
        typename SplayTree<internal::IndexedActedMonoid<M>>::Tree tr;

        Tree(int _len,
             typename SplayTree<internal::IndexedActedMonoid<M>>::Tree _tr)
            : len(_len), tr(_tr) {}
    };

    Tree build(int len) { return Tree(len, manager.build()); }

    typename M::S get(Tree& tr, int k) {
        assert(0 <= k && k < tr.len);
        int i = lower_bound_idx(tr, k);
        if (i < manager.ssize(tr.tr) && manager.get(tr.tr, i).first == k) {
            return manager.get(tr.tr, i).second;
        } else {
            return m.monoid.e.second;
        }
    }

    void set(Tree& tr, int k, typename M::S s) {
        assert(0 <= k && k < tr.len);
        int i = lower_bound_idx(tr, k);
        if (i < manager.ssize(tr.tr) && manager.get(tr.tr, i).first == k) {
            manager.set(tr.tr, i, {k, s});
        } else {
            manager.insert(tr.tr, i, {k, s});
        }
    }
    void set_e(Tree& tr, int k) {
        assert(0 <= k && k < tr.len);
        int i = lower_bound_idx(tr, k);
        if (i < manager.ssize(tr.tr) && manager.get(tr.tr, i).first == k) {
            manager.erase(tr.tr, i);
        }
    }

    typename M::S prod(Tree& tr, int l, int r) {
        assert(0 <= l && l <= r && r <= tr.len);
        int li = lower_bound_idx(tr, l);
        int ri = lower_bound_idx(tr, r);
        return manager.prod(tr.tr, li, ri).second;
    }
    typename M::S all_prod(Tree& tr) { return manager.all_prod(tr.tr).second; }

    void apply(Tree& tr, int l, int r, typename M::F f) {
        assert(0 <= l && l <= r && r <= tr.len);
        int li = lower_bound_idx(tr, l);
        int ri = lower_bound_idx(tr, r);
        manager.apply(tr.tr, li, ri, f);
    }
    void all_apply(Tree& tr, typename M::F f) { manager.all_apply(tr.tr, f); }

  private:
    internal::IndexedActedMonoid<M> m;
    SplayTree<internal::IndexedActedMonoid<M>> manager;

    int lower_bound_idx(Tree& tr, int k) {
        return manager.max_right(tr.tr, [&](auto x) { return x.first < k; });
    }
};

}  // namespace yosupo
