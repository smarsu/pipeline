// Copyright (c) 2019 smarsu. All Rights Reserved.

#pragma once
#include <memory>
#include <thread>
#include <vector>

#include <glog/logging.h>
#include <tsque.h>

#define ToString(name) #name

#define shared_tsque(T) std::shared_ptr<tsque::TsQue<T>>

namespace pipeline {

class ForwardPipeline {
 public:
  ForwardPipeline() {}

  template <typename Ret, typename... Args, typename T, typename... TArgs>
  shared_tsque(Ret) addTask(int num_threads,
                            Ret (*func)(Args...),
                            shared_tsque(T) in,
                            TArgs... rest) {
    CHECK_GE(num_threads, 1);

    shared_tsque(Ret) pipe(new tsque::TsQue<Ret>);

    for (int i = 0; i < num_threads; ++i) {
      std::thread *t = new std::thread(
          &ForwardPipeline::runTask<Ret (*)(Args...), T, Ret, TArgs...>,
          this,
          func,
          in,
          pipe,
          rest...);
      threads.push_back(t);
    }

    return pipe;
  }

  template <typename Ret, typename... Args, typename T, typename... TArgs>
  shared_tsque(Ret) addTaskStackBatch(int num_threads,
                                      Ret (*func)(Args...),
                                      shared_tsque(T) in,
                                      int batch_size,
                                      TArgs... rest) {
    CHECK_GE(num_threads, 1);
    CHECK_GE(batch_size, 1);

    shared_tsque(Ret) pipe(new tsque::TsQue<Ret>);

    for (int i = 0; i < num_threads; ++i) {
      std::thread *t = new std::thread(
          &ForwardPipeline::
              runTaskStackBatch<Ret (*)(Args...), T, Ret, TArgs...>,
          this,
          func,
          in,
          pipe,
          batch_size,
          rest...);
      threads.push_back(t);
    }

    return pipe;
  }
  
  template <typename Ret, typename... Args, typename T, typename... TArgs>
  shared_tsque(Ret) addTaskSplitBatch(int num_threads,
                                      std::vector<Ret> (*func)(Args...),
                                      shared_tsque(T) in,
                                      TArgs... rest) {
    CHECK_GE(num_threads, 1);

    shared_tsque(Ret) pipe(new tsque::TsQue<Ret>);

    for (int i = 0; i < num_threads; ++i) {
      std::thread *t = new std::thread(
          &ForwardPipeline::runTaskSplitBatch<std::vector<Ret> (*)(Args...),
                                              T,
                                              Ret,
                                              TArgs...>,
          this,
          func,
          in,
          pipe,
          rest...);
      threads.push_back(t);
    }

    return pipe;
  }

  template <typename T>
  shared_tsque(T) stackPipe(int num_threads, 
                            std::vector<shared_tsque(T)> in) {
    CHECK_GE(num_threads, 1);

    shared_tsque(T) pipe(new tsque::TsQue<T>);

    auto stack_pipe = [](std::vector<shared_tsque(T)> in, shared_tsque(T) out) {
      bool pop_suc = false;
      while (true) {
        for (auto &in_pipe : in) {
          auto data = in_pipe->pop_ex(pop_suc);

          if (pop_suc) {
            out->push(data);
          }
        }
      }
    };

    for (int i = 0; i < num_threads; ++i) {
      std::thread *t = new std::thread(stack_pipe, in, pipe);
      threads.push_back(t);
    }

    return pipe;
  }

 private:
  template <typename T, typename T1, typename T2, typename... TArgs>
  void runTask(T func,
               shared_tsque(T1) in,
               shared_tsque(T2) out,
               TArgs... rest) {
    // TODO(smarsu): Judge the end of the loop.
    while (true) {
      auto bottom = in->pop();

      auto top = func(bottom, rest...);

      out->push(top);
    }
  }

  template <typename T, typename T1, typename T2, typename... TArgs>
  void runTaskStackBatch(T func,
                         shared_tsque(T1) in,
                         shared_tsque(T2) out,
                         int batch_size,
                         TArgs... rest) {
    // TODO(smarsu): Judge the end of the loop.
    while (true) {
      auto bottoms = in->pop_n(batch_size);

      auto tops = func(bottoms, rest...);

      out->push(tops);
    }
  }

  template <typename T, typename T1, typename T2, typename... TArgs>
  void runTaskSplitBatch(T func,
                         shared_tsque(T1) in,
                         shared_tsque(T2) out,
                         TArgs... rest) {
    // TODO(smarsu): Judge the end of the loop.
    while (true) {
      auto bottoms = in->pop();

      auto tops = func(bottoms, rest...);

      out->push_n(tops);
    }
  }

  ~ForwardPipeline() {
    for (auto t : threads) {
      delete t;
    }
  }

 private:
  std::vector<std::thread *> threads;
};

}  // namespace pipeline
