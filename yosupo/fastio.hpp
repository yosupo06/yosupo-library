#pragma once

#include <unistd.h>
#include <cctype>
#include <cstring>
#include <string>
#include <type_traits>
#include <vector>

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
    template <class T, internal::is_integral_t<T>* = nullptr>
    bool read_single(T& ref) {
        using U = internal::to_unsigned_t<T>;
        if (!succ()) return false;
        bool neg = false;
        if (line[st] == '-') {
            neg = true;
            st++;
        }
        U res = U(0);
        while (isdigit(line[st])) {
            res = 10 * res + (line[st++] & 0xf);
        }
        if (neg) res = -res;
        ref = res;
        return true;
    }

    int fd = -1;
    char line[(1 << 15) + 1];
    size_t st = 0, ed = 0;
    void reread() {
        std::memmove(line, line + st, ed - st);
        ed -= st;
        st = 0;
        ed += ::read(fd, line + ed, (1 << 15) - ed);
        line[ed] = '\0';
    }
    bool succ() {
        while (true) {
            if (st == ed) {
                reread();
                if (st == ed) return false;
            }
            while (st != ed && std::isspace(line[st])) st++;
            if (st != ed) break;
        }
        if (ed - st <= 50) {
            bool sep = false;
            for (size_t i = st; i < ed; i++) {
                if (isspace(line[i])) {
                    sep = true;
                    break;
                }
            }
            if (!sep) reread();
        }
        return true;
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

    Printer(FILE* _fp) : fp(_fp) {}
    ~Printer() {
        flush();
    }
    void flush() {
        fwrite(line, 1, pos, fp);
        pos = 0;
    }

  private:
    static constexpr size_t SIZE = 1 << 15;
    FILE* fp;
    char line[SIZE], small[50];
    size_t pos = 0;
    void write_single(const char& val) {
        if (pos == SIZE) flush();
        line[pos++] = val;
    }
    template <class T, internal::is_integral_t<T>* = nullptr>
    void write_single(T val) {
        using U = internal::to_unsigned_t<T>;
        if (pos > SIZE - 50) flush();
        if (val == 0) {
            write_single('0');
            return;
        }
        U uval = val;
        if (val < 0) {
            write_single('-');
            uval = -uval;  // todo min
        }
        size_t len = 0;
        while (uval) {
            small[len++] = char(0x30 | (uval % 10));
            uval /= 10;
        }
        for (size_t i = 0; i < len; i++) {
            line[pos + i] = small[len - 1 - i];
        }
        pos += len;
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

}  // namespace yosupo
