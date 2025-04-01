#pragma once

#include <cstddef>
#include <iterator>
#include <utility>
#include <vector>

#include "yosupo/hash.hpp"
#include "yosupo/types.hpp"

namespace yosupo {

template <class K, class D, class H = Hasher<K>> struct IncrementalHashMap {
  public:
    using Data = std::pair<K, D>;

  private:
    struct Iterator {
      public:
        using difference_type = i32;
        using value_type = Data;
        using pointer = Data*;
        using reference = Data&;
        using iterator_category = std::forward_iterator_tag;

        IncrementalHashMap& _mp;
        u32 _pos;
        Iterator(IncrementalHashMap& mp, u32 pos) : _mp(mp), _pos(pos) {}

        std::pair<K, D>& operator*() const { return _mp.data[_pos]; }
        std::pair<K, D>* operator->() const { return &_mp.data[_pos]; }

        Iterator& operator++() {
            _pos = _mp.next_bucket(_pos + 1);
            return *this;
        }
        Iterator operator++(int) {
            auto result = *this;
            ++*this;
            return result;
        }

        bool operator==(const Iterator& rhs) const { return _pos == rhs._pos; }
        bool operator!=(const Iterator& rhs) const { return !(*this == rhs); }
    };

  public:
    IncrementalHashMap(size_t s)
        : mask((1 << s) - 1), filled(0), used(mask + 1), data(mask + 1) {}
    IncrementalHashMap() : IncrementalHashMap(2) {}

    Iterator begin() { return Iterator(*this, next_bucket(0)); }
    Iterator end() { return Iterator(*this, mask + 1); }
    using iterator = Iterator;

    D& operator[](const K& k) {
        u32 i = (u32)(H()(k)) & mask;
        while (used[i] && data[i].first != k) {
            i = (i + 1) & mask;
        }
        if (!used[i]) {
            if (filled * 2 > mask) {
                rehash();
                return (*this)[k];
            }
            filled++;
            used[i] = true;
            data[i] = {k, D()};
        }
        return data[i].second;
    }

    Iterator find(const K& k) {
        u32 i = H()(k) & mask;
        while (used[i] && data[i].first != k) {
            i = (i + 1) & mask;
        }
        if (!used[i]) return end();
        return Iterator(*this, i);
    }

    size_t count(const K& k) { return find(k) == end() ? 0 : 1; }

    size_t size() const { return filled; }

  private:
    u32 mask, filled;  // data.size() == 1 << s

    std::vector<bool> used;
    std::vector<Data> data;

    void rehash() {
        u32 pmask = mask;
        mask = mask * 2 + 1;
        filled = 0;
        auto pused = std::exchange(used, std::vector<bool>(mask + 1));
        auto pdata = std::exchange(data, std::vector<Data>(mask + 1));
        for (u32 i = 0; i <= pmask; i++) {
            if (pused[i]) {
                (*this)[pdata[i].first] = pdata[i].second;
            }
        }
    }

    u32 next_bucket(u32 i) const {
        while (i <= mask && !used[i]) i++;
        return i;
    }
};

}  // namespace yosupo
