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
  buf.counter++;

  if (out_) {
    // *out_ << "[" << toString(level) << "] "
    //       << "[" << std::this_thread::get_id() << "] "
    //       << "(Buffer: " << buf.counter << ") " << msg << std::endl;
  }
}

// Инициализация thread-local переменной
thread_local RingBuffer Logger::buf;
}  // namespace logger