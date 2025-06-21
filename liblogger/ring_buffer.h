#pragma once

#include <fcntl.h>
#include <libc.h>
#include <sys/mman.h>
#include <sys/syscall.h>

#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <thread>

namespace logger {

template <typename PackedObject, size_t N>
class RingBuffer {
 public:
  RingBuffer();
  ~RingBuffer();

  bool write(const PackedObject& entry);
  bool read(PackedObject& out);
  bool read(PackedObject* const batch, size_t& batch_size);

 private:
  std::array<char, 18> shm_name_;  // thread unique name for shm_open
  size_t buffer_size_;
  std::atomic<size_t> head_ = 0;  // pointer to read
  std::atomic<size_t> tail_ = 0;  // pointer to write
  uint8_t* buffer_;
};

}  // namespace logger

#include "ring_buffer.tpp"
