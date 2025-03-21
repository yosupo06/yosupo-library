// clang-format off
// verification-helper: PROBLEM https://judge.yosupo.jp/problem/point_set_tree_path_composite_sum_fixed_root
// clang-format on

#include <cassert>
#include <utility>
#include <vector>

#include "yosupo/fastio.hpp"
#include "yosupo/flattenvector.hpp"
#include "yosupo/modint.hpp"
#include "yosupo/toptree.hpp"

using mint = yosupo::ModInt998244353;

inline static yosupo::Scanner sc(stdin);
inline static yosupo::Printer pr(stdout);

struct TreeDP {
    struct Point {
        mint ans, cnt;
    };
    struct Path {
        mint a, b, ans, cnt;  // f(x) = ax + b
    };

    struct PointMonoid {
        using S = Point;
        Point op(Point x, Point y) {
            return Point(x.ans + y.ans, x.cnt + y.cnt);
        }
        Point e = Point(mint(0), mint(0));
    };
    PointMonoid point = PointMonoid();

    struct PathMonoid {
        using S = Path;
        Path op(Path x, Path y) {
            return Path(x.a * y.a, x.b + x.a * y.b,
                        x.ans + x.a * y.ans + x.b * y.cnt, x.cnt + y.cnt);
        }
        Path e = Path(mint(1), mint(0), mint(0), mint(0));
    };
    PathMonoid path = PathMonoid();

    struct Vertex {
        mint a, b;
        mint val;
    };
    std::vector<Vertex> f;
    Path add_vertex(Point x, int u) {
        x.ans += f[u].val;
        x.cnt += mint(1);
        return Path(f[u].a, f[u].b, f[u].a * x.ans + f[u].b * x.cnt, x.cnt);
    }
    Point add_edge(Path x) { return Point(x.ans, x.cnt); }
};

int main() {
    int n, q;
    sc.read(n, q);

    TreeDP dp;
    dp.f = std::vector<TreeDP::Vertex>(n);
    for (int u = 0; u < n; u++) {
        int x;
        sc.read(x);
        dp.f[u].val = x;
    }

    yosupo::StaticTopTree<TreeDP> tr(n, dp);
    struct E {
        int to;
        mint a, b;
    };
    std::vector<std::pair<int, E>> edges;
    for (int i = 0; i < n - 1; i++) {
        int u, v;
        sc.read(u, v);
        int a, b;
        sc.read(a, b);
        tr.add_edge(u, v);
        edges.push_back({u, {v, a, b}});
        edges.push_back({v, {u, a, b}});
    }
    std::vector<int> par(n, -1);
    {
        auto tree = yosupo::FlattenVector(n, edges);
        std::vector<int> topo;
        topo.reserve(n);
        topo.push_back(0);
        dp.f[0].a = mint(1);
        for (int i = 0; i < n; i++) {
            int u = topo[i];
            for (auto e : tree.at(u)) {
                int v = e.to;
                if (v == par[u]) continue;
                par[v] = u;
                dp.f[v].a = e.a;
                dp.f[v].b = e.b;
                topo.push_back(v);
            }
        }
    }
    tr.init();

    struct Query {
        int t;
        int u;
        mint a, b;
    };
    std::vector<Query> queries;
    for (int ph = 0; ph < q; ph++) {
        int t;
        int u;
        sc.read(t);

        if (t == 0) {
            int x;
            sc.read(u, x);
            dp.f[u].val = x;
        } else {
            int e, a, b;
            sc.read(e, a, b);
            u = edges[2 * e].first;
            int v = edges[2 * e].second.to;
            if (par[v] == u) std::swap(u, v);
            assert(par[u] == v);
            dp.f[u].a = a;
            dp.f[u].b = b;
        }
        auto ans = tr.update(u);
        pr.writeln(ans.ans.val());
    }

    return 0;
}