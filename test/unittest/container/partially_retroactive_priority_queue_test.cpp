#include "yosupo/container/partially_retroactive_priority_queue.hpp"

#include <limits>
#include <map>
#include <optional>
#include <random>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "gtest/gtest.h"

using namespace yosupo;

namespace {

struct Operation {
    int type = 0;  // 0: no-op, 1: push, 2: pop
    int value = 0;
};

struct ReplayResult {
    std::map<int, int> entries;
    std::optional<int> minimum;
    int failed_pops = 0;
};

template <class Compare>
ReplayResult replay(const std::vector<Operation>& operations, Compare comp) {
    auto order = [&](const auto& a, const auto& b) {
        if (comp(a.first, b.first)) return true;
        if (comp(b.first, a.first)) return false;
        return a.second < b.second;
    };
    std::set<std::pair<int, int>, decltype(order)> queue(order);
    ReplayResult result;
    for (int t = 0; t < int(operations.size()); t++) {
        auto operation = operations[t];
        if (operation.type == 1) {
            queue.emplace(operation.value, t);
        } else if (operation.type == 2) {
            if (queue.empty()) {
                result.failed_pops++;
            } else {
                queue.erase(queue.begin());
            }
        }
    }
    for (auto [value, time] : queue) result.entries[time] = value;
    if (!queue.empty()) result.minimum = queue.begin()->first;
    return result;
}

template <class Compare>
auto apply(PartiallyRetroactivePriorityQueue<int, Compare>& queue,
           int t,
           Operation operation) {
    if (operation.type == 1) return queue.set_push(t, operation.value);
    if (operation.type == 2) return queue.set_pop(t);
    return queue.clear(t);
}

template <class Compare>
void check_change(
    const PartiallyRetroactivePriorityQueue<int, Compare>& queue,
    const typename PartiallyRetroactivePriorityQueue<int, Compare>::Change&
        change,
    std::map<int, int>& entries,
    const std::vector<Operation>& operations,
    Compare comp) {
    ASSERT_LE(change.added.size() + change.removed.size(), 2);
    for (const auto& entry : change.removed) {
        ASSERT_EQ(entries.count(entry.time), 1);
        EXPECT_EQ(entries.at(entry.time), entry.value);
        entries.erase(entry.time);
    }
    for (const auto& entry : change.added) {
        ASSERT_EQ(entries.count(entry.time), 0);
        entries[entry.time] = entry.value;
    }
    auto expected = replay(operations, comp);
    EXPECT_EQ(entries, expected.entries);
    ASSERT_EQ(queue.size(), int(expected.entries.size()));
    EXPECT_EQ(queue.empty(), expected.entries.empty());
    EXPECT_EQ(queue.failed_pops(), expected.failed_pops);
    if (expected.minimum) {
        EXPECT_EQ(queue.min(), *expected.minimum);
    }
}

struct Direction {
    bool descending;
    bool operator()(int a, int b) const { return descending ? a > b : a < b; }
};

}  // namespace

TEST(PartiallyRetroactivePriorityQueueTest, EmptyTimeline) {
    const PartiallyRetroactivePriorityQueue<int> queue(0);
    EXPECT_TRUE(queue.empty());
    EXPECT_EQ(queue.size(), 0);
    EXPECT_EQ(queue.failed_pops(), 0);
}

TEST(PartiallyRetroactivePriorityQueueTest, Usage) {
    PartiallyRetroactivePriorityQueue<int> queue(4);
    auto change = queue.set_push(2, 5);
    ASSERT_EQ(change.added.size(), 1);
    EXPECT_EQ(change.added[0].time, 2);
    EXPECT_EQ(change.added[0].value, 5);
    EXPECT_EQ(queue.min(), 5);

    change = queue.set_pop(1);
    EXPECT_TRUE(change.added.empty());
    EXPECT_TRUE(change.removed.empty());
    EXPECT_EQ(queue.failed_pops(), 1);
    EXPECT_EQ(queue.size(), 1);

    change = queue.set_push(0, 3);
    EXPECT_TRUE(change.added.empty());
    EXPECT_TRUE(change.removed.empty());
    EXPECT_EQ(queue.failed_pops(), 0);
    EXPECT_EQ(queue.min(), 5);

    change = queue.clear(1);
    ASSERT_EQ(change.added.size(), 1);
    EXPECT_EQ(change.added[0].time, 0);
    EXPECT_EQ(change.added[0].value, 3);
    EXPECT_EQ(queue.size(), 2);
    EXPECT_EQ(queue.min(), 3);
}

TEST(PartiallyRetroactivePriorityQueueTest, ReplacePushWithPop) {
    PartiallyRetroactivePriorityQueue<int> queue(2);
    queue.set_push(0, 5);
    queue.set_push(1, 3);
    auto change = queue.set_pop(1);
    EXPECT_TRUE(change.added.empty());
    ASSERT_EQ(change.removed.size(), 2);
    std::map<int, int> removed;
    for (auto entry : change.removed) removed[entry.time] = entry.value;
    EXPECT_EQ(removed, (std::map<int, int>{{0, 5}, {1, 3}}));
    EXPECT_TRUE(queue.empty());

    change = queue.set_push(1, 3);
    EXPECT_TRUE(change.removed.empty());
    ASSERT_EQ(change.added.size(), 2);
    std::map<int, int> added;
    for (auto entry : change.added) added[entry.time] = entry.value;
    EXPECT_EQ(added, removed);
    EXPECT_EQ(queue.size(), 2);
    EXPECT_EQ(queue.min(), 3);
}

TEST(PartiallyRetroactivePriorityQueueTest, CancelIntermediateChanges) {
    PartiallyRetroactivePriorityQueue<int> queue(3);
    queue.set_push(0, 3);
    queue.set_push(1, 5);
    queue.set_pop(2);
    auto change = queue.set_push(0, 4);
    EXPECT_TRUE(change.added.empty());
    EXPECT_TRUE(change.removed.empty());
    EXPECT_EQ(queue.min(), 5);

    change = queue.set_pop(2);
    EXPECT_TRUE(change.added.empty());
    EXPECT_TRUE(change.removed.empty());
    EXPECT_EQ(queue.min(), 5);
}

TEST(PartiallyRetroactivePriorityQueueTest, EqualPriorities) {
    PartiallyRetroactivePriorityQueue<int> queue(3);
    queue.set_push(1, 7);
    queue.set_push(0, 7);
    auto change = queue.set_pop(2);
    ASSERT_EQ(change.removed.size(), 1);
    EXPECT_EQ(change.removed[0].time, 0);
    EXPECT_EQ(queue.size(), 1);
    EXPECT_EQ(queue.min(), 7);

    change = queue.clear(0);
    ASSERT_EQ(change.removed.size(), 1);
    EXPECT_EQ(change.removed[0].time, 1);
    EXPECT_TRUE(queue.empty());
    EXPECT_EQ(queue.failed_pops(), 0);

    change = queue.clear(1);
    EXPECT_TRUE(change.removed.empty());
    EXPECT_EQ(queue.failed_pops(), 1);
}

TEST(PartiallyRetroactivePriorityQueueTest, FailedPopsRecovery) {
    constexpr int n = 1000;
    PartiallyRetroactivePriorityQueue<int> queue(2 * n);
    for (int t = 0; t < n; t++) queue.set_pop(t);
    for (int t = 0; t < n; t++) queue.set_push(n + t, t);
    EXPECT_EQ(queue.failed_pops(), n);
    EXPECT_EQ(queue.size(), n);

    for (int t = 0; t < n; t++) {
        auto change = queue.clear(t);
        EXPECT_TRUE(change.added.empty());
        EXPECT_TRUE(change.removed.empty());
        EXPECT_EQ(queue.failed_pops(), n - t - 1);
        EXPECT_EQ(queue.size(), n);
        EXPECT_EQ(queue.min(), 0);
    }
}

TEST(PartiallyRetroactivePriorityQueueTest, NumericLimits) {
    PartiallyRetroactivePriorityQueue<int> queue(4);
    queue.set_pop(0);
    queue.set_push(1, std::numeric_limits<int>::max());
    queue.set_push(2, std::numeric_limits<int>::min());
    EXPECT_EQ(queue.min(), std::numeric_limits<int>::min());
    queue.set_pop(3);
    EXPECT_EQ(queue.min(), std::numeric_limits<int>::max());
    EXPECT_EQ(queue.failed_pops(), 1);
}

TEST(PartiallyRetroactivePriorityQueueTest, NonDefaultConstructibleValues) {
    struct Key {
        explicit Key(std::string s) : text(std::move(s)) {}
        std::string text;
    };
    auto comp = [](const Key& a, const Key& b) { return a.text < b.text; };
    PartiallyRetroactivePriorityQueue<Key, decltype(comp)> queue(4, comp);
    queue.set_pop(1);
    queue.set_push(0, Key("apple"));
    queue.set_push(2, Key("banana"));
    EXPECT_EQ(queue.failed_pops(), 0);
    EXPECT_EQ(queue.min().text, "banana");
    auto change = queue.set_push(0, Key("cherry"));
    EXPECT_TRUE(change.added.empty());
    EXPECT_TRUE(change.removed.empty());
    queue.set_push(2, queue.min());
    EXPECT_EQ(queue.min().text, "banana");
    queue.clear(1);
    EXPECT_EQ(queue.min().text, "banana");
    queue.set_pop(3);
    EXPECT_EQ(queue.min().text, "cherry");
}

TEST(PartiallyRetroactivePriorityQueueTest, ExhaustiveReplacements) {
    const std::vector<Operation> choices = {{0, 0}, {2, 0}, {1, 0}, {1, 1}};
    constexpr int n = 4;
    for (int mask = 0; mask < 256; mask++) {
        SCOPED_TRACE(mask);
        PartiallyRetroactivePriorityQueue<int> original(n);
        std::vector<Operation> operations(n);
        for (int t = 0; t < n; t++) {
            operations[t] = choices[(mask >> (2 * t)) & 3];
            apply(original, t, operations[t]);
        }
        auto original_entries = replay(operations, std::less<int>()).entries;
        for (int t = 0; t < n; t++) {
            for (const auto& operation : choices) {
                SCOPED_TRACE(t);
                SCOPED_TRACE(operation.type);
                SCOPED_TRACE(operation.value);
                auto queue = original;
                auto edited = operations;
                edited[t] = operation;
                auto entries = original_entries;
                auto change = apply(queue, t, operation);
                check_change(queue, change, entries, edited, std::less<int>());
            }
        }
    }
}

TEST(PartiallyRetroactivePriorityQueueTest, RandomUpdates) {
    std::mt19937 random(193);
    for (bool descending : {false, true}) {
        Direction comp{descending};
        for (int n : {1, 2, 3, 7, 16, 31, 64, 1025}) {
            SCOPED_TRACE(n);
            SCOPED_TRACE(descending);
            PartiallyRetroactivePriorityQueue<int, Direction> queue(n, comp);
            std::vector<Operation> operations(n);
            std::map<int, int> entries;
            for (int iteration = 0; iteration < 3000; iteration++) {
                SCOPED_TRACE(iteration);
                int t = int(random() % unsigned(n));
                operations[t] =
                    Operation{int(random() % 3), int(random() % 21) - 10};
                auto change = apply(queue, t, operations[t]);
                check_change(queue, change, entries, operations, comp);
            }
        }
    }
}
