#pragma once

#include <vector>
#include <algorithm>
#include <bit>

namespace yosupo {

struct BitVec {
  public:
    BitVec() : BitVec(0) {}
    explicit BitVec(size_t _n) : n(_n), d((n + B - 1) / B) {}
    size_t size() const { return n; }

    void reset() { fill(d.begin(), d.end(), 0); }
    void reset(size_t i) { set(i, false); }
    BitVec& set() {
        for (auto& x : d) {
            x = -1ULL;
        }
        erase_last();
        return *this;
    }
    BitVec& set(size_t i, bool f = true) {
        assert(i < n);
        if (f) {
            d[i / B] |= 1ULL << (i % B);
        } else {
            d[i / B] &= ~(1ULL << (i % B));
        }
        return *this;
    }
    bool test(size_t i) const {
        assert(i < n);
        return (d[i / B] >> (i % B) & 1) != 0;
    }

    size_t count() const {
        size_t sm = 0;
        for (auto x : d) sm += std::popcount(x);
        return sm;
    }
    bool any() const {
        for (auto& x : d)
            if (x) return true;
        return false;
    }
    int find_first() const {
        auto m = d.size();
        for (size_t i = 0; i < m; i++) {
            if (d[i]) return int(i * B + std::countr_zero(d[i]));
        }
        return int(size());
    }

    BitVec& flip() {
        op1(std::bit_not<unsigned long long>());
        if (n % B) d.back() &= ~(-1ULL << (n % B));
        return *this;
    }
    BitVec& flip(size_t i) {
        return set(i, !test(i));
    }

    // operators
    BitVec& operator~() const { return BitVec(*this).flip(); }
    BitVec& operator&=(const BitVec& r) {
        return op2(r, std::bit_and<unsigned long long>());
    }
    BitVec& operator|=(const BitVec& r) {
        return op2(r, std::bit_or<unsigned long long>());
    }
    BitVec& operator^=(const BitVec& r) {
        return op2(r, std::bit_xor<unsigned long long>());
    }
    BitVec& operator<<=(const size_t& s) {
        auto block = s / B, rem = s % B;
        if (d.size() <= block) {
            reset();
            return *this;
        }
        for (size_t i = d.size() - 1; i > block; i--) {
            if (rem == 0)
                d[i] = d[i - block];
            else
                d[i] = d[i - block] << rem | d[i - block - 1] >> (B - rem);
        }
        d[block] = d[0] << rem;
        erase_last();
        fill(d.begin(), d.begin() + block, 0ULL);
        return *this;
    }
    BitVec& operator>>=(const size_t& s) {
        auto block = s / B, rem = s % B;
        if (d.size() <= block) {
            reset();
            return *this;
        }
        for (size_t i = 0; i < d.size() - block - 1; i++) {
            if (rem == 0)
                d[i] = d[i + block];
            else
                d[i] = d[i + block + 1] << (B - rem) | d[i + block] >> rem;
        }
        d[d.size() - block - 1] = d.back() >> rem;
        fill(d.begin() + d.size() - block, d.end(), 0ULL);
        return *this;
    }
    BitVec operator&(const BitVec& r) const { return BitVec(*this) &= r; }
    BitVec operator|(const BitVec& r) const { return BitVec(*this) |= r; }
    BitVec operator^(const BitVec& r) const { return BitVec(*this) ^= r; }
    BitVec operator<<(const size_t& s) const { return BitVec(*this) <<= s; }
    BitVec operator>>(const size_t& s) const { return BitVec(*this) >>= s; }
    bool operator==(const BitVec& r) const { return d == r.d; }
    bool operator!=(const BitVec& r) const { return !(d == r.d); }

    void push_back(bool f) {
        if (n % B == 0) d.push_back(0);
        set(n, f);
        n++;
    }

    BitVec substr(size_t st, size_t le) const {
        assert(st + le <= n);
        BitVec res(le);
        size_t i = 0;
        while (i < le) {
            res.d[i / B] |= d[(st + i) / B] >> ((st + i) % B) << (i % B);
            i += std::min(B - i % B, B - (st + i) % B);
        }
        res.erase_last();
        return res;
    }
    bool substr_match(size_t st, const BitVec& pat) const {
        size_t le = pat.size();
        assert(st + le <= n);
        size_t i = 0;
        while (i < le) {
            size_t u = std::min({le - i, B - i % B, B - (st + i) % B});
            unsigned long long z = pat.d[i / B] >> (i % B) ^ d[(st + i) / B] >> ((st + i) % B);
            if (z << (B - u)) return false;
            i += u;
        }
        return true;
    }

    const std::vector<unsigned long long>& raw_data() const { return d; }

  private:
    static constexpr size_t B = 64;
    size_t n;
    std::vector<unsigned long long> d;
    void erase_last() {
        if (n % B) d.back() &= (unsigned long long)(-1) >> (B - n % B);
    }

    template <class OP> BitVec& op1(OP op) {
        for (auto& x : d) x = op(x);
        return *this;
    }

    template <class OP> BitVec& op2(const BitVec& r, OP op) {
        assert(n == r.n);
        for (size_t i = 0; i < d.size(); i++) d[i] = op(d[i], r.d[i]);
        return *this;
    }
};

namespace internal {

// bitvec
template <class H> auto update(const H& h, const BitVec& bs) {
    return update(h, bs.raw_data());
}

}  // namespace internal

}  // namespace yosupo
