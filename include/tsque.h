// Copyright (c) 2019 smarsu. All Rights Reserved.

#pragma once
#include <deque>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

#include <glog/logging.h>

namespace tsque {

#define USLEEP(t) std::this_thread::sleep_for(std::chrono::microseconds(t));

/* By default, push data to tail, pop data from head.
 *
 */
enum TsQuePosition : int {
  TSQUE_HEAD,
  TSQUE_TAIL,
};

template <typename T>
class TsQue {
 public:
  TsQue() {}

  explicit TsQue(int capacity) : capacity_(capacity) {}

  ~TsQue() {}

  T operator[](int i) {
    locker_.lock();
    if (i < 0) {
      i += size_;
    }
    CHECK(i >= 0 && i < size_) << "index: " << i << " border: " << size_;
    T data = datas_[i];
    locker_.unlock();
    return std::move(data);
  }

  void reserve(int capacity) { capacity_ = capacity; }

  int size() { return size_; }

  void push(const T &data, TsQuePosition pos = TSQUE_TAIL) {
    locker_.lock();

    while (size_ >= capacity_) {
      locker_.unlock();
      USLEEP(100);
      locker_.lock();
    }

    force_push(data, pos);
    locker_.unlock();
  }

  void push_n(const std::vector<T> &datas, TsQuePosition pos = TSQUE_TAIL) {
    for (const auto &data : datas) {
      push(data, pos);
    }
  }

  T pop(TsQuePosition pos = TSQUE_HEAD) {
    T data;
    locker_.lock();

    while (size_ <= 0) {
      locker_.unlock();
      USLEEP(100);
      locker_.lock();
    }

    data = force_pop(pos);
    locker_.unlock();
    return std::move(data);
  }

  /* pop_ex: pop data, if fail (e.g. no data in queue), the ret will be set fo
   * false. */
  T pop_ex(bool *ret, TsQuePosition pos = TSQUE_HEAD) {
    T data;
    locker_.lock();
    if (size_ <= 0) {
      *ret = false;
      locker_.unlock();
      return data;
    } else {
      *ret = true;
      data = force_pop(pos);
      locker_.unlock();
      return std::move(data);
    }
  }

  // wait to pop n datas.
  std::vector<T> force_pop_n(int n, TsQuePosition pos = TSQUE_HEAD) {
    std::vector<T> list;
    for (int i = 0; i < n; ++i) {
      list.emplace_back(pop(pos));
    }
    return std::move(list);
  }

  // pop at least one data, at most n data
  std::vector<T> pop_n(int n, TsQuePosition pos = TSQUE_HEAD) {
    std::vector<T> list;
    bool ret;
    list.emplace_back(pop(pos));
    for (int i = 1; i < n; ++i) {
      T data = pop_ex(&ret, pos);
      if (ret == false) {
        break;
      }
      list.emplace_back(data);
    }
    return std::move(list);
  }

 private:
  // push without lock
  void force_push(const T &data, TsQuePosition pos) {
    switch (pos) {
      case TSQUE_TAIL:
        datas_.emplace_back(data);
        ++size_;
        break;
      case TSQUE_HEAD:
        datas_.emplace_front(data);
        ++size_;
        break;

      default:
        break;
    }
  }

  // pop without lock
  T force_pop(TsQuePosition pos) {
    T data;
    switch (pos) {
      case TSQUE_TAIL:
        data = datas_.back();
        datas_.pop_back();
        --size_;
        break;
      case TSQUE_HEAD:
        data = datas_.front();
        datas_.pop_front();
        --size_;
        break;

      default:
        break;
    }
    return std::move(data);
  }

 private:
  std::deque<T> datas_;
  int capacity_ = 0x7fffffff;
  int size_ = 0;
  std::mutex locker_;
};

}  // namespace tsque
