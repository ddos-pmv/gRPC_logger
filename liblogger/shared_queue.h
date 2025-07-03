#pragma once

#include <boost/lockfree/queue.hpp>
#include <condition_variable>
#include <thread>

namespace logger {

template <typename T, size_t Capacity>
class SharedQueue {
 public:
  using value_type = T;
  using buffer_type = typename std::decay_t<T>;

  SharedQueue() : shared_queue_(Capacity) {}

  void push(T buf_ptr) {
    while (!shared_queue_.push(buf_ptr)) {
      std::this_thread::yield();
      waits++;
    };
    pushs++;
    cv_.notify_one();
  }

  bool pop(T *&entry) { return shared_queue_.pop(entry); }

  std::condition_variable &get_cv() { return cv_; }
  std::mutex &get_mutex() { return mtx_; }

 private:
  alignas(64) std::mutex mtx_;
  alignas(64) std::condition_variable cv_;
  alignas(64) std::atomic<size_t> waits;
  alignas(64) std::atomic<size_t> pushs;
  boost::lockfree::queue<T> shared_queue_;
};
}  // namespace logger