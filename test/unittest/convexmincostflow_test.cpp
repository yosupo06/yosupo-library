#include "yosupo/convexmincostflow.hpp"

#include <algorithm>
#include <limits>
#include <random>
#include <utility>
#include <vector>

#include "gtest/gtest.h"
#include "yosupo/networksimplex.hpp"

namespace {

using Graph = yosupo::ConvexMinCostFlowGraph<int, long long>;

struct InputEdge {
    int from, to;
    std::vector<Graph::Segment> segments;

    int capacity() const {
        int cap = 0;
        for (auto s : segments) cap += s.cap;
        return cap;
    }
    long long cost(int flow) const {
        long long result = 0;
        for (auto s : segments) {
            int used = std::min(flow, s.cap);
            result += used * s.cost;
            flow -= used;
        }
        return result;
    }
};

void check_result(const std::vector<InputEdge>& edges,
                  const std::vector<int>& balance,
                  const Graph::FlowResult<long long>& result) {
    ASSERT_TRUE(result.feasible);
    ASSERT_EQ(result.flow.size(), edges.size());
    ASSERT_EQ(result.potential.size(), balance.size());
    std::vector<int> actual(balance.size());
    long long cost = 0;
    for (int i = 0; i < int(edges.size()); i++) {
        const auto& e = edges[i];
        int x = result.flow[i];
        ASSERT_LE(0, x);
        ASSERT_LE(x, e.capacity());
        actual[e.from] += x;
        actual[e.to] -= x;
        cost += e.cost(x);
        long long dp = result.potential[e.from] - result.potential[e.to];
        if (x < e.capacity()) {
            EXPECT_GE(e.cost(x + 1) - e.cost(x) + dp, 0);
        }
        if (x > 0) {
            EXPECT_GE(e.cost(x - 1) - e.cost(x) - dp, 0);
        }
    }
    EXPECT_EQ(actual, balance);
    EXPECT_EQ(cost, result.cost);
}

TEST(ConvexMinCostFlowTest, Usage) {
    Graph g(2);
    EXPECT_EQ(g.add_edge(0, 1, {{3, 2}, {5, 7}}), 0);
    g.add_excess(0, 2);
    g.add_excess(0, 3);
    g.add_excess(1, -5);
    auto r = std::move(g).solve<long long>();
    ASSERT_TRUE(r.feasible);
    EXPECT_EQ(r.cost, 20);
    EXPECT_EQ(r.flow, std::vector<int>({5}));
    check_result({{0, 1, {{3, 2}, {5, 7}}}}, {5, -5}, r);
}

TEST(ConvexMinCostFlowTest, ReverseAcrossBreakpoints) {
    std::vector<InputEdge> edges = {
        {0, 1, {{2, 0}}}, {0, 2, {{2, 0}}}, {1, 3, {{1, 0}, {1, 1}}},
        {1, 4, {{2, 3}}}, {2, 3, {{2, 2}}}, {2, 4, {{2, 100}}},
        {3, 5, {{2, 0}}}, {4, 5, {{2, 0}}}};
    Graph g(6);
    for (const auto& e : edges) g.add_edge(e.from, e.to, e.segments);
    g.add_excess(0, 4);
    g.add_excess(5, -4);
    auto r = std::move(g).solve<long long>();
    ASSERT_TRUE(r.feasible);
    EXPECT_EQ(r.cost, 10);
    EXPECT_EQ(r.flow, std::vector<int>({2, 2, 0, 2, 2, 0, 2, 2}));
    check_result(edges, {4, 0, 0, 0, 0, -4}, r);
}

TEST(ConvexMinCostFlowTest, NegativeSlopesAndExactBreakpoints) {
    for (int amount = 0; amount <= 9; amount++) {
        SCOPED_TRACE(amount);
        InputEdge e{0, 1, {{2, -7}, {3, -2}, {4, 5}}};
        Graph g(2);
        g.add_edge(e.from, e.to, e.segments);
        g.add_excess(0, amount);
        g.add_excess(1, -amount);
        auto r = std::move(g).solve<long long>();
        ASSERT_TRUE(r.feasible);
        EXPECT_EQ(r.flow, std::vector<int>({amount}));
        check_result({e}, {amount, -amount}, r);
    }
}

TEST(ConvexMinCostFlowTest, DisconnectedCirculationAndSelfLoops) {
    std::vector<InputEdge> edges = {
        {0, 1, {{2, 1}}}, {2, 3, {{3, -5}, {2, 1}}},
        {3, 2, {{4, 2}}}, {4, 4, {{2, -7}, {3, 0}, {1, 8}}},
        {4, 4, {}},       {4, 4, {{3, 9}}}};
    Graph g;
    for (int i = 0; i < 5; i++) EXPECT_EQ(g.add_vertex(), i);
    for (const auto& e : edges) g.add_edge(e.from, e.to, e.segments);
    g.add_excess(0, 2);
    g.add_excess(1, -2);
    auto r = std::move(g).solve<long long>();
    ASSERT_TRUE(r.feasible);
    EXPECT_EQ(r.cost, -21);
    check_result(edges, {2, -2, 0, 0, 0}, r);
}

TEST(ConvexMinCostFlowTest, EmptyAndInfeasible) {
    auto empty = Graph().solve<long long>();
    ASSERT_TRUE(empty.feasible);
    EXPECT_EQ(empty.cost, 0);
    EXPECT_TRUE(empty.flow.empty());
    EXPECT_TRUE(empty.potential.empty());
    for (int mode = 0; mode < 3; mode++) {
        Graph g(3);
        g.add_edge(0, 1, 0, -10);
        g.add_edge(0, 2, 1, 1);
        g.add_excess(0, 2);
        if (mode == 0) g.add_excess(1, -2);
        if (mode == 1) g.add_excess(2, -2);
        auto r = std::move(g).solve<long long>();
        EXPECT_FALSE(r.feasible);
        EXPECT_EQ(r.cost, 0);
        EXPECT_TRUE(r.flow.empty());
        EXPECT_TRUE(r.potential.empty());
    }
}

TEST(ConvexMinCostFlowTest, LargeCapacityAndWideTotalCost) {
    using WideGraph = yosupo::ConvexMinCostFlowGraph<long long, long long>;
    constexpr long long cap = 1'000'000'000'000;
    constexpr long long cost = 1'000'000'000;
    WideGraph g(2);
    g.add_edge(0, 1, {{cap, -cost}, {cap, cost}, {cap, 2 * cost}});
    g.add_excess(0, 3 * cap);
    g.add_excess(1, -3 * cap);
    auto r = std::move(g).solve<__int128>();
    ASSERT_TRUE(r.feasible);
    EXPECT_EQ(r.cost, __int128(2) * cap * cost);
    EXPECT_EQ(r.flow, std::vector<long long>({3 * cap}));
}

TEST(ConvexMinCostFlowTest, ManySegmentsAndEqualSlopes) {
    std::vector<Graph::Segment> segments(100'000, {1, 3});
    Graph g(2);
    g.add_edge(0, 1, segments);
    g.add_edge(0, 1, {{2, 1}, {2, 4}});
    g.add_excess(0, 4);
    g.add_excess(1, -4);
    auto r = std::move(g).solve<long long>();
    ASSERT_TRUE(r.feasible);
    EXPECT_EQ(r.cost, 8);
    EXPECT_EQ(r.flow, std::vector<int>({2, 2}));
}

TEST(ConvexMinCostFlowTest, RandomAgainstExhaustiveSearch) {
    std::mt19937 rng(735421);
    auto random = [&](int bound) { return int(rng() % unsigned(bound)); };
    for (int trial = 0; trial < 500; trial++) {
        SCOPED_TRACE(trial);
        int n = 1 + random(5), m = random(7);
        std::vector<InputEdge> edges;
        std::vector<int> balance(n);
        Graph g(n);
        for (int i = 0; i < m; i++) {
            InputEdge e{random(n), random(n), {}};
            long long cost = random(9) - 4;
            int k = random(4);
            for (int j = 0; j < k; j++) {
                cost += random(3);
                e.segments.push_back({1, cost});
            }
            int f = random(e.capacity() + 1);
            balance[e.from] += f;
            balance[e.to] -= f;
            g.add_edge(e.from, e.to, e.segments);
            edges.push_back(e);
        }
        if (trial % 2) {
            balance[random(n)]++;
            balance[random(n)]--;
        }
        if (trial % 11 == 0) balance[random(n)]++;
        for (int v = 0; v < n; v++) g.add_excess(v, balance[v]);
        long long best = std::numeric_limits<long long>::max();
        std::vector<int> actual(n);
        auto dfs = [&](auto self, int i, long long cost) -> void {
            if (i == m) {
                if (actual == balance) best = std::min(best, cost);
                return;
            }
            const auto& e = edges[i];
            for (int f = 0; f <= e.capacity(); f++) {
                actual[e.from] += f;
                actual[e.to] -= f;
                self(self, i + 1, cost + e.cost(f));
                actual[e.from] -= f;
                actual[e.to] += f;
            }
        };
        dfs(dfs, 0, 0);
        auto r = std::move(g).solve<long long>();
        ASSERT_EQ(r.feasible, best != std::numeric_limits<long long>::max());
        if (r.feasible) {
            EXPECT_EQ(r.cost, best);
            check_result(edges, balance, r);
        }
    }
}

TEST(ConvexMinCostFlowTest, RandomAgainstExpandedNetworkSimplex) {
    std::mt19937 rng(3289421);
    auto random = [&](int bound) { return int(rng() % unsigned(bound)); };
    for (int trial = 0; trial < 1000; trial++) {
        SCOPED_TRACE(trial);
        int n = 1 + random(10), m = random(25);
        Graph g(n);
        yosupo::NetworkSimplexGraph<int, long long> expanded(n);
        std::vector<InputEdge> edges;
        std::vector<int> balance(n);
        for (int i = 0; i < m; i++) {
            InputEdge e{random(n), random(n), {}};
            long long cost = random(21) - 10;
            int k = random(13);
            for (int j = 0; j < k; j++) {
                cost += random(4);
                int cap = 1 + random(6);
                e.segments.push_back({cap, cost});
                expanded.add_edge(e.from, e.to, 0, cap, cost);
            }
            int f = random(e.capacity() + 1);
            balance[e.from] += f;
            balance[e.to] -= f;
            g.add_edge(e.from, e.to, e.segments);
            edges.push_back(e);
        }
        if (trial % 2) {
            balance[random(n)] += 10;
            balance[random(n)] -= 10;
        }
        for (int v = 0; v < n; v++) {
            g.add_excess(v, balance[v]);
            expanded.add_excess(v, balance[v]);
        }
        auto r = std::move(g).solve<long long>();
        auto expected = std::move(expanded).solve<long long>();
        ASSERT_EQ(r.feasible, expected.feasible);
        if (r.feasible) {
            EXPECT_EQ(r.cost, expected.cost);
            check_result(edges, balance, r);
        }
    }
}

}  // namespace
