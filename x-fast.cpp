// #include <cassert>
// #include <cmath>
// #include <iostream>
// #include <memory>
// #include <unordered_map>
// #include <vector>

#include <bits/stdc++.h>
using namespace std;

#if DEBUG
#define error(args...)                       \
  {                                          \
    string _s = #args;                       \
    replace(_s.begin(), _s.end(), ',', ' '); \
    stringstream _ss(_s);                    \
    istream_iterator<string> _it(_ss);       \
    err(_it, args);                          \
  }
void err(istream_iterator<string>) {}
template <typename T, typename... Args>
void err(istream_iterator<string> it, T a, Args... args) {
  cerr << *it << " = " << a << endl;
  err(++it, args...);
}
#else
#define error(...) \
  do {             \
  } while (0)
#endif

template <typename T>
struct node {
  std::shared_ptr<node> prev, next;
  T key;
  bool leaf;

  node(T k = 0, bool _leaf = false) : key(k), leaf(_leaf) {}
};

template <typename T>
class x_fast {
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

    // std::cout << "inserting = " << key << std::endl;

    if (prev) {
      assert(prev->leaf);
      // std::cout << "prev is = " << prev->key << std::endl;
      curr->prev = prev;
      prev->next = curr;
    }

    if (next) {
      // std::cout << "next is = " << next->key << std::endl;
      assert(next->leaf);
      curr->next = next;
      next->prev = curr;
    }

    for (int i = 1; i <= u_exp; i++) {
      auto prefix = key >> (u_exp - i);

      auto parent = levels[i - 1][prefix >> 1];
      if (!levels[i].count(prefix)) {
        auto lvl_node = std::make_shared<node<T>>(key);
        levels[i][prefix] = lvl_node;

        // std::cout << "inserting node, lvl = " << i << ", prefix = " << prefix
        //           << ", key = " << key << std::endl;

        if (prefix & 1) {
          parent->next = lvl_node;
          // update left descendant pointer
          // if (!parent->prev || parent->prev->key > key) {
          //   parent->prev = curr;
          // }
        } else {
          parent->prev = lvl_node;

          // // update right descendant pointer
          // if (!parent->next || parent->next->key < key) {
          //   parent->next = curr;
          // }
        }
      }

      if (prefix & 1) {
        // parent->next = lvl_node;
        // update left descendant pointer
        if (!parent->prev || parent->prev->key > key) {
          parent->prev = curr;
        }
      } else {
        // parent->prev = lvl_node;

        // update right descendant pointer
        if (!parent->next || parent->next->key < key) {
          parent->next = curr;
        }
      }
    }

    // auto parent = levels[u_exp - 1][key >> 1];
    // if (key & 1) {
    //   if (!parent->prev || parent->prev->key > key) {
    //     parent->prev = curr;
    //   }
    // } else {
    //   if (!parent->next || parent->next->key < key) {
    //     parent->next = curr;
    //   }
    // }
  }

  std::shared_ptr<node<T>> succ_node(T key) {
    auto lvl = lcp_level(key);
    auto node = levels[lvl][key >> (u_exp - lvl)];

    if (!node->leaf) {
      // if (node->prev) {
      //   std::cout << "key = " << key << ", prev = " << node->prev->key
      //             << "leaf = " << node->prev->leaf << ", lvl = " << lvl
      //             << std::endl;
      // }

      // std::cout << "key = " << key << ", lvl = " << lvl
      //           << ", node.key = " << node->key << std::endl;

      if (!node->prev && !node->next) {
        // std::cout << "both null" << std::endl;
        return nullptr;
      }

      if (key & (1 << (u_exp - lvl - 1)))
        node = node->next;
      else
        node = node->prev;

      assert(node);
      assert(node->leaf);
      // node = node->prev && node->prev->leaf ? node->prev : node->next;
    }

    if (node->key <= key) {
      // std::cout << "key = " << key << ", node.key = " << node->key
      //           << "is_next = " << !!node->next << std::endl;

      // if (node->next)
      //   std::cout << "and next is = " << node->next->key << std::endl;

      return node->next;
    }

    return node;
  }

  std::shared_ptr<node<T>> pred_node(T key) {
    auto lvl = lcp_level(key);
    auto node = levels[lvl][key >> (u_exp - lvl)];

    // error(lvl, key, node->key, node->leaf);

    if (!node->leaf) {
      if (node->prev) {
        error(node->prev->key);
        // std::cout << "key = " << key << ", prev = " << node->prev->key
        //           << "leaf = " << node->prev->leaf << ", lvl = " << lvl
        //           << std::endl;
      }

      if (node->next) error(node->next->key, node->next->leaf);

      if (!node->prev && !node->next) {
        return nullptr;
      }

      if (key & ((T)1 << (u_exp - lvl - 1))) {
        node = node->next;
      } else {
        node = node->prev;
      }

      assert(node);
      assert(node->leaf);
      // node = node->prev && node->prev->leaf ? node->prev : node->next;
    }

    if (node->key >= key) {
      return node->prev;  // ? node->prev : nullptr;
    }

    return node;
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

int main() {
  auto s = x_fast<int>(22);
  s.insert(4);
  s.insert(10);
  s.insert(11);
  s.insert(1000);
  s.insert(500001);

  std::cout << "succ(1) = " << s.succ(1) << std::endl
            << "succ(9) = " << s.succ(9) << std::endl
            << "succ(10) = " << s.succ(10) << std::endl
            << "pred(10) = " << s.pred(10) << std::endl
            << "pred(11) = " << s.pred(11) << std::endl
            << "pred(1) = " << s.pred(1) << std::endl
            << "pred(1000) = " << s.pred(1000) << std::endl
            << "succ(11) = " << s.succ(11) << std::endl
            << "succ(510000) = " << s.succ(510000) << std::endl
            << "succ(1000) = " << s.succ(1000) << std::endl
            << "find(1000) = " << s.lookup(1000) << std::endl
            << "find(500001) = " << s.lookup(500001) << std::endl
            << "find(1) = " << s.lookup(1) << std::endl;
}