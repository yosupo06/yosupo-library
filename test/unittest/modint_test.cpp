#include "yosupo/modint.hpp"

#include <algorithm>
#include <iostream>

#include "gtest/gtest.h"

TEST(ModIntTest, Ostream) {
    yosupo::modint998244353 a = 2;
    std::cout << a << std::endl;
    yosupo::modint::set_mod(100);
    yosupo::modint b = 3;
    std::cout << b << std::endl;
}
