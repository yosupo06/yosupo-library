#pragma once

#include <array>
#include <bit>
#include <bitset>
#include <cassert>
#include <ranges>
#include <utility>
#include <vector>

#include "yosupo/algebra.hpp"
#include "yosupo/flattenvector.hpp"
#include "yosupo/tree.hpp"
#include "yosupo/types.hpp"

namespace yosupo {

template <static_top_tree_dp TreeDP> struct StaticTopTree {
    using Point = typename TreeDP::Point;
    using Path = typename TreeDP::Path;
    using Vertex = typename TreeDP::Vertex;

    int n;

    StaticTopTree(const RootedTree& tree,
                  std::vector<Vertex> _vertices,
                  const TreeDP& _dp = TreeDP())
        : n(tree.n),
          vertices(std::move(_vertices)),
          dp(_dp),
          points(n + 1, dp.point.e),
          node_ids(n, {n, -1, -1}) {
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
                             merge<false>(x.second, y.second, compressed)});
            };
            while (u != -1) {
                b.push_back({std::countr_zero(std::bit_ceil(mask[u])),
                             {dp.add_vertex(points[u], vertices[u]),
                              &node_ids[u].c_id}});
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
        for (int u : tree.topo | std::ranges::views::reverse) {
            u64 sum_rake = 0;
            for (int v : tree.children.at(u)) {
                sum_rake += std::bit_ceil(mask[v]) << 1;
            }
            mask[u] = std::bit_ceil(sum_rake);
            for (int v : tree.children.at(u)) {
                int d = std::countr_zero(
                    std::bit_ceil(sum_rake - (std::bit_ceil(mask[v]) << 1)));
                u64 s =
                    ((mask[v] + (u64(1) << d) - 1) >> d << d) + (u64(1) << d);
                if (s <= mask[u]) {
                    mask[u] = s;
                    heavy_child[u] = v;
                }
            }

            for (int v : tree.children.at(u)) {
                if (v == heavy_child[u]) continue;
                int d = std::countr_zero(
                    std::bit_ceil(sum_rake - (std::bit_ceil(mask[v]) << 1)));
                Point point = dp.add_edge(build_compress(v));
                Node<Point> data = {point, &node_ids[v].r_id};
                while (has.test(d)) {
                    has.reset(d);
                    data = merge<true>(q[d], data, raked);
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
                data = merge<true>(q[d], data, raked);
            }
            points[u] = data.p;

            for (int v0 : tree.children.at(u)) {
                if (v0 == heavy_child[u]) continue;
                int v = v0;
                while (v != -1) {
                    node_ids[v].h_par = u;
                    node_ids[v].r_id = node_ids[v0].r_id;
                    v = heavy_child[v];
                }
            }
        }
        points[n] = dp.add_edge(build_compress(tree.root));
    }

    Vertex get_vertex(int u) { return vertices[u]; }
    void update(int u, Vertex vertex) {
        vertices[u] = vertex;
        auto up_compress = [&](int id, auto p) {
            while (id >= 0) {
                if (id % 2 == 0) {
                    compressed[id / 2].d.first = p;
                } else {
                    compressed[id / 2].d.second = p;
                }
                p = dp.path.op(compressed[id / 2].d.first,
                               compressed[id / 2].d.second);
                id = compressed[id / 2].par;
            }
            return p;
        };
        auto up_rake = [&](int id, auto p) {
            while (id >= 0) {
                if (id % 2 == 0) {
                    raked[id / 2].d.first = p;
                } else {
                    raked[id / 2].d.second = p;
                }
                p = dp.point.op(raked[id / 2].d.first, raked[id / 2].d.second);
                id = raked[id / 2].par;
            }
            return p;
        };
        while (u != n) {
            auto [h_par, c_id, r_id] = node_ids[u];
            Point p = dp.add_edge(
                up_compress(c_id, dp.add_vertex(points[u], vertices[u])));
            points[h_par] = up_rake(r_id, p);
            u = h_par;
        }
    }

    Point all_prod() { return points[n]; }

    Path path_prod(int u) {
        Path path = dp.path.e;
        Point point = points[u];
        while (true) {
            auto [h_par, c_id, r_id] = node_ids[u];
            Path l = dp.path.e, r = dp.path.e;
            {
                int id = c_id;
                while (id >= 0) {
                    if (id % 2 == 0) {
                        r = dp.path.op(r, compressed[id / 2].d.second);
                    } else {
                        l = dp.path.op(compressed[id / 2].d.first, l);
                    }
                    id = compressed[id / 2].par;
                }
            }
            point = dp.point.op(point, dp.add_edge(r));
            path = dp.path.op(
                l, dp.path.op(dp.add_vertex(point, vertices[u]), path));
            if (h_par == n) return path;
            point = dp.point.e;
            {
                int id = r_id;
                while (id >= 0) {
                    if (id % 2 == 0) {
                        point = dp.point.op(point, raked[id / 2].d.second);
                    } else {
                        point = dp.point.op(raked[id / 2].d.first, point);
                    }
                    id = raked[id / 2].par;
                }
            }
            u = h_par;
        }
    }

  private:
    std::vector<Vertex> vertices;
    TreeDP dp;

    // compress / rake
    template <class D> struct Inner {
        std::pair<D, D> d;
        int par;
    };
    std::vector<Inner<Path>> compressed;
    std::vector<Inner<Point>> raked;

    // compress / rake / leaf
    template <class D> struct Node {
        D p;
        int* par;
    };
    template <bool RAKE, class D>
    Node<D> merge(const Node<D>& l,
                  const Node<D>& r,
                  std::vector<Inner<D>>& nodes) {
        int id = int(nodes.size());
        *l.par = 2 * id;
        *r.par = 2 * id + 1;
        nodes.push_back({{l.p, r.p}, -1});
        if constexpr (RAKE) {
            return {dp.point.op(l.p, r.p), &nodes[id].par};
        } else {
            return {dp.path.op(l.p, r.p), &nodes[id].par};
        }
    }

    std::vector<Point> points;
    struct ID {
        int h_par, c_id, r_id;
    };
    std::vector<ID> node_ids;
};

}  // namespace yosupo
