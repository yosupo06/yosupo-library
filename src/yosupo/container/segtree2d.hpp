#pragma once

#include <bit>
#include <cassert>
#include <vector>

#include "yosupo/algebra.hpp"
#include "yosupo/container/dynamicsegtree.hpp"
#include "yosupo/coord.hpp"

namespace yosupo {

template <monoid M> struct SegTree2D {
    using S = typename M::S;

  public:
    explicit SegTree2D(Coord _size, const M& _m = M())
        : size(_size), m(_m), manager(DynamicSegtree(ActedMonoid(m))) {
        size2 = (int)std::bit_ceil((unsigned int)(size.r()));
        log = std::countr_zero((unsigned int)size2);
        d = std::vector(2 * size2, manager.build(size.c()));
    }

    void add(Coord p, S x) {
        assert(size.contain(p));

        int r = p.r();
        r += size2;
        for (int i = 0; i <= log; i++) {
            manager.set(d[r >> i], p.c(),
                        m.op(manager.get(d[r >> i], p.c()), x));
        }
    }

    S get(Coord p) {
        assert(size.contain(p));
        return manager.get(d[p.r() + size2], p.c());
    }

    S prod(Coord lw, Coord up) {
        assert(0 <= lw.r() && lw.r() <= up.r() && up.r() <= size.r());
        assert(0 <= lw.c() && lw.c() <= up.c() && up.c() <= size.c());

        S sm = m.e;
        int l = lw.r() + size2;
        int r = up.r() + size2;

        while (l < r) {
            if (l & 1) sm = m.op(sm, manager.prod(d[l++], lw.c(), up.c()));
            if (r & 1) sm = m.op(sm, manager.prod(d[--r], lw.c(), up.c()));
            l >>= 1;
            r >>= 1;
        }
        return sm;
    }

    S all_prod() { return manager.all_prod(d[1]); }

  private:
    Coord size;
    M m;

    int size2, log;
    using DS = DynamicSegtree<ActedMonoid<M, NoOpMonoid, IdentityRight>>;
    DS manager;
    std::vector<typename DS::Tree> d;
};

}  // namespace yosupo
