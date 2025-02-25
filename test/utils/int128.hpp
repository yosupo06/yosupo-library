#pragma once

#include <algorithm>
#include <iostream>

inline std::ostream& operator<<(std::ostream& os, __int128_t x) {
    if (x < 0) {
        os << "-";
        x *= -1;
    }
    if (x == 0) {
        return os << "0";
    }
    std::string s;
    while (x) {
        s += char(x % 10 + '0');
        x /= 10;
    }
    std::ranges::reverse(s);
    return os << s;
}
