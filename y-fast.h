#pragma once

#include <cmath>
#include <iostream>
#include <memory>
#include <set>
#include <unordered_map>
#include <vector>

#include "interface.h"
#include "x-fast.h"

template <typename T>
class y_fast : public fast_set<T> {
  const int u_exp;
  const int max_load;

  x_fast<T> top;
  std::unordered_map<T, std::set<T>> bottom;

  void rebalance(T repr) {
    auto s = bottom[repr];
    if ((int)s.size() <= max_load) return;

    int i = 0;
    T new_reprs[2] = {-1, -1};
    std::set<T> sets[2];

    for (auto key : s) {
      int side = i++ < max_load ? 0 : 1;
      if (key == repr || new_reprs[side] == -1) new_reprs[side] = key;

      sets[side].insert(key);
    }

    bottom[new_reprs[0]] = sets[0];
    bottom[new_reprs[1]] = sets[1];
  }

 public:
  y_fast(int _u_exp)
      : u_exp(_u_exp), max_load(2 * (int)std::log2(u_exp)), top(u_exp) {}

  void insert(T key) {
    auto repr = top.succ(key);
    if (!bottom.count(repr)) {
      top.insert(key);
      repr = key;
    }

    bottom[repr].insert(key);
    rebalance(repr);
  }

  bool lookup(T key) {
    if (top.lookup(key)) return true;

    auto it = bottom.find(top.pred(key));
    if (it != bottom.end() && it->second.count(key)) return true;

    it = bottom.find(top.succ(key));
    if (it != bottom.end() && it->second.count(key)) return true;

    return false;
  }

  T succ(T key) {
    T next = top.succ(key);
    T prev = top.pred(key);

    T res = -((T)1 << u_exp);
    if (bottom.count(next)) {
      auto b = bottom[next];
      auto it = b.upper_bound(key);
      if (it != b.end()) res = std::max(res, *it);
    }

    if (bottom.count(prev)) {
      auto b = bottom[prev];
      auto it = b.upper_bound(key);
      if (it != b.end()) res = std::max(res, *it);
    }

    if (res == -((T)1 << u_exp)) return (T)1 << u_exp;
    return res;
  }
};
