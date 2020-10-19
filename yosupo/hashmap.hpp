#pragma once

#include <iostream>
#include <iterator>
#include <utility>
#include <vector>

#include "yosupo/hash.hpp"

namespace yosupo {

template <class K, class D, class H = UniversalHash32<K>> struct HashMap {
  public:
    using Data = std::pair<K, D>;

  private:
    struct Iterator {
      public:
        using difference_type = int;
        using value_type = Data;
        using pointer = Data*;
        using reference = Data&;
        using iterator_category = std::forward_iterator_tag;

        HashMap& _mp;
        int _pos;
        Iterator(HashMap& mp, int pos) : _mp(mp), _pos(pos) {}

        std::pair<K, D> operator*() const { return _mp.data[_pos]; }

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
    HashMap() : mask(3), filled(0), status(mask + 1), data(mask + 1) {}

    Iterator begin() { return Iterator(*this, next_bucket(0)); }
    Iterator end() { return Iterator(*this, mask + 1); }
    using iterator = Iterator;

    D& operator[](const K& k) {
        unsigned int i = H()(k) & mask;
        while (status[i] != Status::Empty && data[i].first != k) {
            i = (i + 1) & mask;
        }
        if (status[i] == Status::Empty) {
            if (filled * 2 > mask) {
                rehash();
                return (*this)[k];
            }
            filled++;
        }
        if (status[i] != Status::Fill) {
            status[i] = Status::Fill;
            data[i] = {k, D()};
        }
        return data[i].second;
    }

  private:
    unsigned int mask, filled;  // data.size() == 1 << s

    enum Status {
        Empty = 0,
        Fill = 1,
        Deleted = 2,
    };

    std::vector<Status> status;
    std::vector<Data> data;

    void rehash() {
        unsigned int pmask = mask;
        mask = mask * 2 + 1;
        filled = 0;
        auto pstatus = std::exchange(status, std::vector<Status>(mask + 1));
        auto pdata = std::exchange(data, std::vector<Data>(mask + 1));
        for (unsigned int i = 0; i <= pmask; i++) {
            if (pstatus[i] == Status::Fill) {
                (*this)[pdata[i].first] = pdata[i].second;
            }
        }
    }

    int next_bucket(int i) {
        while (i <= mask && status[i] != Status::Fill) i++;
        return i;
    }
};

}  // namespace yosupo
