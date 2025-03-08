#pragma once

#include <algorithm>
#include <array>
#include <concepts>
#include <iomanip>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

namespace yosupo {

template <class T> std::string to_debug_string(const T& t);

template <std::integral T> std::string to_debug_string(const T& t) {
    return std::to_string(t);
}

template <> std::string to_debug_string(const std::string& t) { return t; }

template <> std::string to_debug_string(const double& t) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(15) << t;
    std::string s = ss.str();
    if (s.find('.') != std::string::npos) {
        while (s.back() == '0') {
            s.pop_back();
        }
        if (s.back() == '.') {
            s.pop_back();
        }
    }
    return s;
}

template <class T> std::string to_debug_string(const T&) {
    return "unimplemented";
}

template <class T>
    requires std::same_as<T, unsigned __int128>
std::string to_debug_string(const T& t) {
    if (t == 0) {
        return "0";
    }

    std::string s;
    T x = t;
    while (x) {
        s += char(x % 10 + '0');
        x /= 10;
    }
    std::ranges::reverse(s);
    return s;
}

template <class T>
    requires std::same_as<T, __int128>
std::string to_debug_string(const T& t) {
    if (t < 0) {
        return "-" + to_debug_string((unsigned __int128)(-t));
    } else {
        return to_debug_string((unsigned __int128)(t));
    }
}

template <class T, std::size_t N>
std::string to_debug_string(const std::array<T, N>& a) {
    std::string s = "[";
    for (std::size_t i = 0; i < N; ++i) {
        s += to_debug_string(a[i]);
        if (i + 1 != N) {
            s += ", ";
        }
    }
    s += "]";
    return s;
}

template <class T> std::string to_debug_string(const std::vector<T>& v) {
    std::string s = "[";
    for (std::size_t i = 0; i < v.size(); ++i) {
        s += to_debug_string(v[i]);
        if (i + 1 != v.size()) {
            s += ", ";
        }
    }
    s += "]";
    return s;
}

template <class T1, class T2>
std::string to_debug_string(const std::pair<T1, T2>& p) {
    std::string s = "(";
    s += to_debug_string(p.first);
    s += ", ";
    s += to_debug_string(p.second);
    s += ")";
    return s;
}

template <class... Ts> std::string to_debug_string(const std::tuple<Ts...>& t) {
    std::string s = "(";
    [&]<std::size_t... I>(std::index_sequence<I...>) {
        ((s += to_debug_string(std::get<I>(t)) +
               ((I < sizeof...(Ts) - 1) ? ", " : "")),
         ...);
    }(std::make_index_sequence<sizeof...(Ts)>());
    s += ")";
    return s;
}

}  // namespace yosupo
