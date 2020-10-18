#pragma once

#include <unistd.h>
#include <x86intrin.h>
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
        if (!skip_space()) return false;
        ref = "";
        while (true) {
            char c = top();
            if (c < '0') break;
            ref += c;
            st++;
        }
        return true;
    }

    template <class T, internal::is_signed_int_t<T>* = nullptr>
    bool read_single(T& sref) {
        using U = internal::to_unsigned_t<T>;
        if (!skip_space(50)) return false;
        bool neg = false;
        if (line[st] == '-') {
            neg = true;
            st++;
        }
        U ref = 0;
        do {
            ref = 10 * ref + (line[st++] & 0x0f);
        } while (line[st] >= '0');
        sref = neg ? -ref : ref;
        return true;
    }
    template <class U, internal::is_unsigned_int_t<U>* = nullptr>
    bool read_single(U& ref) {
        if (!skip_space(50)) return false;
        ref = 0;
        do {
            ref = 10 * ref + (line[st++] & 0x0f);
        } while (line[st] >= '0');
        return true;
    }

    int fd = -1;
    char line[SIZE];
    size_t st = 0, ed = 0;
    bool eof = false;
    bool reread() {
        if (ed - st >= 50) return true;
        if (st > SIZE / 2) {
            std::memmove(line, line + st, ed - st);
            ed -= st;
            st = 0;
        }
        if (eof) return false;
        auto u = ::read(fd, line + ed, SIZE - ed);
        if (u == 0) {
            eof = true;
            line[ed] = '\0';
            u = 1;
        }
        ed += u;
        return true;
    }

    char top() {
        if (st == ed) {
            bool f = reread();
            assert(f);
        }
        return line[st];
    }

    bool skip_space(int token_len = 0) {
        while (true) {
            while (st != ed && line[st] <= ' ') st++;
            if (ed - st > token_len) return true;
            for (int i = st; i < ed; i++) {
                if (line[i] <= ' ') return true;
            }
            if (!reread()) return false;
        }
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
    static std::array<std::array<char, 2>, 100> small;
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

        char* ptr = line + pos;
        while (uval >= 100) {
            ptr -= 2;
            memcpy(ptr, small[uval % 100].data(), 2);
            uval /= 100;
        }
        if (uval >= 10) {
            memcpy(ptr - 2, small[uval].data(), 2);
        } else {
            *(ptr - 1) = char('0' + uval);
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
std::array<std::array<char, 2>, 100> Printer::small = [] {
    std::array<std::array<char, 2>, 100> table;
    for (int i = 0; i <= 99; i++) {
        table[i][1] = char('0' + (i % 10));
        table[i][0] = char('0' + (i / 10 % 10));
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
