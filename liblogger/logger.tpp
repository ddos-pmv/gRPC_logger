#include <string_view>

#include "logger.h"

namespace logger {

void Logger::setOutStream(std::ostream& oStream) {
  std::lock_guard lock(mtx_);
  out_ = &oStream;
}

// void Logger::log(LogLevel level, const std::string& file, int line,
//                  const std::string_view msg) {
//   if (msg.isLi) }

template <typename MsgType>
inline void Logger::log(LogLevel level, const std::string& file, int line,
                        MsgType&& msg) {}

// Инициализация thread-local переменной
thread_local RingBuffer<PackedEntry, kRingBufferSize> Logger::ring_buffer;
}  // namespace logger