#pragma once

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

#include "shared_queue.h"

namespace logger {

template <typename SharedQueueT>
class WorkerPool {
 public:
  using ValueType = typename SharedQueueT::value_type;
  using BufferType = typename SharedQueueT::buffer_type;
  using PackedObject = typename BufferType::packed_object_type;
  static constexpr size_t Capacity = SharedQueueT::capacity;

  WorkerPool(uint16_t thread_num, SharedQueueT& shared_queue)
      : thread_num_(thread_num),
        queue_(shared_queue),
        cv_(queue_.get_cv()),
        cv_mtx_(queue_.get_mtx()) {
    for (int i = 0; i < thread_num; ++i) {
      workers_.emplace(&WorkerPool::workerThread, this);
    }
  };

 private:
  void workerThread() {
    ValueType buffer_ref = nullptr;
    PackedObject batch[kBatchSize];
    size_t batch_size;
    while (true) {
      batch_size = kBatchSize;
      if (queue_.pop(buffer_ref) && buffer_ref->read(batch, batch_size)) {
        // grpc send
      } else {
        std::lock_guard lock(cv_mtx_);
        cv_.wait(lock);
      }
    }
  }

  std::condition_variable& cv_;
  std::mutex& cv_mtx_;
  uint16_t thread_num_;
  std::vector<std::thread> workers_;
  SharedQueueT& queue_;
};

}  // namespace logger