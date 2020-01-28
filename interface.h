#pragma once

template <typename T>
class fast_set {
 public:
  virtual bool lookup(T key) = 0;
  virtual void insert(T key) = 0;
  virtual T succ(T key) = 0;
};
