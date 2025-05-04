#include <sys/types.h>

#include <array>
#include <cassert>
#include <cstddef>
#include <utility>
#include <vector>

#include "yosupo/algebra.hpp"
#include "yosupo/types.hpp"

namespace yosupo {

template <acted_monoid M> struct SplayTree {
    using S = typename M::S;
    using F = typename M::F;

    SplayTree(const M& _m) : m(_m) {}

    struct Tree {
        u32 id, ex;
        Tree() : id(-1), ex(-1) {}
        Tree(u32 _id, u32 _ex) : id(_id), ex(_ex) {}
        bool empty() const { return id == u32(-1); }
    };

    Tree build() { return Tree(); }
    Tree build(S s) {
        u32 id = u32(nodes.size());
        nodes.push_back({
            Inner{},
            Leaf{s},
        });
        return Tree(id | LEAF_BIT, id);
    }
    Tree build(const std::vector<S>& v) {
        auto rec = [&](auto self, u32 l, u32 r) -> Tree {
            if (r - l == 1) {
                return build(v[l]);
            }
            u32 md = (l + r) / 2;
            return merge(self(self, l, md), self(self, md, r));
        };
        nodes.reserve(nodes.size() + v.size());
        return rec(rec, 0, u32(v.size()));
    }

    size_t size(const Tree& t) {
        if (t.empty()) return 0;
        return len(t.id);
    }
    std::ptrdiff_t ssize(const Tree& t) { return ptrdiff_t(size(t)); }

    Tree merge(Tree&& l, Tree&& r) {
        if (l.empty()) return r;
        if (r.empty()) return l;
        inner(l.ex) = Inner{l.id, r.id, 0, m.monoid.e, m.act.e};
        update(l.ex);
        return Tree(l.ex, r.ex);
    }
    Tree merge3(Tree&& t1, Tree&& t2, Tree&& t3) {
        return merge(merge(std::move(t1), std::move(t2)), std::move(t3));
    }

    Tree split(Tree& t, int k) {
        if (k == 0) return std::exchange(t, build());
        if (k == ssize(t)) return build();
        splay_k(t, k);
        int id = t.id;
        t.id = inner(id).lid;
        return Tree(inner(id).rid, id);
    }
    std::array<Tree, 3> split3(Tree&& t, int l, int r) {
        assert(0 <= l && l <= r && r <= ssize(t));
        auto t3 = split(t, r);
        auto t2 = split(t, l);
        return {std::move(t), std::move(t2), std::move(t3)};
    }

    void insert(Tree& t, int k, S s) {
        assert(0 <= k && k <= ssize(t));
        auto t2 = split(t, k);
        t = merge(std::move(t), build(s));
        t = merge(std::move(t), std::move(t2));
    }
    void erase(Tree& t, int k) {
        assert(0 <= k && k < ssize(t));
        auto [t1, t2, t3] = split3(std::move(t), k, k + 1);
        t = merge(std::move(t1), std::move(t3));
    }

    S get(Tree& t, int k) {
        assert(0 <= k && k < ssize(t));
        S s;
        access_leaf_k(t, k, [&](int id) { s = leaf(id).s; });
        return s;
    }
    void set(Tree& t, int k, S s) {
        assert(0 <= k && k < ssize(t));
        access_leaf_k(t, k, [&](int id) { leaf(id).s = s; });
    }

    S all_prod(const Tree& t) {
        if (t.empty()) return m.monoid.e;
        return all_prod(t.id);
    }
    S prod(Tree& t, int l, int r) {
        assert(0 <= l && l <= r && r <= ssize(t));
        auto [t1, t2, t3] = split3(std::move(t), l, r);
        S s = all_prod(t2);
        t = merge3(std::move(t1), std::move(t2), std::move(t3));
        return s;
    }

    void all_apply(Tree& t, F f) {
        if (t.empty()) return;
        all_apply(t.id, f);
    }
    void apply(Tree& t, int l, int r, F f) {
        assert(0 <= l && l <= r && r <= ssize(t));
        auto [t1, t2, t3] = split3(std::move(t), l, r);
        all_apply(t2, f);
        t = merge3(std::move(t1), std::move(t2), std::move(t3));
    }

    template <class F> int max_right(Tree& t, F f) {
        if (f(all_prod(t))) return int(ssize(t));
        if (ssize(t) == 1) return 0;
        S s = m.monoid.e;
        u32 r = 0;
        splay(t, [&](u32 lid, u32 rid) {
            S s2 = m.monoid.op(s, all_prod(lid));

            if (!f(s2)) return is_leaf(lid) ? 0 : -1;

            r += len(lid);
            s = s2;
            return is_leaf(rid) ? 0 : 1;
        });
        return r;
    }

    std::vector<S> to_vec(const Tree& t) {
        if (t.empty()) return {};
        std::vector<S> buf;
        buf.reserve(len(t.id));
        auto f = [&](auto self, int id) {
            if (is_leaf(id)) {
                buf.push_back(leaf(id).s);
                return;
            }
            push(id);
            self(self, inner(id).lid);
            self(self, inner(id).rid);
        };
        f(f, t.id);
        return buf;
    }

  private:
    M m;

    const u32 LEAF_BIT = u32(1) << 31;

    struct Inner {
        u32 lid, rid;
        u32 len;
        S s;
        F f;
    };
    struct Leaf {
        S s;
    };
    std::vector<std::pair<Inner, Leaf>> nodes;

    bool is_leaf(u32 id) { return id & LEAF_BIT; }
    Inner& inner(u32 id) { return nodes[id].first; }
    Leaf& leaf(u32 id) { return nodes[id ^ LEAF_BIT].second; }

    u32 len(u32 id) { return is_leaf(id) ? 1 : inner(id).len; }
    S all_prod(u32 id) { return is_leaf(id) ? leaf(id).s : inner(id).s; }

    void all_apply(u32 id, const F& f) {
        if (is_leaf(id)) {
            Leaf& n = leaf(id);
            n.s = m.mapping(f, n.s);
        } else {
            Inner& n = inner(id);
            n.s = m.mapping(f, n.s);
            n.f = m.act.op(f, n.f);
        }
    }

    void push(u32 id) {
        Inner& n = inner(id);
        all_apply(n.lid, n.f);
        all_apply(n.rid, n.f);
        n.f = m.act.e;
    }

    void update(u32 id) {
        Inner& n = inner(id);
        n.len = len(n.lid) + len(n.rid);
        n.s = m.monoid.op(all_prod(n.lid), all_prod(n.rid));
    }

    void splay_k(Tree& t, u32 k) {
        assert(0 < k && k < ssize(t));
        splay(t, [&](u32 lid, u32) {
            u32 lsz = len(lid);
            if (k == lsz) return 0;
            if (k < lsz) return -1;
            k -= lsz;
            return 1;
        });
    }

    template <class F> void access_leaf_k(Tree& t, int k, F f) {
        assert(0 <= k && k < ssize(t));
        access_leaf(
            t,
            [&](u32 lid, u32) {
                int lsz = len(lid);
                if (k < lsz) return -1;
                k -= lsz;
                return 1;
            },
            f);
    }
    template <class F, class G> void access_leaf(Tree& t, F f, G g) {
        assert(ssize(t));
        if (ssize(t) == 1) {
            g(t.id);
            return;
        }
        splay(t, [&](u32 lid, u32 rid) {
            int dir = f(lid, rid);
            if (dir == -1 && is_leaf(lid)) {
                g(lid);
                return 0;
            }
            if (dir == 1 && is_leaf(rid)) {
                g(rid);
                return 0;
            }
            return dir;
        });
    }

    template <class F> void splay(Tree& t, F f) {
        assert(!is_leaf(t.id));
        static std::vector<u32> lefts, rights;
        lefts.clear();
        rights.clear();

        u32& id = t.id;
        int zig = 0;
        while (true) {
            push(id);
            u32 lid = inner(id).lid, rid = inner(id).rid;

            auto dir = f(lid, rid);

            if (dir == 0) {
                {
                    u32 tmp = lid;
                    for (auto i = std::ssize(lefts) - 1; i >= 0; i--) {
                        inner(lefts[i]).rid = std::exchange(tmp, lefts[i]);
                        update(lefts[i]);
                    }
                    inner(id).lid = tmp;
                }
                {
                    u32 tmp = rid;
                    for (auto i = std::ssize(rights) - 1; i >= 0; i--) {
                        inner(rights[i]).lid = std::exchange(tmp, rights[i]);
                        update(rights[i]);
                    }
                    inner(id).rid = tmp;
                }
                update(id);
                return;
            }

            if (dir < 0) {
                if (zig == -1) {
                    u32 tmp = rights.back();
                    rights.pop_back();
                    inner(tmp).lid = rid;
                    update(tmp);
                    inner(id).rid = tmp;
                }
                rights.push_back(id);

                id = lid;
                if (zig == 0) {
                    zig = -1;
                } else {
                    zig = 0;
                }
            } else {
                if (zig == 1) {
                    u32 tmp = lefts.back();
                    lefts.pop_back();
                    inner(tmp).rid = lid;
                    update(tmp);
                    inner(id).lid = tmp;
                }
                lefts.push_back(id);

                id = rid;
                if (zig == 0) {
                    zig = 1;
                } else {
                    zig = 0;
                }
            }
        }
    }
};

}  // namespace yosupo
