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

/*
!!!!!!!!!
REALISATION FOR 1 producer!!!!!!
*/
template <typename PackedObject, size_t N>
bool RingBuffer<PackedObject, N>::write(const PackedObject& entry) {
  // std::cout << "BUFFER write()" << std::endl;
  constexpr size_t entry_size = sizeof(PackedObject);
  size_t head = head_.load();

  while (true) {
    size_t tail = tail_.load(std::memory_order_relaxed);

    if (buffer_size_ - (tail - head) < entry_size) {
      // std::cout << "head: " << head << " tail: " << tail_ << std::endl;
      return false;
    }

    std::memcpy(&buffer_[tail], &entry, entry_size);
    tail_.fetch_add(entry_size);
    return true;
    // if (tail_.compare_exchange_weak(tail, tail + entry_size)) {
    //   std::memcpy(&buffer_[tail], &entry, entry_size);
    //   return true;
    // }
  }
}

/*
!!!!! REALISATION FOR ONE CONSUMER THREAD
*/
template <typename PackedObject, size_t N>
bool RingBuffer<PackedObject, N>::read(PackedObject& out) {
  // std::cout << "BUFFER read()" << std::endl;
  size_t tail = tail_.load(std::memory_order_relaxed);
  size_t head = head_.load(std::memory_order_relaxed);

  constexpr size_t entry_size = sizeof(PackedObject);

  // std::cout << "head: " << head << std::endl;
  // std::cout << "tail: " << tail << " ";

  if (tail - head < entry_size) {
    std::cout << "false" << '\n';
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

// template <size_t N>
// bool RingBuffer<N>::read(uint8_t* data, size_t size) {}

template <typename PackedObject, size_t N>
RingBuffer<PackedObject, N>::~RingBuffer() {
  munmap(buffer_, 2 * buffer_size_);
}

}  // namespace logger
