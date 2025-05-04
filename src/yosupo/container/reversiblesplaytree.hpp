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
        return Tree(id | LEAF_BIT, id);
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

    void reverse(Tree& t) {
        if (t.empty()) return;
        reverse(t.id);
    }

  private:
    M m;

    const u32 LEAF_BIT = u32(1) << 31;

    struct Inner {
        u32 lid, rid;
        u32 len;
        bool rev;
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
            n.s = m.mapping(f, n.s, 1);
        } else {
            Inner& n = inner(id);
            n.s = m.mapping(f, n.s, n.len);
            n.f = m.act.op(f, n.f);
        }
    }

    void reverse(u32 id) {
        if (!is_leaf(id)) {
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
        n.len = len(n.lid) + len(n.rid);
        n.s = m.monoid.op(all_prod(n.lid), all_prod(n.rid));
    }

    void splay_k(Tree& t, u32 k) {
        assert(0 < k && k < ssize(t));
        splay(t.id, [&](u32 lid, u32) {
            u32 lsz = len(lid);
            if (k == lsz) return 0;
            if (k < lsz) return -1;
            k -= lsz;
            return 1;
        });
    }

    template <class F> void splay(u32& id, F f) {
        assert(!is_leaf(id));
        static std::vector<u32> lefts, rights;
        lefts.clear();
        rights.clear();

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
