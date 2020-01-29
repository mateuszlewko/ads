#pragma once

#include <cmath>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

#include "interface.h"

template <typename T>
struct node {
  std::shared_ptr<node> prev, next;
  T key;
  bool leaf;

  node(T k = 0, bool _leaf = false) : key(k), leaf(_leaf) {}
};

template <typename T>
class x_fast : public fast_set<T> {
  const int u_exp;

  typedef std::unordered_map<T, std::shared_ptr<node<T>>> level;

  std::vector<level> levels;

  int lcp_level(T key) {
    int beg = 0;
    int end = u_exp + 1;

    while (beg != end - 1) {
      int mid = (beg + end) / 2;
      T prefix = key >> (u_exp - mid);

      if (levels[mid].count(prefix))
        beg = mid;
      else
        end = mid;
    }

    return beg;
  }

  std::shared_ptr<node<T>> pred_node(T key) {
    auto node = find_leaf_node(key);
    if (node && node->key >= key) return node->prev;

    return node;
  }

  std::shared_ptr<node<T>> succ_node(T key) {
    auto node = find_leaf_node(key);
    if (node && node->key <= key) return node->next;

    return node;
  }

  std::shared_ptr<node<T>> find_leaf_node(T key) {
    auto lvl = lcp_level(key);
    // std::cout << "key = " << key << ", u_exp = " << u_exp << ", lvl = " << lvl
    //           << (key >> (u_exp - lvl)) << std::endl;

    auto node = levels[lvl][key >> (u_exp - lvl)];

    if (!node->leaf) {
      if (!node->prev && !node->next) return nullptr;

      bool right_child = key & ((T)1 << (u_exp - lvl - 1));
      node = right_child ? node->next : node->prev;
    }

    return node;
  }

 public:
  x_fast(int _u_exp) : u_exp(_u_exp) {
    levels.resize(u_exp + 1);
    levels[0][0] = std::make_shared<node<T>>();
  }

  bool lookup(T key) {
    auto& leafs = levels[u_exp];
    return leafs.count(key) != 0;
  }

  void insert(T key) {
    if (lookup(key)) return;

    auto prev = pred_node(key);
    auto next = succ_node(key);

    auto curr = std::make_shared<node<T>>(key, true);
    levels[u_exp][key] = curr;

    if (prev) {
      curr->prev = prev;
      prev->next = curr;
    }

    if (next) {
      curr->next = next;
      next->prev = curr;
    }

    for (int i = 1; i <= u_exp; i++) {
      auto prefix = key >> (u_exp - i);

      auto parent = levels[i - 1][prefix >> 1];
      if (!levels[i].count(prefix)) {
        auto lvl_node = std::make_shared<node<T>>(key);
        levels[i][prefix] = lvl_node;

        if (prefix & 1) {
          parent->next = lvl_node;
        } else {
          parent->prev = lvl_node;
        }
      }

      if (prefix & 1) {
        // update left descendant pointer
        if (!parent->prev || parent->prev->key > key) {
          parent->prev = curr;
        }
      } else {
        // update right descendant pointer
        if (!parent->next || parent->next->key < key) {
          parent->next = curr;
        }
      }
    }
  }

  T succ(T key) {
    auto node = succ_node(key);
    return node ? node->key : ((T)1 << u_exp);
  }

  T pred(T key) {
    auto node = pred_node(key);
    return node ? node->key : -((T)1 << u_exp);
  }

  ~x_fast() {
    for (auto [_, node] : levels[u_exp]) node->next = node->prev = nullptr;
  }
};