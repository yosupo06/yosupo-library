#include "yosupo/algebra.hpp"

using namespace yosupo;

static_assert(monoid<Max<int>>);
static_assert(monoid<Min<int>>);
