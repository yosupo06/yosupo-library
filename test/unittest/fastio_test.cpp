#include "yosupo/fastio.hpp"

#include "yosupo/random.hpp"

#include <string>
#include <cstdint>

#include "gtest/gtest.h"

using namespace yosupo;

TEST(FastIOTest, ScannerSeparator) {
    auto tmpf = tmpfile();
    fputs(std::string(1 << 15, 'a').c_str(), tmpf);
    fputs(" ", tmpf);
    fputs(std::string(1 << 15, 'b').c_str(), tmpf);
    rewind(tmpf);
    Scanner sc(tmpf);
    std::string s, t;
    sc.read(s, t);
    ASSERT_EQ(std::string(1 << 15, 'a'), s);
    ASSERT_EQ(std::string(1 << 15, 'b'), t);
}

TEST(FastIOTest, ScannerInteractive) {
    int pipefd[2];
    assert(pipe(pipefd) == 0);

    Scanner sc(fdopen(pipefd[0], "r"));
    FILE* fw = fdopen(pipefd[1], "w");
    fprintf(fw, "1234\n");
    fflush(fw);
    int x;
    sc.read(x);
    ASSERT_EQ(1234, x);
}

TEST(FastIOTest, ScannerInt128) {
    auto tmpf = tmpfile();
    fputs("1234567890123456", tmpf);
    rewind(tmpf);
    Scanner sc(tmpf);
    __int128 x;
    sc.read(x);
    ASSERT_EQ(1234567890123456LL, x);
}

TEST(FastIOTest, ScannerIntMin) {
    {
        auto tmpf = tmpfile();
        fputs("-2147483648", tmpf);
        rewind(tmpf);
        Scanner sc(tmpf);
        int32_t x;
        sc.read(x);
        ASSERT_EQ(std::numeric_limits<int32_t>::min(), x);
    }
    {
        auto tmpf = tmpfile();
        fputs("-9223372036854775808", tmpf);
        rewind(tmpf);
        Scanner sc(tmpf);
        int64_t x;
        sc.read(x);
        ASSERT_EQ(std::numeric_limits<int64_t>::min(), x);
    }
}

TEST(FastIOTest, PrinterIntMin) {
    {
        auto tmpf = tmpfile();
        Printer pr(tmpf);
        pr.writeln(std::numeric_limits<int32_t>::min());
        pr.flush();

        rewind(tmpf);
        char buf[100];
        fgets(buf, 100, tmpf);
        ASSERT_EQ("-2147483648\n", std::string(buf));
    }
    {
        auto tmpf = tmpfile();
        Printer pr(tmpf);
        pr.writeln(std::numeric_limits<int64_t>::min());
        pr.flush();

        rewind(tmpf);
        char buf[100];
        fgets(buf, 100, tmpf);
        ASSERT_EQ("-9223372036854775808\n", std::string(buf));
    }
}

TEST(FastIOTest, PrinterSmallInt) {
    auto tmpf = tmpfile();
    Printer pr(tmpf);
    for (int i = 0; i <= 123456; i++) {
        pr.writeln(i);
    }
    pr.flush();

    rewind(tmpf);
    char buf[100];
    for (int i = 0; i <= 123456; i++) {
        fgets(buf, 100, tmpf);
        ASSERT_EQ(std::to_string(i) + "\n", std::string(buf));
    }
}

TEST(FastIOTest, PrinterInt) {
    auto tmpf = tmpfile();
    std::vector<int> v(10000);
    for (int i = 0; i < 10000; i++) {
        v[i] = uniform(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
    }

    Printer pr(tmpf);
    for (int i = 0; i < 10000; i++) {
        pr.writeln(v[i]);
    }
    pr.flush();

    rewind(tmpf);
    char buf[100];
    for (int i = 0; i < 10000; i++) {
        fgets(buf, 100, tmpf);
        ASSERT_EQ(std::to_string(v[i]) + "\n", std::string(buf));
    }
}
