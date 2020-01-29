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

  int res = 0;
  for (int x : test1_query) res ^= s->succ(x);

  auto end = std::chrono::steady_clock::now();
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
                .count();

  printf("Test 1 (%s), n = %d, took %.2fs\n", ds_name.c_str(), n, ms / 1000.0);

  return res;
}

int main() {
  int u_exp = 24;
  int u = 1 << u_exp;

  std::shared_ptr<fast_set<int>> veb = std::make_shared<v_eb<int>>(u_exp);
  std::shared_ptr<fast_set<int>> xf = std::make_shared<x_fast<int>>(u_exp);
  std::shared_ptr<fast_set<int>> yf = std::make_shared<y_fast<int>>(u_exp);
  std::shared_ptr<fast_set<int>> sw = std::make_shared<set_wrapper<int>>(u_exp);

  srand(123);
  // for (int i = 0; i < 100000; i++) {
  //   int x = rand() % 100000;
  //   int y = rand() % 100000;

  //   std::cout << x << ", ";

  //   sw->insert(x);
  //   yf->insert(x);
  //   int a1 = sw->succ(y);
  //   std::cout << "-----\n";
  //   int a2 = yf->succ(y);

  //   if (a1 != a2) {
  //     std::cout << "WA on query = " << y << "\n";
  //     std::cout << "\n" << a1 << "\n" << a2 << std::endl;
  //     return 1;
  //   }
  // }

  // for (int y : data) {
  //   printf("%d\n%d\n--------\n", yf->succ(y), sw->succ(y));
  // }

  std::cout << "Initalized data structures.";

  int N[] = {10000, 1000000, 10000000, 15000000};

  for (int i = 0; i < 4; i++) {
    int n = N[i];

    init_test1(n, u);
    std::cout << "output xor = " << test1(veb, n, "vEB") << std::endl;
    std::cout << "output xor = " << test1(xf, n, "x-fast") << std::endl;
    std::cout << "output xor = " << test1(yf, n, "y-fast") << std::endl;
    std::cout << "output xor = " << test1(sw, n, "std::set") << std::endl;
  }

  // init_test1(100000, u);
  // cout << test1(veb, 100000)

  //         init_test1(1000000, u);

  // init_test1(10000000, u);

  // init_

  // srand(123);

  // std::vector<int> vs;

  // for (int i = 0; i < 10000; i++) {
  //   int x = rand() % (u / 2);
  //   veb.insert(x);
  //   xf.insert(x);
  //   yf.insert(x);
  //   sw.insert(x);

  //   vs.push_back(x);
  // }

  // for (int i = 0; i < 100; i++) {
  //   int x = rand() % (u / 2);

  //   int a1 = veb.succ(x);
  //   int a2 = xf.succ(x);
  //   int a3 = yf.succ(x);
  //   int a4 = sw.succ(x);

  //   std::cout << a1 << "\n" << a2 << "\n" << a3 << "\n" << a4 <<
  //   "\n--------\n";
  // }

  // s.insert(4);
  // s.insert(10);
  // s.insert(11);
  // s.insert(1000);
  // s.insert(500001);

  // std::cout << "succ(1) = " << s.succ(1) << std::endl
  //           << "succ(9) = " << s.succ(9) << std::endl
  //           << "succ(10) = " << s.succ(10) << std::endl
  //           << "succ(11) = " << s.succ(11) << std::endl
  //           << "succ(510000) = " << s.succ(510000) << std::endl
  //           << "succ(1000) = " << s.succ(1000) << std::endl
  //           << "find(1000) = " << s.lookup(1000) << std::endl
  //           << "find(500001) = " << s.lookup(500001) << std::endl
  //           << "find(1) = " << s.lookup(1) << std::endl;
}