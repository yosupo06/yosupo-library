#pragma once

#include <cstddef>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

#include "yosupo/dump.hpp"
#include "yosupo/hash.hpp"
#include "yosupo/types.hpp"

namespace yosupo {

template <class K, class H = Hasher> struct IncrementalHashSet {
  private:
    struct Iterator {
      public:
        using difference_type = i32;
        using value_type = K;
        using pointer = K*;
        using reference = K&;
        using iterator_category = std::forward_iterator_tag;

        IncrementalHashSet& _mp;
        u32 _pos;
        Iterator(IncrementalHashSet& mp, u32 pos) : _mp(mp), _pos(pos) {}

        K operator*() const { return _mp.keys[_pos]; }

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

    struct ConstIterator {
      public:
        using difference_type = i32;
        using value_type = const K;
        using pointer = const K*;
        using reference = const K&;
        using iterator_category = std::forward_iterator_tag;

        const IncrementalHashSet& _mp;
        u32 _pos;
        ConstIterator(const IncrementalHashSet& mp, u32 pos)
            : _mp(mp), _pos(pos) {}

        const K& operator*() const { return _mp.keys[_pos]; }

        ConstIterator& operator++() {
            _pos = _mp.next_bucket(_pos + 1);
            return *this;
        }
        ConstIterator operator++(int) {
            auto result = *this;
            ++*this;
            return result;
        }

        bool operator==(const ConstIterator& rhs) const {
            return _pos == rhs._pos;
        }
        bool operator!=(const ConstIterator& rhs) const {
            return !(*this == rhs);
        }
    };

  public:
    IncrementalHashSet(size_t s, const H& _h = H())
        : h(_h),
          mask((1 << s) - 1),
          filled(0),
          used(mask + 1),
          keys(mask + 1) {}
    IncrementalHashSet(const H& _h = H()) : IncrementalHashSet(2, _h) {}

    Iterator begin() { return Iterator(*this, next_bucket(0)); }
    Iterator end() { return Iterator(*this, mask + 1); }
    ConstIterator begin() const { return ConstIterator(*this, next_bucket(0)); }
    ConstIterator end() const { return ConstIterator(*this, mask + 1); }
    ConstIterator cbegin() const { return begin(); }
    ConstIterator cend() const { return end(); }

    using iterator = Iterator;
    using const_iterator = ConstIterator;

    void insert(const K& k) {
        u32 i = start_bucket(k);
        while (used[i] && keys[i] != k) {
            i = (i + 1) & mask;
        }
        if (!used[i]) {
            if (filled * 2 > mask) {
                rehash();
                return this->insert(k);
            }
            filled++;
            used[i] = true;
            keys[i] = k;
        }
    }

    Iterator find(const K& k) {
        u32 i = start_bucket(k);
        while (used[i] && keys[i] != k) {
            i = (i + 1) & mask;
        }
        if (!used[i]) return end();
        return Iterator(*this, i);
    }

    ConstIterator find(const K& k) const {
        u32 i = start_bucket(k);
        while (used[i] && keys[i] != k) {
            i = (i + 1) & mask;
        }
        if (!used[i]) return end();
        return ConstIterator(*this, i);
    }

    bool contains(const K& k) const { return this->find(k) != end(); }

    int size() const { return filled; }

    std::string dump() const {
        std::string s = "{";
        bool first = true;
        for (const auto& key : *this) {
            if (!first) {
                s += ", ";
            }
            first = false;
            s += yosupo::dump(key);
        }
        s += "}";
        return s;
    }

  private:
    Hasher h;

    u32 mask, filled;  // data.size() == 1 << s

    std::vector<bool> used;
    std::vector<K> keys;

    void rehash() {
        u32 pmask = mask;
        mask = mask * 2 + 1;
        filled = 0;
        auto pused = std::exchange(used, std::vector<bool>(mask + 1));
        auto pkeys = std::exchange(keys, std::vector<K>(mask + 1));
        for (u32 i = 0; i <= pmask; i++) {
            if (pused[i]) {
                this->insert(pkeys[i]);
            }
        }
    }

    u32 start_bucket(const K& k) const { return (u32)(h(k)) & mask; }

    u32 next_bucket(u32 i) const {
        while (i <= mask && !used[i]) i++;
        return i;
    }
};

}  // namespace yosupo
