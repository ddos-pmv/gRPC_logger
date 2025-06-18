#pragma once

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/syscall.h>

#include <array>
#include <atomic>
#include <cstdint>

namespace logger {

template <size_t N>
class RingBuffer {
 public:
  RingBuffer();
  bool write(uint8_t* data, size_t size);
  bool read(uint8_t* data, size_t size);

 private:
  std::array<char, 18> shm_name_;
  size_t buffer_size_;
  std::atomic<size_t> head_;  // pointer to read

  uint8_t* buffer_;
  std::atomic<size_t> tail_;  // pointer to write
};

}  // namespace logger

#include "RingBuffer.tpp"
