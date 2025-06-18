#include "logger.h"

namespace logger {

Logger& Logger::getInstance() {
  static Logger instance;
  return instance;
}
void Logger::setOutStream(std::ostream& oStream) {
  std::lock_guard lock(mtx_);
  out_ = &oStream;
}

void Logger::log(LogLevel level, const std::string& file, int line,
                 const std::string& msg) {
  if (out_) {
    // *out_ << "[" << toString(level) << "] "
    //       << "[" << std::this_thread::get_id() << "] "
    //       << "(Buffer: " << buf.counter << ") " << msg << std::endl;
  }
}

// Инициализация thread-local переменной
thread_local RingBuffer<8192> Logger::ring_buffer;
}  // namespace logger