#include "yosupo/container/linkcuttree.hpp"

#include <algorithm>
#include <cstddef>
#include <random>
#include <string>
#include <utility>
#include <vector>

#include "gtest/gtest.h"
#include "yosupo/algebra.hpp"
#include "yosupo/modint.hpp"

namespace {

struct NaiveForest {
    std::vector<std::vector<int>> adj;
    explicit NaiveForest(int n) : adj(n) {}

    void link(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }
    void cut(int u, int v) {
        std::erase(adj[u], v);
        std::erase(adj[v], u);
    }
    std::vector<int> path(int u, int v) const {
        std::vector<int> parent(adj.size(), -1), queue{u};
        parent[u] = u;
        for (size_t i = 0; i < queue.size(); i++) {
            for (int w : adj[queue[i]]) {
                if (parent[w] != -1) continue;
                parent[w] = queue[i];
                queue.push_back(w);
            }
        }
        if (parent[v] == -1) return {};
        std::vector<int> result;
        for (; v != u; v = parent[v]) result.push_back(v);
        result.push_back(u);
        std::ranges::reverse(result);
        return result;
    }
};

// 等値比較を持たない作用。-1 は恒等写像、その他は文字の一括代入。
struct Assign {
    int c;
};

auto string_monoid() {
    return yosupo::ActedMonoid(
        yosupo::Monoid(
            std::string(),
            [](const std::string& a, const std::string& b) { return a + b; }),
        yosupo::Monoid(Assign{-1},
                       [](Assign f, Assign g) { return f.c == -1 ? g : f; }),
        [](Assign f, const std::string& s) {
            return f.c == -1 ? s : std::string(s.size(), char(f.c));
        });
}

TEST(LinkCutTreeTest, EmptyAndSingleton) {
    auto m = yosupo::ActedMonoid(yosupo::Sum<int>(0));
    yosupo::LinkCutTree empty(0, m);
    EXPECT_EQ(empty.size(), 0U);
    yosupo::LinkCutTree tree(1, m);
    EXPECT_EQ(tree.size(), 1U);
    EXPECT_EQ(tree.get(0), 0);
    tree.set(0, 7);
    EXPECT_EQ(tree.prod(0, 0), 7);
    tree.apply(0, 0, {});
    tree.evert(0);
    EXPECT_EQ(tree.root(0), 0);
    EXPECT_TRUE(tree.same(0, 0));
    EXPECT_EQ(tree.lca(0, 0), 0);
    EXPECT_EQ(tree.get(0), 7);
}

TEST(LinkCutTreeTest, RootsAndLca) {
    yosupo::LinkCutTree tree(6, yosupo::ActedMonoid(yosupo::Sum<int>(0)));
    tree.link(1, 0);
    tree.link(2, 0);
    tree.link(3, 1);
    tree.link(4, 1);
    EXPECT_EQ(tree.root(4), 0);
    EXPECT_EQ(tree.lca(3, 4), 1);
    EXPECT_EQ(tree.lca(2, 4), 0);
    EXPECT_EQ(tree.lca(0, 4), 0);
    EXPECT_EQ(tree.lca(3, 3), 3);
    EXPECT_EQ(tree.lca(3, 5), -1);
    EXPECT_EQ(tree.root(4), 0);

    tree.evert(3);
    EXPECT_EQ(tree.lca(2, 4), 1);
    EXPECT_EQ(tree.lca(0, 4), 1);
    EXPECT_EQ(tree.root(2), 3);
    tree.set(4, 10);
    EXPECT_EQ(tree.get(4), 10);
    EXPECT_EQ(tree.root(4), 3);
    tree.cut(1, 0);
    EXPECT_EQ(tree.root(3), 1);
    EXPECT_EQ(tree.root(2), 0);
    EXPECT_FALSE(tree.same(3, 2));
    EXPECT_EQ(tree.lca(3, 2), -1);
    tree.link(4, 2);
    EXPECT_EQ(tree.root(3), 0);
    EXPECT_EQ(tree.prod(3, 0), 10);
    EXPECT_EQ(tree.root(2), 3);
}

TEST(LinkCutTreeTest, ReversalAndPendingAssignment) {
    yosupo::LinkCutTree tree(std::vector<std::string>{"a", "b", "c", "d", "e"},
                             string_monoid());
    for (int i = 1; i < 5; i++) tree.link(i, i - 1);
    EXPECT_EQ(tree.prod(0, 4), "abcde");
    EXPECT_EQ(tree.prod(4, 0), "edcba");
    tree.apply(4, 1, {'x'});
    tree.apply(3, 0, {'y'});
    tree.cut(2, 3);
    tree.link(0, 4);
    EXPECT_EQ(tree.prod(2, 3), "yyyxy");
    tree.set(4, "z");
    EXPECT_EQ(tree.prod(3, 2), "yzyyy");
    tree.apply(0, 0, {'w'});
    EXPECT_EQ(tree.get(0), "w");
    EXPECT_EQ(tree.prod(2, 3), "yywzy");
}

TEST(LinkCutTreeTest, RandomForest) {
    std::mt19937 rng(712367);
    for (int n : {1, 2, 31, 100}) {
        auto random = [&](int limit) { return int(rng() % unsigned(limit)); };
        std::vector<std::string> values(n, "a");
        yosupo::LinkCutTree tree(values, string_monoid());
        NaiveForest naive(n);
        for (int i = 1; i < n; i++) {
            tree.link(i, i - 1);
            naive.link(i, i - 1);
        }
        for (int step = 0; step < 5000; step++) {
            SCOPED_TRACE(testing::Message() << "n=" << n << ", step=" << step);
            int u = random(n), v = random(n), type = random(8);
            auto path = naive.path(u, v);
            if (type == 0 && path.empty()) {
                tree.link(u, v);
                naive.link(u, v);
            } else if (type == 1 && path.size() > 1) {
                int k = random(int(path.size()) - 1);
                tree.cut(path[k], path[k + 1]);
                naive.cut(path[k], path[k + 1]);
            } else if (type == 2) {
                values[u] = std::string(1, char('a' + random(26)));
                tree.set(u, values[u]);
            } else if (type == 3 && !path.empty()) {
                int c = 'a' + random(26);
                tree.apply(u, v, {c});
                for (int w : path) values[w] = std::string(1, char(c));
            } else if (type == 4 && !path.empty()) {
                std::string expected;
                for (int w : path) expected += values[w];
                EXPECT_EQ(tree.prod(u, v), expected);
                std::ranges::reverse(expected);
                EXPECT_EQ(tree.prod(v, u), expected);
            } else if (type == 5) {
                EXPECT_EQ(tree.same(u, v), !path.empty());
            } else if (type == 6) {
                int w = random(n);
                tree.evert(u);
                auto other = naive.path(u, w);
                if (!path.empty() && !other.empty()) {
                    int k = 0;
                    while (k + 1 < int(std::min(path.size(), other.size())) &&
                           path[k + 1] == other[k + 1]) {
                        k++;
                    }
                    EXPECT_EQ(tree.lca(v, w), path[k]);
                } else if (naive.path(v, w).empty()) {
                    EXPECT_EQ(tree.lca(v, w), -1);
                }
                EXPECT_EQ(tree.root(u), u);
            } else {
                EXPECT_EQ(tree.get(u), values[u]);
            }
        }
        for (int i = 0; i < n; i++) EXPECT_EQ(tree.get(i), values[i]);
    }
}

TEST(LinkCutTreeTest, RandomAffinePathUpdate) {
    using mint = yosupo::ModInt998244353;
    using S = std::pair<mint, mint>;
    auto monoid = yosupo::Monoid(S(0, 0), [](S a, S b) {
        return S(a.first + b.first, a.second + b.second);
    });
    auto act = yosupo::Monoid(S(1, 0), [](S f, S g) {
        return S(f.first * g.first, f.first * g.second + f.second);
    });
    auto m = yosupo::ActedMonoid(monoid, act, [](S f, S s) {
        return S(f.first * s.first + f.second * s.second, s.second);
    });
    const int n = 30;
    std::mt19937 rng(98923);
    auto random = [&](int limit) { return int(rng() % unsigned(limit)); };
    std::vector<mint> values(n);
    yosupo::LinkCutTree tree(std::vector<S>(n, {0, 1}), m);
    NaiveForest naive(n);
    for (int i = 1; i < n; i++) {
        tree.link(i, 0);
        naive.link(i, 0);
    }
    for (int step = 0; step < 5000; step++) {
        SCOPED_TRACE(step);
        int u = random(n), v = random(n);
        auto path = naive.path(u, v);
        int type = random(5);
        if (type == 0 && path.size() > 1) {
            tree.cut(path[0], path[1]);
            naive.cut(path[0], path[1]);
            do {
                u = random(n);
                v = random(n);
            } while (!naive.path(u, v).empty());
            tree.link(u, v);
            naive.link(u, v);
        } else if (type == 1) {
            values[u] = random(100);
            tree.set(u, {values[u], 1});
        } else if (type == 2) {
            S f(random(10), random(10));
            tree.apply(u, v, f);
            for (int w : path) values[w] = f.first * values[w] + f.second;
        } else if (type == 3) {
            mint sum = 0;
            for (int w : path) sum += values[w];
            auto result = tree.prod(u, v);
            EXPECT_EQ(result.first, sum);
            EXPECT_EQ(result.second, mint(path.size()));
        } else {
            EXPECT_EQ(tree.get(u), S(values[u], 1));
        }
    }
    for (int i = 0; i < n; i++) EXPECT_EQ(tree.get(i), S(values[i], 1));
}

TEST(LinkCutTreeTest, LongChain) {
    const int n = 200000;
    yosupo::LinkCutTree tree(std::vector<int>(n, 1),
                             yosupo::ActedMonoid(yosupo::Sum<int>(0)));
    for (int i = 1; i < n; i++) tree.link(i - 1, i);
    EXPECT_EQ(tree.prod(0, n - 1), n);
    EXPECT_EQ(tree.prod(n - 1, 0), n);
    tree.cut(n / 2 - 1, n / 2);
    EXPECT_FALSE(tree.same(0, n - 1));
    EXPECT_EQ(tree.prod(0, n / 2 - 1), n / 2);
    tree.link(0, n - 1);
    EXPECT_EQ(tree.prod(n / 2 - 1, n / 2), n);
}

}  // namespace
