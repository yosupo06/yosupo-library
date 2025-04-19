// clang-format off
// verification-helper: PROBLEM https://judge.yosupo.jp/problem/point_add_rectangle_sum
// clang-format on

#include <algorithm>
#include <cassert>
#include <vector>

#include "yosupo/algebra.hpp"
#include "yosupo/container/segtree2d.hpp"
#include "yosupo/coord.hpp"
#include "yosupo/fastio.hpp"
#include "yosupo/util.hpp"

using namespace yosupo;
using ll = long long;

int main() {
    Scanner sc(stdin);
    Printer pr(stdout);

    int n, q;
    sc.read(n, q);

    struct Point {
        int x, y;
        ll w;
    };

    std::vector<int> xs;

    std::vector<Point> points(n);
    for (int i = 0; i < n; i++) {
        sc.read(points[i].x, points[i].y, points[i].w);
        xs.push_back(points[i].x);
    }
    struct Query {
        int ty;
        int x1, y1, x2, y2;
        ll w;
    };
    std::vector<Query> queries(q);
    for (int i = 0; i < q; i++) {
        sc.read(queries[i].ty);
        if (queries[i].ty == 0) {
            sc.read(queries[i].x1, queries[i].y1, queries[i].w);
            xs.push_back(queries[i].x1);
        } else {
            sc.read(queries[i].x1, queries[i].y1, queries[i].x2, queries[i].y2);
        }
    }

    std::sort(xs.begin(), xs.end());
    dedup(xs);

    auto get_idx = [&](int x) -> int {
        return int(std::lower_bound(xs.begin(), xs.end(), x) - xs.begin());
    };

    SegTree2D seg(Coord(int(xs.size()), 1 << 30), Sum<ll>(0));
    for (int i = 0; i < n; i++) {
        int x = get_idx(points[i].x);
        seg.add({x, points[i].y}, points[i].w);
    }

    for (int i = 0; i < q; i++) {
        if (queries[i].ty == 0) {
            int x = get_idx(queries[i].x1);
            seg.add({x, queries[i].y1}, queries[i].w);
        } else {
            int x1 = get_idx(queries[i].x1);
            int x2 = get_idx(queries[i].x2);
            pr.writeln(seg.prod({x1, queries[i].y1}, {x2, queries[i].y2}));
        }
    }
    return 0;
}
