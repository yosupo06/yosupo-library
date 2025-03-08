#pragma once

#include <algorithm>
#include <array>
#include <concepts>
#include <string>
#include <tuple>
#include <vector>

namespace yosupo {

template <class T> std::string dump(const T&);

template <> inline std::string dump(const std::string& t) { return t; }

template <class T>
    requires requires(const T& t) { t.val(); }
std::string dump(const T& t) {
    return dump(t.val());
}

template <std::integral T> std::string dump(const T& t) {
    return std::to_string(t);
}
template <class T>
    requires std::same_as<T, unsigned __int128>
std::string dump(const T& t) {
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
std::string dump(const T& t) {
    if (t < 0) {
        return "-" + dump((unsigned __int128)(-t));
    } else {
        return dump((unsigned __int128)(t));
    }
}

template <class T, std::size_t N> std::string dump(const std::array<T, N>&);
template <class T> std::string dump(const std::vector<T>&);
template <class T1, class T2> std::string dump(const std::pair<T1, T2>&);
template <class... Ts> std::string dump(const std::tuple<Ts...>& t);

template <class T, std::size_t N> std::string dump(const std::array<T, N>& a) {
    std::string s = "[";
    for (size_t i = 0; i < N; i++) {
        if (i) {
            s += ", ";
        }
        s += dump(a[i]);
    }
    s += "]";
    return s;
}

template <class T> std::string dump(const std::vector<T>& v) {
    std::string s = "[";
    for (std::size_t i = 0; i < v.size(); ++i) {
        s += dump(v[i]);
        if (i + 1 != v.size()) {
            s += ", ";
        }
    }
    s += "]";
    return s;
}

template <class T1, class T2> std::string dump(const std::pair<T1, T2>& p) {
    std::string s = "(";
    s += dump(p.first);
    s += ", ";
    s += dump(p.second);
    s += ")";
    return s;
}

template <class... Ts> std::string dump(const std::tuple<Ts...>& t) {
    std::string s = "(";
    [&]<std::size_t... I>(std::index_sequence<I...>) {
        ((s += dump(std::get<I>(t)) + ((I < sizeof...(Ts) - 1) ? ", " : "")),
         ...);
    }(std::make_index_sequence<sizeof...(Ts)>());
    s += ")";
    return s;
}

}  // namespace yosupo
