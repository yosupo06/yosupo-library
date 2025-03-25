#pragma once

#include <algorithm>
#include <array>
#include <bit>
#include <cmath>

#include "yosupo/types.hpp"

namespace yosupo {

template <int B, int PLUS = 0> struct HyperLogLog {
    static_assert(4 <= B && B <= 30);

    HyperLogLog() {}

    // insert random hash
    void add(u32 hash) {
        if (count != -1) {
            int idx = int(
                std::lower_bound(hashes.begin(), hashes.begin() + count, hash) -
                hashes.begin());
            if (idx != count && hashes[idx] == hash) return;
            if (count == PLUS) {
                count = -1;
                for (int i = 0; i < PLUS; i++) {
                    add_dense(hashes[i]);
                }
                add_dense(hash);
            } else {
                count++;
                std::copy_backward(hashes.begin() + idx,
                                   hashes.begin() + count - 1,
                                   hashes.begin() + count);
                hashes[idx] = hash;
            }
        } else {
            add_dense(hash);
        }
    }

    int estimate() const {
        if (count != -1) return count;

        double est = 0.0;

        for (int reg : regs) {
            est += 1.0 / (double)(1LL << reg);
        }
        est = 1.0 / est;
        est *= ALPHA * M * M;

        if (est <= 2.5 * M) {
            int zeros = int(std::ranges::count(regs, 0));
            if (zeros != 0) {
                est = M * std::log(double(M) / (double)zeros);
            }
        }
        return int(round(est));
    }

    friend HyperLogLog operator+(const HyperLogLog& lhs,
                                 const HyperLogLog& rhs) {
        if (rhs.count != -1) {
            auto res = HyperLogLog(lhs);
            res.add_sparse(rhs);
            return res;
        }
        if (lhs.count != -1) {
            auto res = HyperLogLog(rhs);
            res.add_sparse(lhs);
            return res;
        }
        HyperLogLog res;
        res.count = -1;
        for (int i = 0; i < M; i++) {
            res.regs[i] = std::max(lhs.regs[i], rhs.regs[i]);
        }
        return res;
    }

  private:
    static constexpr int M = 1 << B;
    static constexpr double ALPHA = []() {
        switch (B) {
            case 4:
                return 0.673;
            case 5:
                return 0.697;
            case 6:
                return 0.709;
            default:
                return 0.7213 / (1 + 1.079 / M);
        }
    }();

    int count = 0;
    std::array<u8, M> regs = {};
    std::array<u32, PLUS> hashes = {};

    void add_dense(u32 hash) {
        u32 index = hash >> (32 - B);
        regs[index] = std::max(regs[index], (u8)(std::countr_zero(hash) + 1));
    }

    void add_sparse(const HyperLogLog& rhs) {
        for (int i = 0; i < rhs.count; i++) {
            add(rhs.hashes[i]);
        }
    }
};

}  // namespace yosupo