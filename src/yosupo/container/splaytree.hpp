#include <sys/types.h>

#include <array>
#include <cassert>
#include <cstddef>
#include <utility>
#include <vector>

#include "yosupo/algebra.hpp"

namespace yosupo {

template <acted_monoid M> struct SplayTree {
    using S = typename M::S;
    using F = typename M::F;

    SplayTree(const M& _m) : m(_m) {
        // last node
        nodes.push_back(Node{
            .l = -1,
            .r = -1,
            .len = 0,
            .s = m.monoid.e,
            .prod = m.monoid.e,
            .f = m.act.e,
        });
    }

    struct Tree {
        int id;
        Tree() : id(0) {}
        Tree(int _id) : id(_id) {}
        bool empty() const { return id == 0; }
    };

    size_t size(const Tree& t) { return nodes[t.id].len; }
    ptrdiff_t ssize(const Tree& t) { return size(t); }

    Tree build() { return Tree(); }
    Tree build(const S& s) { return build(std::vector<S>{s}); }
    Tree build(const std::vector<S>& v) {
        auto f = [&](auto self, int lidx, int ridx) -> int {
            if (lidx == ridx) return 0;
            assert(lidx <= ridx);
            if (ridx - lidx == 1) {
                int id = new_node(v[lidx], 0, 0);
                return id;
            }
            int mid = (lidx + ridx) / 2;
            int id = new_node(v[mid], self(self, lidx, mid),
                              self(self, mid + 1, ridx));
            return id;
        };
        return Tree(f(f, 0, int(v.size())));
    }

    S get(Tree& t, int k) {
        assert(0 <= k && k < int(size(t)));
        t.id = splay_k(t.id, k);
        return nodes[t.id].s;
    }
    void set(Tree& t, int k, S s) {
        assert(0 <= k && k < int(size(t)));
        t.id = splay_k(t.id, k);
        nodes[t.id].s = s;
        update(t.id);
    }

    void insert(Tree& t, int k, S s) {
        assert(0 <= k && k <= int(size(t)));
        auto t2 = split(t, k);
        auto t1 = build(s);
        t = merge3(std::move(t), std::move(t1), std::move(t2));
    }
    void erase(Tree& t, int k) {
        assert(0 <= k && k < ssize(t));
        auto t2 = split(t, k);
        auto t3 = split(t2, 1);
        t = merge(std::move(t), std::move(t3));
    }

    Tree merge(Tree&& l, Tree&& r) {
        if (l.empty()) return r;
        if (r.empty()) return l;
        int rid = splay_k(r.id, 0);
        nodes[rid].l = l.id;
        update(rid);
        return Tree(rid);
    }
    Tree merge3(Tree&& t1, Tree&& t2, Tree&& t3) {
        return merge(merge(std::move(t1), std::move(t2)), std::move(t3));
    }
    Tree split(Tree& t, int k) {
        assert(0 <= k && k <= int(size(t)));
        if (k == int(size(t))) return Tree();
        int rid = splay_k(t.id, k);
        int lid = nodes[rid].l;
        nodes[rid].l = 0;
        update(rid);
        t.id = lid;
        return Tree(rid);
    }
    std::array<Tree, 3> split3(Tree&& t, int l, int r) {
        assert(0 <= l && l <= r && r <= ssize(t));
        auto t3 = split(t, r);
        auto t2 = split(t, l);
        return {std::move(t), std::move(t2), std::move(t3)};
    }

    std::vector<S> to_vec(const Tree& t) {
        std::vector<S> buf;
        buf.reserve(nodes[t.id].len);
        auto f = [&](auto self, int id) {
            if (id == 0) return;
            push(id);
            self(self, nodes[id].l);
            buf.push_back(nodes[id].s);
            self(self, nodes[id].r);
        };
        f(f, t.id);
        return buf;
    }

    S all_prod(const Tree& t) { return nodes[t.id].prod; }
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

    template <class F> int max_right(Tree& t, F f) {
        assert(f(m.monoid.e));
        if (f(all_prod(t))) return nodes[t.id].len;
        S s = m.monoid.e;
        int r = 0;
        t.id = splay(t.id, [&](int lid, int id, int) {
            S s2 = m.monoid.op(s, all_prod(lid));
            if (!f(s2)) return -1;
            r += nodes[lid].len;
            s = s2;
            S s3 = m.monoid.op(s, nodes[id].s);
            if (!f(s3)) return 0;
            r++;
            s = s3;
            return 1;
        });
        return r;
    }

  private:
    M m;

    struct Node {
        int l, r;
        int len;
        S s, prod;
        F f;
    };
    std::vector<Node> nodes;

    int new_node(const S& s, int l, int r) {
        int id = int(nodes.size());
        Node n;
        n.l = l;
        n.r = r;
        n.s = s;
        n.f = m.act.e;
        nodes.push_back(n);
        update(id);
        return id;
    }

    void push(int id) {
        Node& n = nodes[id];
        all_apply(n.l, n.f);
        all_apply(n.r, n.f);
        n.f = m.act.e;
    }

    void all_apply(int id, const F& f) {
        if (id == 0) return;
        Node& n = nodes[id];
        n.s = m.mapping(f, n.s);
        n.prod = m.mapping(f, n.prod);
        n.f = m.act.op(f, n.f);
    }

    void update(int id) {
        Node& n = nodes[id];
        n.len = nodes[n.l].len + 1 + nodes[n.r].len;
        n.prod =
            m.monoid.op(m.monoid.op(nodes[n.l].prod, n.s), nodes[n.r].prod);
    }

    int splay_k(int id, int k) {
        return splay(id, [&](int l, int, int) {
            int lsz = nodes[l].len;
            if (k < lsz) return -1;
            k -= lsz;
            if (k == 0) return 0;
            k--;
            return 1;
        });
    }

    template <class Cond> int splay(int id, Cond cond) {
        static std::vector<int> lefts, rights;
        lefts.clear();
        rights.clear();

        int zig = 0;
        while (true) {
            push(id);
            int l = nodes[id].l, r = nodes[id].r;

            auto dir = cond(l, id, r);

            if (dir == 0) {
                {
                    int tmp = nodes[id].l;
                    for (auto i = std::ssize(lefts) - 1; i >= 0; i--) {
                        nodes[lefts[i]].r = std::exchange(tmp, lefts[i]);
                        update(lefts[i]);
                    }
                    nodes[id].l = tmp;
                }
                {
                    int tmp = nodes[id].r;
                    for (auto i = std::ssize(rights) - 1; i >= 0; i--) {
                        nodes[rights[i]].l = std::exchange(tmp, rights[i]);
                        update(rights[i]);
                    }
                    nodes[id].r = tmp;
                }
                update(id);
                return id;
            }
            if (dir < 0) {
                if (zig == -1) {
                    // zig-zig
                    int p = rights.back();
                    rights.pop_back();
                    nodes[p].l = r;
                    update(p);
                    nodes[id].r = p;
                    zig = 0;
                } else {
                    zig = -1;
                }
                rights.push_back(id);
                id = l;
            } else {
                if (zig == 1) {
                    // zig-zig
                    int p = lefts.back();
                    lefts.pop_back();
                    nodes[p].r = l;
                    update(p);
                    nodes[id].l = p;
                    zig = 0;
                } else {
                    zig = 1;
                }
                lefts.push_back(id);
                id = r;
            }
        }
    }
};

}  // namespace yosupo
