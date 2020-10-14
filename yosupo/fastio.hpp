#pragma once

#include <unistd.h>
#include <algorithm>
#include <array>
#include <cctype>
#include <cstring>
#include <string>
#include <type_traits>
#include <vector>

#include "yosupo/bit.hpp"
#include "yosupo/internal_type_traits.hpp"

namespace yosupo {

struct Scanner {
  public:
    Scanner(FILE* fp) : fd(fileno(fp)) {}

    void read() {}
    template <class H, class... T> void read(H& h, T&... t) {
        bool f = read_single(h);
        assert(f);
        read(t...);
    }

    int read_unsafe() { return 0; }
    template <class H, class... T> int read_unsafe(H& h, T&... t) {
        bool f = read_single(h);
        if (!f) return 0;
        return 1 + read_unsafe(t...);
    }

  private:
    static constexpr size_t SIZE = 1 << 15;
    template <class T,
              std::enable_if_t<std::is_same<T, std::string>::value>* = nullptr>
    bool read_single(T& ref) {
        if (!succ()) return false;
        while (true) {
            size_t sz = 0;
            while (st + sz < ed && !isspace(line[st + sz])) sz++;
            ref.append(line + st, sz);
            st += sz;
            if (!sz || st != ed) break;
            reread();
        }
        return true;
    }
    template <class T, internal::is_signed_int_t<T>* = nullptr>
    bool read_single(T& ref) {
        using U = internal::to_unsigned_t<T>;
        if (!succ()) return false;
        bool neg = false;
        if (line[st] == '-') {
            neg = true;
            st++;
        }        
        U x;
        read_uint(x);
        ref = neg ? -x : x;
        return true;
    }
    template <class U, internal::is_unsigned_int_t<U>* = nullptr>
    bool read_single(U& ref) {
        if (!succ()) return false;
        read_uint(ref);
        return true;
    }

    template <class U, internal::is_unsigned_int_t<U>* = nullptr>
    void read_uint(U& ref) {
        ref = 0;
        while (line[st] >= '0') {
            ref = 10 * ref + (line[st++] & 0x0f);
        }
    }

    int fd = -1;
    char line[SIZE + 1];
    size_t st = 0, ed = 0;
    void reread() {
        std::memmove(line, line + st, ed - st);
        ed -= st;
        st = 0;
        ed += ::read(fd, line + ed, SIZE - ed);
        line[ed] = '\0';
    }
    bool succ() {
        while (st < ed && line[st] <= ' ') st++;
        if (st == ed || SIZE - 50 <= st) reread();
        return st != ed;
    }
};

struct Printer {
  public:
    template <bool F = false> void write() {}
    template <bool F = false, class H, class... T>
    void write(const H& h, const T&... t) {
        if (F) write_single(' ');
        write_single(h);
        write<true>(t...);
    }
    template <class... T> void writeln(const T&... t) {
        write(t...);
        write_single('\n');
    }

    Printer(FILE* _fp) : fd(fileno(_fp)) {}
    ~Printer() { flush(); }

    void flush() {
        ::write(fd, line, pos);
        pos = 0;
    }

  private:
    static std::array<std::array<char, 4>, 10000> small;
    static std::array<unsigned long long, 20> tens;

    static int calc_len(unsigned long long x) {
        int i = (bsr(x) * 3 + 3) / 10;
        if (x < tens[i])
            return i;
        else
            return i + 1;
    }

    static constexpr size_t SIZE = 1 << 15;
    int fd;
    char line[SIZE];
    size_t pos = 0;
    void write_single(const char& val) {
        if (pos == SIZE) flush();
        line[pos++] = val;
    }

    template <class T, internal::is_signed_int_t<T>* = nullptr>
    void write_single(const T& val) {
        using U = internal::to_unsigned_t<T>;
        if (val == 0) {
            write_single('0');
            return;
        }
        if (pos > SIZE - 50) flush();
        U uval = val;
        if (val < 0) {
            write_single('-');
            uval = -uval;
        }
        write_unsigned(uval);
    }

    template <class U, internal::is_unsigned_int_t<U>* = nullptr>
    void write_single(U uval) {
        if (uval == 0) {
            write_single('0');
            return;
        }
        if (pos > SIZE - 50) flush();

        write_unsigned(uval);
    }

    template <class U,
              internal::is_unsigned_int_t<U>* = nullptr,
              std::enable_if_t<sizeof(U) == 4>* = nullptr>
    void write_unsigned(U uval) {
        write_unsigned(uint64_t(uval));
    }

    template <class U,
              internal::is_unsigned_int_t<U>* = nullptr,
              std::enable_if_t<sizeof(U) == 8>* = nullptr>
    void write_unsigned(U uval) {
        size_t len = calc_len(uval);
        pos += len;
        if (len >= 17) {
            uint64_t v0 = uval % 10000;
            uval /= 10000;
            uint64_t v1 = uval % 10000;
            uval /= 10000;
            uint64_t v2 = uval % 10000;
            uval /= 10000;
            uint64_t v3 = uval % 10000;
            uval /= 10000;

            memcpy(line + pos - len, small[uval].data() + (20 - len), len - 16);
            memcpy(line + pos - 4, small[v0].data(), 4);
            memcpy(line + pos - 8, small[v1].data(), 4);
            memcpy(line + pos - 12, small[v2].data(), 4);
            memcpy(line + pos - 16, small[v3].data(), 4);
        } else {
            char* ptr = line + pos;
            size_t rem_len = len;
            while (uval >= 10000) {
                ptr -= 4;
                rem_len -= 4;
                memcpy(ptr, small[uval % 10000].data(), 4);
                uval /= 10000;
            }
            memcpy(ptr - rem_len, small[uval].data() + (4 - rem_len), rem_len);
        }
    }

    void write_single(const std::string& s) {
        for (char c : s) write_single(c);
    }
    void write_single(const char* s) {
        size_t len = strlen(s);
        for (size_t i = 0; i < len; i++) write_single(s[i]);
    }
    template <class T> void write_single(const std::vector<T>& val) {
        auto n = val.size();
        for (size_t i = 0; i < n; i++) {
            if (i) write_single(' ');
            write_single(val[i]);
        }
    }
};
std::array<std::array<char, 4>, 10000> Printer::small = [] {
    std::array<std::array<char, 4>, 10000> table;
    for (int i = 0; i <= 9999; i++) {
        int z = i;
        for (int j = 0; j < 4; j++) {
            table[i][3 - j] = char('0' + (z % 10));
            z /= 10;
        }
    }
    return table;
}();
std::array<unsigned long long, 20> Printer::tens = [] {
    std::array<unsigned long long, 20> table;
    for (int i = 0; i < 20; i++) {
        table[i] = 1;
        for (int j = 0; j < i; j++) {
            table[i] *= 10;
        }
    }
    return table;
}();

}  // namespace yosupo
