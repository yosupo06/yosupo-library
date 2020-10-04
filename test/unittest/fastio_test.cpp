#include "yosupo/fastio.hpp"
#include <string>

#include "gtest/gtest.h"

using namespace yosupo;

TEST(FastIOTest, Separator) {
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

TEST(FastIOTest, Interactive) {
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
