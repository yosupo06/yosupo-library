#include <sys/types.h>

#include <array>
#include <cassert>
#include <cstddef>
#include <utility>
#include <vector>

#include "yosupo/algebra.hpp"

namespace yosupo {

template <class M> struct SplayTree {
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
            .rev = false,
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

    int new_node(const S& s, int l, int r) {
        int id = int(nodes.size());
        nodes.push_back(Node{.l = l, .r = r, .s = s, .f = m.act.e});
        update(id);
        return id;
    }

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
        if (k == 0) return std::exchange(t, Tree());
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

    void reverse(Tree& t) {
        if (t.empty()) return;
        reverse(t.id);
    }

  private:
    M m;

    struct Node {
        int l, r, len;
        S s, prod;
        F f;
        bool rev;
    };
    std::vector<Node> nodes;

    void reverse(int id) {
        Node& n = nodes[id];
        n.rev = !n.rev;
        std::swap(n.l, n.r);
    }

    void push(int id) {
        Node& n = nodes[id];
        if (n.rev) {
            reverse(n.l);
            reverse(n.r);
            n.rev = false;
        }
        if (n.f != m.act.e) {
            all_apply(n.l, n.f);
            all_apply(n.r, n.f);
            n.f = m.act.e;
        }
    }

    void all_apply(int id, const F& f) {
        if (id == 0) return;
        Node& n = nodes[id];
        n.s = m.mapping(f, n.s, 1);
        n.prod = m.mapping(f, n.prod, n.len);
        n.f = m.act.op(f, n.f);
    }

    void update(int id) {
        Node& n = nodes[id];
        n.len = nodes[n.l].len + 1 + nodes[n.r].len;
        n.prod =
            m.monoid.op(m.monoid.op(nodes[n.l].prod, n.s), nodes[n.r].prod);
    }

    int splay_k(int id, int k) {
        return splayx(id, [&](int l, int, int) {
            int lsz = nodes[l].len;
            if (k < lsz) return -1;
            k -= lsz;
            if (k == 0) return 0;
            k--;
            return 1;
        });
    }

    template <class Cond> int splay(int id, Cond cond) {
        auto down = [&](auto self) -> std::pair<int, int> {
            int id1 = id;
            push(id1);
            int l = nodes[id1].l, r = nodes[id1].r;
            auto dir = cond(l, id1, r);
            if (dir == 0) {
                return {nodes[id1].l, nodes[id1].r};
            }

            id = (dir < 0) ? l : r;
            int id2 = id;
            push(id2);
            int l2 = nodes[id].l, r2 = nodes[id].r;
            auto dir2 = cond(l2, id, r2);

            if (dir < 0) {
                if (dir2 == 0) {
                    nodes[id1].l = r2;
                    update(id1);
                    return {l2, id1};
                }
                if (dir2 < 0) {
                    id = l2;
                    nodes[id1].l = r2;
                    update(id1);
                    nodes[id2].r = id1;
                    auto [l3, r3] = self(self);
                    nodes[id2].l = r3;
                    update(id2);
                    return {l3, id2};
                } else {
                    id = r2;
                    auto [l3, r3] = self(self);
                    nodes[id1].l = r3;
                    nodes[id2].r = l3;
                    update(id1);
                    update(id2);
                    return {id2, id1};
                }
            } else {
                if (dir2 == 0) {
                    nodes[id1].r = l2;
                    update(id1);
                    return {id1, r2};
                }
                if (dir2 > 0) {
                    id = r2;
                    nodes[id1].r = l2;
                    update(id1);
                    nodes[id2].l = id1;
                    auto [l3, r3] = self(self);
                    nodes[id2].r = l3;
                    update(id2);
                    return {id2, r3};
                } else {
                    id = l2;
                    auto [l3, r3] = self(self);
                    nodes[id1].r = l3;
                    nodes[id2].l = r3;
                    update(id1);
                    update(id2);
                    return {id1, id2};
                }
            }
        };
        auto [l, r] = down(down);
        nodes[id].l = l;
        nodes[id].r = r;
        update(id);
        return id;
    }

    template <class F> int splayx(int id, F f) {
        static std::vector<int> lefts, rights;
        lefts.clear();
        rights.clear();

        int zig = 0;
        while (true) {
            push(id);
            int l = nodes[id].l, r = nodes[id].r;

            auto dir = f(l, id, r);

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
