#include "yosupo/fastio.hpp"

#include "yosupo/random.hpp"

#include <cstdint>
#include <string>

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

    fprintf(fw, "5678 90\n");
    fflush(fw);
    int y;
    sc.read(x, y);
    ASSERT_EQ(5678, x);
    ASSERT_EQ(90, y);

    close(pipefd[0]);
    close(pipefd[1]);
}

TEST(FastIOTest, PrinterInteractive) {
    int pipefd[2];
    assert(pipe(pipefd) == 0);

    FILE* fr = fdopen(pipefd[0], "r");
    Printer pr(fdopen(pipefd[1], "w"));
    pr.writeln("1234");
    pr.flush();
    int x;
    fscanf(fr, "%d", &x);
    ASSERT_EQ(1234, x);

    close(pipefd[0]);
    close(pipefd[1]);
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

TEST(FastIOTest, ScannerInt) {
    auto tmpf = tmpfile();
    std::vector<int> v;
    for (int i = 0; i <= 123456; i++) {
        v.push_back(i);
    }
    for (int i = 0; i < 10000; i++) {
        v.push_back(uniform(std::numeric_limits<int>::min(),
                            std::numeric_limits<int>::max()));
    }

    for (auto x : v) {
        fputs((std::to_string(x) + "\n").c_str(), tmpf);
    }
    rewind(tmpf);

    Scanner sc(tmpf);
    int y;
    for (auto x : v) {
        sc.read(y);
        ASSERT_EQ(x, y);
    }
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

TEST(FastIOTest, PrinterInt) {
    auto tmpf = tmpfile();
    std::vector<int> v;
    for (int i = 0; i <= 123456; i++) {
        v.push_back(i);
    }
    for (int i = 0; i < 10000; i++) {
        v.push_back(uniform(std::numeric_limits<int>::min(),
                            std::numeric_limits<int>::max()));
    }

    Printer pr(tmpf);
    for (auto x : v) {
        pr.writeln(x);
    }
    pr.flush();

    rewind(tmpf);
    char buf[100];
    for (auto x : v) {
        fgets(buf, 100, tmpf);
        ASSERT_EQ(std::to_string(x) + "\n", std::string(buf));
    }
}

TEST(FastIOTest, PrinterLongLong) {
    auto tmpf = tmpfile();
    std::vector<long long> v;
    for (int i = 0; i < 10000; i++) {
        v.push_back(uniform(std::numeric_limits<long long>::min(),
                            std::numeric_limits<long long>::max()));
    }

    Printer pr(tmpf);
    for (auto x : v) {
        pr.writeln(x);
    }
    pr.flush();

    rewind(tmpf);
    char buf[100];
    for (auto x : v) {
        fgets(buf, 100, tmpf);
        ASSERT_EQ(std::to_string(x) + "\n", std::string(buf));
    }
}

TEST(FastIOTest, PrinterLongLong2) {
    auto tmpf = tmpfile();
    std::vector<long long> v;
    for (int i = 0; i < 18; i++) {
        long long x = 1;
        for (int j = 0; j < i; j++) {
            x *= 10;
        }
        for (int j = -10; j < 10; j++) {
            v.push_back(x + j);
        }
    }

    Printer pr(tmpf);
    for (auto x : v) {
        pr.writeln(x);
    }
    pr.flush();

    rewind(tmpf);
    char buf[100];
    for (auto x : v) {
        fgets(buf, 100, tmpf);
        ASSERT_EQ(std::to_string(x) + "\n", std::string(buf));
    }
}

TEST(FastIOTest, PrinterUnsignedLongLong) {
    auto tmpf = tmpfile();
    std::vector<unsigned long long> v;
    for (int i = 0; i < 1000; i++) {
        v.push_back(
            uniform(0ULL, std::numeric_limits<unsigned long long>::max()));
    }
    for (int i = 0; i < 100; i++) {
        v.push_back(i);
        v.push_back(std::numeric_limits<unsigned long long>::max() - i);
    }
    for (int i = 1; i <= 19; i++) {
        unsigned long long x = 1;
        for (int j = 0; j < i; j++) {
            x *= 10;
        }
        for (int j = -10; j < 10; j++) {
            v.push_back(x + j);
        }
    }

    Printer pr(tmpf);
    for (auto x : v) {
        pr.writeln(x);
    }
    pr.flush();

    rewind(tmpf);
    char buf[100];
    for (auto x : v) {
        fgets(buf, 100, tmpf);
        ASSERT_EQ(std::to_string(x) + "\n", std::string(buf));
    }
}

TEST(FastIOTest, ScannerPreSpace) {
    auto tmpf = tmpfile();
    fputs(" 1234", tmpf);
    rewind(tmpf);
    Scanner sc(tmpf);
    int x;
    sc.read(x);
    ASSERT_EQ(1234, x);
}
