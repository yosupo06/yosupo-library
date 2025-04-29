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

template <class K, class D, class H = Hasher> struct IncrementalHashMap {
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

    struct ConstIterator {
      public:
        using difference_type = i32;
        using value_type = const Data;
        using pointer = const Data*;
        using reference = const Data&;
        using iterator_category = std::forward_iterator_tag;

        const IncrementalHashMap& _mp;
        u32 _pos;
        ConstIterator(const IncrementalHashMap& mp, u32 pos)
            : _mp(mp), _pos(pos) {}

        const std::pair<K, D>& operator*() const { return _mp.data[_pos]; }
        const std::pair<K, D>* operator->() const { return &_mp.data[_pos]; }

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
    IncrementalHashMap(size_t s, const H& _h = H())
        : h(_h),
          mask((1 << s) - 1),
          filled(0),
          used(mask + 1),
          data(mask + 1) {}
    IncrementalHashMap(const H& _h = H()) : IncrementalHashMap(2, _h) {}

    Iterator begin() { return Iterator(*this, next_bucket(0)); }
    Iterator end() { return Iterator(*this, mask + 1); }
    ConstIterator begin() const { return ConstIterator(*this, next_bucket(0)); }
    ConstIterator end() const { return ConstIterator(*this, mask + 1); }
    ConstIterator cbegin() const { return begin(); }
    ConstIterator cend() const { return end(); }

    using iterator = Iterator;
    using const_iterator = ConstIterator;

    D& operator[](const K& k) {
        u32 i = start_bucket(k);
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
        u32 i = start_bucket(k);
        while (used[i] && data[i].first != k) {
            i = (i + 1) & mask;
        }
        if (!used[i]) return end();
        return Iterator(*this, i);
    }

    ConstIterator find(const K& k) const {
        u32 i = start_bucket(k);
        while (used[i] && data[i].first != k) {
            i = (i + 1) & mask;
        }
        if (!used[i]) return cend();
        return ConstIterator(*this, i);
    }

    size_t count(const K& k) const { return this->find(k) != cend() ? 1 : 0; }
    bool contains(const K& k) const { return this->find(k) != cend(); }

    size_t size() const { return filled; }

    std::string dump() const {
        std::string s = "{";
        bool first = true;
        for (const auto& [key, val] : *this) {
            if (!first) {
                s += ", ";
            }
            first = false;
            s += yosupo::dump(key);
            s += ": ";
            s += yosupo::dump(val);
        }
        s += "}";
        return s;
    }

  private:
    Hasher h;

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

    u32 start_bucket(const K& k) const { return (u32)(h(k)) & mask; }

    u32 next_bucket(u32 i) const {
        while (i <= mask && !used[i]) i++;
        return i;
    }
};

}  // namespace yosupo
