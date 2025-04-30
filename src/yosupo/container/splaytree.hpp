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

    SplayTree(const M& _m) : m(_m) {}

    struct Tree {
        int id;
        Tree() : id(-1) {}
        Tree(int _id) : id(_id) {}
        bool empty() const { return id == -1; }
    };

    size_t size(const Tree& t) { return len(t.id); }
    ptrdiff_t ssize(const Tree& t) { return size(t); }

    Tree build() { return Tree(); }
    Tree build(S s) { return build(std::vector<S>{s}); }
    Tree build(const std::vector<S>& v) {
        auto f = [&](auto self, int lidx, int ridx) -> int {
            if (lidx == ridx) return -1;
            assert(lidx <= ridx);
            if (ridx - lidx == 1) {
                int id = int(nodes.size());
                nodes.push_back(Node{-1, -1, 1, v[lidx], v[lidx], m.act.e});
                return id;
            }
            int mid = (lidx + ridx) / 2;
            int l = self(self, lidx, mid);
            int r = self(self, mid + 1, ridx);
            int id = int(nodes.size());
            nodes.push_back(Node{l, r, -1, v[mid], m.monoid.e, m.act.e});
            update(id);
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
        auto [lid, id, rid] = splay3(t.id, [&](int l, int, int) {
            if (k <= len(l)) return -1;
            k -= len(l) + 1;
            return 1;
        });
        assert(id == -1);

        t.id = int(nodes.size());
        nodes.push_back(Node{lid, rid, -1, s, s, m.act.e});
        update(t.id);
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
    Tree split(Tree& t, int k) {
        assert(0 <= k && k <= int(size(t)));
        if (k == int(size(t))) return Tree();
        int rid = splay_k(t.id, k);
        int lid = nodes[rid].l;
        nodes[rid].l = -1;
        update(rid);
        t.id = lid;
        return Tree(rid);
    }

    std::vector<S> to_vec(const Tree& t) {
        std::vector<S> buf;
        buf.reserve(len(t.id));
        auto f = [&](auto self, int id) {
            if (id == -1) return;
            push(id);
            self(self, nodes[id].l);
            buf.push_back(nodes[id].s);
            self(self, nodes[id].r);
        };
        f(f, t.id);
        return buf;
    }

    S all_prod(const Tree& t) { return all_prod(t.id); }
    S prod(Tree& t, int l, int r) {
        assert(0 <= l && l <= r && r <= ssize(t));
        auto t3 = split(t, r);
        auto t2 = split(t, l);
        S s = all_prod(t2);
        t = merge(std::move(t), std::move(t2));
        t = merge(std::move(t), std::move(t3));
        return s;
    }

    void all_apply(Tree& t, F f) {
        if (t.empty()) return;
        all_apply(t.id, f);
    }
    void apply(Tree& t, int l, int r, F f) {
        assert(0 <= l && l <= r && r <= ssize(t));
        auto t3 = split(t, r);
        auto t2 = split(t, l);
        all_apply(t2, f);
        t = merge(std::move(t), std::move(t2));
        t = merge(std::move(t), std::move(t3));
    }

    template <class F> int max_right(Tree& t, F f) {
        assert(f(m.monoid.e));

        if (f(all_prod(t))) return len(t.id);
        if (len(t.id) == 1) return 0;
        S s = m.monoid.e;
        int r = 0;
        t.id = splay(t.id, [&](int lid, int id, int) {
            S s2 = m.monoid.op(s, all_prod(lid));
            if (!f(s2)) return -1;
            r += len(lid);
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
    int len(int id) { return id == -1 ? 0 : nodes[id].len; }
    S all_prod(int id) { return id == -1 ? m.monoid.e : nodes[id].prod; }

    void push(int id) {
        Node& n = nodes[id];
        if (n.l != -1) all_apply(n.l, n.f);
        if (n.r != -1) all_apply(n.r, n.f);
        n.f = m.act.e;
    }

    void all_apply(int id, F f) {
        Node& n = nodes[id];
        n.s = m.mapping(f, n.s);
        n.prod = m.mapping(f, n.prod);
        n.f = m.act.op(f, n.f);
    }

    void update(int id) {
        Node& n = nodes[id];
        n.len = 1;
        n.prod = n.s;
        if (n.l != -1) {
            n.len += nodes[n.l].len;
            n.prod = m.monoid.op(nodes[n.l].prod, n.prod);
        }
        if (n.r != -1) {
            n.len += nodes[n.r].len;
            n.prod = m.monoid.op(n.prod, nodes[n.r].prod);
        }
    }

    template <class F> int splay(int id, F f) {
        auto [l, id2, r] = splay3(id, f);
        assert(id2 != -1);
        nodes[id2].l = l;
        nodes[id2].r = r;
        update(id2);
        return id2;
    }
    int splay_k(int id, int k) {
        return splay(id, [&](int l, int, int) {
            int lsz = len(l);
            if (k < lsz) return -1;
            k -= lsz;
            if (k == 0) return 0;
            k--;
            return 1;
        });
    }

    template <class F> std::array<int, 3> splay3(int id, F f) {
        static std::vector<int> lefts, rights;
        lefts.clear();
        rights.clear();

        int zig = 0;
        while (id != -1) {
            push(id);
            int l = nodes[id].l, r = nodes[id].r;

            auto dir = f(l, id, r);

            if (dir == 0) {
                break;
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

        int l = id == -1 ? -1 : nodes[id].l;
        int r = id == -1 ? -1 : nodes[id].r;
        {
            for (auto i = std::ssize(lefts) - 1; i >= 0; i--) {
                nodes[lefts[i]].r = std::exchange(l, lefts[i]);
                update(lefts[i]);
            }
        }
        {
            for (auto i = std::ssize(rights) - 1; i >= 0; i--) {
                nodes[rights[i]].l = std::exchange(r, rights[i]);
                update(rights[i]);
            }
        }

        return {l, id, r};
    }

    void to_vec(int id, std::vector<S>& buf) {
        if (id == -1) return;
        push(id);
        to_vec(nodes[id].l, buf);
        buf.push_back(nodes[id].s);
        to_vec(nodes[id].r, buf);
    }
};

}  // namespace yosupo