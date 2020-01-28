#include <cmath>
#include <iostream>
#include <memory>
#include <vector>

template <typename T>
class vEB {
  /// expononent of universe size
  const int u_exp;
  const int u_exp_low;
  const int u_exp_high;
  const T u;

  std::vector<vEB<T>> buckets;
  std::unique_ptr<vEB<T>> non_empty_high;

  T min, max;

  T get_high(T key) { return key >> u_exp_low; }

  T get_low(T key) {
    T low_bits = ((T)1 << u_exp_low) - 1;
    return key & low_bits;
  }

 public:
  vEB(int _u_exp)
      : u_exp(std::max(_u_exp, 1)),
        u_exp_low((u_exp + 1) / 2),
        u_exp_high(u_exp / 2),
        u((T)1 << u_exp),
        min(u),
        max((T)-1) {
    if (u_exp == 1) return;

    non_empty_high = std::make_unique<vEB<T>>(u_exp_high);

    for (T i = 0; i < (1 << u_exp_high); i++) {
      buckets.emplace_back(u_exp_low);
    }
  }

  bool find(T key) { return succ(key - 1) == key; }

  void insert(T key) {
    if (min > max) {
      min = max = key;
      return;
    }

    if (key < min) std::swap(key, min);
    max = std::max(key, max);

    auto high = get_high(key);
    auto low = get_low(key);

    std::cout << "high = " << high << ", bs.size: " << buckets.size()
              << std::endl;
    buckets[high].insert(low);

    if (buckets[high].min == buckets[high].max) {
      non_empty_high->insert(high);
    }
  }

  void erase(T key) {}

  T succ(T key) {
    if (key < min) {
      std::cout << "min = " << min << std::endl;
      return min;
    }
    if (key >= max) {
      std::cout << "u = " << u << std::endl;
      return u;
    }

    auto high = get_high(key);
    auto low = get_low(key);

    if (low < buckets[high].max) {
      auto res = buckets[high].succ(low);
      std::cout << "high = " << high << ", high << exp = " << (high << u_exp_low)
                << ", succ = " << res << std::endl;
      return (high << u_exp_low) + res;
    }

    auto next_high = non_empty_high->succ(high);
    return (next_high << u_exp_low) + buckets[next_high].min;
  }
};

int main() {
  auto veb_set = vEB<int>(22);
  veb_set.insert(4);
  veb_set.insert(10);
  veb_set.insert(1000);
  veb_set.insert(500001);

  std::cout << "succ(1) = " << veb_set.succ(1) << std::endl
            << "succ(9) = " << veb_set.succ(9) << std::endl
            << "succ(1000) = " << veb_set.succ(1000) << std::endl;
}