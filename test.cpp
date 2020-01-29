#include <unistd.h>
#include <chrono>
#include <iostream>
#include <vector>

#include "interface.h"
#include "v-eb.h"
#include "x-fast.h"
#include "y-fast.h"

template <typename T>
class set_wrapper : public fast_set<T> {
  const int u_exp;

  std::set<T> s;

 public:
  set_wrapper(int _u_exp) : u_exp(_u_exp) {}

  bool lookup(T key) { return s.count(key); }

  void insert(T key) { s.insert(key); }

  T succ(T key) {
    auto it = s.upper_bound(key);
    if (it == s.end()) return (T)1 << u_exp;
    return *it;
  }
};

void print_duration() {}

std::vector<int> test1_insert, test1_query;

void init_test1(int n, int u) {
  test1_insert.clear();
  test1_query.clear();

  for (int i = 0; i < n; i++) test1_insert.push_back(rand() % u);
  for (int i = 0; i < n; i++) test1_query.push_back(rand() % u);
}

int test1(std::shared_ptr<fast_set<int>> s, int n, std::string ds_name) {
  auto start = std::chrono::steady_clock::now();

  for (int x : test1_insert) s->insert(x);
  auto end1 = std::chrono::steady_clock::now();

  int res = 0;
  for (int x : test1_query) res ^= s->succ(x);

  auto end2 = std::chrono::steady_clock::now();

  for (int x : test1_insert) s->lookup(x);

  auto end3 = std::chrono::steady_clock::now();

  auto ms_insert =
      std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start)
          .count();
  auto ms_succ =
      std::chrono::duration_cast<std::chrono::milliseconds>(end2 - end1)
          .count();
  auto ms_look =
      std::chrono::duration_cast<std::chrono::milliseconds>(end3 - end2)
          .count();

  printf("Test 1 (%s), n = %d, insert %.2fs | succ %.2fs | look %.2fs\n", ds_name.c_str(), n,
         ms_insert / 1000.0, ms_succ / 1000.0, ms_look / 1000.0);

  return res;
}

int main() {
  // Test 1
  {
    int u_exp = 24;
    int u = 1 << u_exp;

    std::shared_ptr<fast_set<int>> veb = std::make_shared<v_eb<int>>(u_exp);
    std::shared_ptr<fast_set<int>> xf = std::make_shared<x_fast<int>>(u_exp);
    std::shared_ptr<fast_set<int>> yf = std::make_shared<y_fast<int>>(u_exp);
    std::shared_ptr<fast_set<int>> sw =
        std::make_shared<set_wrapper<int>>(u_exp);

    srand(123);

    std::cout << "Initalized data structures.\n";

    int N[] = {100000, 1000000, 2000000, 4000000, 8000000, 12000000};

    for (int i = 0; i < 6; i++) {
      int n = N[i];

      init_test1(n, u);

      printf("output xor = %d\n", test1(veb, n, "vEB"));
      printf("output xor = %d\n", test1(xf, n, "x-fast"));
      printf("output xor = %d\n", test1(yf, n, "y-fast"));
      printf("output xor = %d\n", test1(sw, n, "std::set"));

      puts("");
    }
  }
}