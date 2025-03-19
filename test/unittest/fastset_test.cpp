#include "yosupo/fastset.hpp"
#include <iterator>
#include <limits>
#include <set>
#include "yosupo/random.hpp"

#include "gtest/gtest.h"

using namespace yosupo;
using ll = long long;
using ull = unsigned long long;

TEST(FastSetTest, Usage) {
    FastSet f(1);
    ASSERT_FALSE(f[0]);
    f.set(0);
    ASSERT_TRUE(f[0]);
    f.reset(0);
    ASSERT_FALSE(f[0]);
    ASSERT_EQ(f.size(), 1);
}

TEST(FastSetTest, Constructor) {
    bool a[] = {true, false, true};
    FastSet f(3, [&](int i) { return a[i]; });
    ASSERT_TRUE(f[0]);
    ASSERT_FALSE(f[1]);
    ASSERT_TRUE(f[2]);
}

TEST(FastSetTest, Zero) { FastSet f(0); }

TEST(FastSetTest, Stress) {
    for (int n = 1; n <= 200; n++) {
        FastSet f(n);
        std::set<int> st;
        st.insert(-1);
        st.insert(n);
        for (int ph = 0; ph <= 1000; ph++) {
            int ty = uniform(0, 2);
            int p = uniform(0, n - 1);

            ASSERT_EQ(st.count(p), f[p]);

            if (ty == 0) {
                f.set(p);
                st.insert(p);
            } else if (ty == 1) {
                f.reset(p);
                if (st.count(p)) st.erase(p);
            } else if (ty) {
                auto it = st.lower_bound(p);
                int or_more = *it;
                int more = (*it != p) ? or_more : *next(it);
                int less = *prev(it);
                int or_less = (*it == p) ? p : less;
                ASSERT_EQ(or_more, f.or_more(p));
                ASSERT_EQ(more, f.more(p));
                ASSERT_EQ(or_less, f.or_less(p));
                ASSERT_EQ(less, f.less(p));
            }
        }
    }
}

TEST(FastSetTest, OutOfRange) {
    FastSet f(10);
    f.set(0);
    f.set(9);

    ASSERT_EQ(10, f.or_more(1000));
    ASSERT_EQ(10, f.or_more(11));
    ASSERT_EQ(10, f.or_more(10));
    ASSERT_EQ(10, f.more(9));
    ASSERT_EQ(10, f.or_more(std::numeric_limits<int>::max()));
    ASSERT_EQ(10, f.more(std::numeric_limits<int>::max()));
    ASSERT_EQ(0, f.or_more(std::numeric_limits<int>::min()));
    ASSERT_EQ(0, f.more(std::numeric_limits<int>::min()));

    ASSERT_EQ(-1, f.or_less(-1000));
    ASSERT_EQ(-1, f.or_less(-2));
    ASSERT_EQ(-1, f.or_less(-1));
    ASSERT_EQ(-1, f.less(0));
    ASSERT_EQ(-1, f.or_less(std::numeric_limits<int>::min()));
    ASSERT_EQ(-1, f.less(std::numeric_limits<int>::min()));
    ASSERT_EQ(9, f.or_less(std::numeric_limits<int>::max()));
    ASSERT_EQ(9, f.less(std::numeric_limits<int>::max()));
}
