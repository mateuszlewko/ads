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

  printf("(%s), n = %d, insert %.2fs | succ %.2fs | look %.2fs\n",
         ds_name.c_str(), n, ms_insert / 1000.0, ms_succ / 1000.0,
         ms_look / 1000.0);

  return res;
}

int main() {
  // Test 1
  {
    int u_exp = 25;
    int u = 1 << u_exp;

    std::shared_ptr<fast_set<int>> veb = std::make_shared<v_eb<int>>(u_exp);
    std::shared_ptr<fast_set<int>> xf = std::make_shared<x_fast<int>>(u_exp);
    std::shared_ptr<fast_set<int>> yf = std::make_shared<y_fast<int>>(u_exp);
    std::shared_ptr<fast_set<int>> sw =
        std::make_shared<set_wrapper<int>>(u_exp);

    srand(123);

    std::cout << "Initalized data structures.\n";

    int N[] = {100000,  1000000,  2000000,  4000000,
               8000000, 12000000, 20000000, 25000000};

    for (int i = 0; i < 8; i++) {
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

/* Wynik programu

Initalized data structures.
(vEB), n = 100000, insert 0.03s | succ 0.01s | look 0.01s
output xor = 18425189
(x-fast), n = 100000, insert 0.36s | succ 0.04s | look 0.01s
output xor = 18425189
(y-fast), n = 100000, insert 0.17s | succ 0.08s | look 0.04s
output xor = 18425189
(std::set), n = 100000, insert 0.02s | succ 0.01s | look 0.01s
output xor = 18425189

(vEB), n = 1000000, insert 0.45s | succ 0.19s | look 0.14s
output xor = 21755001
(x-fast), n = 1000000, insert 3.75s | succ 0.57s | look 0.10s
output xor = 21755001
(y-fast), n = 1000000, insert 3.06s | succ 1.51s | look 0.93s
output xor = 21755001
(std::set), n = 1000000, insert 0.46s | succ 0.47s | look 0.41s
output xor = 21755001

(vEB), n = 2000000, insert 1.01s | succ 0.44s | look 0.37s
output xor = 7823828
(x-fast), n = 2000000, insert 6.71s | succ 1.21s | look 0.21s
output xor = 7823828
(y-fast), n = 2000000, insert 6.46s | succ 3.25s | look 2.19s
output xor = 7823828
(std::set), n = 2000000, insert 1.50s | succ 1.37s | look 1.26s
output xor = 7823828

(vEB), n = 4000000, insert 2.10s | succ 0.99s | look 0.87s
output xor = 12555142
(x-fast), n = 4000000, insert 11.98s | succ 2.64s | look 0.46s
output xor = 12555142
(y-fast), n = 4000000, insert 13.16s | succ 6.69s | look 4.43s
output xor = 12555142
(std::set), n = 4000000, insert 3.74s | succ 3.36s | look 3.16s
output xor = 12555142

(vEB), n = 8000000, insert 3.95s | succ 2.13s | look 1.99s
output xor = 930003
(x-fast), n = 8000000, insert 20.48s | succ 5.25s | look 0.95s
output xor = 930003
(y-fast), n = 8000000, insert 24.29s | succ 15.71s | look 10.45s
output xor = 930003
(std::set), n = 8000000, insert 8.70s | succ 7.78s | look 7.42s
output xor = 930003

(vEB), n = 12000000, insert 5.28s | succ 3.31s | look 3.15s
output xor = 6782043
(x-fast), n = 12000000, insert 24.22s | succ 8.01s | look 1.49s
output xor = 6782043
(y-fast), n = 12000000, insert 33.87s | succ 23.98s | look 15.30s
output xor = 6782043
(std::set), n = 12000000, insert 14.17s | succ 12.58s | look 12.45s
output xor = 6782043

(vEB), n = 20000000, insert 9.97s | succ 5.56s | look 5.15s
output xor = 61321516
(x-fast), n = 20000000, insert 53.41s | succ 13.64s | look 2.59s
output xor = 61321516
(y-fast), n = 20000000, insert 63.49s | succ 38.16s | look 22.71s
output xor = 61321516
(std::set), n = 20000000, insert 20.07s | succ 20.33s | look 19.03s
output xor = 61321516

(vEB), n = 25000000, insert 9.77s | succ 7.03s | look 6.84s
output xor = 39750439
(x-fast), n = 25000000, insert 36.92s | succ 16.72s | look 3.05s
output xor = 39750439
(y-fast), n = 25000000, insert 60.30s | succ 52.95s | look 33.30s
output xor = 39750439
(std::set), n = 25000000, insert 30.61s | succ 28.24s | look 27.56s
output xor = 39750439
*/