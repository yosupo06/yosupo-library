#pragma once

#include <array>
#include <bit>
#include <bitset>
#include <ranges>
#include <utility>
#include <vector>
#include "yosupo/flattenvector.hpp"
#include "yosupo/types.hpp"

namespace yosupo {

template <class TreeDP> struct StaticTopTree {
    using Point = TreeDP::Point;
    using Path = TreeDP::Path;

    int n;
    std::vector<std::pair<int, int>> edges;
    TreeDP& dp;

    StaticTopTree(int _n, TreeDP& _dp)
        : n(_n), dp(_dp), points(n + 1, dp.id()), node_ids(n, {n, -1, -1}) {
        edges.reserve(2 * (n - 1));
    }

    void add_edge(int u, int v) {
        edges.push_back({u, v});
        edges.push_back({v, u});
    }

    // compress / rake
    template <class D> struct Inner {
        std::pair<D, D> d;
        int par;
    };
    std::vector<Inner<Path>> compressed;
    std::vector<Inner<Point>> raked;

    Path op(const Path& l, const Path& r) { return dp.compress(l, r); }
    Point op(const Point& l, const Point& r) { return dp.rake(l, r); }

    // compress / rake / leaf
    template <class D> struct Node {
        D p;
        int* par;
    };
    template <class D>
    Node<D> merge(const Node<D>& l,
                  const Node<D>& r,
                  std::vector<Inner<D>>& nodes) {
        int id = int(nodes.size());
        *l.par = 2 * id;
        *r.par = 2 * id + 1;
        nodes.push_back({{l.p, r.p}, -1});
        return {op(l.p, r.p), &nodes[id].par};
    }

    void init(int r = 0) {
        FlattenVector tree(n, edges);
        std::vector<int> topo;
        {
            std::vector<int> parent(n, -1);
            topo.reserve(n);
            topo.push_back(r);
            for (int i = 0; i < n; i++) {
                int u = topo[i];
                for (int v : tree.at(u)) {
                    if (v == parent[u]) continue;
                    parent[v] = u;
                    topo.push_back(v);
                }
            }
            edges.erase(remove_if(edges.begin(), edges.end(),
                                  [&](auto edge) {
                                      return parent[edge.second] != edge.first;
                                  }),
                        edges.end());
            tree = FlattenVector(n, edges);
        }

        std::vector<int> heavy_child(n, -1);
        std::vector<u64> mask(n);

        std::vector<std::pair<int, Node<Path>>> b;
        b.reserve(n);
        auto build_compress = [&](int u) {
            b.clear();
            auto merge_last = [&]() {
                auto y = b.back();
                b.pop_back();
                auto x = b.back();
                b.pop_back();
                b.push_back({std::max(x.first, y.first) + 1,
                             merge(x.second, y.second, compressed)});
            };
            while (u != -1) {
                b.push_back({std::countr_zero(std::bit_ceil(mask[u])),
                             {dp.add_vertex(points[u], u), &node_ids[u].c_id}});
                while (true) {
                    int len = int(b.size());
                    if (len >= 3 && (b[len - 3].first == b[len - 2].first ||
                                     b[len - 3].first <= b[len - 1].first)) {
                        auto last = b.back();
                        b.pop_back();
                        merge_last();
                        b.push_back(last);
                    } else if (len >= 2 &&
                               b[len - 2].first <= b[len - 1].first) {
                        merge_last();
                    } else {
                        break;
                    }
                }
                u = heavy_child[u];
            }
            while (b.size() != 1) {
                merge_last();
            }
            return b.back().second.p;
        };

        compressed.reserve(n - 1);
        raked.reserve(n - 1);

        const int MAX_H = 128;
        std::array<Node<Point>, MAX_H> q;
        std::bitset<MAX_H> has = {};
        for (int u : topo | std::ranges::views::reverse) {
            u64 sum_rake = 0;
            for (int v : tree.at(u)) {
                sum_rake += std::bit_ceil(mask[v]) << 1;
            }
            mask[u] = std::bit_ceil(sum_rake);
            for (int v : tree.at(u)) {
                int d = std::countr_zero(
                    std::bit_ceil(sum_rake - (std::bit_ceil(mask[v]) << 1)));
                u64 s =
                    ((mask[v] + (u64(1) << d) - 1) >> d << d) + (u64(1) << d);
                if (s <= mask[u]) {
                    mask[u] = s;
                    heavy_child[u] = v;
                }
            }

            for (int v : tree.at(u)) {
                if (v == heavy_child[u]) continue;
                int d = std::countr_zero(
                    std::bit_ceil(sum_rake - (std::bit_ceil(mask[v]) << 1)));
                Point point = dp.to_point(build_compress(v));
                Node<Point> data = {point, &node_ids[v].r_id};
                while (has.test(d)) {
                    has.reset(d);
                    data = merge(q[d], data, raked);
                    d++;
                }
                q[d] = data;
                has.set(d);
            }
            int d = int(has._Find_first());
            if (d == int(has.size())) continue;

            Node data = q[d];
            has.reset(d);
            while (true) {
                d = int(has._Find_first());
                if (d == int(has.size())) break;
                has.reset(d);
                data = merge(q[d], data, raked);
            }
            points[u] = data.p;

            for (int v0 : tree.at(u)) {
                if (v0 == heavy_child[u]) continue;
                int v = v0;
                while (v != -1) {
                    node_ids[v].h_par = u;
                    node_ids[v].r_id = node_ids[v0].r_id;
                    v = heavy_child[v];
                }
            }
        }
        build_compress(r);
    }

    std::vector<Point> points;
    struct ID {
        int h_par, c_id, r_id;
    };
    std::vector<ID> node_ids;

    Point update(int u) {
        auto up = [&](int id, auto p, auto& nodes) {
            while (id >= 0) {
                if (id % 2 == 0) {
                    nodes[id / 2].d.first = p;
                } else {
                    nodes[id / 2].d.second = p;
                }
                p = op(nodes[id / 2].d.first, nodes[id / 2].d.second);
                id = nodes[id / 2].par;
            }
            return p;
        };
        while (u != n) {
            auto [h_par, c_id, r_id] = node_ids[u];
            Point p =
                dp.to_point(up(c_id, dp.add_vertex(points[u], u), compressed));
            points[h_par] = up(r_id, p, raked);
            u = h_par;
        }
        return points[n];
    }
};

}  // namespace yosupo