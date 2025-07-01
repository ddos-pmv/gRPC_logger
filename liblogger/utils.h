#pragma once

#include <boost/lockfree/queue.hpp>
#include <cstdint>

namespace logger {

struct PackedEntry {
  uint8_t log_level;
  uint16_t log_line;
  uint64_t log_timestamp;
  char log_file[64];
  char message[256];
} __attribute__((packed));

constexpr size_t kRingBufferSize = 8192;

enum class LogLevel {
  DEBUG,
  INFO,
  WARNING,
  ERROR,
};

boost::lockfree::queue<RingBuffer<PackedEntry, kRingBufferSize>*> buffers_queue;

const char* toString(LogLevel level) {
  static const char* levelStr[] = {"DEBUG", "INFO", "WARNING", "ERROR"};
  return levelStr[static_cast<int>(level)];
}

}  // namespace logger