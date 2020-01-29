#pragma once

#include <cmath>
#include <iostream>
#include <memory>
#include <vector>

#include "interface.h"

template <typename T>
class v_eb : public fast_set<T> {
  /// expononent of universe size
  const int u_exp;
  const int u_exp_low;
  const int u_exp_high;
  const T u;

  std::vector<v_eb<T>> buckets;
  std::unique_ptr<v_eb<T>> non_empty_high;

  T min, max;

  T get_high(T key) { return key >> u_exp_low; }

  T get_low(T key) {
    T low_bits = ((T)1 << u_exp_low) - 1;
    return key & low_bits;
  }

 public:
  v_eb(int _u_exp)
      : u_exp(std::max(_u_exp, 1)),
        u_exp_low((u_exp + 1) / 2),
        u_exp_high(u_exp / 2),
        u((T)1 << u_exp),
        min(u),
        max((T)-1) {
    if (u_exp == 1) return;

    non_empty_high = std::make_unique<v_eb<T>>(u_exp_high);

    for (T i = 0; i < (1 << u_exp_high); i++) {
      buckets.emplace_back(u_exp_low);
    }
  }

  bool lookup(T key) {
    if (key == 0) return min == key;
    return succ(key - 1) == key;
  }

  void insert(T key) {
    if (lookup(key)) return;

    if (min > max) {
      min = max = key;
      return;
    }

    if (key < min) std::swap(key, min);
    max = std::max(key, max);

    if (u_exp == 1) return;

    auto high = get_high(key);
    auto low = get_low(key);

    buckets[high].insert(low);

    if (buckets[high].min == buckets[high].max) {
      non_empty_high->insert(high);
    }
  }

  T succ(T key) {
    if (key < min) return min;
    if (u == 2 && max > key) return max;
    if (key >= max || u == 2) return u;

    auto high = get_high(key);
    auto low = get_low(key);

    if (low < buckets[high].max) {
      return (high << u_exp_low) + buckets[high].succ(low);
    }

    auto next_high = non_empty_high->succ(high);
    return (next_high << u_exp_low) + buckets[next_high].min;
  }
};
