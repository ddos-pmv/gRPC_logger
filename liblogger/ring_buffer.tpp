#include <libc.h>
#include <sys/syscall.h>

#include <cmath>
#include <thread>

#include "ring_buffer.h"

#ifndef PAGE_SIZE
#warning "PAGE_SIZE not defined"
#endif

namespace logger {

#if defined(__linux__) && \
    (__GLIBC__ < 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ < 27))
#include <sys/syscall.h>
#ifndef __NR_memfd_create
#include <asm-generic/unistd.h>
#endif

static inline int memfd_create(const char* name, unsigned int flags) {
  return syscall(__NR_memfd_create, name, flags);
}
#endif

template <typename PackedObject, size_t N>
RingBuffer<PackedObject, N>::RingBuffer() {
  constexpr static size_t NUM_PAGES = (N + PAGE_SIZE - 1) / PAGE_SIZE;
  buffer_size_ = NUM_PAGES * PAGE_SIZE;

  // init unique(thread id) name for shm_open ( NAME ...)
  std::snprintf(shm_name_.data(), shm_name_.size(), "/b_%016zx",
                std::hash<std::thread::id>{}(std::this_thread::get_id()));

#if defined(__APPLE__)
  int fd = shm_open(shm_name_.data(), O_CREAT | O_RDWR | O_TRUNC, 0600);
  shm_unlink(shm_name_.data());  // Удаляем из namespace, оставляя только fd
#elif defined(__linux__)
  int fd = memfd_create(shm_name_.data(), 0);
#endif
  if (fd < 0) std::exit(EXIT_FAILURE);
  if (ftruncate(fd, buffer_size_) < 0) std::exit(EXIT_FAILURE);

  // resever memroy for 2 pages
  buffer_ = static_cast<uint8_t*>(mmap(NULL, buffer_size_ * 2, PROT_NONE,
                                       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));

  if (buffer_ == MAP_FAILED) std::exit(EXIT_FAILURE);

  // mapping first page
  if (mmap(buffer_, buffer_size_, PROT_READ | PROT_WRITE,
           MAP_SHARED | MAP_FIXED, fd, 0) == MAP_FAILED)
    std::exit(EXIT_FAILURE);

  // mapping second page
  if (mmap(buffer_ + buffer_size_, buffer_size_, PROT_READ | PROT_WRITE,
           MAP_SHARED | MAP_FIXED, fd, 0) == MAP_FAILED)
    std::exit(EXIT_FAILURE);
}

//
//
// SPSC write(entry)
//
template <typename PackedObject, size_t N>
bool RingBuffer<PackedObject, N>::write(const PackedObject& entry) {
  // std::cout << "BUFFER write()" << std::endl;
  constexpr size_t entry_size = sizeof(PackedObject);
  ssize_t tail = tail_.load(std::memory_order_relaxed);
  ssize_t head = head_.load(std::memory_order_relaxed);

  size_t available_space;

  // wait consumer to sub the tail
  if (tail + entry_size > buffer_size_ * 2) return false;

  if (tail >= head)
    available_space = buffer_size_ - (tail - head);
  else
    return false;

  if (available_space < entry_size) return false;  // Не хватает места

  std::memcpy(&buffer_[tail], &entry, entry_size);
  tail_.fetch_add(entry_size, std::memory_order_acq_rel);
  return true;
}

//
//
// SPSC read(out)
//
template <typename PackedObject, size_t N>
bool RingBuffer<PackedObject, N>::read(PackedObject& out) {
  bool expect = false;

  for (int i = 0; i < spinCount; i++) {
    if (is_reading_.compare_exchange_weak(expect, true,
                                          std::memory_order_acquire))
      break;
    expect = false;
    std::this_thread::yield();
  }

  if (!is_reading_.load(std::memory_order_relaxed)) {
    return false;
  }

  ssize_t tail = tail_.load(std::memory_order_acquire);
  ssize_t head = head_.load(std::memory_order_relaxed);

  constexpr size_t entry_size = sizeof(PackedObject);

  if (tail - head < entry_size) {
    return false;
  }

  // copy data from buffer to object
  std::memcpy(&out, &buffer_[head], entry_size);

  head += entry_size;

  if (head > buffer_size_) {
    head -= buffer_size_;
    tail_.fetch_sub(buffer_size_, std::memory_order_relaxed);
  }
  head_.store(head, std::memory_order_release);
  is_reading_.store(false, std::memory_order_release);
  return true;
}

//
// isReading
//
template <typename PackedObject, size_t N>
bool RingBuffer<PackedObject, N>::isReading() {
  return is_reading_.load(std::memory_order_relaxed);
}

//
// read(batch, batc_size)
//
template <typename PackedObject, size_t N>
bool RingBuffer<PackedObject, N>::read(PackedObject* const batch,
                                       size_t& batch_size) {
  bool expect = false;
  for (int i = 0; i < spinCount; i++) {
    if (is_reading_.compare_exchange_weak(expect, true,
                                          std::memory_order_acquire))
      break;
    expect = false;
    std::this_thread::yield();
  }

  if (!is_reading_.load(std::memory_order_relaxed)) {
    return false;
  }

  ssize_t tail = tail_.load(std::memory_order_acquire);
  ssize_t head = head_.load(std::memory_order_relaxed);

  constexpr size_t entry_size = sizeof(PackedObject);

  if (tail - head < entry_size) {
    return false;
  }

  size_t objects_to_read =
      std::min<size_t>(batch_size, (tail - head) / entry_size);

  batch_size = objects_to_read;

  // copy data from buffer to object
  std::memcpy(batch, &buffer_[head], objects_to_read * entry_size);

  head += objects_to_read * entry_size;

  if (head > buffer_size_) {
    head -= buffer_size_;
    tail_.fetch_sub(buffer_size_, std::memory_order_relaxed);
  }
  head_.store(head, std::memory_order_release);
  is_reading_.store(false, std::memory_order_release);
  return true;
}

// bool get(queue_t *q, uint8_t *data, size_t size) {
//     if(q->tail - q->head < size){
//         return false;
//     }
//     memcpy(data, &q->buffer[q->head], size);
//     q->head += size;
//     if(q->head > q->buffer_size) {
//        q->head -= q->buffer_size;
//        q->tail -= q->buffer_size;
//     }
//     return true;
// }

template <typename PackedObject, size_t N>
RingBuffer<PackedObject, N>::~RingBuffer() {
  munmap(buffer_, 2 * buffer_size_);
}

}  // namespace logger
