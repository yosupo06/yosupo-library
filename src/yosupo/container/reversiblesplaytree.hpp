#include <sys/types.h>

#include <array>
#include <cassert>
#include <cstddef>
#include <utility>
#include <vector>

#include "yosupo/types.hpp"

namespace yosupo {

template <class M> struct SplayTree {
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
        return Tree(2 * id + 1, 2 * id);
    }
    Tree build(const std::vector<S>& v) {
        auto _build = [&](auto self, u32 l, u32 r) -> Tree {
            if (r - l == 1) {
                return build(v[l]);
            }
            u32 md = (l + r) / 2;
            return merge(self(self, l, md), self(self, md, r));
        };
        return _build(_build, 0, u32(v.size()));
    }

    size_t size(const Tree& t) {
        if (t.empty()) return 0;
        return len(t.id);
    }
    std::ptrdiff_t ssize(const Tree& t) { return ptrdiff_t(size(t)); }

    Tree merge(Tree&& l, Tree&& r) {
        if (l.empty()) return r;
        if (r.empty()) return l;
        inner(l.ex) = Inner{l.id, r.id, 0, false, m.monoid.e, m.act.e};
        update(l.ex);
        return Tree(l.ex, r.ex);
    }
    Tree merge3(Tree&& t1, Tree&& t2, Tree&& t3) {
        return merge(merge(std::move(t1), std::move(t2)), std::move(t3));
    }
    std::pair<Tree, Tree> split(Tree&& t, int k) {
        assert(0 <= k && k <= ssize(t));
        if (k == 0) return {build(), t};
        if (k == ssize(t)) return {t, build()};
        splay_k(t, k);
        u32 id = t.id;
        return {Tree(inner(id).lid, t.ex), Tree(inner(id).rid, id)};
    }
    std::array<Tree, 3> split3(Tree&& t, int l, int r) {
        assert(0 <= l && l <= r && r <= ssize(t));
        auto [_t, t3] = split(std::move(t), r);
        auto [t1, t2] = split(std::move(_t), l);
        return {std::move(t1), std::move(t2), std::move(t3)};
    }

    void insert(Tree& t, int k, S s) {
        assert(0 <= k && k <= ssize(t));
        auto [t1, t2] = split(std::move(t), k);
        t = merge(merge(std::move(t1), build(s)), std::move(t2));
    }

    void erase(Tree& t, int k) {
        assert(0 <= k && k < ssize(t));
        auto [t1, t2, t3] = split3(std::move(t), k, k + 1);
        t = merge(std::move(t1), std::move(t3));
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

    void all_apply(Tree& t, F f) { all_apply(t.id, f); }
    void apply(Tree& t, int l, int r, F f) {
        assert(0 <= l && l <= r && r <= ssize(t));
        auto [t1, t2, t3] = split3(std::move(t), l, r);
        all_apply(t2, f);
        t = merge3(std::move(t1), std::move(t2), std::move(t3));
    }

    void reverse(Tree& t) {
        if (t.empty()) return;
        reverse(t.id);
    }

  private:
    M m;

    struct Inner {
        u32 lid, rid, sz;
        bool rev;
        S s;
        F f;
    };
    struct Leaf {
        S s;
    };
    std::vector<std::pair<Inner, Leaf>> nodes;

    bool leaf_id(u32 id) { return id % 2; }
    Inner& inner(u32 id) { return nodes[id >> 1].first; }
    Leaf& leaf(u32 id) { return nodes[id >> 1].second; }

    u32 len(u32 id) { return leaf_id(id) ? 1 : inner(id).sz; }
    S all_prod(u32 id) { return leaf_id(id) ? leaf(id).s : inner(id).s; }

    void all_apply(u32 id, const F& f) {
        if (leaf_id(id)) {
            Leaf& n = leaf(id);
            n.s = m.mapping(f, n.s, 1);
        } else {
            Inner& n = inner(id);
            n.s = m.mapping(f, n.s, n.sz);
            n.f = m.act.op(f, n.f);
        }
    }

    void reverse(u32 id) {
        if (!leaf_id(id)) {
            Inner& n = inner(id);
            std::swap(n.lid, n.rid);
            n.rev = !n.rev;
        }
    }

    void push(u32 id) {
        Inner& n = inner(id);
        if (n.rev) {
            reverse(n.lid);
            reverse(n.rid);
            n.rev = false;
        }

        if (n.f != m.act.e) {
            all_apply(n.lid, n.f);
            all_apply(n.rid, n.f);
            n.f = m.act.e;
        }
    }

    void update(u32 id) {
        Inner& n = inner(id);
        n.sz = len(n.lid) + len(n.rid);
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

    template <class F> void splay(Tree& t, F f) {
        assert(!leaf_id(t.id));
        static std::vector<u32> lefts, rights;
        lefts.clear();
        rights.clear();

        int zig = 0;
        u32 id = t.id;
        while (true) {
            push(id);
            u32 lid = inner(id).lid, rid = inner(id).rid;

            auto dir = f(lid, rid);

            if (dir == -1 && !leaf_id(lid)) {
                if (zig == -1) {
                    inner(rights.back()).lid = rid;
                    update(rights.back());
                    inner(id).rid = rights.back();
                    rights.pop_back();
                }
                rights.push_back(id);

                id = lid;
                if (zig == 0) {
                    zig = -1;
                } else {
                    zig = 0;
                }
            } else if (dir == 1 && !leaf_id(rid)) {
                if (zig == 1) {
                    inner(lefts.back()).rid = lid;
                    update(lefts.back());
                    inner(id).lid = lefts.back();
                    lefts.pop_back();
                }
                lefts.push_back(id);

                id = rid;
                if (zig == 0) {
                    zig = 1;
                } else {
                    zig = 0;
                }
            } else {
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
                break;
            }
        }
        t.id = id;
    }
};

}  // namespace yosupo
